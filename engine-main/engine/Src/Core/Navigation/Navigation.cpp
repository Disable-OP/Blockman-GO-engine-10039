#include "Core.h"
#include "Navigation.h"
#include "Recast.h"
#include "DetourNavMeshBuilder.h"
#include "DetourCommon.h"
#include "InputGeometryData.h"
#include "Resource/DataStream.h"
#include "DetourCrowd.h"

namespace LORD
{

	inline bool inRange(const float* v1, const float* v2, const float r, const float h)
	{
		const float dx = v2[0] - v1[0];
		const float dy = v2[1] - v1[1];
		const float dz = v2[2] - v1[2];
		return (dx*dx + dz*dz) < r*r && fabsf(dy) < h;
	}


	static int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
		const dtPolyRef* visited, const int nvisited)
	{
		int furthestPath = -1;
		int furthestVisited = -1;

		// Find furthest common polygon.
		for (int i = npath-1; i >= 0; --i)
		{
			bool found = false;
			for (int j = nvisited-1; j >= 0; --j)
			{
				if (path[i] == visited[j])
				{
					furthestPath = i;
					furthestVisited = j;
					found = true;
				}
			}
			if (found)
				break;
		}

		// If no intersection found just return current path. 
		if (furthestPath == -1 || furthestVisited == -1)
			return npath;

		// Concatenate paths.	

		// Adjust beginning of the buffer to include the visited.
		const int req = nvisited - furthestVisited;
		const int orig = rcMin(furthestPath+1, npath);
		int size = rcMax(0, npath-orig);
		if (req+size > maxPath)
			size = maxPath-req;
		if (size)
			memmove(path+req, path+orig, size*sizeof(dtPolyRef));

		// Store visited
		for (int i = 0; i < req; ++i)
			path[i] = visited[(nvisited-1)-i];				

		return req+size;
	}

	static bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
		const float minTargetDist,
		const dtPolyRef* path, const int pathSize,
		float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
		float* outPoints = 0, int* outPointCount = 0)							 
	{
		// Find steer target.
		static const int MAX_STEER_POINTS = 3;
		float steerPath[MAX_STEER_POINTS*3];
		unsigned char steerPathFlags[MAX_STEER_POINTS];
		dtPolyRef steerPathPolys[MAX_STEER_POINTS];
		int nsteerPath = 0;
		navQuery->findStraightPath(startPos, endPos, path, pathSize,
			steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
		if (!nsteerPath)
			return false;

		if (outPoints && outPointCount)
		{
			*outPointCount = nsteerPath;
			for (int i = 0; i < nsteerPath; ++i)
				dtVcopy(&outPoints[i*3], &steerPath[i*3]);
		}


		// Find vertex far enough to steer to.
		int ns = 0;
		while (ns < nsteerPath)
		{
			// Stop at Off-Mesh link or when point is further than slop away.
			if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
				!inRange(&steerPath[ns*3], startPos, minTargetDist, 1000.0f))
				break;
			ns++;
		}
		// Failed to find good point to steer to.
		if (ns >= nsteerPath)
			return false;

		dtVcopy(steerPos, &steerPath[ns*3]);
		steerPos[1] = startPos[1];
		steerPosFlag = steerPathFlags[ns];
		steerPosRef = steerPathPolys[ns];

		return true;
	}

	Navigation::Navigation()
		: m_navMesh(NULL)
		, m_navQuery(NULL)
		, m_triareas(NULL)
		, m_solid(NULL)
		, m_chf(NULL)
		, m_cset(NULL)
		, m_pmesh(NULL)
		, m_dmesh(NULL)
		, m_InputGeometryData(NULL)
		, m_navData(0)
		, m_navDataSize(0)
		, m_startRef(0)
		, m_endRef(0)
		, m_npolys(0)
		, m_nsmoothPath(0)
	{
		m_polyPickExt[0] = 2;
		m_polyPickExt[1] = 4;
		m_polyPickExt[2] = 2;

		m_filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
		m_filter.setExcludeFlags(0);

		resetCommonSettings();
		m_navQuery = dtAllocNavMeshQuery();
		m_crowd =  dtAllocCrowd();
	}

	Navigation::~Navigation()
	{
		if (m_navData)
		{
			LordFree(m_navData);
			m_navData = NULL;
		}

		dtFreeCrowd(m_crowd);
		dtFreeNavMeshQuery(m_navQuery);
		cleanupNavigationMesh();
	}

	void Navigation::resetCommonSettings()
	{
		m_cellSize = 0.3f;
		m_cellHeight = 0.1f;
		m_agentHeight = 2.0f;
		m_agentRadius = 0.3f;
		m_agentMaxClimb = 0.9f;
		m_agentMaxSlope = 45.0f;
		m_regionMinSize = 8;
		m_regionMergeSize = 20;
		m_monotonePartitioning = false;
		m_edgeMaxLen = 12.0f;
		m_edgeMaxError = 1.3f;
		m_vertsPerPoly = 6.0f;
		m_detailSampleDist = 6.0f;
		m_detailSampleMaxError = 1.0f;
	}

	bool Navigation::buildNavigationMesh()
	{
		LordAssert(m_InputGeometryData);

		cleanupNavigationMesh();

		const float* bmin = (float*)&(m_InputGeometryData->getAABB().vMin);
		const float* bmax = (float*)&(m_InputGeometryData->getAABB().vMax);
		const float* verts = m_InputGeometryData->getVerts();
		const int nverts = m_InputGeometryData->getVertCount();
		const int* tris = m_InputGeometryData->getTris();
		const int ntris = m_InputGeometryData->getTriCount();

		rcConfig buildConfig;
		//
		// Step 1. Initialize build config.
		//

		// Init build configuration from GUI
		memset(&buildConfig, 0, sizeof(buildConfig));
		buildConfig.cs = m_cellSize;
		buildConfig.ch = m_cellHeight;
		buildConfig.walkableSlopeAngle = m_agentMaxSlope;
		buildConfig.walkableHeight = (int)ceilf(m_agentHeight / buildConfig.ch);
		buildConfig.walkableClimb = (int)floorf(m_agentMaxClimb / buildConfig.ch);
		buildConfig.walkableRadius = (int)ceilf(m_agentRadius / buildConfig.cs);
		buildConfig.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
		buildConfig.maxSimplificationError = m_edgeMaxError;
		buildConfig.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
		buildConfig.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
		buildConfig.maxVertsPerPoly = (int)m_vertsPerPoly;
		buildConfig.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
		buildConfig.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

		// Set the area where the navigation will be build.
		// Here the bounds of the input mesh are used, but the
		// area could be specified by an user defined box, etc.
		rcVcopy(buildConfig.bmin, bmin);
		rcVcopy(buildConfig.bmax, bmax);
		rcCalcGridSize(buildConfig.bmin, buildConfig.bmax, buildConfig.cs, &buildConfig.width, &buildConfig.height);

		//
		// Step 2. Rasterize input polygon soup.
		//

		// Allocate voxel heightfield where we rasterize our input data to.
		m_solid = rcAllocHeightfield();
		if (!m_solid)
		{
			LordLogError("buildNavigation: Out of memory 'solid'.");
			return false;
		}
		if (!rcCreateHeightfield(&m_ctx, *m_solid, buildConfig.width, buildConfig.height, buildConfig.bmin, buildConfig.bmax, buildConfig.cs, buildConfig.ch))
		{
			LordLogError("buildNavigation: Could not create solid heightfield.");
			return false;
		}

		// Allocate array that can hold triangle area types.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		m_triareas = (unsigned char*)LordMalloc(ntris);
		if (!m_triareas)
		{
			LordLogError("buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
			return false;
		}

		// Find triangles which are walkable based on their slope and rasterize them.
		// If your input data is multiple meshes, you can transform them here, calculate
		// the are type for each of the meshes and rasterize them.
		memset(m_triareas, 0, ntris*sizeof(unsigned char));
		rcMarkWalkableTriangles(&m_ctx, buildConfig.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
		rcRasterizeTriangles(&m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, buildConfig.walkableClimb);

		LordFree(m_triareas);
		m_triareas = 0;

		//
		// Step 3. Filter walkables surfaces.
		//

		// Once all geoemtry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		rcFilterLowHangingWalkableObstacles(&m_ctx, buildConfig.walkableClimb, *m_solid);
		rcFilterLedgeSpans(&m_ctx, buildConfig.walkableHeight, buildConfig.walkableClimb, *m_solid);
		rcFilterWalkableLowHeightSpans(&m_ctx, buildConfig.walkableHeight, *m_solid);


		//
		// Step 4. Partition walkable surface to simple regions.
		//

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		m_chf = rcAllocCompactHeightfield();
		if (!m_chf)
		{
			LordLogError("buildNavigation: Out of memory 'chf'.");
			return false;
		}
		if (!rcBuildCompactHeightfield(&m_ctx, buildConfig.walkableHeight, buildConfig.walkableClimb, *m_solid, *m_chf))
		{
			LordLogError("buildNavigation: Could not build compact data.");
			return false;
		}

		rcFreeHeightField(m_solid);
		m_solid = 0;

		// ------------------------------------------------------------------------------------------
		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(&m_ctx, buildConfig.walkableRadius, *m_chf))
		{
			LordLogError("buildNavigation: Could not erode.");
			return false;
		}

		// (Optional) Mark areas.
		//const ConvexVolume* vols = m_geom->getConvexVolumes();
		//for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
		//	rcMarkConvexPolyArea(&m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);
		//---------------------------------------------------------------------------------------------------------------------------------

		if (m_monotonePartitioning)
		{
			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if (!rcBuildRegionsMonotone(&m_ctx, *m_chf, 0, buildConfig.minRegionArea, buildConfig.mergeRegionArea))
			{
				LordLogError("buildNavigation: Could not build regions.");
				return false;
			}
		}
		else
		{
			// Prepare for region partitioning, by calculating distance field along the walkable surface.
			if (!rcBuildDistanceField(&m_ctx, *m_chf))
			{
				LordLogError("buildNavigation: Could not build distance field.");
				return false;
			}

			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildRegions(&m_ctx, *m_chf, 0, buildConfig.minRegionArea, buildConfig.mergeRegionArea))
			{
				LordLogError("buildNavigation: Could not build regions.");
				return false;
			}
		}

		//
		// Step 5. Trace and simplify region contours.
		//

		// Create contours.
		m_cset = rcAllocContourSet();
		if (!m_cset)
		{
			LordLogError("buildNavigation: Out of memory 'cset'.");
			return false;
		}
		if (!rcBuildContours(&m_ctx, *m_chf, buildConfig.maxSimplificationError, buildConfig.maxEdgeLen, *m_cset))
		{
			LordLogError("buildNavigation: Could not create contours.");
			return false;
		}

		//
		// Step 6. Build polygons mesh from contours.
		//

		// Build polygon navmesh from the contours.
		m_pmesh = rcAllocPolyMesh();
		if (!m_pmesh)
		{
			LordLogError("buildNavigation: Out of memory 'pmesh'.");
			return false;
		}
		if (!rcBuildPolyMesh(&m_ctx, *m_cset, buildConfig.maxVertsPerPoly, *m_pmesh))
		{
			LordLogError("buildNavigation: Could not triangulate contours.");
			return false;
		}

		//
		// Step 7. Create detail mesh which allows to access approximate height on each polygon.
		//

		m_dmesh = rcAllocPolyMeshDetail();
		if (!m_dmesh)
		{
			LordLogError("buildNavigation: Out of memory 'pmdtl'.");
			return false;
		}

		if (!rcBuildPolyMeshDetail(&m_ctx, *m_pmesh, *m_chf, buildConfig.detailSampleDist, buildConfig.detailSampleMaxError, *m_dmesh))
		{
			LordLogError("buildNavigation: Could not build detail mesh.");
			return false;
		}

		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;

		// At this point the navigation mesh data is ready, you can access it from m_pmesh.
		// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

		//
		// (Optional) Step 8. Create Detour data from Recast poly mesh.
		//

		// The GUI may allow more max points per polygon than Detour can handle.
		// Only build the detour navmesh if we do not exceed the limit.
		if (buildConfig.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
		{
			unsigned char* navData = 0;
			int navDataSize = 0;

			// Update poly flags from areas.
			for (int i = 0; i < m_pmesh->npolys; ++i)
			{
				if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
					m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

				if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
					m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
					m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_TYPE_0)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_TYPE_0;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_TYPE_1)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_TYPE_1;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_TYPE_2)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_TYPE_2;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_TYPE_3)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_TYPE_3;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_TYPE_4)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_TYPE_4;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_TYPE_5)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_TYPE_5;
				}
			}


			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = m_pmesh->verts;
			params.vertCount = m_pmesh->nverts;
			params.polys = m_pmesh->polys;
			params.polyAreas = m_pmesh->areas;
			params.polyFlags = m_pmesh->flags;
			params.polyCount = m_pmesh->npolys;
			params.nvp = m_pmesh->nvp;
			params.detailMeshes = m_dmesh->meshes;
			params.detailVerts = m_dmesh->verts;
			params.detailVertsCount = m_dmesh->nverts;
			params.detailTris = m_dmesh->tris;
			params.detailTriCount = m_dmesh->ntris;
			//params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
			//params.offMeshConRad = m_geom->getOffMeshConnectionRads();
			//params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
			//params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
			//params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
			//params.offMeshConUserID = m_geom->getOffMeshConnectionId();
			//params.offMeshConCount = m_geom->getOffMeshConnectionCount();
			params.walkableHeight = m_agentHeight;
			params.walkableRadius = m_agentRadius;
			params.walkableClimb = m_agentMaxClimb;
			rcVcopy(params.bmin, m_pmesh->bmin);
			rcVcopy(params.bmax, m_pmesh->bmax);
			params.cs = buildConfig.cs;
			params.ch = buildConfig.ch;
			params.buildBvTree = true;


			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				LordLogError("Could not build Detour navmesh.");
				return false;
			}
			
			// backup a navigation mesh data for export
			if (m_navData)
			{
				LordFree(m_navData);
				m_navData = NULL;
			}

			m_navData = (unsigned char*)LordMalloc(navDataSize);
			memcpy(m_navData, navData, navDataSize);
			m_navDataSize = navDataSize;
			// ------------------------------------------------------

			m_navMesh = dtAllocNavMesh();
			if (!m_navMesh)
			{
				dtFree(navData);
				LordLogError("Could not create Detour navmesh");
				return false;
			}

			dtStatus status;

			status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
			if (dtStatusFailed(status))
			{
				dtFree(navData);
				LordLogError("Could not init Detour navmesh");
				return false;
			}

			status = m_navQuery->init(m_navMesh, 2048);
			if (dtStatusFailed(status))
			{
				LordLogError("Could not init Detour navmesh query");
				return false;
			}

			InitCrowd();
		}

		return true;
	}

	void Navigation::cleanupNavigationMesh()
	{
		LordFree(m_triareas);
		m_triareas = 0;
		rcFreeHeightField(m_solid);
		m_solid = 0;
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
		rcFreePolyMesh(m_pmesh);
		m_pmesh = 0;
		rcFreePolyMeshDetail(m_dmesh);
		m_dmesh = 0;
		dtFreeNavMesh(m_navMesh);
		m_navMesh = 0;
	}

	void Navigation::exportData( DataStream* pStream )
	{
		pStream->write(&m_navDataSize, sizeof(int));
		pStream->write(m_navData, m_navDataSize);
	}

	void Navigation::importData( DataStream* pStream )
	{
		int navDataSize = 0;
		pStream->read(&navDataSize, sizeof(int));

		unsigned char* navData = (unsigned char*)LordMalloc(navDataSize);
		LordAssert(navData);
		pStream->read(navData, navDataSize);

		cleanupNavigationMesh();

		m_navMesh = dtAllocNavMesh();
		if (!m_navMesh)
		{
			dtFree(navData);
			LordLogError("Could not create Detour navmesh");
			return;
		}

		dtStatus status;

		status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			LordLogError("Could not init Detour navmesh");
			return;
		}

		status = m_navQuery->init(m_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			LordLogError("Could not init Detour navmesh query");
			return;
		}
		
		InitCrowd();
	}

	void Navigation::findPath( const Vector3& startPos, const Vector3& endPos, float*& path, int& pathCount )
	{
		memcpy(m_spos, &startPos, sizeof(float)*3);
		memcpy(m_epos, &endPos, sizeof(float)*3);
		if (!m_navMesh)
			return;
		m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
		m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

		if (m_startRef && m_endRef)
		{
			m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);

			m_nsmoothPath = 0;

			if (m_npolys)
			{
				// Iterate over the path to find smooth path on the detail mesh surface.
				dtPolyRef polys[MAX_POLYS];
				memcpy(polys, m_polys, sizeof(dtPolyRef)*m_npolys); 
				int npolys = m_npolys;

				float iterPos[3], targetPos[3];
				m_navQuery->closestPointOnPoly(m_startRef, m_spos, iterPos);
				m_navQuery->closestPointOnPoly(polys[npolys-1], m_epos, targetPos);

				static const float STEP_SIZE = 0.5f;
				static const float SLOP = 0.01f;

				m_nsmoothPath = 0;

				dtVcopy(&m_smoothPath[m_nsmoothPath*3], iterPos);
				m_nsmoothPath++;

				// Move towards target a small advancement at a time until target reached or
				// when ran out of memory to store the path.
				while (npolys && m_nsmoothPath < MAX_SMOOTH)
				{
					// Find location to steer towards.
					float steerPos[3];
					unsigned char steerPosFlag;
					dtPolyRef steerPosRef;

					if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP,
						polys, npolys, steerPos, steerPosFlag, steerPosRef))
						break;

					bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
					bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

					// Find movement delta.
					float delta[3], len;
					dtVsub(delta, steerPos, iterPos);
					len = dtSqrt(dtVdot(delta,delta));
					// If the steer target is end of path or off-mesh link, do not move past the location.
					if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
						len = 1;
					else
						len = STEP_SIZE / len;
					float moveTgt[3];
					dtVmad(moveTgt, iterPos, delta, len);

					// Move
					float result[3];
					dtPolyRef visited[16];
					int nvisited = 0;
					m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter,
						result, visited, &nvisited, 16);

					npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
					float h = 0;
					m_navQuery->getPolyHeight(polys[0], result, &h);
					result[1] = h;
					dtVcopy(iterPos, result);

					// Handle end of path and off-mesh links when close enough.
					if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
					{
						// Reached end of path.
						dtVcopy(iterPos, targetPos);
						if (m_nsmoothPath < MAX_SMOOTH)
						{
							dtVcopy(&m_smoothPath[m_nsmoothPath*3], iterPos);
							m_nsmoothPath++;
						}
						break;
					}
					else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f))
					{
						// Reached off-mesh connection.
						float startPos[3], endPos[3];

						// Advance the path up to and over the off-mesh connection.
						dtPolyRef prevRef = 0, polyRef = polys[0];
						int npos = 0;
						while (npos < npolys && polyRef != steerPosRef)
						{
							prevRef = polyRef;
							polyRef = polys[npos];
							npos++;
						}
						for (int i = npos; i < npolys; ++i)
							polys[i-npos] = polys[i];
						npolys -= npos;

						// Handle the connection.
						dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
						if (dtStatusSucceed(status))
						{
							if (m_nsmoothPath < MAX_SMOOTH)
							{
								dtVcopy(&m_smoothPath[m_nsmoothPath*3], startPos);
								m_nsmoothPath++;
								// Hack to make the dotted path not visible during off-mesh connection.
								if (m_nsmoothPath & 1)
								{
									dtVcopy(&m_smoothPath[m_nsmoothPath*3], startPos);
									m_nsmoothPath++;
								}
							}
							// Move position at the other side of the off-mesh link.
							dtVcopy(iterPos, endPos);
							float eh = 0.0f;
							m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
							iterPos[1] = eh;
						}
					}

					// Store results.
					if (m_nsmoothPath < MAX_SMOOTH)
					{
						dtVcopy(&m_smoothPath[m_nsmoothPath*3], iterPos);
						m_nsmoothPath++;
					}
				}
			}

		}
		else
		{
			m_npolys = 0;
			m_nsmoothPath = 0;
		}

		path = m_smoothPath;
		pathCount = m_nsmoothPath;
	}

	// 射线检测(求最近点)
	bool Navigation::rayDetect( const Vector3& startPos, const Vector3& dir, float& oLength, Vector3& oHitPosition) const
	{
		return m_navQuery->rayDetect( startPos, dir, oLength, &m_filter, oHitPosition);
	}

	void Navigation::updateCrowd( ui32 nElapes )
	{
		if (m_crowd)
		{
			m_crowd->update(nElapes*0.001f, NULL);
		}
	}

	LORD::Vector3 Navigation::GetCrowdPosition(int nAgentIndex)
	{
		Vector3 position(Vector3::ZERO);
		if (m_crowd)
		{
			if (nAgentIndex < 0 || nAgentIndex >= m_crowd->getAgentCount())
			{
				return position;
			}

			const dtCrowdAgent* pAgent = m_crowd->getAgent(nAgentIndex);
			position.x = pAgent->npos[0];
			position.y = pAgent->npos[1];
			position.z = pAgent->npos[2];
		}

		return position;
	}

	void Navigation::MoveCrowdToword( const Vector3& dir, int nAgentIndex)
	{
		if (m_crowd)
		{
			m_crowd->requestMoveVelocity(nAgentIndex, (float*)&dir);
		}
	}

	int Navigation::AddAgent( const Vector3& position )
	{
		float pos[3];
		pos[0] = position.x;
		pos[1] = position.y;
		pos[2] = position.z;
		dtCrowdAgentParams ap;
		memset(&ap, 0, sizeof(ap));
		ap.radius = m_agentRadius;
		ap.height = m_agentHeight;
		ap.maxAcceleration = 8.0f;
		ap.maxSpeed = 3.5f;
		ap.collisionQueryRange = ap.radius * 12.0f;
		ap.pathOptimizationRange = ap.radius * 30.0f;
		ap.updateFlags = 0; 
		//if (m_toolParams.m_anticipateTurns)
		//	ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
		//if (m_toolParams.m_optimizeVis)
		//	ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
		//if (m_toolParams.m_optimizeTopo)
		//	ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
		//if (m_toolParams.m_obstacleAvoidance)
		//	ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
		//if (m_toolParams.m_separation)
		//	ap.updateFlags |= DT_CROWD_SEPARATION;
		//ap.obstacleAvoidanceType = (unsigned char)m_toolParams.m_obstacleAvoidanceType;
		//ap.separationWeight = m_toolParams.m_separationWeight;
		return m_crowd->addAgent(pos, &ap);
	}

	void Navigation::InitCrowd()
	{
		if (m_crowd)
		{
			m_crowd->init(1, m_agentRadius, m_navMesh);

			// 添加一个默认的agent，用于主角的移动
			AddAgent(Vector3::ZERO);
		}
	}

	void Navigation::StopCrowdMove( int nAgentIndex /*= 0*/ )
	{
		if (m_crowd)
		{
			m_crowd->resetMoveTarget(nAgentIndex);
		}
	}

	void Navigation::RemoveAgent( int nAgentIndex )
	{
		if (m_crowd)
		{
			m_crowd->removeAgent(nAgentIndex);
		}
	}

	//void Navigation::EnableFilterFlag( size_t nIndex )
	//{
	//	switch (nIndex)
	//	{
	//	case 0:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() | SAMPLE_POLYFLAGS_TYPE_0);
	//		}
	//		break;
	//	case 1:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() | SAMPLE_POLYFLAGS_TYPE_1);
	//		}
	//		break;
	//	case 2:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() | SAMPLE_POLYFLAGS_TYPE_2);
	//		}
	//		break;
	//	case 3:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() | SAMPLE_POLYFLAGS_TYPE_3);
	//		}
	//		break;
	//	case 4:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() | SAMPLE_POLYFLAGS_TYPE_4);
	//		}
	//		break;
	//	case 5:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() | SAMPLE_POLYFLAGS_TYPE_5);
	//		}
	//		break;
	//	}

	//	if (m_crowd)
	//	{
	//		dtQueryFilter* pFilter = const_cast<dtQueryFilter*>(m_crowd->getFilter());
	//		pFilter->setIncludeFlags(m_filter.getIncludeFlags());
	//	}
	//}

	//void Navigation::DisableFilterFlag( size_t nIndex )
	//{
	//	switch (nIndex)
	//	{
	//	case 0:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() & ~SAMPLE_POLYFLAGS_TYPE_0);
	//		}
	//		break;
	//	case 1:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() & ~SAMPLE_POLYFLAGS_TYPE_1);
	//		}
	//		break;
	//	case 2:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() & ~SAMPLE_POLYFLAGS_TYPE_2);
	//		}
	//		break;
	//	case 3:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() & ~SAMPLE_POLYFLAGS_TYPE_3);
	//		}
	//		break;
	//	case 4:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() & ~SAMPLE_POLYFLAGS_TYPE_4);
	//		}
	//		break;
	//	case 5:
	//		{
	//			m_filter.setIncludeFlags(m_filter.getIncludeFlags() & ~SAMPLE_POLYFLAGS_TYPE_5);
	//		}
	//		break;
	//	}

	//	if (m_crowd)
	//	{
	//		dtQueryFilter* pFilter = const_cast<dtQueryFilter*>(m_crowd->getFilter());
	//		pFilter->setIncludeFlags(m_filter.getIncludeFlags());
	//	}
	//}

	void Navigation::SetIncludeFlag( unsigned int nFlag )
	{
		m_filter.setIncludeFlags(nFlag);
		if (m_crowd)
		{
			dtQueryFilter* pFilter = const_cast<dtQueryFilter*>(m_crowd->getFilter());
			pFilter->setIncludeFlags(nFlag);
		}
	}

	void Navigation::SetExcludeFlag( unsigned int nFlag )
	{
		m_filter.setExcludeFlags(nFlag);
		if (m_crowd)
		{
			dtQueryFilter* pFilter = const_cast<dtQueryFilter*>(m_crowd->getFilter());
			pFilter->setExcludeFlags(nFlag);
		}
	}

	void Navigation::findStraightPath( const Vector3& startPos, const Vector3& endPos, float*& path, int& pathCount )
	{
		memcpy(m_spos, &startPos, sizeof(float)*3);
		memcpy(m_epos, &endPos, sizeof(float)*3);
		if (!m_navMesh)
			return;
		m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
		m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

		if (m_startRef && m_endRef)
		{
			m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
			m_nstraightPath = 0;
			if (m_npolys)
			{
				// In case of partial path, make sure the end point is clamped to the last polygon.
				float epos[3];
				dtVcopy(epos, m_epos);
				if (m_polys[m_npolys-1] != m_endRef)
					m_navQuery->closestPointOnPoly(m_polys[m_npolys-1], m_epos, epos);

				m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys,
					m_straightPath, m_straightPathFlags,
					m_straightPathPolys, &m_nstraightPath, MAX_POLYS);
			}
		}
		else
		{
			m_npolys = 0;
			m_nstraightPath = 0;
		}

		path = m_straightPath;
		pathCount = m_nstraightPath;
	}

	bool Navigation::rayCast( const Vector3& startPos, const Vector3& endPos, float& oLength, Vector3& hitPosition )
	{
		if (!m_navMesh)
		{
			return false;
		}

		memcpy(m_spos, &startPos, sizeof(float)*3);
		memcpy(m_epos, &endPos, sizeof(float)*3);

		m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);

		float fHit = FLT_MAX;
		float fNormal[3];

		m_navQuery->raycast(m_startRef, m_spos, m_epos, &m_filter, &fHit, fNormal, m_polys, &m_npolys, MAX_POLYS);

		if (fHit != FLT_MAX)
		{
			Ray rayHit = Ray(startPos, endPos - startPos);
			oLength = fHit;
			hitPosition = rayHit.getPoint(fHit);
			return true;
		} 
		else
		{
			return false;
		}
	}

}
