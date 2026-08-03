#include "Core.h"
#include "ModelEntity.h"
#include "SceneManager.h"
#include "Render/RenderQueue.h"
#include "Render/RenderInput.h"
#include "Render/Renderer.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"
#include "Model/Mesh.h"
#include "Model/MeshManager.h"
#include "Scene/SceneNode.h"
#include "Scene/Camera.h"
#include "Object/Root.h"
#include "Tessolator/TessManager.h"

namespace LORD
{
	LORD::Vector4 ModelEntity::m_UIFogParam = Vector4::ZERO;

	static Vector4 sh_light_param[4] =
	{
		Vector4(0.2f, -0.5f, -0.7f, 1.0f),
		Vector4(0.8f,  0.8f,  0.8f, 1.0f),
		Vector4(0.6f,  0.6f,  0.6f, 1.0f),
		Vector4(-0.2f, -0.5f,  0.7f, 1.0f)
	};

	ModelEntity::ModelEntity()
		: m_pMesh(NULL)
		, m_pSceneNode(NULL)
		, m_renderType(RT_FOR_ACTOR)
		, m_naturalColor(Color::WHITE)
		, m_customColor(Color::WHITE)
		, m_alphaColor(Color::WHITE)
		, m_edgeColor(Color::WHITE)
		, m_ambientColor(Color::WHITE)
		, editor_Color(Color::YELLOW)
		, m_RenderParam(0.0f, 1.0f)
		, m_UVAnimationPos(Vector2::ZERO)
		, m_colorParams(Vector4::ONE)
		, m_xrayColor(0.f, 0.62f, 0.f, 0.5f)
		, editor_subId(-1)
		, isVisible(true)
		, isFadeOut(false)
		, m_isUseXRay(false)
		, m_isAlphaTest(false)
		, m_isUIActor(false)
		, m_Material(NULL)
		, m_brightness(Color::WHITE)
	{
	}

	ModelEntity::~ModelEntity()
	{
		LordSafeDeleteVector(m_renderInputs);

		for (size_t i = 0; i < m_renderables.size(); i++)
		{
			Material* pMaterial = m_renderables[i]->getMaterial();
			LordSafeDelete(pMaterial);
			LordSafeDelete(m_renderables[i]);
		}
		m_renderables.clear();

		for (size_t i = 0; i < m_xrayRenderables.size(); i++)
		{
			RenderInput* renderInput = m_xrayRenderables[i]->getRenderInput();
			Material* pMaterial = m_xrayRenderables[i]->getMaterial();
			LordSafeDelete(renderInput);
			LordSafeDelete(pMaterial);
			LordSafeDelete(m_xrayRenderables[i]);
		}

		m_xrayRenderables.clear();

		for (list<TextureLoadEvent*>::type::iterator it = m_TextureEventList.begin(); it != m_TextureEventList.end(); ++it)
		{
			(*it)->mEntity = NULL;
		}
	}

	void ModelEntity::setMaterial(Material* material)
	{
		m_Material = material;
	}

