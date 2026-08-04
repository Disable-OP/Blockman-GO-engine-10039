#include "DecalProjector.h"
#include "SceneManager.h"
#include "TerrainMeshObject.h"
#include "TerrainEntity.h"
#include "Render/OceanRender.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"
#include "Camera.h"
#include "Geom/Box3.h"
#include "Geom/Triangle.h"

namespace LORD
{	
	void GetProjection( const Vector3& rkAxis, const Box3& rkBox, float& fMin, float& fMax)
	{
		float fOrigin = rkAxis.dot(rkBox.m_center);

		float fMaximumExtent =
			Math::Abs(rkBox.m_extent[0] * rkAxis.dot(rkBox.m_axis[0])) +
			Math::Abs(rkBox.m_extent[1] * rkAxis.dot(rkBox.m_axis[1])) +
			Math::Abs(rkBox.m_extent[2] * rkAxis.dot(rkBox.m_axis[2]));

		fMin = fOrigin - fMaximumExtent;
		fMax = fOrigin + fMaximumExtent;
	}

	void GetProjection( const Vector3& rkAxis, const Triangle& rkTriangle, float& fMin, float& fMax)
	{
		float afDot[3] = 
		{
			rkAxis.dot(rkTriangle.v0),
			rkAxis.dot(rkTriangle.v1),
			rkAxis.dot(rkTriangle.v2)
		};

		fMin = afDot[0];
		fMax = fMin;

		if( afDot[1] < fMin)
			fMin = afDot[1];
		else if( afDot[1] > fMax)
			fMax = afDot[1];

		if( afDot[2] < fMin)
			fMin = afDot[2];
		else if( afDot[2] > fMax)
			fMax = afDot[2];
	}

	void ClipConvexPolygonAgainstPlane( const Vector3& rkNormal, float fConstant, int& rQuantity, Vector3* akp)
	{
		int iPositive = 0, iNegative = 0, iPIndex = -1;
		int iQuantity = rQuantity;

		float afTest[8];
		for ( int i=0; i<rQuantity; i++)
		{
			afTest[i] = rkNormal.dot(akp[i]) - fConstant + Math::Abs( fConstant * Math::EPSILON);

			if( afTest[i] >= 0.f)
			{
				iPositive++;
				if( iPIndex < 0)
				{
					iPIndex = i;
				}
			}
			else
			{
				iNegative++;
			}
		}

		if( rQuantity == 2)
		{
			if( iPositive > 0)
			{
				if( iNegative > 0)
				{
					int iClip;

					if( iPIndex == 0)
					{
						iClip = 1;
					}
					else	
					{
						iClip = 0;
					}

					float fT = afTest[ iPIndex] / ( afTest[iPIndex] - afTest[iClip]);
					akp[iClip] = akp[iPIndex] + fT * ( akp[iClip] - akp[iPIndex]);
				}
			}
			else
			{
				rQuantity = 0;
			}
		}
		else
		{
			if( iPositive > 0)
			{
				if( iNegative > 0)
				{
					Vector3 akCV[8];
					int iCQuantity = 0, iCur, iPrv;
					float fT;

					if( iPIndex > 0)
					{
						iCur = iPIndex;
						iPrv = iCur - 1;
						fT   = afTest[iCur] / ( afTest[iCur] - afTest[iPrv]);
						akCV[iCQuantity++] = akp[iCur] + fT * (akp[iPrv] - akp[iCur]);

						while ( iCur < iQuantity && afTest[iCur] >= 0.f)
						{
							akCV[iCQuantity++] = akp[iCur++];
						}

						if( iCur < iQuantity)
						{
							iPrv = iCur - 1;
						}
						else
						{
							iCur = 0;
							iPrv = iQuantity - 1;
						}

						fT = afTest[iCur] / (afTest[iCur]-afTest[iPrv]);
						akCV[iCQuantity++] = akp[iCur] + fT * (akp[iPrv] - akp[iCur]);
					}
					else
					{
						iCur = 0;
						while( iCur < iQuantity && afTest[iCur] >= 0.f)
						{
							akCV[iCQuantity++] = akp[iCur++];
						}

						iPrv = iCur - 1;
						fT   = afTest[iCur] / (afTest[iCur] - afTest[iPrv]);
						akCV[iCQuantity++] = akp[iCur] + fT * (akp[iPrv] - akp[iCur]);

						while( iCur < iQuantity && afTest[iCur] < 0.f)
						{
							iCur++;
						}
						if( iCur < iQuantity)
						{
							iPrv = iCur - 1;
							fT = afTest[iCur] / (afTest[iCur] - afTest[iPrv]);
							akCV[iCQuantity++] = akp[iCur] + fT * ( akp[iPrv] - akp[iCur]);

							while( iCur < iQuantity && afTest[iCur] >= 0.f)
							{
								akCV[iCQuantity++] = akp[iCur++];
							}
						}
						else
						{
							iPrv = iQuantity - 1;
							fT = afTest[0] / ( afTest[0]-afTest[iPrv]);
							akCV[iCQuantity++] = akp[0] + fT*( akp[iPrv] - akp[0]);
						}
					}

					iQuantity = iCQuantity;
					memcpy( akp, akCV, iCQuantity * sizeof( Vector3));
				}

				rQuantity = iQuantity;
			}
			else
			{
				rQuantity = 0;
			}
		}
	}

