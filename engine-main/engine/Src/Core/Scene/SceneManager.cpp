#include "Core.h"
#include "SceneManager.h"
#include "Render/Renderer.h"
#include "Object/Root.h"
#include "Scene.h"
#include "Render/Material.h"
#include "SceneNode.h"
#include "Resource/LordThread.h"
#include "Anim/Animation.h"
#include "TerrainMeshObject.h"
#include "Actor/ActorManager.h"
#include "Util/QueryObject.h"
#include "Render/RenderStage.h"
#include "Tessolator/TessManager.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "rapidjson/document.h"

using namespace rapidxml;

namespace LORD
{
	//---------------------------------------------------------------------
	SceneManager::SceneManager()
		: m_bNeedUpate(false)
		, m_isAnotherSceneLoading(false)
		, m_bIsRenderActorHigh(false)
		, m_isUseXRay(false)
		, m_bisComputeActorFog(false)
		, m_pMainCamera(NULL)
		, m_p2DCamera(NULL)
		, m_pGUICamera(NULL)
		, m_pRootNode(NULL)
		, m_pCurrentScene(NULL)
		, m_pAnotherScene(NULL)
		, m_DeltaPosition(Vector3::ZERO)
	{
		memset(&m_LightParam, 0, sizeof(Vector4) * 4);
		memset(&m_FogParam, 0, sizeof(Vector4) * 2);
		memset(&m_FogParam_far, 0, sizeof(Vector4) * 2);

		setMainLightDir(Vector3(0.2f, 1.f, -0.7f).normalizedCopy());
		setMainLightColor(Color(0.6f, 0.6f, 0.6f, 1.f));
		setSubLightColor(Color(0.6f, 0.6f, 0.6f, 1.f));
		setAmbientColor(Color(0.4f, 0.4f, 0.4f, 1.f));
		setFogParam(Vector3(10.f, 128.f, 0.9f));
		setFogColor(Color(0.5f, 0.5f, 1.0f, 1.0f));

		m_FogParam.mainPosition = Vector4::INVALID;
		m_FogParam_far.mainPosition = Vector4::INVALID;
	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::initRenderQueueGroup()
	{
		//add by maxicheng
		DataStream* pJSONStream = ResourceGroupManager::Instance()->openResource("RenderQueueDefine.json");
		if (pJSONStream == NULL)
		{
			LordLogError("RenderQueueDefine file RenderQueueDefine.json not found.");
			return;
		}

		size_t fileSize = pJSONStream->size();
		char* jsonData = (char*)LordMalloc(fileSize + 1);
		pJSONStream->read(jsonData, fileSize);
		jsonData[fileSize] = 0;

		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(jsonData);
		if (doc->HasParseError())
		{
			LordLogError("invalid render queue define file.");
		}
		else
		{
			LordAssert(doc->IsObject());
			//LordLogInfo("parse RenderQueueDefine.json successful!");
			const rapidjson::Value& renderQueueGroup = doc->FindMember("RenderQueueGroup")->value;
			const rapidjson::Value& renderQueue = renderQueueGroup.FindMember("RenderQueue")->value;

			LordAssert(renderQueue.IsArray());

			for (rapidjson::Value::ConstValueIterator it = renderQueue.Begin(); it != renderQueue.End(); ++it)
			{
				LordAssert(it->FindMember("name")->value.IsString());
				String strRenderQueueName = it->FindMember("name")->value.GetString();
				RenderQueue* pRenderQueue = LordNew RenderQueue(strRenderQueueName);
				m_RenderQueueGroup.push_back(pRenderQueue);
			}

		}

		LordDelete(doc);
		LordFree(jsonData);
		LordDelete(pJSONStream);
	}

	void SceneManager::destroyRenderQueueGroup()
	{
		ui32 nCount = (ui32)m_RenderQueueGroup.size();
		for (ui32 i = 0; i < nCount; ++i)
		{
			LordSafeDelete(m_RenderQueueGroup[i]);
		}

		m_RenderQueueGroup.clear();
	}

	bool SceneManager::initialize()
	{
		initRenderQueueGroup();
		// create main camera
		m_pMainCamera = LordNew CameraMain(Camera::PM_PERSPECTIVE, false);

		Vector3 vCamPos(0, 100, 100);
		Vector3 vCamDir = Vector3::ZERO - vCamPos;
		vCamDir.normalize();

		m_pMainCamera->setPosition(vCamPos);
		m_pMainCamera->setDirection(vCamDir);

		// create 2D camera
		Vector3 vCam2DPos(0, 0, 1);
		Vector3 vCam2DDir = -Vector3::UNIT_Z;
		m_p2DCamera = LordNew Camera(Camera::PM_ORTHO);

		m_p2DCamera->setPosition(vCam2DPos);
		m_p2DCamera->setDirection(vCam2DDir);
		m_p2DCamera->setNearClip(0.1f);
		m_p2DCamera->setFarClip(100.0f);
		m_p2DCamera->update();

		// create main light
		setMainLightDir(Vector3(0.2f, 1.f, -0.7f).normalizedCopy());
		setMainLightColor(Color(0.6f, 0.6f, 0.6f, 1.f));
		setAmbientColor(Color(0.6f, 0.6f, 0.6f, 1.0f));
		//setMainLightColor(Color(0.682f, 0.459f, 0.227f, 0.319f));
		//setSubLightColor(Color(0.612f, 0.808f, 1.f, 0.066f));

		m_FogParam.fogColor = Vector4(0.5f, 0.5f, 1.0f, 1.0f);
		m_FogParam.fogParam = Vector4(10.f, 128.f, 0.9f, 1.f);

		m_FogParam_far.fogColor = Vector4(0.5f, 0.5f, 1.0f, 1.0f);
		m_FogParam_far.fogParam = Vector4(128.f, 512.f, 0.9f, 1.f);
		// create root node
		m_pRootNode = LordNew SceneNode;

		update();

		return true;
	}

	void SceneManager::destroy()
	{
		if (m_pRootNode)
			m_pRootNode->destroyAllChildren();

		LordSafeDelete(m_pRootNode);
		LordSafeDelete(m_pMainCamera);
		LordSafeDelete(m_p2DCamera);

		destroyRenderQueueGroup();
	}

	void SceneManager::setMainLightDir(const Vector3& dir)
	{
		//m_MainLightDir = dir;
		memcpy(&m_LightParam.mainLightDir, &dir, sizeof(Vector3)); m_LightParam.mainLightDir.w = 1.0f;

		Vector3 rDir = -(Vector3(m_LightParam.mainLightDir).Reflect(Vector3::UNIT_Y));

		memcpy(&m_LightParam.subLightDir, &rDir, sizeof(Vector3)); m_LightParam.subLightDir.w = 1.0f;
	}

	void SceneManager::setMainLightColor(const Color& color)
	{
		//m_MainLightColor = color;
		memcpy(&m_LightParam.mainLightColor, &color, sizeof(Color));
	}

	void SceneManager::setSubLightColor(const Vector4& color)
	{
		memcpy(&m_LightParam.subLightColor, &color, sizeof(Color));
	}

	void SceneManager::setAmbientColor(const Color& color)
	{
		//m_ambientColor = color;
		memcpy(&m_LightParam.ambientColor, &color, sizeof(Color));
	}

	void SceneManager::setFogColor(const Color& color)
	{
		//m_FogColor = color;
		memcpy(&m_FogParam.fogColor, &color, sizeof(Color));
		memcpy(&m_FogParam_far.fogColor, &color, sizeof(Color));
	}

	void SceneManager::setFogParam(const Vector3& param)
	{
		//m_FogParam = param;
		memcpy(&m_FogParam.fogParam, param.ptr(), sizeof(Vector3));
		m_FogParam_far.fogParam.x = param.y;
		m_FogParam_far.fogParam.y = param.y * 3;
		m_FogParam_far.fogParam.z = m_FogParam.fogParam.z;
		m_FogParam_far.fogParam.w = m_FogParam.fogParam.w;
	}

	void SceneManager::update()
	{
		// update camera
		m_pMainCamera->update();
		((CameraMain*)m_pMainCamera)->ProcessMainCameraShake((float)Root::Instance()->getFrameTime()*0.001f);

		if (m_pCurrentScene)
		{
			m_pCurrentScene->updateRenderQueue();
		}
	}

	void SceneManager::render()
	{
		//ui8 i = Root::Instance()->isEditorMode() ? getRenderQueueIndex("Background") : getRenderQueueIndex("Scene");
		ui8 i = getRenderQueueIndex("Background");
		ui8 j = getRenderQueueIndex("UI");

		TessManager::Instance()->endRender1();
		for (; i <= j; ++i)
		{
			if (i == 5)
			{
				TessManager::Instance()->endRender2();
			}
			if (m_RenderQueueGroup[i])
			{
				m_RenderQueueGroup[i]->renderQueue();
				m_RenderQueueGroup[i]->beginRender();
			}
		}
	}

	void SceneManager::renderScene()
	{
		ui8 i = getRenderQueueIndex("Background");

		ui8 nUIIndex = (ui8)getRenderQueueIndex("UI");

		if (RenderStageManager::Instance()->isBlurLDRSceneTarget())
		{
			for (; i < nUIIndex; ++i)
			{
				RenderQueue* pCurRenderQueue = m_RenderQueueGroup[i];
				if (pCurRenderQueue)
				{
					pCurRenderQueue->beginRender();
				}
			}
		}
		else
		{
			for (; i < nUIIndex; ++i)
			{
				RenderQueue* pCurRenderQueue = m_RenderQueueGroup[i];
				if (pCurRenderQueue)
				{
					pCurRenderQueue->renderQueue();
					pCurRenderQueue->beginRender();
				}
			}
		}
	}

	Scene* SceneManager::createScene(const String& name, bool hasTerrain, bool bSphere, float blockWidth, float blockHeight, ui16 BlockNumH, ui16 BlockNumV, bool bNormalFov, float radius, bool autocalc)
	{
		LordAssert(!m_pCurrentScene);
		m_pCurrentScene = LordNew Scene(name, hasTerrain, bSphere);
		m_pCurrentScene->setSizeofBlock(blockWidth, blockHeight);
		m_pCurrentScene->setBlockCount(BlockNumH, BlockNumV);
		m_pCurrentScene->setUseCameraFov45(bNormalFov);
		m_pCurrentScene->setSphereParam(radius, autocalc);
		m_pCurrentScene->InitScene();
		m_pCurrentScene->prepare();

		return m_pCurrentScene;
	}

	Scene* SceneManager::loadScene(const String& name, QueryObjectManager* manager, int aiLoadLv)
	{
		LordAssert(!m_pCurrentScene);
		m_pCurrentScene = LordNew Scene(name);
		m_pCurrentScene->setLoadLevel(aiLoadLv);
		LordAssert(m_pCurrentScene);
		m_pCurrentScene->setQueryObjectManager(manager);
		m_pCurrentScene->loadSceneFromXml();

		Vector3 dir = m_pCurrentScene->getLightDirection();
		dir.normalize();

		setMainLightDir(dir);
		setMainLightColor(m_pCurrentScene->getLightColor());
		setAmbientColor(m_pCurrentScene->getAmbientColor());

		return m_pCurrentScene;
	}

	Scene* SceneManager::loadScene(const String& name, int aiLoadLv /* = 3 */)
	{
		return loadScene(name, NULL, aiLoadLv);
	}

	void SceneManager::closeScene()
	{
		if (m_pCurrentScene)
		{
			((CameraMain*)m_pMainCamera)->StopCameraShake();
			//finish the preparing
			StreamThread* pThread = StreamThread::Instance();
			if (pThread)
			{
				while (pThread->HasTask())
				{
					pThread->RespondEvents();
				}
			}
			//ActorManager::Instance()->DestroyDelayActors(false);
			LordDelete m_pCurrentScene;
			m_pCurrentScene = NULL;

			if (m_pAnotherScene)
			{
				m_isAnotherSceneLoading = true;
				LordDelete m_pAnotherScene;
				m_isAnotherSceneLoading = false;
				m_pAnotherScene = NULL;
			}
			//finish all event
			//卸载后，先等待资源线程完成所有加载和卸载请求,以免导致后来主线程立即申请加载的资源状态冲突
			if (pThread)
			{
				while (pThread->HasTask())
				{
					pThread->RespondEvents();
				}
			}
			m_FogParam.mainPosition = Vector4::INVALID;
			m_FogParam_far.mainPosition = Vector4::INVALID;
		}
	}

	void SceneManager::saveScene(const String& filename)
	{
		if (m_pCurrentScene)
		{
			m_pCurrentScene->saveSceneToXml(filename);
		}
	}

	void SceneManager::refleshMainPositon()
	{
		m_FogParam.mainPosition = Vector4(m_pMainCamera->getPosition() + m_DeltaPosition, 1.0f);
		m_FogParam_far.mainPosition = m_FogParam.mainPosition;
	}

	void SceneManager::RenderMaskStaticMesh(bool bRender)
	{
		m_pCurrentScene->EnableRenderType(GOT_SMESH, bRender);
	}

	void SceneManager::RenderMaskStaticMesh_Light(bool bRender)
	{
		m_pCurrentScene->EnableRenderType(GOT_SMESH_L, bRender);
	}

	void SceneManager::RenderMaskSkeletonMesh(bool bRender)
	{
		m_pCurrentScene->EnableRenderType(GOT_DMESH, bRender);
	}
	void SceneManager::RenderMaskEffect(bool bRender)
	{
		m_pCurrentScene->EnableRenderType(GOT_EFFECT, bRender);
	}

	void SceneManager::RenderMaskTerrain(bool bRender)
	{
		m_pCurrentScene->EnableRenderType(GOT_TERRAIN, bRender);
	}

	bool SceneManager::IsRenderMaskStaticMesh() const
	{
		return m_pCurrentScene->IsEnableRenderType(GOT_SMESH);
	}

	bool SceneManager::IsRenderMaskStaticMesh_Light() const
	{
		return m_pCurrentScene->IsEnableRenderType(GOT_SMESH_L);
	}

	bool SceneManager::IsRenderMaskSkeletonMesh() const
	{
		return m_pCurrentScene->IsEnableRenderType(GOT_DMESH);
	}

	bool SceneManager::IsRenderMaskEffect() const
	{
		return m_pCurrentScene->IsEnableRenderType(GOT_EFFECT);
	}

	bool SceneManager::IsRenderMaskTerrain() const
	{
		return m_pCurrentScene->IsEnableRenderType(GOT_TERRAIN);
	}

	Scene* SceneManager::loadChildScene(const String& name, QueryObjectManager* manager /*= NULL*/, int aiLoadLv /*= 3 */)
	{
		LordAssert(!m_pAnotherScene);
		m_pAnotherScene = LordNew Scene(name);
		m_pAnotherScene->setLoadLevel(aiLoadLv);
		LordAssert(m_pAnotherScene);
		m_pAnotherScene->setQueryObjectManager(manager);
		m_isAnotherSceneLoading = true;
		m_pAnotherScene->loadSceneFromXml();
		m_isAnotherSceneLoading = false;

		return m_pAnotherScene;
	}

	Scene* SceneManager::swapScene()
	{
		if (!m_pCurrentScene || !m_pAnotherScene)
		{
			return NULL;
		}

		Scene* tempScene = m_pCurrentScene;
		m_pCurrentScene = m_pAnotherScene;
		m_pAnotherScene = tempScene;

		Vector3 dir = m_pCurrentScene->getLightDirection();
		dir.normalize();

		setMainLightDir(dir);
		setMainLightColor(m_pCurrentScene->getLightColor());
		setAmbientColor(m_pCurrentScene->getAmbientColor());

		return m_pCurrentScene;
	}

	Scene* SceneManager::getCurrentScene()
	{
		if (m_isAnotherSceneLoading)
			return m_pAnotherScene;

		return m_pCurrentScene;
	}

	void SceneManager::loading(float x, float y)
	{
		if (m_pCurrentScene)
		{
			m_pCurrentScene->loadSceneObject(x, y);
		}
	}

	float SceneManager::getLoadingPercent()
	{
		float percent = 0.0f;
		if (m_pCurrentScene)
		{
			m_pCurrentScene->preLoad(percent);
		}

		return percent;
	}

	LORD::Vector3 SceneManager::getCameraRayIntersectPoint()
	{
		Vector3 ret = Vector3::ZERO;

		Ray ray;
		if (m_pMainCamera)
		{
			m_pMainCamera->getCameraRay(ray, Vector2((float)Renderer::Instance()->getScreenWidth() / 2, (float)Renderer::Instance()->getScreenHeight() / 2));

			Box box(Vector3(-1024.0f, -0.1f, -1024.0f), Vector3(1024.0f, 0.0f, 1024.0f));

			bool isHit = ray.hitBox(box);
			if (isHit)
			{
				Real tMin;
				HitInfo hitInfo;
				ray.hitBox(box, tMin, hitInfo);
				Vector3 hitPoint = hitInfo.hitPos;

				return hitPoint;
			}
		}

		return ret;
	}

	bool SceneManager::isPlayingCameraAnimate()
	{
		CameraMain* mainCamera = (CameraMain*)m_pMainCamera;

		return mainCamera->getCameraState() == CameraMain::ANIMA;
	}

	void SceneManager::setDeltaPosition(const Vector3& pos)
	{
		m_DeltaPosition = pos;
		refleshMainPositon();
	}

	void SceneManager::addCameraShake(float beginTime, float scale, float durationTime, int shakeTimes)
	{
		CameraMain* mainCamera = (CameraMain*)m_pMainCamera;
		mainCamera->AddCameraShake(beginTime, scale, durationTime, shakeTimes);
	}

	void SceneManager::stopCameraAnimations()
	{
		m_pCurrentScene->StopCameraAnimations();
		ActorManager::Instance()->stopCameraAnims();
	}

	void SceneManager::enableCameraShake(bool bEnable)
	{
		((CameraMain*)m_pMainCamera)->EnableCameraShake(bEnable);
	}

}