	// 设置是否使用XRay
	void ModelEntity::setUseXRay(bool isUseXRay, const Vector4* color)
	{
		LordAssert(m_Material);

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
					// XRayRenderable
					Material* pXrayMaterial = LordNew Material();
					Renderable* pXrayRenderable = LordNew Renderable();
					ShaderProgram* pXrayshaderProgram = m_Material->getShaderProgram();
					RenderInput*   pXrayRenderInput = pRenderer->createRenderInput(pSubMesh->getVertexElements(), pXrayshaderProgram);
					pXrayMaterial->setShaderProgram(pXrayshaderProgram);
					pXrayRenderable->setMaterial(pXrayMaterial);

					pXrayMaterial->setBlendState(m_Material->getBlendState()->getDesc());
					pXrayMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
					pXrayMaterial->setDepthState(m_Material->getDepthState()->getDesc());
					for (int j = 0; j < MAX_TEXTURE_SAMPLER; ++j)
					{
						pXrayMaterial->setSamplerState(j, m_Material->getSamplerState(j)->getDesc());
					}

					pXrayRenderInput->bindGPUBuffers(pSubMesh->getVertexBuffer(), pSubMesh->getVertexStride(), pSubMesh->getIndexBuffer(), pSubMesh->getIndexStride());
					pXrayMaterial->beginShaderParams(2);
					pXrayMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
					pXrayMaterial->setShaderParam("xrayColor", SPT_VEC4, &m_xrayColor);
					pXrayMaterial->endShaderParams();
					pXrayRenderable->setRenderInput(pXrayRenderInput);

					m_xrayRenderables.push_back(pXrayRenderable);
				}
			}
		}
	}

	void ModelEntity::createRenderable(bool isUIActor)
	{
		LordAssert(m_pSceneNode&&m_pMesh && (!m_renderInputs.size()));

		SceneManager* pSceneManager = SceneManager::Instance();
		Renderer*	  pRenderer = Renderer::Instance();
		ShaderProgram* shaderProgram = m_Material->getShaderProgram();

		int subNum = m_pMesh->getSubMeshNum();
		m_lightmapUV.reserve(subNum);
		m_lightmapUV.resize(subNum);
		m_lightmapScale.reserve(subNum);
		m_lightmapScale.resize(subNum);

		m_isUIActor = isUIActor;

		bool bRenderHighLevel = SceneManager::Instance()->isRenderActorHigh();
		bool bComputeFog = SceneManager::Instance()->isComputeActorFog();

		for (int i = 0; i < subNum; i++)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i); LordAssert(pSubMesh);

			Material*		pMaterial = LordNew Material();
			Renderable*		pRenderable = LordNew Renderable();
			RenderInput*	pRenderInput = pRenderer->createRenderInput(pSubMesh->getVertexElements(), shaderProgram);

			pMaterial->setBlendState(m_Material->getBlendState()->getDesc());
			pMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
			pMaterial->setDepthState(m_Material->getDepthState()->getDesc());
			for (int j = 0; j < MAX_TEXTURE_SAMPLER; ++j)
			{
				pMaterial->setSamplerState(j, m_Material->getSamplerState(j)->getDesc());
			}

			pRenderInput->bindGPUBuffers(pSubMesh->getVertexBuffer(), pSubMesh->getVertexStride(), pSubMesh->getIndexBuffer(), pSubMesh->getIndexStride());
			pRenderable->setMaterial(pMaterial);
			pRenderable->setRenderInput(pRenderInput);
			pMaterial->setShaderProgram(shaderProgram);
			SamplerState::SamplerDesc sampler_desc;				// sampler
			sampler_desc.minFilter = SamplerState::FO_POINT;
			sampler_desc.magFilter = SamplerState::FO_POINT;
			sampler_desc.mipFilter = SamplerState::FO_LINEAR;
			pMaterial->setSamplerState(0, sampler_desc);
			pMaterial->setSamplerState(1, sampler_desc);

			pMaterial->activeTexture(0, pSubMesh->getDiffuseTexture());
			pMaterial->activeTexture(1, pSubMesh->getMasktexture()); // 此时如果mesh是第一次加载，则mask贴图为BLANK贴图
			m_renderInputs.push_back(pRenderInput);
			m_renderables.push_back(pRenderable);
			if (m_renderType == RT_FOR_SCENE)
			{
				pMaterial->beginShaderParams(3);
				pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
				pMaterial->setShaderParam("UVScale", SPT_VEC2, (void*)&(m_UVAnimationPos));
				pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
				pMaterial->endShaderParams();
			}
			else if (m_renderType == RT_FOR_HAZE)
			{
				pMaterial->beginShaderParams(2);
				pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
				pMaterial->setShaderParam("alpha", SPT_FLOAT, (void*)&m_naturalColor.a);
				pMaterial->endShaderParams();
			}
			else
			{
				if (m_renderType == RT_FOR_SCENE_L)
				{
					pMaterial->beginShaderParams(10);
					pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
					pMaterial->setShaderParam("LMSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
					pMaterial->setShaderParam("factor", SPT_VEC2, (void*)&(m_RenderParam));
					pMaterial->setShaderParam("UVScale", SPT_VEC2, (void*)&(m_UVAnimationPos));
					pMaterial->setShaderParam("LMScale", SPT_VEC3, (void*)&(m_lightmapScale[i]));
					pMaterial->setShaderParam("LMUV", SPT_VEC4, (void*)&(m_lightmapUV[i]));
					pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
					pMaterial->setShaderParam("fogParam", SPT_VEC4, (void*)pSceneManager->getFogParamVec4Ptr(), 3);
					pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
					pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
					pMaterial->endShaderParams();
				}
				else if (m_renderType == RT_FOR_SCENE_L_NOFOG)
				{
					pMaterial->beginShaderParams(5);
					pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
					pMaterial->setShaderParam("factor", SPT_VEC2, (void*)&(m_RenderParam));
					pMaterial->setShaderParam("UVScale", SPT_VEC2, (void*)&(m_UVAnimationPos));
					pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
					pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
					pMaterial->endShaderParams();
				}
				else if (m_renderType == RT_FOR_ACTOR && bRenderHighLevel)
				{
					pMaterial->setShaderProgram(shaderProgram);
					pMaterial->beginShaderParams(12);
					pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
					pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
					pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
					pMaterial->setShaderParam("fogParam", SPT_VEC4, isUIActor ? (void*)&m_UIFogParam : (void*)&(pSceneManager->getFogParam()));
					pMaterial->setShaderParam("cameraPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
					pMaterial->setShaderParam("mainLightDir", SPT_VEC3, (void*)pSceneManager->getMainLightDirPtr());
					pMaterial->setShaderParam("mainLightColor", SPT_VEC4, (void*)pSceneManager->getMainLightColorPtr());
					pMaterial->setShaderParam("subLightColor", SPT_VEC4, (void*)pSceneManager->getSubLightColorPtr());
					pMaterial->setShaderParam("brightness", SPT_VEC4, (void*)&m_brightness);
					pMaterial->setShaderParam("fogColor", SPT_VEC4, (void*)&(pSceneManager->getFogColor()));
					pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
					pMaterial->setShaderParam("customColor", SPT_VEC4, (void*)&m_customColor);
					pMaterial->endShaderParams();
				}
				else if (m_renderType == RT_FOR_ACTOR_TRANSPARENT)
				{
					pMaterial->setShaderProgram(shaderProgram);
					pMaterial->beginShaderParams(13);
					pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
					pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
					pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
					pMaterial->setShaderParam("fogParam", SPT_VEC4, isUIActor ? (void*)&m_UIFogParam : (void*)&(pSceneManager->getFogParam()));
					pMaterial->setShaderParam("cameraPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
					pMaterial->setShaderParam("mainLightDir", SPT_VEC3, (void*)pSceneManager->getMainLightDirPtr());
					pMaterial->setShaderParam("mainLightColor", SPT_VEC4, (void*)pSceneManager->getMainLightColorPtr());
					pMaterial->setShaderParam("subLightColor", SPT_VEC4, (void*)pSceneManager->getSubLightColorPtr());
					pMaterial->setShaderParam("brightness", SPT_VEC4, (void*)&m_brightness);
					pMaterial->setShaderParam("fogColor", SPT_VEC4, (void*)&(pSceneManager->getFogColor()));
					pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
					pMaterial->setShaderParam("customColor", SPT_VEC4, (void*)&m_customColor);
					pMaterial->setShaderParam("alphaColor", SPT_VEC4, (void*)&m_alphaColor);
					pMaterial->endShaderParams();
				}
				else
				{
					LordAssert(false);
				}
			}
		}

		setUseXRay(false, NULL);
	}

	void ModelEntity::update(const Matrix4* viewproj)
	{
		LordAssert(m_pSceneNode);
		Matrix4 matVP;
		if (viewproj)
		{
			matVP = *viewproj;
		}
		else
		{
			matVP = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
		}

		m_matWVP = m_pSceneNode->getWorldMatrix() * matVP;
	}

	void ModelEntity::update2()
	{
		const Matrix4& viewBobing = TessManager::Instance()->getViewBobing();
		Camera* camera = SceneManager::Instance()->getMainCamera();
		const Matrix4& projMat = camera->getProjMatrix2();
		m_matWVP = viewBobing * m_pSceneNode->getWorldMatrix() * projMat;
	}

	Real ModelEntity::getViewDepth(Camera* pCamera) const
	{
		LordAssert(pCamera);

		const Matrix4& matView = pCamera->getViewMatrix();

		// optimazed the depth calculation
		const Vector3 vPosWorld = m_pSceneNode->getWorldPosition();
		return (vPosWorld.x * matView.m02 + vPosWorld.y * matView.m12 + vPosWorld.z * matView.m22 + matView.m32);
	}

	Box ModelEntity::getLocalAABB() const
	{
		return m_pMesh->getBox();
	}

	Box ModelEntity::getWorldAABB() const
	{
		Box ret = getLocalAABB();
		ret.transform(m_pSceneNode->getWorldMatrix());
		return ret;
	}

	void ModelEntity::updateRenderQueue(RenderQueue* pQueue)
	{
		for (size_t i = 0; i < m_renderables.size(); i++)
		{
			pQueue->addRenderable(m_renderables[i]);
		}
		if (m_isUseXRay)
		{
			for (size_t i = 0; i < m_xrayRenderables.size(); i++)
			{
				pQueue->addRenderable(m_xrayRenderables[i]);
			}
		}
	}

	void ModelEntity::setLMTexture(int subId, Texture* pTexture)
	{
		Material* pMaterial = m_renderables[subId]->getMaterial();
		Texture* pTex = pMaterial->getTexture(1);
		if (pTex)
		{
			TextureManager::Instance()->releaseResource(pTex);
		}
		pTex = pTexture;
		pTex->addRefrenceCount();
		pMaterial->activeTexture(1, pTex);
	}

	void ModelEntity::unloadLMTexture()
	{
		for (ui32 i = 0; i < m_renderables.size(); ++i)
		{
			Material* pMaterial = m_renderables[i]->getMaterial();
			Texture* pTex = pMaterial->getTexture(1);
			if (pTex)
			{
				TextureManager::Instance()->releaseResource(pTex);
			}
			pMaterial->activeTexture(1, NULL);
		}
	}

	void ModelEntity::setEditorColor(i32 subId)
	{
		assert(subId >= 0);
		Material* pMaterial = m_renderables[subId]->getMaterial();
		ShaderProgram* shaderProgram = pMaterial->getShaderProgram();
		if (editor_subId != -1)
		{
			pMaterial->modifyShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
		}
		else
		{
			pMaterial->modifyShaderParam("naturalColor", SPT_VEC4, (void*)&editor_Color);
		}
		editor_subId = subId;
	}

	void ModelEntity::resetEditorColor()
	{
		if (editor_subId != -1)
		{
			Material* pMaterial = m_renderables[editor_subId]->getMaterial();
			ShaderProgram* shaderProgram = pMaterial->getShaderProgram();
			pMaterial->modifyShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
		}
	}

	void ModelEntity::setEffectTexture(Texture* pTexture, int effect)
	{
		SceneManager* pSceneManager = SceneManager::Instance();
		for (size_t i = 0; i < m_renderables.size(); i++)
		{
			Material* pMaterial = m_renderables[i]->getMaterial();
			pMaterial->setBlendState(m_Material->getBlendState()->getDesc());
			pMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
			pMaterial->setDepthState(m_Material->getDepthState()->getDesc());
			pMaterial->setSamplerState(0, m_Material->getSamplerState(0)->getDesc());
			pMaterial->setSamplerState(1, m_Material->getSamplerState(1)->getDesc());
			pMaterial->setSamplerState(2, m_Material->getSamplerState(2)->getDesc());

			pMaterial->setShaderProgram(m_Material->getShaderProgram());
			pMaterial->beginShaderParams(11);
			pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
			pMaterial->setShaderParam("texMaskSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
			pMaterial->setShaderParam("texSampler2", SPT_TEXTURE, (void*)&TEXTURE_UNIT2);
			pMaterial->setShaderParam("darkRadio", SPT_FLOAT, (void*)&m_RenderParam.y);
			pMaterial->setShaderParam("camPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
			pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
			pMaterial->setShaderParam("ambientColor", SPT_VEC4, (void*)&m_ambientColor);
			pMaterial->setShaderParam("colorParams", SPT_VEC4, (void*)&m_colorParams);
			pMaterial->setShaderParam("litParam", SPT_VEC4, m_isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 4);
			pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
			pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
			pMaterial->endShaderParams();

			pMaterial->activeTexture(2, pTexture);
		}
	}

	void ModelEntity::revertEffectTexture()
	{
		SceneManager* pSceneManager = SceneManager::Instance();
		for (size_t i = 0; i < m_renderables.size(); i++)
		{
			Material* pMaterial = m_renderables[i]->getMaterial();
			pMaterial->setShaderProgram(m_Material->getShaderProgram());
			pMaterial->setBlendState(m_Material->getBlendState()->getDesc());
			pMaterial->setRasterizerState(m_Material->getRasterizerState()->getDesc());
			pMaterial->setDepthState(m_Material->getDepthState()->getDesc());
			pMaterial->setSamplerState(0, m_Material->getSamplerState(0)->getDesc());
			pMaterial->setSamplerState(1, m_Material->getSamplerState(1)->getDesc());
			if (m_renderType == RT_FOR_ACTOR_TRANSPARENT)
			{
				pMaterial->beginShaderParams(5);
				pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
				pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
				pMaterial->setShaderParam("litParam", SPT_VEC4, m_isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 3);
				pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
				pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
				pMaterial->endShaderParams();

			}
			else
			{
				pMaterial->beginShaderParams(10);
				pMaterial->setShaderParam("texSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT0);
				pMaterial->setShaderParam("texMaskSampler", SPT_TEXTURE, (void*)&TEXTURE_UNIT1);
				pMaterial->setShaderParam("darkRadio", SPT_FLOAT, (void*)&m_RenderParam.y);
				pMaterial->setShaderParam("camPos", SPT_VEC3, (void*)pSceneManager->getMainCamera()->getPositionPtr());
				pMaterial->setShaderParam("naturalColor", SPT_VEC4, (void*)&m_naturalColor);
				pMaterial->setShaderParam("ambientColor", SPT_VEC4, (void*)&m_ambientColor);
				pMaterial->setShaderParam("colorParams", SPT_VEC4, (void*)&m_colorParams);
				pMaterial->setShaderParam("litParam", SPT_VEC4, m_isUIActor ? (void*)&sh_light_param : (void*)pSceneManager->getLightParamPtr(), 4);
				pMaterial->setShaderParam("matWorld", SPT_MAT4, (void*)m_pSceneNode->getWorldMatrixPtr());
				pMaterial->setShaderParam("matWVP", SPT_MAT4, (void*)&m_matWVP);
				pMaterial->endShaderParams();
			}
		}
	}

	void ModelEntity::EnableAlphaTest(bool bEnable)
	{
		if (m_isAlphaTest != bEnable)
		{
			m_isAlphaTest = bEnable;
			if (!m_renderables.empty())
			{
				SceneManager* pSceneManager = SceneManager::Instance();

				ShaderProgram* program = m_Material->getShaderProgram();
				for (size_t i = 0; i < m_renderables.size(); ++i)
				{
					Material* pMaterial = m_renderables[i]->getMaterial();
					pMaterial->setShaderProgram(program);
				}//for
			}
		}
	}

	void ModelEntity::replaceTexture(Texture* pTexture, size_t subid, ui32 uint)
	{
		if (!MeshManager::Instance()->validResource(m_pMesh))
		{
			TextureManager::Instance()->releaseResource(pTexture);
			return;
		}
		m_pMesh->getSubMesh((int)subid)->setTexture(uint, pTexture); LordAssert(m_renderables[subid]);
		Material* pMaterial = m_renderables[subid]->getMaterial();
		pMaterial->activeTexture(uint, pTexture);
	}

	void ModelEntity::prepareTexture(bool bSingleThread, const String& func)
	{
		for (int i = 0; i < m_pMesh->getSubMeshNum(); ++i)
		{
			SubMesh* pSubMesh = m_pMesh->getSubMesh(i);
			bool multithread = StreamThread::Instance()->IsRunning() && !bSingleThread;
			if (multithread)
			{
				TextureLoadEvent* env = LordNew TextureLoadEvent(i, 0, this);
				StreamThread::Instance()->AddRequest(env);
				if (pSubMesh->getMasktexture())
				{
					TextureLoadEvent* env = LordNew TextureLoadEvent(i, 1, this);
					StreamThread::Instance()->AddRequest(env);
				}
			}
			else
			{
				String name = m_pMesh->getTextureName(i, 0);
				Texture* pTex = TextureManager::Instance()->createTexture(name);
				if (pTex->load())
				{
					replaceTexture(pTex, i, 0);
				}
				else
				{
					LordLogError("%s: Texture isn't exits %s.", func.c_str(), name.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "%s: Texture isn't exits %s.", func.c_str(), name.c_str());
					MessageBox(0, buf, 0, 0);
#endif
				}
				if (pSubMesh->getMasktexture())
				{
					name = m_pMesh->getTextureName(i, 1);
					pTex = TextureManager::Instance()->createTexture(name);
					if (pTex->load())
					{
						replaceTexture(pTex, i, 1);
					}
					else
					{
						LordLogError("%s: Texture isn't exits %s.", func.c_str(), name.c_str());
					}
				}
			}
		}
	}


	TextureLoadEvent::TextureLoadEvent(int subid, ui32 uint, ModelEntity* entity)
		: mSubMeshId(subid)
		, mTextureUint(uint)
		, mEntity(entity)
	{
		mName = mEntity->getMesh()->getTextureName(subid, uint);
		m_Texture = TextureManager::Instance()->createTexture(mName);
		mEntity->addTextureLoadEvent(this);
	}

	TextureLoadEvent::~TextureLoadEvent()
	{
		if (mEntity)
		{
			mEntity->deleteTextureLoadEvent(this);
		}
	}

	bool TextureLoadEvent::ProcessEvent()
	{
		if (!m_Texture->prepare() && mTextureUint == 0)
		{
			LordLogError("TextureLoadEvent::ProcessEvent:Texture isn't exits %s.", mName.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS 
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "Texture isn't exits %s.", mName.c_str());
			MessageBox(0, buf, 0, 0);
#endif
		}
		return true;
	}

	bool TextureLoadEvent::RespondEvent()
	{
		if (m_Texture->load())
		{
			if (mEntity)
			{
				mEntity->replaceTexture(m_Texture, mSubMeshId, mTextureUint);
				mEntity->deleteTextureLoadEvent(this);
			}
			else
			{
				TextureManager::Instance()->releaseResource(m_Texture);
			}
		}
		else
		{
			if (mTextureUint == 0)
			{
				LordLogError("Texture load error %s.", mName.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS 
				char buf[MAX_PATH];
				sprintf_s(buf, MAX_PATH, "TextureLoadEvent::RespondEvent:Texture load error %s.", mName.c_str());
				MessageBox(0, buf, 0, 0);
#endif
			}

			TextureManager::Instance()->releaseResource(m_Texture);
			if (mEntity)
			{
				mEntity->deleteTextureLoadEvent(this);
			}
		}

		return true;
	}
}