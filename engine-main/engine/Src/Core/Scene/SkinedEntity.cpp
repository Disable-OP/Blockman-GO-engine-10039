#include "Core.h"
#include "SkinedEntity.h"
#include "SceneManager.h"
#include "SceneNode.h"
#include "Camera.h"
#include "Render/Renderer.h"
#include "Render/ShaderProgramManager.h"
#include "Render/Material.h"
#include "Render/GPUBuffer.h"
#include "Object/Root.h"
#include "Anim/AABBKeyFrame.h"
#include "Anim/SRTTrack.h"
#include "Anim/Skeleton.h"
#include "Anim/AnimState.h"
#include "Anim/AnimBlender.h"
#include "Model/SkinnedMesh.h"
#include "Model/MeshManager.h"
#include "Anim/Bone.h"

namespace LORD
{
	int SkinedEntity::s_maxBoneCount = 38;
	static Vector4 sh_light_param[4] =
	{
		Vector4(0.2f, -0.5f, -0.7f, 1.0f),
		Vector4(0.8f,	 0.8f,  0.8f, 1.0f),
		Vector4(0.6f,  0.6f,  0.6f, 1.0f),
		Vector4(-0.2f, -0.5f,  0.7f, 1.0f)
	};

	SkinedEntity::SkinedEntity()
		: ModelEntity()
		, m_pSkeleton(NULL)
		, m_pAnimBlender(NULL)
		, m_boneCount(0)
		, m_boneMatRows(NULL)
		, m_pSkinedEvent(NULL)
		, m_pDefaultTexture(NULL)
	{
		m_bSuppertGPU = Root::Instance()->isSupportGPUSkin();
	}

	SkinedEntity::~SkinedEntity()
	{
		if (m_pSkinedEvent)
		{
			m_pSkinedEvent->RestoreEvent();
			m_pSkinedEvent = NULL;
		}
		LordSafeFreeVector(m_vertexCPUs);
		LordSafeDeleteVector(m_vertexBuffers);
		LordSafeFree(m_boneMatRows);
		LordSafeFreeVector(m_subMeshBoneMatRows);
	}

	// 重载设置Mesh
	void SkinedEntity::setMesh(Mesh* pMesh)
	{
		ModelEntity::setMesh(pMesh);

		LordAssert(pMesh);
		LordSafeFreeVector(m_subMeshBoneMatRows);

		// 分配数据
		if (m_pMesh->getVersion() >= 3)
		{
			m_subMeshBoneMatRows.resize(m_pMesh->getSubMeshNum(), NULL);
			for (int i = 0; i < m_pMesh->getSubMeshNum(); i++)
			{
				SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
				if (pSubMesh && pSubMesh->getBoneNum())
				{
					m_subMeshBoneMatRows[i] = LordAlloc(Vector4, pSubMesh->getBoneNum() * 3);
					memset(m_subMeshBoneMatRows[i], 0, sizeof(Vector4) * pSubMesh->getBoneNum() * 3);
				}
			}
		}
	}