	bool Test(const Box3& box, const Triangle& triangle)
	{
		float   fMin0, fMax0, fMin1, fMax1;
		Vector3 kD, akE[3];

		// Test direction of triangle normal
		kD    = triangle.GetNormal();
		fMin0 = kD.dot(triangle.v0);
		fMax0 = fMin0;
		GetProjection( kD, box, fMin1, fMax1);
		if( fMax1 < fMin0 || fMax0 < fMin1)
		{
			return false;
		}

		// Test direction of box faces
		for ( int i=0; i<3; i++)
		{
			kD = box.m_axis[i];
			GetProjection( kD, triangle, fMin0, fMax0);
			float fDdC = kD.dot(box.m_center);
			fMin1 = fDdC - box.m_extent[i];
			fMax1 = fDdC + box.m_extent[i];
			if( fMax1 < fMin0 || fMax0 < fMin1)
			{
				return false;
			}
		}

		// Test direction of triangle-box edge cross products
		akE[2] = akE[1] - akE[0];
		for ( int i0=0; i0<3; i0++)
		{
			for ( int i1=0; i1<3; i1++)
			{
				kD = akE[i0].cross(box.m_axis[i1]);
				GetProjection( kD, triangle, fMin0, fMax0);
				GetProjection( kD, box, fMin1, fMax1);
				if( fMax1 < fMin0 || fMax0 < fMin1)
				{
					return false;
				}
			}
		}
		return true;
	}

	bool Find(const Box3& box, const Triangle& triangle, Vector3* akPoint, int& quantity)
	{
		// start with the triangle and clip it against each face of the box
		quantity = 3;

		akPoint[0] = triangle.v0;
		akPoint[1] = triangle.v1;
		akPoint[2] = triangle.v2;

		for ( int iDir=-1; iDir<=1; iDir+=2)
		{
			for( int iSide=0; iSide<3; iSide++)
			{
				Vector3 kInnerNormal = ((float)iDir) * box.m_axis[iSide];
				float fConstant = kInnerNormal.dot(box.m_center) - box.m_extent[iSide];

				ClipConvexPolygonAgainstPlane( kInnerNormal, fConstant, quantity, akPoint);
			}
		}

		return quantity > 0 ? true : false;
	}

	void Intersection(vector<Vector3>::type& vertexs, vector<Word>::type& indices, const Box3& box3, const Triangle& triangle)
	{
		vertexs.clear();
		indices.clear();
		if( Test(box3, triangle))
		{
			int quantity;
			Vector3 akPoint[7];
			Find(box3, triangle, akPoint, quantity);
			for( int i=0; i<quantity; i++)
				vertexs.push_back(akPoint[i]);

			for ( int i=2; i<quantity; i++)
			{
				indices.push_back( 0);
				indices.push_back( i-1);
				indices.push_back( i);
			}
		}
	}

	const size_t MAX_STATIC_DECAL_VERTEX_COUNT = 1000;

