#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "Core.h"
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

class dtNavMesh;
class dtNavMeshQuery;
class dtCrowd;
struct rcHeightfield;
struct rcCompactHeightfield;
struct rcContourSet;
struct rcPolyMesh;	
struct rcPolyMeshDetail;

namespace LORD
{
	class DataStream;
	class InputGeometryData;

	/// These are just sample areas to use consistent values across the samples.
	/// The use should specify these base on his needs.
	enum SamplePolyAreas
	{
		SAMPLE_POLYAREA_GROUND,
		SAMPLE_POLYAREA_WATER,
		SAMPLE_POLYAREA_ROAD,
		SAMPLE_POLYAREA_DOOR,
		SAMPLE_POLYAREA_GRASS,
		SAMPLE_POLYAREA_JUMP,
		SAMPLE_POLYAREA_TYPE_0,
		SAMPLE_POLYAREA_TYPE_1,
		SAMPLE_POLYAREA_TYPE_2,
		SAMPLE_POLYAREA_TYPE_3,
		SAMPLE_POLYAREA_TYPE_4,
		SAMPLE_POLYAREA_TYPE_5,
	};
	enum SamplePolyFlags
	{
		SAMPLE_POLYFLAGS_WALK		= 0x01,		// Ability to walk (ground, grass, road)
		SAMPLE_POLYFLAGS_SWIM		= 0x02,		// Ability to swim (water).
		SAMPLE_POLYFLAGS_DOOR		= 0x04,		// Ability to move through doors.
		SAMPLE_POLYFLAGS_JUMP		= 0x08,		// Ability to jump.
		SAMPLE_POLYFLAGS_TYPE_0		= 0x10,
		SAMPLE_POLYFLAGS_TYPE_1		= 0x20,
		SAMPLE_POLYFLAGS_TYPE_2		= 0x40,
		SAMPLE_POLYFLAGS_TYPE_3		= 0x80,
		SAMPLE_POLYFLAGS_TYPE_4		= 0x100,
		SAMPLE_POLYFLAGS_TYPE_5		= 0x200,
		SAMPLE_POLYFLAGS_DISABLED	= 0x400,	// Disabled polygon
		SAMPLE_POLYFLAGS_ALL		= 0xffff	// All abilities.
	};

	class LORD_CORE_API Navigation: public ObjectAlloc
	{
	public:
		Navigation();

		~Navigation();


		bool	buildNavigationMesh();

		void	cleanupNavigationMesh();
	
		void	setInputGeometryData(InputGeometryData* inputData) { m_InputGeometryData = inputData; }

		const dtNavMesh* getNavMesh() const { return m_navMesh; }

		const dtNavMeshQuery*	getNavMeshQuery() const { return m_navQuery; }

		// build navigation mesh from imort data
		void	importData(DataStream* pStream);

		// export data after build navigation mesh
		void	exportData(DataStream* pStream);

		void	findPath(const Vector3& startPos, const Vector3& endPos, float*& path, int& pathCount);

		void	findStraightPath(const Vector3& startPos, const Vector3& endPos, float*& path, int& pathCount);

		bool	rayCast(const Vector3& startPos, const Vector3& endPos, float& oLength, Vector3& hitPosition);

		/**
		 * 射线检测(求射线与导航模型相交最近点)
		 * 参数[in]		startPos	射线起点
		 * 参数[in]		dir			射线朝向(务必单位化)
		 * 参数[in out] oLenght		射线长度(探测长度越短，效率越高)
		 * 参数[out]    oHitPosition相交结果(最近点)
		 */ 
		bool rayDetect( const Vector3& startPos, const Vector3& dir, float& oLength, Vector3& oHitPosition) const;

		void	updateCrowd(ui32 nElapes);

		// 默认是主角
		void	MoveCrowdToword(const Vector3& dir, int nAgentIndex = 0);

		void	StopCrowdMove(int nAgentIndex = 0);

		Vector3 GetCrowdPosition(int nAgentIndex = 0);

		int		AddAgent(const Vector3& position);

		void	RemoveAgent(int nAgentIndex);

		//// 设置寻路的标志位, 目前nIndex 0 ～ 5
		//void	EnableFilterFlag(size_t nIndex);

		//void	DisableFilterFlag(size_t nIndex);
		void	SetIncludeFlag(unsigned int nFlag);

		void	SetExcludeFlag(unsigned int nFlag);

	protected:

		void	resetCommonSettings();

		void	InitCrowd();

	private:

		dtNavMesh*					m_navMesh;
		dtNavMeshQuery*				m_navQuery;
		dtCrowd*					m_crowd;

		float						m_cellSize;
		float						m_cellHeight;
		float						m_agentHeight;
		float						m_agentRadius;
		float						m_agentMaxClimb;
		float						m_agentMaxSlope;
		float						m_regionMinSize;
		float						m_regionMergeSize;
		bool						m_monotonePartitioning;
		float						m_edgeMaxLen;
		float						m_edgeMaxError;
		float						m_vertsPerPoly;
		float						m_detailSampleDist;
		float						m_detailSampleMaxError;

		unsigned char*				m_triareas;
		rcHeightfield*				m_solid;
		rcCompactHeightfield*		m_chf;
		rcContourSet*				m_cset;
		rcPolyMesh*					m_pmesh;
		rcPolyMeshDetail*			m_dmesh;

		rcContext					m_ctx;

		// input geometry data to build
		InputGeometryData*			m_InputGeometryData;

		// data after build
		unsigned char*				m_navData;
		int							m_navDataSize;

		// path finder data
		static const int MAX_POLYS = 256;
		static const int MAX_SMOOTH = 2048;

		dtPolyRef					m_startRef;
		dtPolyRef					m_endRef;
		dtPolyRef					m_polys[MAX_POLYS];
		int							m_npolys;

		float						m_polyPickExt[3];
		float						m_smoothPath[MAX_SMOOTH*3];
		int							m_nsmoothPath;

		float						m_spos[3];
		float						m_epos[3];
		dtQueryFilter				m_filter;

		// straitpath
		float						m_straightPath[MAX_POLYS*3];
		unsigned char				m_straightPathFlags[MAX_POLYS];
		dtPolyRef					m_straightPathPolys[MAX_POLYS];
		int							m_nstraightPath;
	};

}

#endif