	void SkinedEntity::setSkeleton(Skeleton* pSkeleton)
	{
		// if you need re use the skinned entity to render another skin mesh 
		// please delete this, and malloc a new skinedEntity.
		//LordAssert(m_pSkeleton == NULL);
		m_pSkeleton = pSkeleton;

		m_boneCount = m_pSkeleton->getBoneCount() + m_pSkeleton->getDummyCount();

		if (m_bSuppertGPU)
		{
			LordSafeFree(m_boneMatRows);
			m_boneMatRows = LordAlloc(Vector4, m_boneCount * 3);

			const Matrix4& mat = Matrix4::IDENTITY;
			for (size_t i = 0; i < m_boneCount; ++i)
			{
				m_boneMatRows[i * 3].set(mat.m00, mat.m10, mat.m20, mat.m30);
				m_boneMatRows[i * 3 + 1].set(mat.m01, mat.m11, mat.m21, mat.m31);
				m_boneMatRows[i * 3 + 2].set(mat.m02, mat.m12, mat.m22, mat.m32);
			}
		}
	}
	// 设置是否使用XRay
	void SkinedEntity::setUseXRay(bool isUseXRay, const Vector4* color)
	{
		m_isUseXRay = isUseXRay;
		if (color)
			m_xrayColor = *color;

		bool bUseXRay = SceneManager::Instance()->isUseXRay();
		if (bUseXRay && m_isUseXRay && !m_xrayRenderables.size())
		{
			SceneManager* pSceneManager = SceneManager::Instance();
			Renderer* pRenderer = Renderer::Instance();
			for (int i = 0; i < m_pMesh->getSubMeshNum(); i++)
			{
				SubMesh* pSubMesh = m_pMesh->getSubMesh(i); LordAssert(pSubMesh);

				if (m_renderType == RT_FOR_ACTOR)
				{
					if (m_bSuppertGPU)
					{
						Vector4* pBoneMatRaws = m_pMesh->getVersion() >= 3 ? m_subMeshBoneMatRows[i] : m_boneMatRows;

						ShaderProgram* pXrayshaderProgram = m_Material->getShaderProgram();
						Renderable*	pXrayRenderable = LordNew Renderable();
						Material* pXrayMaterial = LordNew Material();

						pXrayMaterial->setBlendState(m_Material->getBlendState()->getDesc());
						pXrayMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
						pXrayMaterial->setDepthState(m_Material->getDepthState()->getDesc());
						for (int j = 0; j < MAX_TEXTURE_SAMPLER; ++j)
						{
							pXrayMaterial->setSamplerState(j, m_Material->getSamplerState(j)->getDesc());
						}

						pXrayRenderable->setMaterial(pXrayMaterial);
						pXrayMaterial->setShaderProgram(pXrayshaderProgram);
						pXrayMaterial->beginShaderParams(3);
						pXrayMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
						pXrayMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3 /*s_maxBoneCount * 3*/);
						pXrayMaterial->setShaderParam("xrayColor", SPT_VEC4, &m_xrayColor);
						pXrayMaterial->endShaderParams();
						RenderInput* pXrayRenderInput = pRenderer->createRenderInput(pSubMesh->getVertexElements(), pXrayshaderProgram);
						pXrayRenderInput->bindGPUBuffers(pSubMesh->getVertexBuffer(), pSubMesh->getVertexStride(), pSubMesh->getIndexBuffer(), pSubMesh->getIndexStride());
						pXrayRenderable->setRenderInput(pXrayRenderInput);
						m_xrayRenderables.push_back(pXrayRenderable);
					}
				}
			}
		}
	}

	// 提交到渲染队列
	void SkinedEntity::updateRenderQueue(RenderQueue* pRenderqueue)
	{
		for (size_t i = 0; i < m_renderables.size(); i++)
		{
			pRenderqueue->addRenderable(m_renderables[i]);
		}

		if (m_isUseXRay)
		{
			for (size_t i = 0; i < m_xrayRenderables.size(); i++)
			{
				pRenderqueue->addRenderable(m_xrayRenderables[i]);
			}
		}
	}

	void SkinedEntity::createRenderable(bool isUIActor)
	{
		LordAssert(m_pMesh);
		LordAssert(m_pSkeleton);
		LordAssert(m_pSceneNode);
		LordAssert(!m_renderInputs.size());

		m_isUIActor = isUIActor;
		bool bRenderHighLevel = SceneManager::Instance()->isRenderActorHigh();
		bool bComputeFog = SceneManager::Instance()->isComputeActorFog();

		SceneManager* pSceneManager = SceneManager::Instance();
		Renderer* pRenderer = Renderer::Instance();

		RenderInput::VertexElementList cpuElm;
		RenderInput::VertexElement posElm, colorElm, texElm;
		posElm.semantic = RenderInput::VS_POSITION;
		posElm.pixFmt = PF_RGB32_FLOAT;
		colorElm.semantic = RenderInput::VS_COLOR;
		colorElm.pixFmt = PF_RGBA8_UNORM;
		texElm.semantic = RenderInput::VS_TEXCOORD;
		texElm.pixFmt = PF_RG32_FLOAT;
		cpuElm.push_back(posElm);
		cpuElm.push_back(colorElm);
		cpuElm.push_back(texElm);

		for (int i = 0; i < m_pMesh->getSubMeshNum(); i++)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i); LordAssert(pSubMesh);

			RenderInput*	pRenderInput = NULL;
			Renderable* pRenderable = LordNew Renderable();
			Material* pMaterial = LordNew Material();
			ShaderProgram*  shaderProgram = m_Material->getShaderProgram();
			pMaterial->setBlendState(m_Material->getBlendState()->getDesc());
			pMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
			pMaterial->setDepthState(m_Material->getDepthState()->getDesc());
			for (int j = 0; j < MAX_TEXTURE_SAMPLER; ++j)
			{
				pMaterial->setSamplerState(j, m_Material->getSamplerState(j)->getDesc());
			}

			pMaterial->setShaderProgram(shaderProgram);
			
			SamplerState::SamplerDesc sampler_desc;				// sampler
			sampler_desc.minFilter = SamplerState::FO_POINT;
			sampler_desc.magFilter = SamplerState::FO_POINT;
			sampler_desc.mipFilter = SamplerState::FO_LINEAR;
			pMaterial->setSamplerState(0, sampler_desc);
			pMaterial->setSamplerState(1, sampler_desc);

			pMaterial->activeTexture(0, pSubMesh->getDiffuseTexture());
			pMaterial->activeTexture(1, pSubMesh->getMasktexture());
			pRenderable->setMaterial(pMaterial);
			m_renderables.push_back(pRenderable);

			if (m_renderType == RT_FOR_SCENE)
			{
				if (m_bSuppertGPU)
				{
					pRenderInput = pRenderer->createRenderInput(pSubMesh->getVertexElements(), shaderProgram);
					pRenderInput->bindGPUBuffers(pSubMesh->getVertexBuffer(), pSubMesh->getVertexStride(), pSubMesh->getIndexBuffer(), pSubMesh->getIndexStride());
					Vector4* pBoneMatRaws = m_pMesh->getVersion() >= 3 ? m_subMeshBoneMatRows[i] : m_boneMatRows;
					pMaterial->beginShaderParams(7);
					pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
					pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
					pMaterial->setShaderParam("fogParam", SPT_VEC4, isUIActor ? (void*)&m_UIFogParam : (void*)pSceneManager->getFogParamVec4Ptr(), 3);
					pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
					pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
					pMaterial->setShaderParam("litParam", SPT_MAT4, isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 1);
					pMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3);
					pMaterial->endShaderParams();
				}
				else
				{
					generateVBForScene();
					pRenderInput = pRenderer->createRenderInput(cpuElm, shaderProgram);
					pRenderInput->bindGPUBuffers(m_vertexBuffers[i], sizeof(VF_SKIN_SCENE), pSubMesh->getIndexBuffer(), pSubMesh->getIndexStride());
					pMaterial->beginShaderParams(3);
					pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
					pMaterial->setShaderParam("UVScale", SPT_VEC2, (void*)&(m_UVAnimationPos));
					pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
					pMaterial->endShaderParams();
				}
			}
			else
			{
				if (m_bSuppertGPU)
				{
					pRenderInput = pRenderer->createRenderInput(pSubMesh->getVertexElements(), shaderProgram);
					pRenderInput->bindGPUBuffers(pSubMesh->getVertexBuffer(), pSubMesh->getVertexStride(), pSubMesh->getIndexBuffer(), pSubMesh->getIndexStride());
					Vector4* pBoneMatRaws = m_pMesh->getVersion() >= 3 ? m_subMeshBoneMatRows[i] : m_boneMatRows;
					if (m_renderType == RT_FOR_ACTOR_WRITEDEPTH)
					{
						pMaterial->beginShaderParams(2);
						pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
						pMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3);
						pMaterial->endShaderParams();
					}
					else if (m_renderType == RT_FOR_ACTOR_TRANSPARENT)
					{
						pMaterial->beginShaderParams(14);
						pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
						pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
						pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
						pMaterial->setShaderParam("fogParam", SPT_VEC4, isUIActor ? (void*)&m_UIFogParam : (void*)&(pSceneManager->getFogParam()));
						pMaterial->setShaderParam("cameraPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
						pMaterial->setShaderParam("mainLightDir", SPT_VEC3, (void*)pSceneManager->getMainLightDirPtr());
						pMaterial->setShaderParam("mainLightColor", SPT_VEC4, (void*)pSceneManager->getMainLightColorPtr());
						pMaterial->setShaderParam("subLightColor", SPT_VEC4, (void*)pSceneManager->getSubLightColorPtr());
						pMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3);
						pMaterial->setShaderParam("brightness", SPT_VEC4, (void*)&m_brightness);
						pMaterial->setShaderParam("fogColor", SPT_VEC4, (void*)&(pSceneManager->getFogColor()));
						pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
						pMaterial->setShaderParam("customColor", SPT_VEC4, (void*)&m_customColor);
						pMaterial->setShaderParam("alphaColor", SPT_VEC4, (void*)&m_alphaColor);
						pMaterial->endShaderParams();
					}
					else if (m_renderType == RT_FOR_ACTOR && bRenderHighLevel)
					{
						pMaterial->beginShaderParams(13);
						pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
						pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
						pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
						pMaterial->setShaderParam("fogParam", SPT_VEC4, isUIActor ? (void*)&m_UIFogParam : (void*)&(pSceneManager->getFogParam()));
						pMaterial->setShaderParam("cameraPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
						pMaterial->setShaderParam("mainLightDir", SPT_VEC3, (void*)pSceneManager->getMainLightDirPtr());
						pMaterial->setShaderParam("mainLightColor", SPT_VEC4, (void*)pSceneManager->getMainLightColorPtr());
						pMaterial->setShaderParam("subLightColor", SPT_VEC4, (void*)pSceneManager->getSubLightColorPtr());
						pMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3);
						pMaterial->setShaderParam("brightness", SPT_VEC4, (void*)&m_brightness);
						pMaterial->setShaderParam("fogColor", SPT_VEC4, (void*)&(pSceneManager->getFogColor()));
						pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
						pMaterial->setShaderParam("customColor", SPT_VEC4, (void*)&m_customColor);
						pMaterial->endShaderParams();
					}
					else
					{
						LordAssert(false);
					}
				}
				else
				{
					generateVBForActor();
					pRenderInput = pRenderer->createRenderInput(cpuElm, shaderProgram);
					pRenderInput->bindGPUBuffers(m_vertexBuffers[i], pSubMesh->getVertexStride(), pSubMesh->getIndexBuffer(), pSubMesh->getIndexStride());
					if (m_renderType == RT_FOR_ACTOR)
					{
						if (bComputeFog)
						{
							pMaterial->beginShaderParams(6);
							pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
							pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
							pMaterial->setShaderParam("fogParam", SPT_VEC4, isUIActor ? (void*)&m_UIFogParam : (void*)pSceneManager->getFogParamVec4Ptr(), 3);
							pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
							pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
							pMaterial->setShaderParam("litParam", SPT_MAT4, isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 1);
							pMaterial->endShaderParams();
						}
						else
						{
							pMaterial->beginShaderParams(6);
							pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
							pMaterial->setShaderParam("camPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
							pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
							pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
							pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
							pMaterial->setShaderParam("litParam", SPT_MAT4, isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 1);
							pMaterial->endShaderParams();
						}
					}
					else
					{
						pMaterial->beginShaderParams(6);
						pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
						pMaterial->setShaderParam("camPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
						pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
						pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
						pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
						pMaterial->setShaderParam("litParam", SPT_MAT4, isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 1);
						pMaterial->endShaderParams();
					}
				}
			}
			pRenderable->setRenderInput(pRenderInput);
			m_renderInputs.push_back(pRenderInput);
		}

		setUseXRay(false, NULL);
	}

	void SkinedEntity::generateVBForScene()
	{
		LordAssert(m_pMesh);
		// the mesh must be backUp.
		LordAssert(m_vertexBuffers.empty());
		LordAssert(m_vertexCPUs.empty());

		m_vertexCPUs.reserve(m_pMesh->getSubMeshNum());
		m_vertexBuffers.reserve(m_pMesh->getSubMeshNum());
		for (int i = 0; i < m_pMesh->getSubMeshNum(); i++)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
			LordAssert(pSubMesh->getVertices());

			ui32 vertexCount = pSubMesh->getVertexCount();
			ui32 vertexBufferSize = vertexCount * sizeof(VF_SKIN_SCENE);
			Byte* pVertexCPU = (Byte*)LordMalloc(vertexBufferSize);

			// ready need copy? yes, if no animblender seted. 
			Byte* srcVertes = pSubMesh->getVertices();
			Byte* dstVertes = pVertexCPU;
			Dword defaultColor = 0xFFFFFFFF;
			bool hasColor = pSubMesh->hasVertexUsage(RenderInput::VS_COLOR);
			bool hasNormal = pSubMesh->hasVertexUsage(RenderInput::VS_NORMAL);
			for (size_t i = 0; i < vertexCount; ++i)
			{
				memcpy(dstVertes, srcVertes, sizeof(Vector3)); // pos
				dstVertes += 12;
				srcVertes += hasNormal ? 24 : 12;
				if (hasColor)
				{
					memcpy(dstVertes, srcVertes, sizeof(Dword)); // color;
					dstVertes += 4;
					srcVertes += 4;
				}
				else
				{
					memcpy(dstVertes, &defaultColor, sizeof(Dword));
					dstVertes += 4;
				}
				memcpy(dstVertes, srcVertes, sizeof(Vector2));
				dstVertes += 8;
				srcVertes += 24; // texcoord 8 + blendID 4 + blendWeights 12
			}

			// create VBO
			Buffer vertBuff(vertexBufferSize, pVertexCPU);
			Dword usage = GPUBuffer::GBU_GPU_READ;
			if (Root::Instance()->isSupportGPUSkin())
				usage |= GPUBuffer::GBU_CPU_WRITE;

			GPUBuffer* pVertexBuffer = Renderer::Instance()->createVertexBuffer(usage, vertBuff);

			m_vertexCPUs.push_back(pVertexCPU);
			m_vertexBuffers.push_back(pVertexBuffer);
		}
	}

	void SkinedEntity::generateVBForActor()
	{
		LordAssert(m_pMesh);
		// the mesh must be backUp.
		LordAssert(m_vertexBuffers.empty());
		LordAssert(m_vertexCPUs.empty());

		m_vertexCPUs.reserve(m_pMesh->getSubMeshNum());
		m_vertexBuffers.reserve(m_pMesh->getSubMeshNum());
		for (int i = 0; i < m_pMesh->getSubMeshNum(); i++)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
			LordAssert(pSubMesh->getVertices());

			ui32 vertexCount = pSubMesh->getVertexCount();
			ui32 vertexBufferSize = vertexCount * sizeof(VF_SKIN_ACTOR_CPU);
			Byte* pVertexCPU = (Byte*)LordMalloc(vertexBufferSize);

			// ready need copy? yes, if no animblender seted. 
			Byte* srcVertes = pSubMesh->getVertices();
			Byte* dstVertes = pVertexCPU;
			Vector3 defaultNormal = Vector3::UNIT_Y;
			bool hasColor = pSubMesh->hasVertexUsage(RenderInput::VS_COLOR);
			bool hasNormal = pSubMesh->hasVertexUsage(RenderInput::VS_NORMAL);
			for (size_t i = 0; i < vertexCount; ++i)
			{
				memcpy(dstVertes, srcVertes, sizeof(Vector3)); // pos
				dstVertes += 12;
				srcVertes += 12;
				if (hasNormal)
				{
					memcpy(dstVertes, srcVertes, sizeof(Vector3)); // normal;
					dstVertes += 12;
					srcVertes += 12;
				}
				else
				{
					memcpy(dstVertes, &defaultNormal, sizeof(Vector3));
					dstVertes += 12;
				}
				if (hasColor)
					srcVertes += 4;
				memcpy(dstVertes, srcVertes, sizeof(Vector2));
				dstVertes += 8;
				srcVertes += 24; // texcoord 8 + blendID 4 + blendWeights 12
			}

			// create VBO
			Buffer vertBuff(vertexBufferSize, pVertexCPU);
			GPUBuffer* pVertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);

			m_vertexCPUs.push_back(pVertexCPU);
			m_vertexBuffers.push_back(pVertexBuffer);
		}
	}

	Box SkinedEntity::getLocalAABB() const
	{
		if (m_pAnimBlender)
		{
			AABBKeyFrame* pKeyFrame = (AABBKeyFrame*)m_pAnimBlender->getAnimState()->getCurrentAnimKeyFrame();
			Box ret = pKeyFrame->getBox();
			ret.unionBox(m_pMesh->getBox());
			return ret;
		}
		else
		{
			return m_pMesh->getBox();
		}
	}

	// 更新子模型用骨骼动画数据
	void SkinedEntity::refreshSubMeshBoneMatRows()
	{
		// 复制动画结果数据
		if (m_pMesh->getVersion() >= 3)
		{
			for (int i = 0; i < m_pMesh->getSubMeshNum(); i++)
			{
				SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
				for (uint j = 0; j < pSubMesh->getBoneNum(); j++)
				{
					uint srcIdx = pSubMesh->getBoneIdx(j);
					memcpy(&m_subMeshBoneMatRows[i][j * 3], &m_boneMatRows[srcIdx * 3], sizeof(Vector4) * 3);
				}
			}
		}
	}

	void SkinedEntity::update(const Matrix4* viewproj)
	{
		LordAssert(m_pSkeleton);
		LordAssert(m_pSceneNode);
		if (!m_pMesh)
			return;

		// this place dont update the animBlender.
		// only orgnization the render data.
		// you must scneeNode->update()
		// animBlender->frameMove()
		// then skinedEntity->frameMove() by yourself.
		Matrix4 matVP = viewproj ? *viewproj : SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
		m_matWVP = m_pSceneNode->getWorldMatrix() * matVP;

		if (m_bSuppertGPU)
		{
			if (m_pAnimBlender && m_pAnimBlender->getAnimState())
			{
				LordAssert(m_vertexBuffers.empty());
				LordAssert(m_vertexCPUs.empty());

				// only to modify matrixs
				for (uint i = 0; i < m_boneCount; ++i)
				{
					if (i < m_pSkeleton->getBoneCount())
					{
						const Matrix4& mat = m_pAnimBlender->getAnimMatrix(i);
						m_boneMatRows[i * 3].set(mat.m00, mat.m10, mat.m20, mat.m30);
						m_boneMatRows[i * 3 + 1].set(mat.m01, mat.m11, mat.m21, mat.m31);
						m_boneMatRows[i * 3 + 2].set(mat.m02, mat.m12, mat.m22, mat.m32);
					}
					else
					{
						Bone* pBone = m_pSkeleton->getBoneById(i);
						LordAssert(pBone->isDummy());
						int parentBoneId = pBone->getParentId();
						Bone* pParent = pBone->getParent();
						Quaternion ori = m_pAnimBlender->getAnimOrientation(parentBoneId) * pParent->getRotation();
						Vector3 position = m_pAnimBlender->getAnimOrientation(parentBoneId) * pParent->getPosition() + m_pAnimBlender->getAnimPosition(parentBoneId);
						Quaternion oriret = ori * pBone->getRotation();
						Vector3 posret = ori * pBone->getPosition() + position;

						Matrix4 mat;
						oriret.toMat4(mat);
						mat.translate(posret);
						m_boneMatRows[i * 3].set(mat.m00, mat.m10, mat.m20, mat.m30);
						m_boneMatRows[i * 3 + 1].set(mat.m01, mat.m11, mat.m21, mat.m31);
						m_boneMatRows[i * 3 + 2].set(mat.m02, mat.m12, mat.m22, mat.m32);
					}
				}
			}

			refreshSubMeshBoneMatRows();
		}
		else
		{
			if (m_pAnimBlender && m_pAnimBlender->getAnimState())
			{
				LordAssert(!m_vertexBuffers.empty());
				LordAssert(!m_vertexCPUs.empty());

				for (int i = 0; i < m_pMesh->getSubMeshNum(); i++)
				{
					SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
					ui32 vertexBufferSize = 0;
					uint vertexCount = pSubMesh->getVertexCount();

					if (m_renderType == RT_FOR_SCENE) // pos, color, texcoord
					{
						VF_SKIN_SCENE* pDest = (VF_SKIN_SCENE*)m_vertexCPUs[i];
						const SkinnedMesh::SkinnedMeshVertex* pSrc =
							(SkinnedMesh::SkinnedMeshVertex*)pSubMesh->getVertices();
						LordAssert(pSrc);
						const Matrix4* pMatrix = m_pAnimBlender->getAnimMatrixPtr();

						for (size_t vIndex = 0; vIndex < vertexCount; ++vIndex)
						{
							pDest->position.zero();
							for (ui32 k = 0; k < SkinnedMesh::MAX_BONE_WEIGHT_NUM; ++k)
							{
								uint idx = (uint)pSrc->blendIndices[k];
								pDest->position += pSrc->position * pMatrix[idx] * pSrc->blendWeights[k];
							}
							pDest++;
							pSrc++;
						}
						vertexBufferSize = pSubMesh->getVertexCount() * sizeof(VF_SKIN_SCENE);
					}
					else // pos, normal, texcoord
					{
						VF_SKIN_ACTOR_CPU* pDest = (VF_SKIN_ACTOR_CPU*)m_vertexCPUs[i];
						const SkinnedMesh::SkinnedMeshVertex* pSrc =
							(SkinnedMesh::SkinnedMeshVertex*)pSubMesh->getVertices();
						LordAssert(pSrc);
						const Matrix4* pMatrix = m_pAnimBlender->getAnimMatrixPtr();

						for (size_t vIndex = 0; vIndex < vertexCount; ++vIndex)
						{
							pDest->position.zero();
							pDest->normal.zero();
							for (ui32 k = 0; k < SkinnedMesh::MAX_BONE_WEIGHT_NUM; ++k)
							{
								uint idx = (uint)pSrc->blendIndices[k];
								pDest->position += pSrc->position * pMatrix[k] * pSrc->blendWeights[k];
								pDest->normal += pSrc->normal * pMatrix[k] * pSrc->blendWeights[k];
							}
							pDest++;
							pSrc++;
						}
						vertexBufferSize = pSubMesh->getVertexCount() * sizeof(VF_SKIN_ACTOR_CPU);
					}
					// update gpu buffer
					Buffer vbuff(vertexBufferSize, m_vertexCPUs[i]);
					m_vertexBuffers[i]->updateSubData(0, vbuff);
				}
			}//if(m_pAnimBlender)
		}//m_bSuppertGPU
	}

	void SkinedEntity::updateForCalcThread(bool isUIActor)
	{
		LordAssert(m_pSkeleton);
		LordAssert(m_pSceneNode);
		if (!m_pMesh)
			return;

		Matrix4 matVP;
		if (isUIActor)
		{
			matVP = SceneManager::Instance()->getGUICamera()->getViewProjMatrix();
		}
		else
		{
			matVP = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
		}

		m_matWVP = m_pSceneNode->getWorldMatrix() * matVP;

		if (m_pAnimBlender && m_pAnimBlender->getAnimState())
		{
			if (m_bSuppertGPU && m_renderType != RT_FOR_SCENE)
			{
				LordAssert(m_vertexBuffers.empty());
				LordAssert(m_vertexCPUs.empty());

				// only to modify matrixs
				for (uint i = 0; i < m_boneCount; ++i)
				{
					const Matrix4& mat = m_pAnimBlender->getAnimMatrix(i);
					m_boneMatRows[i * 3].set(mat.m00, mat.m10, mat.m20, mat.m30);
					m_boneMatRows[i * 3 + 1].set(mat.m01, mat.m11, mat.m21, mat.m31);
					m_boneMatRows[i * 3 + 2].set(mat.m02, mat.m12, mat.m22, mat.m32);
				}
			}
			else
			{
				if (!m_pSkinedEvent)
				{
					m_pSkinedEvent = LordNew SkinedEvent(this);
				}
				if (!m_pSkinedEvent->GetInCalcThread())
				{
					Root::Instance()->getCalcThread()->AddRequest(m_pSkinedEvent);
					m_pSkinedEvent->SetInCalcThread(true);
				}
			}
		}

		refreshSubMeshBoneMatRows();
	}

	void SkinedEntity::excludeCalcThread()
	{
		if (m_pSkinedEvent && m_pSkinedEvent->GetInCalcThread())
		{
			m_pSkinedEvent->RestoreEvent();
			m_pSkinedEvent->SetInCalcThread(false);
			m_pSkinedEvent = NULL;
		}
	}

	void SkinedEntity::setEffectTexture(Texture* pTexture, int effect)
	{
		SceneManager* pSceneManager = SceneManager::Instance();
		for (size_t i = 0; i < m_renderables.size(); i++)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
			Vector4* pBoneMatRaws = m_pMesh->getVersion() >= 3 ? m_subMeshBoneMatRows[i] : m_boneMatRows;
			Material* pMaterial = m_renderables[i]->getMaterial();
			pMaterial->setBlendState(m_Material->getBlendState()->getDesc());
			pMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
			pMaterial->setDepthState(m_Material->getDepthState()->getDesc());
			pMaterial->setSamplerState(0, m_Material->getSamplerState(0)->getDesc());
			pMaterial->setSamplerState(1, m_Material->getSamplerState(1)->getDesc());
			pMaterial->setSamplerState(2, m_Material->getSamplerState(2)->getDesc());

			pMaterial->setShaderProgram(m_Material->getShaderProgram());
			pMaterial->beginShaderParams(12);
			pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
			pMaterial->setShaderParam("texMaskSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
			pMaterial->setShaderParam("texSampler2", SPT_TEXTURE, (void*)&TEXTURE_UNIT2);
			pMaterial->setShaderParam("darkRadio", SPT_FLOAT, (void*)&m_RenderParam.y);
			pMaterial->setShaderParam("camPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
			pMaterial->setShaderParam("ambientColor", SPT_VEC4, (void*)&m_ambientColor);
			pMaterial->setShaderParam("colorParams", SPT_VEC4, (void*)&m_colorParams);
			pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
			pMaterial->setShaderParam("litParam", SPT_MAT4, m_isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 1);
			pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
			pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
			pMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3);
			pMaterial->endShaderParams();
			pMaterial->activeTexture(2, pTexture);
		}
	}

	void SkinedEntity::revertEffectTexture()
	{
		SceneManager* pSceneManager = SceneManager::Instance();
		for (size_t i = 0; i < m_renderables.size(); i++)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
			Vector4* pBoneMatRaws = m_pMesh->getVersion() >= 3 ? m_subMeshBoneMatRows[i] : m_boneMatRows;
			Material* pMaterial = m_renderables[i]->getMaterial();
			pMaterial->setBlendState(m_Material->getBlendState()->getDesc());
			pMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
			pMaterial->setDepthState(m_Material->getDepthState()->getDesc());
			pMaterial->setSamplerState(0, m_Material->getSamplerState(0)->getDesc());
			pMaterial->setSamplerState(1, m_Material->getSamplerState(1)->getDesc());

			pMaterial->setShaderProgram(m_Material->getShaderProgram());

			if (m_renderType == RT_FOR_ACTOR_TRANSPARENT)
			{
				pMaterial->beginShaderParams(7);
				pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
				pMaterial->setShaderParam("ambientColor", SPT_VEC4, (void*)&m_ambientColor);
				pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
				pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
				pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
				pMaterial->setShaderParam("litParam", SPT_MAT4, m_isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 1);
				pMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3);
				pMaterial->endShaderParams();
			}
			else
			{
				pMaterial->beginShaderParams(11);
				pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
				pMaterial->setShaderParam("texMaskSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
				pMaterial->setShaderParam("darkRadio", SPT_FLOAT, (void*)&m_RenderParam.y);
				pMaterial->setShaderParam("camPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
				pMaterial->setShaderParam("ambientColor", SPT_VEC4, (void*)&m_ambientColor);
				pMaterial->setShaderParam("colorParams", SPT_VEC4, (void*)&m_colorParams);
				pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
				pMaterial->setShaderParam("litParam", SPT_MAT4, m_isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 1);
				pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
				pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
				pMaterial->setShaderParam("boneMatRows", SPT_VEC4, pBoneMatRaws, pSubMesh->getBoneNum() * 3);
				pMaterial->endShaderParams();
			}
		}
	}

	void SkinedEntity::setAnimBlender(AnimBlender* pAnimation)
	{
		// only one animBlender
		m_pAnimBlender = pAnimation;
	}

	void SkinedEntity::changeDefaultTexture(Texture* pTexture)
	{
		if (m_pDefaultTexture)
		{
			TextureManager::Instance()->releaseResource(m_pDefaultTexture);
			m_pDefaultTexture = NULL;
		}

		m_pDefaultTexture = pTexture;
		m_pDefaultTexture->addRefrenceCount();
		for (ui32 i = 0; i < m_renderables.size(); i++)
		{
			m_renderables[i]->getMaterial()->activeTexture(0, pTexture);
		}
	}

	void SkinedEntity::revertDefaultTexture()
	{
		if (m_pDefaultTexture)
		{
			TextureManager::Instance()->releaseResource(m_pDefaultTexture);
			m_pDefaultTexture = NULL;
		}
		for (ui32 i = 0; i < m_renderables.size(); i++)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
			m_renderables[i]->getMaterial()->activeTexture(0, pSubMesh->getDiffuseTexture());
		}
	}

	void SkinedEntity::replaceTexture(Texture* pTexture, size_t subid, ui32 uint)
	{
		if (!MeshManager::Instance()->validResource(m_pMesh))
		{
			TextureManager::Instance()->releaseResource(pTexture);
			return;
		}
		else
		{
			m_pMesh->getSubMesh(subid)->setTexture(uint, pTexture);
			LordAssert(m_renderables[subid]);
			Material* pMaterial = m_renderables[subid]->getMaterial();
			pMaterial->activeTexture(uint, pTexture);
		}
	}

	////---------------------------------------SkinedEvent--------------------------------------
	SkinedEvent::SkinedEvent(SkinedEntity* pSkinedEntity)
		: m_pSkinedEntity(pSkinedEntity)
		, m_InCalcThread(false)
	{
	}

	SkinedEvent::~SkinedEvent()
	{
		m_pSkinedEntity = NULL;
	}

	bool SkinedEvent::ProcessEvent()
	{
		if (!m_pSkinedEntity)
			return false;

		for (int i = 0; i < m_pSkinedEntity->m_pMesh->getSubMeshNum(); i++)
		{
			SubMesh* pSubMesh = m_pSkinedEntity->m_pMesh->getSubMesh(i); LordAssert(pSubMesh);
			size_t vertexCount = pSubMesh->getVertexCount();

			SkinedEntity::VF_SKIN_ACTOR_CPU* pDest = (SkinedEntity::VF_SKIN_ACTOR_CPU*)m_pSkinedEntity->m_vertexCPUs[i];
			const SkinnedMesh::SkinnedMeshVertex* pSrc =
				(SkinnedMesh::SkinnedMeshVertex*)pSubMesh->getVertices();
			LordAssert(pSrc);
			const Matrix4* pMatrix = m_pSkinedEntity->m_pAnimBlender->getAnimMatrixPtr();

			for (size_t vIndex = 0; vIndex < vertexCount; ++vIndex)
			{
				pDest->position.zero();
				pDest->normal.zero();
				for (ui32 k = 0; k < SkinnedMesh::MAX_BONE_WEIGHT_NUM; ++k)
				{
					uint idx = (uint)pSrc->blendIndices[k];
					pDest->position += pSrc->position * pMatrix[k] * pSrc->blendWeights[k];
					pDest->normal += pSrc->normal * pMatrix[k] * pSrc->blendWeights[k];
				}
				pDest++;
				pSrc++;
			}
		}

		return true;
	}

	bool SkinedEvent::RespondEvent()
	{
		if (m_pSkinedEntity)
		{
			// update gpu buffer
			for (int i = 0; i < m_pSkinedEntity->m_pMesh->getSubMeshNum(); i++)
			{
				SubMesh* pSubMesh = m_pSkinedEntity->m_pMesh->getSubMesh(i); LordAssert(pSubMesh);
				Buffer vbuff(pSubMesh->getVertexCount() * sizeof(SkinedEntity::VF_SKIN_ACTOR_CPU), m_pSkinedEntity->m_vertexCPUs[i]);
				m_pSkinedEntity->m_vertexBuffers[i]->updateSubData(0, vbuff);
			}

			return true;
		}
		return false;
	}

	void SkinedEvent::SetInCalcThread(bool inThread)
	{
		m_InCalcThread = inThread;
	}

	bool SkinedEvent::GetInCalcThread() const
	{
		return m_InCalcThread;
	}

	void SkinedEvent::RestoreEvent()
	{
		m_pSkinedEntity = NULL;
	}

}