	DecalProjector::DecalProjector()
		: m_pTerrainObject(NULL)
		, m_pOceanRender(NULL)
		, m_pTexture(NULL)
		, m_Color(Color::WHITE)
		, m_Radius(1.0f)
		, m_VertexGpuBuf(NULL)
		, m_IndexGpuBuf(NULL)
		, m_pDecalEnt(NULL)
	{
	}
	//-----------------------------------------------------------------------
	DecalProjector::~DecalProjector()
	{
		_unprepare();
	}
	//-----------------------------------------------------------------------
	void DecalProjector::setTexture(const String& name)
	{
		if (m_pTexture)
		{
			TextureManager::Instance()->releaseResource(m_pTexture);
		}
		m_pTexture = TextureManager::Instance()->createTexture(name);
		m_pTexture->load();
		if (m_pDecalEnt)
		{
			Material* pMaterial = m_pDecalEnt->getMaterial();
			pMaterial->activeTexture(0, m_pTexture);
		}
	}
	//-----------------------------------------------------------------------
	bool DecalProjector::UpdateDecalData()
	{
		PopulateForMesh();
		return true;
	}
	//-----------------------------------------------------------------------
	void DecalProjector::_updateRenderQueue()
	{
		if (m_pDecalEnt && m_pDecalEnt->getRenderInput())
		{
			SceneManager::Instance()->getRenderQueue("Scene")->addRenderable(m_pDecalEnt);
		}
	}

	//-----------------------------------------------------------------------
	bool DecalProjector::PopulateForMesh(void)
	{		
		// 查询模型，框框查询
		ClearRenderableList();
		
		AddSMeshRenderable();

		return true;
	}

	void DecalProjector::setColor( const Color& color )
	{
		m_Color = color;
	}

	void DecalProjector::ClearRenderableList()
	{
		if (m_pDecalEnt)
		{
			m_pDecalEnt->unprepare();
		}
	}
	
	void DecalProjector::AddSMeshRenderable()
	{
		if (!m_pDecalEnt)
		{
			m_pDecalEnt = LordNew DecalEntity;
			m_pDecalEnt->getMaterial()->activeTexture(0, m_pTexture);
		}

		vector<Triangle>::type TempTriangle;
		Camera* pCamera = SceneManager::Instance()->getMainCamera();
		if (m_pTerrainObject)
		{
			int entNum = m_pTerrainObject->getEntityNum();
			if (entNum == 0)
				return;

			m_pDecalEnt->m_WVPMat = m_pTerrainObject->m_SceneNode->getWorldMatrix() * pCamera->getViewProjMatrix();
			for (int i = 0; i < entNum; ++i)
			{
				TerrainEntity* pEnt = m_pTerrainObject->getEntity(i);
				size_t vertexSize = pEnt->getVertexStride() / sizeof(float);
				Vector3	temp0, temp1, temp2;
				const float* vertexPtr = static_cast<float*>((void*)pEnt->getVertices());
				const ui16*	indexPtr = static_cast<ui16*>((void*)pEnt->getIndices());
				ui16 indexNum = pEnt->getIndexCount();

				for (ui16 j = 0; j < indexNum; j += 3)
				{
					temp0 = *(LORD::Vector3*)(vertexPtr + indexPtr[j] * vertexSize);
					temp1 = *(LORD::Vector3*)(vertexPtr + indexPtr[j + 1] * vertexSize);
					temp2 = *(LORD::Vector3*)(vertexPtr + indexPtr[j + 2] * vertexSize);

					Triangle triangle(LORD::Vector3(temp0.x, temp0.y + 0.1f, temp0.z),
						LORD::Vector3(temp1.x, temp1.y + 0.1f, temp1.z),
						LORD::Vector3(temp2.x, temp2.y + 0.1f, temp2.z));
					TempTriangle.push_back(triangle);
				}
			}
		}
		else if (m_pOceanRender)
		{
			m_pDecalEnt->m_WVPMat = pCamera->getViewProjMatrix();

			Vector3	temp0, temp1, temp2;
			ui32 vertexSize = m_pOceanRender->getVertexStride() / sizeof(float);
			const float* vertexPtr = m_pOceanRender->getVertexBuffer();
			const ui16* indexPtr = m_pOceanRender->getIndexBuffer();
			ui32 indexNum = m_pOceanRender->getIndexCount();

			for (ui32 i = 0; i < indexNum; i+=3)
			{
				temp0 = *(LORD::Vector3*)(vertexPtr + indexPtr[i] * vertexSize);
				temp1 = *(LORD::Vector3*)(vertexPtr + indexPtr[i + 1] * vertexSize);
				temp2 = *(LORD::Vector3*)(vertexPtr + indexPtr[i + 2] * vertexSize);

				Triangle triangle(LORD::Vector3(temp0.x, temp0.y + 0.1f, temp0.z),
					LORD::Vector3(temp1.x, temp1.y + 0.1f, temp1.z),
					LORD::Vector3(temp2.x, temp2.y + 0.1f, temp2.z));
				TempTriangle.push_back(triangle);
			}
		}
		else
		{
			m_pDecalEnt->m_WVPMat = pCamera->getViewProjMatrix();
			Scene::TerrainBlockList blocklst = SceneManager::Instance()->getCurrentScene()->getTerrainBlockList();
			for (Scene::TerrainBlockList::iterator it = blocklst.begin(); it != blocklst.end(); ++it)
			{
				TerrainBlock* pBlock = *it;
				Box decalbox;
				decalbox.fromBSphere(Sphere(m_position, m_Radius));
				if (pBlock->getTerrainBlockAABB().isIntersected(decalbox))
				{
					Vector3	temp0, temp1, temp2;
					const float* vertexPtr = pBlock->editor_getVertexData();
					const Word*	indexPtr = pBlock->editor_getIndexData();
					ui16 indexNum = pBlock->editor_getIndexNum();
					ui32 vertexSize = pBlock->editor_getVertexStride() / sizeof(float);
					for (ui16 i = 0; i < indexNum; i += 3)
					{
						temp0 = *(LORD::Vector3*)(vertexPtr + indexPtr[i] * vertexSize);
						temp1 = *(LORD::Vector3*)(vertexPtr + indexPtr[i + 1] * vertexSize);
						temp2 = *(LORD::Vector3*)(vertexPtr + indexPtr[i + 2] * vertexSize);

						Triangle triangle(LORD::Vector3(temp0.x, temp0.y + 0.1f, temp0.z),
							LORD::Vector3(temp1.x, temp1.y + 0.1f, temp1.z),
							LORD::Vector3(temp2.x, temp2.y + 0.1f, temp2.z));
						TempTriangle.push_back(triangle);
					}
				}
			}
		}

		Box3 box3(m_position, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, m_Radius, 5.0f, m_Radius);

		vector<Vector3>::type vertices, vertices2;
		vector<Word>::type indices, indices2;
		for (size_t i = 0; i < TempTriangle.size(); ++i)
		{
			Intersection(vertices, indices, box3, TempTriangle[i]);
			for (size_t j = 0; j < indices.size(); j++)
				indices2.push_back(indices[j] + static_cast<Word>(vertices2.size()));

			for (size_t j = 0; j < vertices.size(); j++)
			{
				vertices2.push_back(vertices[j]);
			}
		}

		ui16 indexNum = (ui16)indices2.size();
		if (indexNum == 0)
			return;

		ui32 numVertices = (ui32)vertices2.size();
		float* temBuffer = static_cast<float*>(LordMalloc(sizeof(float) * 5 * numVertices));
		ui16* idxBuffer = static_cast<ui16*>(LordMalloc(sizeof(Word)*indexNum));
		float* lockPtr = temBuffer;
		float oneOverW = 1.0f / (m_Radius * 2);

		for (ui32 i = 0; i < (ui32)vertices2.size(); ++i)
		{
			Vector3 v = vertices2[i] - m_position;
			*lockPtr++ = vertices2[i].x;
			*lockPtr++ = vertices2[i].y;
			*lockPtr++ = vertices2[i].z;
			*lockPtr++ = v.dot(Vector3::UNIT_X) * oneOverW + 0.5f;
			*lockPtr++ = v.dot(Vector3::UNIT_Z) * oneOverW + 0.5f;
		}

		ui16* pIndex = idxBuffer;
		for (ui16 i = 0; i < (ui16)indices2.size(); ++i)
		{
			pIndex[i] = indices2[i];
		}

		LordSafeDelete(m_VertexGpuBuf);
		LordSafeDelete(m_IndexGpuBuf);
		Buffer vertBuff(sizeof(float) * 5 * numVertices, temBuffer);
		Buffer indexBuff(sizeof(Word)*indexNum, idxBuffer);
		m_VertexGpuBuf = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ, vertBuff);
		m_IndexGpuBuf = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ, indexBuff);

		m_pDecalEnt->createRenderable(m_VertexGpuBuf, numVertices, m_IndexGpuBuf, indexNum);

		LordSafeFree(idxBuffer);
		LordSafeFree(temBuffer);
	}

	void DecalProjector::setRadius( float radius )
	{
		m_Radius = radius;
	}

	void DecalProjector::_unprepare()
	{
		ClearRenderableList();
		LordSafeDelete(m_pDecalEnt);
		LordSafeDelete(m_VertexGpuBuf);
		LordSafeDelete(m_IndexGpuBuf);
		// 此纹理指针由外面射入，无须删除
		if (m_pTexture)
		{
			TextureManager::Instance()->releaseResource(m_pTexture);
		}
		m_pTexture = NULL;
	}

	void DecalProjector::setPosition( const Vector3& pos )
	{
		m_position = pos;
	}
		
	//--------------------------DecalEntity---------------------------------
	DecalEntity::DecalEntity()
		: Renderable()
		, m_WVPMat(Matrix4::IDENTITY)
	{
		m_Material = LordNew Material;

		BlendState::BlendDesc blenddesc = m_Material->getBlendState()->getDesc();
		blenddesc.bBlendEnable = true;
		blenddesc.srcBlend = BlendState::BF_SRC_ALPHA;
		blenddesc.dstBlend = BlendState::BF_ONE;
		blenddesc.srcAlphaBlend = BlendState::BF_ONE;
		blenddesc.dstAlphaBlend = BlendState::BF_ZERO;

		DepthStencilState::DepthStencilDesc depthdesc = m_Material->getDepthState()->getDesc();
		depthdesc.bWriteDepth = false;

		RasterizerState::RasterizerDesc rasdesc = m_Material->getRasterizerState()->getDesc();
		rasdesc.cullMode = RasterizerState::CULL_NONE;
		
		SamplerState::SamplerDesc samdesc = m_Material->getSamplerState(0)->getDesc();
		samdesc.minFilter = SamplerState::FO_LINEAR;
		samdesc.magFilter = SamplerState::FO_LINEAR;
		samdesc.mipFilter = SamplerState::FO_NONE;
		samdesc.addrUMode = SamplerState::AM_MIRROR;
		samdesc.addrVMode = SamplerState::AM_MIRROR;
		
		m_Material->setBlendState(blenddesc);
		m_Material->setDepthState(depthdesc);
		m_Material->setRasterizerState(rasdesc);
		m_Material->setSamplerState(0, samdesc);

		m_Material->setShaderProgram(ShaderProgramManager::Instance()->getShaderProgram("TerrainDecal"));
		m_Material->beginShaderParams(2);
		m_Material->setShaderParam("matWVP", SPT_MAT4, (void*)m_WVPMat.ptr());
		m_Material->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
		m_Material->endShaderParams();
	}

	void DecalEntity::createRenderable( GPUBuffer* vertexBuf, ui32 vertexCount, GPUBuffer* indexBuf, ui32 indexCount )
	{
		RenderInput::VertexElement posElem, uvElem;
		posElem.semantic = RenderInput::VS_POSITION;
		posElem.pixFmt = PF_RGB32_FLOAT;
		uvElem.semantic = RenderInput::VS_TEXCOORD;
		uvElem.pixFmt = PF_RG32_FLOAT;
		m_VertexElement.push_back(posElem);
		m_VertexElement.push_back(uvElem);

		Renderer* pRenderer = Renderer::Instance();
		m_RenderInput = pRenderer->createRenderInput(m_VertexElement, m_Material->getShaderProgram());
		m_RenderInput->bindGPUBuffers(vertexBuf, vertexCount*(sizeof(Vector3)+sizeof(Vector2)), indexBuf, indexCount);
	}

	DecalEntity::~DecalEntity()
	{
		unprepare();
		LordSafeDelete(m_Material);
	}

	void DecalEntity::unprepare()
	{
		LordSafeDelete(m_RenderInput);
		m_VertexElement.clear();
	}

} 
