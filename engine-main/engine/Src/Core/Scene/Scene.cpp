#include "Core.h"
#include "Scene.h"
#include "Object/Root.h"
#include "SceneManager.h"
#include "Camera.h"
#include "SceneActorObject.h"
#include "StaticMeshObject.h"
#include "StaticMeshObject_Normal.h"
#include "StaticMeshObject_Normal_noFog.h"
#include "SceneEffectObject.h"
#include "SkeletonMeshObject.h"
#include "SceneLightObject.h"
#include "SceneLightObject_dir.h"
#include "SceneLightObject_spot.h"
#include "DynamicPointLightSimple.h"
#include "GroupObject.h"
#include "HazeObject.h"
#include "Resource/ResourceGroupManager.h"
#include "Util/PathUtil.h"
#include "Util/QueryObject.h"
#include "Navigation/Navigation.h"
#include "Zoom.h"
#include "Geom/Ray.h"
#include "TerrainMeshObject.h"
#include "LightmapMgr.h"
#include "TextureSet.h"
#include "Render/RenderStage.h"
#include "CameraAnimation.h"
#include "Actor/ActorManager.h"
#include "Render/OceanRender.h"
#include "NavigatePoint.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidxml;

namespace LORD
{
	inline void prepareEventFunc(GameObject* pObject)
	{
		StreamThread* pSteamThread = StreamThread::Instance();
		if (!pObject->getIOprepared() && !pObject->getRequestPrepare())
		{
			pObject->_prepare_res();
			ObjectPrepareEvent* pEvent = LordNew ObjectPrepareEvent(pObject);
			pSteamThread->AddRequest(pEvent);
		}
	}

	inline void ObjectUnprepareFunc(GameObject* pObject)
	{

	}

	const Scene::GridZoom Scene::sFieldZoom1[9] =
	{
		0,  0,
		-1, -1,	 0, -1,  1, -1,  -1,  0,
		1,  0,  -1,  1,  0,  1,   1,  1,
	};

	const Scene::GridZoom Scene::sFieldZoom2[16] =
	{
		-2,  0,   0,  2,   0, -2,   2,  0,
		-2, -1,  -2,  1,   1,  2,  -1,  2,
		2, -1,  -1, -2,   1, -2,   2,  1,
		-2, -2,  -2,  2,   2,  2,   2, -2,
	};

	const Scene::GridZoom Scene::sFieldZoom3[24] =
	{
		0, -3,  -3,  0,   3,  0,   0,  3,
		-1, -3,   1, -3,   1,  3,  -1,  3,
		-3, -1,   3, -1,  -3,  1,   3,  1,
		-3,  2,   3,  2,  -2,  3,   2, -3,
		2,  3,   3, -2,  -3, -2,  -2, -3,
		3,  3,  -3, -3,  -3,  3,   3, -3

	};
	const Scene::GridZoom Scene::sFieldZoom4[32] =
	{
		0, -4,   4,  0,   0,  4,  -4,  0,
		4, -1,  -1, -4,   1, -4,   4,  1,
		-4, -1,  -4,  1,   1,  4,  -1,  4,
		-2, -4,   4, -2,   2, -4,   4,  2,
		2,  4,  -2,  4,  -4, -2,  -4,  2,
		3,  4,   4,  3,   4, -3,  -4, -3,
		3, -4,  -3,  4,  -4,  3,  -3, -4,
		-4, -4,  -4,  4,   4, -4,   4,  4,
	};

	Scene::Scene(const String& name, bool hasTerrain, bool bSphere)
		: m_bHasTerrain(hasTerrain)
		, m_bSphere(bSphere)
		, m_bUseCameraFov45(false)
		, m_isRenderTerrain(true)
		, m_AutoCalc(true)
		, m_isZoomCull(true)
		, m_loadlevel(2)
		, m_blockNumHorizontal(0)
		, m_blockNumVertical(0)
		, m_RenderMask(0xffffffff)
		, mSceneObjectID(1)
		, m_widthPerBlock(9.92f)  //(512-16) / 50
		, m_heightPerBlock(19.84f) //(512-16) / 25
		, m_SphereRadius(200.0f)
		, m_queryObjectManager(NULL)
		, m_pLightmapMgr(NULL)
		, m_pNavigation(NULL)
		, m_pOceanRender(NULL)
		, m_NavigatePoint(NULL)
		, m_SphereTerrainCenter(Vector3::INVALID)
		, m_sceneName(name)
	{
		m_pNavigation = LordNew Navigation;

		m_defaultCamraPos = Vector3(50, 50, 50 + 50 * 1.732f);
		Vector3 vLookAt(50, 0, 50);
		m_defaultCameraDir = vLookAt - m_defaultCamraPos;
		m_defaultCameraDir.normalize();

		m_bEditorMode = Root::Instance()->isEditorMode();

		m_LastZoom.x = m_CurZoom.x = 0;
		m_LastZoom.z = m_CurZoom.z = 0;

		m_lightDirection = SceneManager::Instance()->getMainLightDir();
		m_lightColor = SceneManager::Instance()->getMainLightColor();
		m_ambientColor = SceneManager::Instance()->getAmbientColor();
	}

	Scene::~Scene()
	{
		DestroyScene();
	}

	void Scene::InitScene()
	{
		if (m_bHasTerrain)
		{
			m_terrainBlockList.reserve(m_blockNumHorizontal*m_blockNumVertical);
			m_terrainBlockList.resize(m_blockNumHorizontal*m_blockNumVertical);

			for (ui16 i = 0; i < m_blockNumHorizontal; ++i)
			{
				for (ui16 j = 0; j < m_blockNumVertical; ++j)
				{
					TerrainBlock* terrainBlock = LordNew TerrainBlock(this, i, j, m_bSphere);
					m_terrainBlockList[j*m_blockNumHorizontal + i] = terrainBlock;
				}
			}
			if (m_bSphere)
			{
				DataStream* stream = ResourceGroupManager::Instance()->openResource(m_sceneName + ".terraindata");
				if (stream)
				{
					for (TerrainBlockList::iterator it = m_terrainBlockList.begin(); it != m_terrainBlockList.end(); ++it)
					{
						TerrainBlock* terrainBlock = *it;
						terrainBlock->importData(stream);
					}
					LordDelete stream;
				}
				else
				{
					LordLogInfo("Scene::InitScene:load %s.terraindata failed! may be the file isn't exist.", m_sceneName.c_str());
				}

				LoadNavigatePoint(m_sceneName + ".navipoint");
			}
		}

		// init zoom list 
		// just consistent with terrain block
		m_zoomList.reserve(m_blockNumHorizontal*m_blockNumVertical);
		m_zoomList.resize(m_blockNumHorizontal*m_blockNumVertical);

		for (ui16 i = 0; i < m_blockNumHorizontal; ++i)
		{
			for (ui16 j = 0; j < m_blockNumVertical; ++j)
			{
				Zoom* zoom = LordNew Zoom(this, i, j);
				m_zoomList[j*m_blockNumHorizontal + i] = zoom;
			}
		}

		ObjectSubset* pSubSet = NULL;
		pSubSet = LordNew DynamicLightSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew GroupSubset();			mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew SceneActorSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew SceneEffectSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew PointLightSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew DirLightSubset();			mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew SpotLightSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew SkeletonMeshSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew StaticMeshSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew NormalMeshSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew NormalNoFogMeshSubset();	mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew TerrainMeshSubset();		mObjectSubsets.push_back(pSubSet);
		pSubSet = LordNew HazeSubset();				mObjectSubsets.push_back(pSubSet);
	}

	void Scene::prepare()
	{
		TerrainBlockList::iterator it = m_terrainBlockList.begin();
		for (; it != m_terrainBlockList.end(); ++it)
		{
			(*it)->prepare();
		}
	}

	//void Scene::render()
	//{
	//	// render zoom object
	//	for (size_t i = 0; i < m_zoomList.size(); ++i)
	//	{
	//		//bool isVisble = SceneMgr::Instance()->getMainCamera()->getFrustum().intersect(m_terrainBlockList[i]->getTerrainBlockAABB());
	//		//if (isVisble)
	//		{
	//			m_zoomList[i]->render();
	//		}
	//	}
	//}

	GameObject* Scene::createGameObject(ui32 type)
	{
		GameObject* obj = NULL;
		switch (type)
		{
		case GOT_SMESH:
			obj = LordNew StaticMeshObject;
			break;
		case GOT_EFFECT:
			obj = LordNew SceneEffectObject;
			break;
		case GOT_DMESH:
			obj = LordNew SkeletonMeshObject;
			break;
		case GOT_POINTLIGHT:
			obj = LordNew SceneLightObject;
			break;
		case GOT_SMESH_L:
			obj = LordNew StaticMeshObject_Normal;
			break;
		case GOT_SMESH_L_NOFOG:
			obj = LordNew StaticMeshObject_Normal_noFog;
			break;
		case GOT_DIRLIGHT:
			obj = LordNew SceneLightObject_dir;
			break;
		case GOT_SPOTLIGHT:
			obj = LordNew SceneLightObject_spot;
			break;
		case GOT_TERRAIN:
			obj = LordNew TerrainMeshObject;
			break;
		case GOT_SIMDPOINTLIGHT:
			obj = LordNew DynamicPointLightSimple;
			break;
		case GOT_HAZE:
			obj = LordNew HazeObejct;
			break;
		default:
			break;
		}

		if (obj)
		{
			m_gameObjectList.push_back(obj);
		}
		return obj;
	}

	void Scene::destroyGameObject(GameObject* obj)
	{
		if (obj)
		{
			switch (obj->getType())
			{
			case GOT_SMESH:
				break;
			case GOT_EFFECT:
				break;
			case GOT_DMESH:
				break;
			case GOT_POINTLIGHT:
				break;
			case GOT_SMESH_L:
				break;
			case GOT_SMESH_L_NOFOG:
				break;
			case GOT_DIRLIGHT:
				break;
			case GOT_SPOTLIGHT:
				break;
			case GOT_TERRAIN:
				break;
			default:
				break;
			}
			EraseSubSet(obj);
			unregisterGameObjectFromZoom(obj);
			GameObjectList::iterator itor = std::find(m_gameObjectList.begin(), m_gameObjectList.end(), obj);
			if (itor != m_gameObjectList.end())
			{
				m_gameObjectList.erase(itor);
			}
			LordSafeDelete(obj);
		}
	}

	void Scene::destroyGameObjectByType(GameObjectType type)
	{
		GameObjectList::iterator it = m_gameObjectList.begin();

		for (; it != m_gameObjectList.end(); )
		{
			GameObject* obj = *it; LordAssert(obj);
			if (obj->getType() == type)
			{
				EraseSubSet(obj);
				unregisterGameObjectFromZoom(obj);
				LordSafeDelete(obj);
				it = m_gameObjectList.erase(it);
			}
			else
				++it;
		}
	}

	String Scene::getAutoObjectName()
	{
		return StringUtil::ToString(mSceneObjectID++);
	}

	GameObject* Scene::getGameObject(const String& name)
	{
		GameObjectList::iterator it;
		for (it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
		{
			if ((*it)->getName() == name)
				return *it;
		}

		return NULL;
	}

	GameObject* Scene::GetGameObjectByIndex(size_t nIndex)
	{
		if (nIndex >= m_gameObjectList.size())
		{
			return NULL;
		}

		return m_gameObjectList[nIndex];
	}

	TerrainBlock* Scene::getRayIntersectTerrainBlock(const Ray& ray)
	{
		if (!m_bHasTerrain)
			return NULL;

		float maxX = m_blockNumHorizontal * m_widthPerBlock;
		float maxZ = m_blockNumVertical * m_heightPerBlock;
		Box terrainBox(0.0f, -MAX_TERRAIN_BLOCK_HEIGHT, 0.0f, maxX, 0.0, maxZ);

		bool isHit = ray.hitBox(terrainBox);
		if (isHit)
		{
			Real tMin;
			HitInfo hitInfo;
			ray.hitBox(terrainBox, tMin, hitInfo);
			Vector3 hitPoint = hitInfo.hitPos;

			size_t xBlockOffset = (size_t)(hitPoint.x / m_widthPerBlock);
			size_t yBlockOffset = (size_t)(hitPoint.z / m_heightPerBlock);

			size_t BlockIndex = yBlockOffset*m_blockNumHorizontal + xBlockOffset;
			if (m_terrainBlockList.size() <= BlockIndex)
			{
				return NULL;
			}

			return m_terrainBlockList[BlockIndex];
		}
		else
		{
			return NULL;
		}
	}

	bool Scene::getRayIntersectPoint(const Ray& ray, Vector3& point, GameObjectList* culllist, ui32 queryMask)
	{
		float tmin = Math::MAX_FLOAT;
		Vector3 blockpoint;
		bool hitterrain = false;
		if (m_bSphere)
		{
			hitterrain = getRayIntersectPointForBlock(ray, blockpoint, tmin);
		}

		if (m_bUseCameraFov45)
		{
			GameObject* pObject;
			std::pair<bool, Real> result;
			map<float, rayinfo>::type resultList;
			// Camera* pCamera = SceneManager::Instance()->getMainCamera();
			GameObjectList::iterator it = m_gameObjectList.begin();
			for (; it != m_gameObjectList.end(); ++it)
			{
				pObject = *it;
				if (!pObject->m_IOprepared || pObject->m_bRequestPrepare)
					continue;
				if (culllist)
				{
					GameObjectList::iterator it = std::find(culllist->begin(), culllist->end(), pObject);
					if (it != culllist->end())
						continue;
				}
				if (queryMask & pObject->getType())
				{
					if (ray.hitBox(pObject->getWorldAABB()))
					{
						result = pObject->rayIntersects(ray);
						if (result.first)
						{
							rayinfo p;
							Ray rayResult(ray);
							Matrix4 worldMat = pObject->m_SceneNode->getWorldMatrix();
							rayResult.transform(worldMat.detInverse());
							p.p = rayResult.getPoint(result.second);//获取本地世界的坐标
							p.p = p.p * pObject->m_SceneNode->getWorldMatrix();//转换成世界坐标
							p.o = pObject;
							float lensqr = (p.p - ray.o).lenSqr();
							if (lensqr < tmin)
							{
								resultList.insert(std::pair<float, rayinfo>(lensqr, p));
							}
						}
					}
				}
			}

			map<float, rayinfo>::type::iterator itor = resultList.begin();
			if (itor != resultList.end())
			{
				pObject = itor->second.o;
				point = itor->second.p;
				//Ray rayResult(ray);
				//Matrix4 worldMat = pObject->m_SceneNode->getWorldMatrix();
				//rayResult.transform(worldMat.detInverse());
				//point = rayResult.getPoint(itor->first);//获取本地世界的坐标
				//point = point * pObject->m_SceneNode->getWorldMatrix();//转换成世界坐标
				return true;
			}

			if (hitterrain)
			{
				point = blockpoint;
				return true;
			}

			return false;
		}
		else
		{
			float maxX = m_blockNumHorizontal * m_widthPerBlock;
			float maxZ = m_blockNumVertical * m_heightPerBlock;
			Box terrainBox(0.0f, -MAX_TERRAIN_BLOCK_HEIGHT, 0.0f, maxX, 0.0, maxZ);

			bool isHit = ray.hitBox(terrainBox);
			if (isHit)
			{
				Real tMin;
				HitInfo hitInfo;
				ray.hitBox(terrainBox, tMin, hitInfo);
				point = ray.getPoint(tMin);

				return true;
			}
		}

		return false;
	}

	bool Scene::getRayIntersectPointForBlock(const Ray& ray, Vector3& point, float& tmin)
	{
		float lastDis = Math::MAX_FLOAT;
		for (TerrainBlockList::iterator it = m_terrainBlockList.begin(); it != m_terrainBlockList.end(); ++it)
		{
			std::pair<bool, Real> result = (*it)->editor_rayIntersect(ray);
			if (result.first)
			{
				if (result.second < lastDis)
					lastDis = result.second;
			}
		}
		tmin = lastDis;
		if (lastDis < Math::MAX_FLOAT)
		{
			point = ray.getPoint(lastDis);
			return true;
		}

		return false;
	}

	void Scene::editor_modifySphereBlockHeight(const Vector3& pos, const TerrainBlock::TerrainBrushInfo& info, float escapeTime)
	{
		for (TerrainBlockList::iterator it = m_terrainBlockList.begin(); it != m_terrainBlockList.end(); ++it)
		{
			TerrainBlock* pBlock = *it;
			pBlock->editor_modifyTerrainHeight(pos, info, escapeTime);
		}
	}

	void Scene::editor_enableBlockMeshVisible(bool bVisible)
	{
		for (TerrainBlockList::iterator it = m_terrainBlockList.begin(); it != m_terrainBlockList.end(); ++it)
		{
			TerrainBlock* pBlock = *it;
			pBlock->editor_setMeshVisible(bVisible);
		}
	}

	void Scene::editor_saveSphereTerrain(DataStream* stream)
	{
		for (TerrainBlockList::iterator it = m_terrainBlockList.begin(); it != m_terrainBlockList.end(); ++it)
		{
			TerrainBlock* pBlock = *it;
			pBlock->exportData(stream);
		}
	}

	void Scene::loadSphereTerrain(DataStream* stream)
	{
		for (TerrainBlockList::iterator it = m_terrainBlockList.begin(); it != m_terrainBlockList.end(); ++it)
		{
			TerrainBlock* pBlock = *it;
			pBlock->importData(stream);
		}
	}

	void Scene::modifySphereRadius(float radius)
	{
		if (m_SphereRadius != radius)
		{
			setSphereParam(radius, false);
			for (TerrainBlockList::iterator it = m_terrainBlockList.begin(); it != m_terrainBlockList.end(); ++it)
			{
				TerrainBlock* pBlock = *it;
				pBlock->reBuildGeometry();
			}
		}
	}

	void Scene::registerGameObjectToZoom(GameObject* object)
	{
		LordAssert(object);
		Vector3 tmin = object->getWorldAABB().vMin, tmax = object->getWorldAABB().vMax;
		int minx = Math::Max<int>(int(tmin.x / m_widthPerBlock), 0);
		int minz = Math::Max<int>(int(tmin.z / m_heightPerBlock), 0);
		int maxx = Math::Max<int>(Math::Min<int>(int(tmax.x / m_widthPerBlock), m_blockNumHorizontal - 1), 0);
		int maxz = Math::Max<int>(Math::Min<int>(int(tmax.z / m_heightPerBlock), m_blockNumVertical - 1), 0);
		for (int z = minz; z <= maxz; ++z)
		{
			for (int x = minx; x <= maxx; ++x)
			{
				m_zoomList[z*m_blockNumHorizontal + x]->registerObjectToZoom(object);
			}
		}
	}

	void Scene::unregisterGameObjectFromZoom(GameObject* object)
	{
		Vector3 tmin = object->getWorldAABB().vMin, tmax = object->getWorldAABB().vMax;
		int minx = Math::Max<int>(int(tmin.x / m_widthPerBlock), 0);
		int minz = Math::Max<int>(int(tmin.z / m_heightPerBlock), 0);
		int maxx = Math::Max<int>(Math::Min<int>(int(tmax.x / m_widthPerBlock), m_blockNumHorizontal - 1), 0);
		int maxz = Math::Max<int>(Math::Min<int>(int(tmax.z / m_heightPerBlock), m_blockNumVertical - 1), 0);
		for (int z = minz; z <= maxz; ++z)
		{
			for (int x = minx; x <= maxx; ++x)
			{
				m_zoomList[z*m_blockNumHorizontal + x]->unregisterObjectFromZoom(object);
			}
		}
	}

	void Scene::registerGameObjectToZoom_unprepare(GameObject* object)
	{
		LordAssert(object);
		Vector3 tmin = object->getWorldAABB().vMin, tmax = object->getWorldAABB().vMax;
		int minx = Math::Max<int>(int(tmin.x / m_widthPerBlock), 0);
		int minz = Math::Max<int>(int(tmin.z / m_heightPerBlock), 0);
		int maxx = Math::Min<int>(Math::Min<int>(int(tmax.x / m_widthPerBlock), m_blockNumHorizontal - 1), 0);
		int maxz = Math::Min<int>(Math::Min<int>(int(tmax.z / m_heightPerBlock), m_blockNumVertical - 1), 0);
		for (int z = minz; z <= maxz; ++z)
		{
			for (int x = minx; x <= maxx; ++x)
			{
				m_zoomList[z*m_blockNumHorizontal + x]->registerObjectToZoom_UnPrepare(object);
			}
		}
	}

	void Scene::unregisterGameObjectFromZoom_unprepare(GameObject* object)
	{
		LordAssert(object);
		Vector3 tmin = object->getWorldAABB().vMin, tmax = object->getWorldAABB().vMax;
		int minx = Math::Max<int>(int(tmin.x / m_widthPerBlock), 0);
		int minz = Math::Max<int>(int(tmin.z / m_heightPerBlock), 0);
		int maxx = Math::Min<int>(Math::Min<int>(int(tmax.x / m_widthPerBlock), m_blockNumHorizontal - 1), 0);
		int maxz = Math::Min<int>(Math::Min<int>(int(tmax.z / m_heightPerBlock), m_blockNumVertical - 1), 0);
		for (int z = minz; z <= maxz; ++z)
		{
			for (int x = minx; x <= maxx; ++x)
			{
				m_zoomList[z*m_blockNumHorizontal + x]->unregisterObjectFromZoom_UnPrepare(object);
			}
		}
	}

	void Scene::saveSceneToXml(const String& filename)
	{
		xml_document<>* doc = LordNewT(xml_document<>);

		xml_node<>* rot = doc->allocate_node(rapidxml::node_pi, doc->allocate_string("xml version='1.0' encoding='utf-8'"));
		doc->append_node(rot);

		// --------------------------write scene property-------------------------------------------
		// scene block count
		// value
		String value = StringUtil::ToString(m_bHasTerrain);
		char* strValue = doc->allocate_string(value.c_str());
		xml_node<>* hasTerrainNode = doc->allocate_node(rapidxml::node_element, "HasTerrain", strValue);
		doc->append_node(hasTerrainNode);

		// use fov45 camera
		value = StringUtil::ToString(m_bUseCameraFov45);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* useCameraFov45 = doc->allocate_node(rapidxml::node_element, "useCameraFov45", strValue);
		doc->append_node(useCameraFov45);

		// block size;
		Vector2 blockSize(m_widthPerBlock, m_heightPerBlock);
		value = StringUtil::ToString(blockSize);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* blockSizeNode = doc->allocate_node(rapidxml::node_element, "BlockSize", strValue);
		doc->append_node(blockSizeNode);

		// block nums
		Vector2 blockCount(m_blockNumHorizontal, m_blockNumVertical);
		value = StringUtil::ToString(blockCount);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* blockCountNode = doc->allocate_node(rapidxml::node_element, "BlockCount", strValue);
		doc->append_node(blockCountNode);

		// sphere terrain
		value = StringUtil::ToString(m_bSphere);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* SphereNode = doc->allocate_node(rapidxml::node_element, "isSphere", strValue);
		doc->append_node(SphereNode);

		value = StringUtil::ToString(m_SphereRadius);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* SphereRadiusNode = doc->allocate_node(rapidxml::node_element, "SphereRadius", strValue);
		doc->append_node(SphereRadiusNode);

		// default camera Pos
		value = StringUtil::ToString(m_defaultCamraPos);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* defaultCameraPos = doc->allocate_node(rapidxml::node_element, "DefaultCameraPos", strValue);
		doc->append_node(defaultCameraPos);

		// default camera dir
		value = StringUtil::ToString(m_defaultCameraDir);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* defaultCameraDir = doc->allocate_node(rapidxml::node_element, "DefaultCameraDir", strValue);
		doc->append_node(defaultCameraDir);

		// light dir
		value = StringUtil::ToString(m_lightDirection);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* lightdirNode = doc->allocate_node(rapidxml::node_element, "LightDir", strValue);
		doc->append_node(lightdirNode);

		// light color
		value = StringUtil::ToString(m_lightColor);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* lightColorNode = doc->allocate_node(rapidxml::node_element, "LightColor", strValue);
		doc->append_node(lightColorNode);

		// ambient color
		value = StringUtil::ToString(m_ambientColor);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* ambientColorNode = doc->allocate_node(rapidxml::node_element, "AmbientColor", strValue);
		doc->append_node(ambientColorNode);

		// zoom cull
		value = StringUtil::ToString(m_isZoomCull);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* zoomcullNode = doc->allocate_node(rapidxml::node_element, "ZoomCull", strValue);
		doc->append_node(zoomcullNode);

		/*#ifdef LORD_USE_RENDERTARGET
		// postprocess
		value = StringUtil::ToString(PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMean);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* gaussianMeanNode = doc->allocate_node(rapidxml::node_element,"gaussianMean",strValue);
		doc->append_node(gaussianMeanNode);

		value = StringUtil::ToString(PostProcessRenderStage::GaussianBlurringHelper::m_gaussianDev);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* gaussianDevNode = doc->allocate_node(rapidxml::node_element,"gaussianDev",strValue);
		doc->append_node(gaussianDevNode);

		value = StringUtil::ToString(PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMultiplier);
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* gaussianMultiplierNode = doc->allocate_node(rapidxml::node_element,"gaussianMultiplier",strValue);
		doc->append_node(gaussianMultiplierNode);

		PostProcessRenderStage* pPostProcess = (PostProcessRenderStage*)RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess);
		value = StringUtil::ToString(pPostProcess->getSceneBrightnessThreshold());
		strValue = doc->allocate_string(value.c_str());
		xml_node<>* BrightnessNode = doc->allocate_node(rapidxml::node_element, "Brightness", strValue);
		doc->append_node(BrightnessNode);
		#endif*/

		if (m_bHasTerrain)
			// terrain 
		{
			value = StringUtil::BLANK;
			TerrainBlockList::iterator it = m_terrainBlockList.begin();
			for (; it != m_terrainBlockList.end(); ++it)
			{
				value += StringUtil::ToString((*it)->isNeedToBeDraw());
				value += " ";
			}
			strValue = doc->allocate_string(value.c_str());
			xml_node<>* ambientColorNode = doc->allocate_node(rapidxml::node_element, "TerrainBlockVisible", strValue);
			doc->append_node(ambientColorNode);
		}
		// ---------------------------write game object--------------------------------------------
		// root node
		xml_node<>* gameObjectRoot = doc->allocate_node(rapidxml::node_element, "GameObjectRoot", NULL);
		doc->append_node(gameObjectRoot);

		// child node
		GameObjectList::iterator itorObject = m_gameObjectList.begin();
		for (; itorObject != m_gameObjectList.end(); ++itorObject)
		{
			if ((*itorObject)->isNeedSave())
			{
				String objectType = StringUtil::ToString((*itorObject)->getType());
				char* strType = doc->allocate_string(objectType.c_str());
				char* strName = doc->allocate_string((*itorObject)->getName().c_str());

				xml_node<>* gameObject = doc->allocate_node(rapidxml::node_element, "GameObject", NULL);
				gameObject->append_attribute(doc->allocate_attribute("type", strType));
				gameObject->append_attribute(doc->allocate_attribute("name", strName));

				gameObjectRoot->append_node(gameObject);

				(*itorObject)->exportData(gameObject, *doc);
				//// write property
				//GameObject::PropertyList propertyList;
				//(*itorObject)->getPropertyList(propertyList);
				//GameObject::PropertyList::iterator itor = propertyList.begin();
				//for (; itor != propertyList.end(); ++itor)
				//{
				//	String propertyName = (*itor).first;
				//	String proertyValue;
				//	// oritation save this ortation property
				//	if (propertyName == objToken[TOKEN_OBJ_PITCH] ||
				//		propertyName == objToken[TOKEN_OBJ_YAW] ||
				//		propertyName == objToken[TOKEN_OBJ_ROLL] )
				//	{
				//		continue;
				//	}

				//	(*itorObject)->getPropertyValue(propertyName, proertyValue);
				//	char* strPropertyName = doc->allocate_string(propertyName.c_str());
				//	char* strPropertyValue = doc->allocate_string(proertyValue.c_str());

				//	xml_node<>* propertyNode = doc->allocate_node(rapidxml::node_element,"Property",NULL);
				//	propertyNode->append_attribute(doc->allocate_attribute("name", strPropertyName));
				//	propertyNode->append_attribute(doc->allocate_attribute("value", strPropertyValue));

				//	gameObject->append_node(propertyNode);
				//}
			}
		}

		// ---------------------------write zoom--------------------------------------------
		xml_node<>* zoomRoot = doc->allocate_node(rapidxml::node_element, "ZoomRoot", NULL);
		doc->append_node(zoomRoot);

		xml_node<>* zoomCountNode = doc->allocate_node(rapidxml::node_element, "ZoomCount", NULL);
		char* strZoomsizeValue = doc->allocate_string(StringUtil::ToString((ui32)m_zoomList.size()).c_str());
		zoomCountNode->append_attribute(doc->allocate_attribute("value", strZoomsizeValue));
		zoomRoot->append_node(zoomCountNode);

		ZoomList::iterator itzoom = m_zoomList.begin();
		for (; itzoom != m_zoomList.end(); ++itzoom)
		{
			Zoom* pZoom = *itzoom;
			const Zoom::GameObjectList& lst = pZoom->getGameObjecs();

			xml_node<>* zoomNode = doc->allocate_node(rapidxml::node_element, "Zoom", NULL);
			char* strObjectcountValue = doc->allocate_string(StringUtil::ToString((ui32)lst.size()).c_str());
			zoomNode->append_attribute(doc->allocate_attribute("ObjectCount", strObjectcountValue));
			zoomCountNode->append_node(zoomNode);

			Zoom::GameObjectList::iterator itobject = lst.begin();
			for (; itobject != lst.end(); ++itobject)
			{
				GameObject* pObj = *itobject;
				xml_node<>* objectNode = doc->allocate_node(rapidxml::node_element, "Object", NULL);
				char* ObjectnameValue = doc->allocate_string(pObj->getName().c_str());
				objectNode->append_attribute(doc->allocate_attribute("Name", ObjectnameValue));
				zoomNode->append_node(objectNode);
			}
		}

		xml_node<>* CameraRoot = doc->allocate_node(rapidxml::node_element, "CameraRoot", NULL);
		doc->append_node(CameraRoot);
		xml_node<>* nearclipNode = doc->allocate_node(rapidxml::node_element, "NearClip", NULL);
		xml_node<>* farclipNode = doc->allocate_node(rapidxml::node_element, "FarClip", NULL);
		xml_node<>* fovclipNode = doc->allocate_node(rapidxml::node_element, "Fov", NULL);
		char* strnearclip = doc->allocate_string(StringUtil::ToString(SceneManager::Instance()->getMainCamera()->getNearClip()).c_str());
		char* strfarclip = doc->allocate_string(StringUtil::ToString(SceneManager::Instance()->getMainCamera()->getFarClip()).c_str());
		char* strfov = doc->allocate_string(StringUtil::ToString(SceneManager::Instance()->getMainCamera()->getFov()).c_str());

		nearclipNode->append_attribute(doc->allocate_attribute("value", strnearclip));
		farclipNode->append_attribute(doc->allocate_attribute("value", strfarclip));
		fovclipNode->append_attribute(doc->allocate_attribute("value", strfov));
		CameraRoot->append_node(nearclipNode);
		CameraRoot->append_node(farclipNode);
		CameraRoot->append_node(fovclipNode);

		xml_node<>* FogRoot = doc->allocate_node(rapidxml::node_element, "FogRoot", NULL);
		doc->append_node(FogRoot);
		xml_node<>* StartNode = doc->allocate_node(rapidxml::node_element, "FogStart", NULL);
		xml_node<>* EndNode = doc->allocate_node(rapidxml::node_element, "FogEnd", NULL);
		xml_node<>* DensityNode = doc->allocate_node(rapidxml::node_element, "DensityEnd", NULL);
		xml_node<>* FogcolorNode = doc->allocate_node(rapidxml::node_element, "FogColor", NULL);

		Vector3 fogparam = SceneManager::Instance()->getFogParam();
		Color fogcolor = SceneManager::Instance()->getFogColor();
		char* startval = doc->allocate_string(StringUtil::ToString(fogparam.x).c_str());
		char* endval = doc->allocate_string(StringUtil::ToString(fogparam.y).c_str());
		char* densityval = doc->allocate_string(StringUtil::ToString(fogparam.z).c_str());
		char* colorval = doc->allocate_string(StringUtil::ToString(fogcolor).c_str());

		StartNode->append_attribute(doc->allocate_attribute("value", startval));
		EndNode->append_attribute(doc->allocate_attribute("value", endval));
		DensityNode->append_attribute(doc->allocate_attribute("value", densityval));
		FogcolorNode->append_attribute(doc->allocate_attribute("value", colorval));
		FogRoot->append_node(StartNode);
		FogRoot->append_node(EndNode);
		FogRoot->append_node(DensityNode);
		FogRoot->append_node(FogcolorNode);


		xml_node<>* OceanRoot = doc->allocate_node(rapidxml::node_element, "OceanRoot", NULL);
		doc->append_node(OceanRoot);
		if (m_pOceanRender)
		{
			m_pOceanRender->exportData(OceanRoot, *doc);
		}

		std::ofstream out(filename.c_str());
		out << *doc;
		LordDeleteT(doc, xml_document<>);
	}

	void Scene::loadSceneFromXml()
	{
		String filename = m_sceneName + ".scene";
		try
		{
			DataStream* pXMLStream = ResourceGroupManager::Instance()->openResource(filename);
			if (pXMLStream == NULL)
			{
				LordLogError("scene file not found [%s].", filename.c_str());
				return;
			}

			size_t fileSize = pXMLStream->size();
			char* xmlData = (char*)LordMalloc(fileSize + 1);
			pXMLStream->read(xmlData, fileSize);
			xmlData[fileSize] = 0;

			xml_document<>* doc = LordNewT(xml_document<>);        // character type defaults to cha
			doc->parse<0>(xmlData);

			// ---------------------------first section-------------------------------------
			// ----------------------------block number ----------------------------------
			xml_node<> *pBlockCountNode = doc->first_node();
			if (!pBlockCountNode)
			{
				LordLogError("The scene file content is valid.");
				throw 0;
			}

			String sceneName = PathUtil::GetPureFilename(filename, false);
			String blockNodeName = pBlockCountNode->name();
			if (blockNodeName != "HasTerrain")
			{
				LordLogError("The scene file content is valid. It does not contain the Block Count!");
				throw 0;
			}
			String strHasTerrain = pBlockCountNode->value();
			m_bHasTerrain = StringUtil::ParseBool(strHasTerrain);
			bool isDisableStream = !StreamThread::Instance()->IsRunning();

			xml_node<>* temp = pBlockCountNode->next_sibling();
			String name = temp->name();
			String value;
			while (name != "GameObjectRoot")
			{
				value = temp->value();
				if (name == "useCameraFov45")
				{
					bool useCameraFov45 = StringUtil::ParseBool(value);
					setUseCameraFov45(useCameraFov45);
					SceneManager::Instance()->setComputeActorFog(useCameraFov45);
				}
				else if (name == "BlockCount")
				{
					Vector2 blockCount = StringUtil::ParseVec2(value);
					setBlockCount(ui16(blockCount.x), ui16(blockCount.y));
				}
				else if (name == "BlockSize")
				{
					Vector2 blockSize = StringUtil::ParseVec2(value);
					setSizeofBlock(blockSize.x, blockSize.y);
				}
				else if (name == "isSphere")
				{
					m_bSphere = StringUtil::ParseBool(value);
				}
				else if (name == "SphereRadius")
				{
					setSphereParam(StringUtil::ParseFloat(value), false);
				}
				else if (name == "DefaultCameraPos")
				{
					m_defaultCamraPos = StringUtil::ParseVec3(value);
				}
				else if (name == "DefaultCameraDir")
				{
					m_defaultCameraDir = StringUtil::ParseVec3(value);
				}
				else if (name == "LightDir")
				{
					m_lightDirection = StringUtil::ParseVec3(value);
					SceneManager::Instance()->setMainLightDir(m_lightDirection);
				}
				else if (name == "LightColor")
				{
					m_lightColor = StringUtil::ParseColor(value);
					SceneManager::Instance()->setMainLightColor(m_lightColor);
				}
				else if (name == "AmbientColor")
				{
					m_ambientColor = StringUtil::ParseColor(value);
					SceneManager::Instance()->setAmbientColor(m_ambientColor);
				}
				else if (name == "ZoomCull")
				{
					m_isZoomCull = StringUtil::ParseBool(value);
				}
				else if (name == "TerrainBlockVisible")
				{
					StringArray stringArr = StringUtil::Split(value, " ");
					StringArray::iterator its = stringArr.begin();
					TerrainBlockList::iterator it = m_terrainBlockList.begin();
					for (; it != m_terrainBlockList.end() && its != stringArr.end(); ++it, ++its)
					{
						(*it)->setNeedToBeDraw(StringUtil::ParseBool(*its));
					}
				}
				//#ifdef LORD_USE_RENDERTARGET
				//else if (name == "gaussianMean")
				//{
				//	PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMean =  StringUtil::ParseFloat(value);
				//}
				//else if (name == "gaussianDev")
				//{
				//	PostProcessRenderStage::GaussianBlurringHelper::m_gaussianDev =  StringUtil::ParseFloat(value);
				//}
				//else if (name == "gaussianMultiplier")
				//{
				//	PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMultiplier =  StringUtil::ParseFloat(value);
				//}
				//else if (name == "Brightness")
				//{
				//	PostProcessRenderStage* pStage = (PostProcessRenderStage*)RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess);
				//                if (pStage)
				//	{
				//                    pStage->setSceneBrightnessThreshold(StringUtil::ParseFloat(value));
				//                }
				//}
				//#endif
				temp = temp->next_sibling();
				name = temp->name();
			}

			if (m_bUseCameraFov45)
			{
				LightmapMgr* pLmMgr = LightmapMgr::Instance();
				pLmMgr->load();
			}

			InitScene();
			prepare();
			//------------------------------second section--------------------------------------
			// ------------------------------game object list--------------------------------------
			xml_node<> *pGameObjectRootNode = temp;
			if (!pGameObjectRootNode)
			{
				LordLogError("The scene file content is valid.It does not contain game object root node!");
				throw 0;
			}

			// parse sub nodes
			xml_node<> *pGameObjectNode = pGameObjectRootNode->first_node();
			while (pGameObjectNode)
			{
				// first decide game object type
				xml_attribute<> *pObjectType = pGameObjectNode->first_attribute();
				String objectType = pObjectType->value();
				xml_attribute<> *pObjectName = pObjectType->next_attribute();
				String objectName = pObjectName->value();

				assert(getGameObject(name) == NULL);
				// create object
				GameObject* newObject = createGameObject(StringUtil::ParseUI32(objectType));
				LordAssert(newObject);
				newObject->setName(objectName);

				i32 idname = StringUtil::ParseI32(objectName);
				if (mSceneObjectID <= idname)
					mSceneObjectID = idname + 1;

				newObject->importData(pGameObjectNode);
				//// parse property nodes
				//xml_node<> *pPropertyNode = pGameObjectNode->first_node();
				//while(pPropertyNode)
				//{
				//	xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				//	String propertyName = pPropertyName->value();
				//	xml_attribute<> *pPropertyValue = pPropertyName->next_attribute();
				//	String propertyValue = pPropertyValue->value();

				//	newObject->setPropertyValue(propertyName, propertyValue);

				//	pPropertyNode = pPropertyNode->next_sibling();
				//}

				// res prepare multi thread?
				if (isDisableStream)
				{
					newObject->_prepare_sys();
				}
				else if (!m_isZoomCull)
				{
					if ((newObject->m_Type != GOT_POINTLIGHT && newObject->m_Type != GOT_DIRLIGHT && newObject->m_Type != GOT_SPOTLIGHT))
					{
						newObject->_prepare_res();		//资源对象
						newObject->_prepare_io();		//磁盘数据
						newObject->_prepare_obj();		//渲染资源
					}
				}

				pGameObjectNode = pGameObjectNode->next_sibling();
			} // while

			xml_node<>* pZoomRootNode = pGameObjectRootNode->next_sibling();
			if (pZoomRootNode)
			{
				xml_node<>* pZoomCountNode = pZoomRootNode->first_node();
				if (pZoomCountNode)
				{
					xml_attribute<>* coutValue = pZoomCountNode->first_attribute();
					ui32 count = StringUtil::ParseUI32(String(coutValue->value()));

					xml_node<>* pZoomNode = pZoomCountNode->first_node();
					for (ui32 i = 0; i<count; ++i)
					{
						xml_attribute<>* objectcountVal = pZoomNode->first_attribute();
						assert(objectcountVal);
						ui32 objectcout = StringUtil::ParseUI32(String(objectcountVal->value()));
						xml_node<>* objectNode = pZoomNode->first_node();
						for (ui32 j = 0; j<objectcout; ++j)
						{
							xml_attribute<>* objectNameNode = objectNode->first_attribute();
							GameObject* pObject = getGameObject(String(objectNameNode->value()));
							if (pObject)
							{
								m_zoomList[i]->registerObjectToZoom_UnPrepare(pObject);
							}

							objectNode = objectNode->next_sibling();
						}
						pZoomNode = pZoomNode->next_sibling();
					}
				}
				else
				{
					LordLogError("there is no count of zoom info!");
				}

				xml_node<>* pCameraNode = pZoomRootNode->next_sibling();
				if (pCameraNode)
				{
					xml_node<>* pNearNode = pCameraNode->first_node();
					xml_attribute<>* NearValue = pNearNode->first_attribute();
					assert(NearValue);
					SceneManager::Instance()->getMainCamera()->setNearClip(StringUtil::ParseFloat(String(NearValue->value())));

					xml_node<>* pFarNode = pNearNode->next_sibling();
					xml_attribute<>* FarValue = pFarNode->first_attribute();
					assert(FarValue);
					SceneManager::Instance()->getMainCamera()->setFarClip(StringUtil::ParseFloat(String(FarValue->value())));

					xml_node<>* pFovNode = pFarNode->next_sibling();
					if (pFovNode)
					{
						xml_attribute<>* fovValue = pFovNode->first_attribute();
						assert(fovValue);
						SceneManager::Instance()->getMainCamera()->setFov(StringUtil::ParseFloat(String(fovValue->value())));
					}

					xml_node<>* pFogNode = pCameraNode->next_sibling();
					if (pFogNode)
					{
						Vector3 param;
						Color color;
						xml_node<>* pStartNode = pFogNode->first_node();
						xml_attribute<>* StartValue = pStartNode->first_attribute();

						xml_node<>* pEndNode = pStartNode->next_sibling();
						xml_attribute<>* EndValue = pEndNode->first_attribute();

						xml_node<>* pDensityNode = pEndNode->next_sibling();
						xml_attribute<>* DensityValue = pDensityNode->first_attribute();

						xml_node<>* pColorNode = pDensityNode->next_sibling();
						xml_attribute<>* ColorValue = pColorNode->first_attribute();

						param.x = StringUtil::ParseFloat(String(StartValue->value()));
						param.y = StringUtil::ParseFloat(String(EndValue->value()));
						param.z = StringUtil::ParseFloat(String(DensityValue->value()));
						color = StringUtil::ParseColor(String(ColorValue->value()));
						param.y = Math::Max(param.y, param.x);
						LordLogInfo("fogstart:%s, fogend:%s, fogDensity:%s \n", StartValue->value(), EndValue->value(), DensityValue->value());
						SceneManager::Instance()->setFogParam(param);
						SceneManager::Instance()->setFogColor(color);

						xml_node<>* pOceanNode = pFogNode->next_sibling();
						if (pOceanNode)
						{
							if (pOceanNode->first_node())
							{
								CreateOcean();
								m_pOceanRender->importData(pOceanNode);
								m_pOceanRender->prepare_sys();
							}
						}//if (pOceanNode)
					}//if (pFogNode)
				}//if (pCameraNode)
			}
			else
			{
				LordLogError("this scene is old format, please resave it!");
			}

			LordDeleteT(doc, xml_document<>);
			LordFree(xmlData);
			LordDelete pXMLStream;

			//if (!m_bEditorMode)
			//{
			//	String strNavFile = m_sceneName + ".nav";
			//	DataStream* pNavStream = ResourceGroupManager::Instance()->openResource(strNavFile);
			//	if(pNavStream)
			//	{
			//		m_pNavigation->importData(pNavStream);
			//		LordDelete pNavStream;
			//	}
			//}
		}
		catch (...)
		{
			LordLogError("Parse dialog file [%s] failed.", filename.c_str());
		}
	}

	void Scene::saveSceneToJson(const String& filename)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->SetObject();


		// --------------------------write scene property-------------------------------------------
		// scene block count
		// value
		String value = StringUtil::ToString(m_bHasTerrain);
		rapidjson::Value hasTerrainNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("HasTerrain", hasTerrainNode, doc->GetAllocator());

		// use fov45 camera
		value = StringUtil::ToString(m_bUseCameraFov45);
		rapidjson::Value useCameraFov45(value.c_str(), doc->GetAllocator());
		doc->AddMember("useCameraFov45", useCameraFov45, doc->GetAllocator());

		// block size;
		Vector2 blockSize(m_widthPerBlock, m_heightPerBlock);
		value = StringUtil::ToString(blockSize);
		rapidjson::Value blockSizeNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("BlockSize", blockSizeNode, doc->GetAllocator());

		// block nums
		Vector2 blockCount(m_blockNumHorizontal, m_blockNumVertical);
		value = StringUtil::ToString(blockCount);
		rapidjson::Value blockCountNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("BlockCount", blockCountNode, doc->GetAllocator());

		// sphere terrain
		value = StringUtil::ToString(m_bSphere);
		rapidjson::Value SphereNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("isSphere", SphereNode, doc->GetAllocator());

		value = StringUtil::ToString(m_SphereRadius);
		rapidjson::Value SphereRadiusNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("SphereRadius", SphereRadiusNode, doc->GetAllocator());

		// default camera Pos
		value = StringUtil::ToString(m_defaultCamraPos);
		rapidjson::Value defaultCameraPos(value.c_str(), doc->GetAllocator());
		doc->AddMember("DefaultCameraPos", defaultCameraPos, doc->GetAllocator());

		// default camera dir
		value = StringUtil::ToString(m_defaultCameraDir);
		rapidjson::Value defaultCameraDir(value.c_str(), doc->GetAllocator());
		doc->AddMember("DefaultCameraDir", defaultCameraDir, doc->GetAllocator());

		// light dir
		value = StringUtil::ToString(m_lightDirection);
		rapidjson::Value lightdirNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("LightDir", lightdirNode, doc->GetAllocator());

		// light color
		value = StringUtil::ToString(m_lightColor);
		rapidjson::Value lightColorNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("LightColor", lightColorNode, doc->GetAllocator());

		// ambient color
		value = StringUtil::ToString(m_ambientColor);
		rapidjson::Value ambientColorNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("AmbientColor", ambientColorNode, doc->GetAllocator());

		// zoom cull
		value = StringUtil::ToString(m_isZoomCull);
		rapidjson::Value zoomcullNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("ZoomCull", zoomcullNode, doc->GetAllocator());

		/*#ifdef LORD_USE_RENDERTARGET
		// postprocess
		value = StringUtil::ToString(PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMean);
		rapidjson::Value gaussianMeanNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("gaussianMean", gaussianMeanNode, doc->GetAllocator());

		value = StringUtil::ToString(PostProcessRenderStage::GaussianBlurringHelper::m_gaussianDev);
		rapidjson::Value gaussianDevNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("gaussianDev", gaussianDevNode, doc->GetAllocator());

		value = StringUtil::ToString(PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMultiplier);
		rapidjson::Value gaussianMultiplierNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("gaussianMultiplier", gaussianMultiplierNode, doc->GetAllocator());

		PostProcessRenderStage* pPostProcess = (PostProcessRenderStage*)RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess);
		value = StringUtil::ToString(pPostProcess->getSceneBrightnessThreshold());
		rapidjson::Value BrightnessNode(value.c_str(), doc->GetAllocator());
		doc->AddMember("Brightness", BrightnessNode, doc->GetAllocator());
		#endif*/

		if (m_bHasTerrain)
			// terrain 
		{
			value = StringUtil::BLANK;
			TerrainBlockList::iterator it = m_terrainBlockList.begin();
			for (; it != m_terrainBlockList.end(); ++it)
			{
				value += StringUtil::ToString((*it)->isNeedToBeDraw());
				value += " ";
			}
			rapidjson::Value ambientColorNode(value.c_str(), doc->GetAllocator());
			doc->AddMember("TerrainBlockVisible", ambientColorNode, doc->GetAllocator());
		}
		// ---------------------------write game object--------------------------------------------
		// root node

		rapidjson::Value gameObjectRoot(rapidjson::kObjectType);
		rapidjson::Value& refGameObjectRoot = (*doc)["GameObjectRoot"];
		doc->AddMember("GameObjectRoot", gameObjectRoot, doc->GetAllocator());
		rapidjson::Value gameObjects(rapidjson::kArrayType);
		refGameObjectRoot.AddMember("GameObject", gameObjects, doc->GetAllocator());
		rapidjson::Value& refGameObjects = refGameObjectRoot["GameObject"];

		// child node
		GameObjectList::iterator itorObject = m_gameObjectList.begin();
		for (; itorObject != m_gameObjectList.end(); ++itorObject)
		{
			if ((*itorObject)->isNeedSave())
			{
				String objectType = StringUtil::ToString((*itorObject)->getType());
				rapidjson::Value valType(objectType.c_str(), doc->GetAllocator());
				rapidjson::Value valName(((*itorObject)->getName().c_str()), doc->GetAllocator());

				rapidjson::Value gameObject(rapidjson::kObjectType);
				gameObject.AddMember("type", valType, doc->GetAllocator());
				gameObject.AddMember("name", valName, doc->GetAllocator());

				(*itorObject)->exportData(gameObject, doc);

				refGameObjects.PushBack(gameObject, doc->GetAllocator());

			}
		}

		// ---------------------------write zoom--------------------------------------------


		rapidjson::Value zoomRoot(rapidjson::kObjectType);
		doc->AddMember("ZoomRoot", zoomRoot, doc->GetAllocator());
		rapidjson::Value& refZoomRoot = (*doc)["ZoomRoot"];
		rapidjson::Value zooms(rapidjson::kArrayType);
		refZoomRoot.AddMember("Zoom", zooms, doc->GetAllocator());
		rapidjson::Value& refZooms = refZoomRoot["Zoom"];

		ZoomList::iterator itzoom = m_zoomList.begin();
		for (; itzoom != m_zoomList.end(); ++itzoom)
		{
			Zoom* pZoom = *itzoom;
			const Zoom::GameObjectList& lst = pZoom->getGameObjecs();

			rapidjson::Value zoom(rapidjson::kObjectType);
			rapidjson::Value objects(rapidjson::kArrayType);


			Zoom::GameObjectList::iterator itobject = lst.begin();
			for (; itobject != lst.end(); ++itobject)
			{
				GameObject* pObj = *itobject;
				rapidjson::Value object(rapidjson::kObjectType);
				rapidjson::Value objectName(pObj->getName().c_str(), doc->GetAllocator());
				object.AddMember("Name", objectName, doc->GetAllocator());
				objects.PushBack(object, doc->GetAllocator());
			}

			zoom.AddMember("Object", objects, doc->GetAllocator());
			refZooms.PushBack(zoom, doc->GetAllocator());
		}

		rapidjson::Value cameraRoot(rapidjson::kObjectType);
		doc->AddMember("CameraRoot", cameraRoot, doc->GetAllocator());
		rapidjson::Value& refCameraRoot = (*doc)["CameraRoot"];

		String strNearClip = StringUtil::ToString(SceneManager::Instance()->getMainCamera()->getNearClip());
		String strFarClip = StringUtil::ToString(SceneManager::Instance()->getMainCamera()->getFarClip());
		String strFov = StringUtil::ToString(SceneManager::Instance()->getMainCamera()->getFov());

		rapidjson::Value valNearClip(strNearClip.c_str(), doc->GetAllocator());
		rapidjson::Value valFarClip(strFarClip.c_str(), doc->GetAllocator());
		rapidjson::Value valFov(strFov.c_str(), doc->GetAllocator());

		refCameraRoot.AddMember("NearClip", valNearClip, doc->GetAllocator());
		refCameraRoot.AddMember("FarClip", valFarClip, doc->GetAllocator());
		refCameraRoot.AddMember("Fov", valFov, doc->GetAllocator());

		rapidjson::Value fogRoot(rapidjson::kObjectType);
		doc->AddMember("FogRoot", fogRoot, doc->GetAllocator());
		rapidjson::Value& refFogRoot = (*doc)["FogRoot"];

		Vector3 fogparam = SceneManager::Instance()->getFogParam();
		Color fogcolor = SceneManager::Instance()->getFogColor();
		String strStart = StringUtil::ToString(fogparam.x);
		String strEnd = StringUtil::ToString(fogparam.y);
		String strDensity = StringUtil::ToString(fogparam.z);
		String strColor = StringUtil::ToString(fogcolor);

		rapidjson::Value valStart(strStart.c_str(), doc->GetAllocator());
		rapidjson::Value valEnd(strEnd.c_str(), doc->GetAllocator());
		rapidjson::Value valDensity(strDensity.c_str(), doc->GetAllocator());
		rapidjson::Value valColor(strColor.c_str(), doc->GetAllocator());

		refFogRoot.AddMember("FogStart", valStart, doc->GetAllocator());
		refFogRoot.AddMember("FogEnd", valEnd, doc->GetAllocator());
		refFogRoot.AddMember("DensityEnd", valDensity, doc->GetAllocator());
		refFogRoot.AddMember("FogColor", valColor, doc->GetAllocator());

		rapidjson::Value oceanRoot(rapidjson::kObjectType);
		doc->AddMember("OceanRoot", oceanRoot, doc->GetAllocator());
		rapidjson::Value& refOceanRoot = (*doc)["OceanRoot"];

		if (m_pOceanRender)
		{
			m_pOceanRender->exportData(refOceanRoot, doc);
		}

		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		doc->Accept(writer);

		std::ofstream out(filename.c_str());
		out << sb.GetString();

		delete doc;
	}

	void Scene::loadSceneFromJson()
	{
		String filename = m_sceneName + ".scene";
		try
		{
			DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(filename);
			if (pDataStream == NULL)
			{
				LordLogError("scene file not found [%s].", filename.c_str());
				return;
			}

			size_t fileSize = pDataStream->size();
			char* pJSONData = (char*)LordMalloc(fileSize + 1);
			pDataStream->read(pJSONData, fileSize);
			pJSONData[fileSize] = 0;

			rapidjson::Document* doc = new rapidjson::Document;
			doc->Parse(pJSONData);

			// ---------------------------first section-------------------------------------
			// ----------------------------block number ----------------------------------

			rapidjson::Value::MemberIterator itBlockCountNode = doc->MemberBegin();
			if (itBlockCountNode == doc->MemberEnd())
			{
				LordLogError("The scene file content is valid.");
				throw 0;
			}

			String sceneName = PathUtil::GetPureFilename(filename, false);
			String blockNodeName = itBlockCountNode->name.GetString();
			if (blockNodeName != "HasTerrain")
			{
				LordLogError("The scene file content is valid. It does not contain the Block Count!");
				throw 0;
			}
			String strHasTerrain = itBlockCountNode->value.GetString();
			m_bHasTerrain = StringUtil::ParseBool(strHasTerrain);
			bool isDisableStream = !StreamThread::Instance()->IsRunning();


			rapidjson::Value::MemberIterator itTemp = itBlockCountNode + 1;

			String name = itTemp->name.GetString();
			String value;
			while (name != "GameObjectRoot")
			{
				value = itTemp->value.GetString();
				if (name == "useCameraFov45")
				{
					bool useCameraFov45 = StringUtil::ParseBool(value);
					setUseCameraFov45(useCameraFov45);
					SceneManager::Instance()->setComputeActorFog(useCameraFov45);
				}
				else if (name == "BlockCount")
				{
					Vector2 blockCount = StringUtil::ParseVec2(value);
					setBlockCount(ui16(blockCount.x), ui16(blockCount.y));
				}
				else if (name == "BlockSize")
				{
					Vector2 blockSize = StringUtil::ParseVec2(value);
					setSizeofBlock(blockSize.x, blockSize.y);
				}
				else if (name == "isSphere")
				{
					m_bSphere = StringUtil::ParseBool(value);
				}
				else if (name == "SphereRadius")
				{
					setSphereParam(StringUtil::ParseFloat(value), false);
				}
				else if (name == "DefaultCameraPos")
				{
					m_defaultCamraPos = StringUtil::ParseVec3(value);
				}
				else if (name == "DefaultCameraDir")
				{
					m_defaultCameraDir = StringUtil::ParseVec3(value);
				}
				else if (name == "LightDir")
				{
					m_lightDirection = StringUtil::ParseVec3(value);
					SceneManager::Instance()->setMainLightDir(m_lightDirection);
				}
				else if (name == "LightColor")
				{
					m_lightColor = StringUtil::ParseColor(value);
					SceneManager::Instance()->setMainLightColor(m_lightColor);
				}
				else if (name == "AmbientColor")
				{
					m_ambientColor = StringUtil::ParseColor(value);
					SceneManager::Instance()->setAmbientColor(m_ambientColor);
				}
				else if (name == "ZoomCull")
				{
					m_isZoomCull = StringUtil::ParseBool(value);
				}
				else if (name == "TerrainBlockVisible")
				{
					StringArray stringArr = StringUtil::Split(value, " ");
					StringArray::iterator its = stringArr.begin();
					TerrainBlockList::iterator it = m_terrainBlockList.begin();
					for (; it != m_terrainBlockList.end() && its != stringArr.end(); ++it, ++its)
					{
						(*it)->setNeedToBeDraw(StringUtil::ParseBool(*its));
					}
				}
				//#ifdef LORD_USE_RENDERTARGET
				//else if (name == "gaussianMean")
				//{
				//	PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMean =  StringUtil::ParseFloat(value);
				//}
				//else if (name == "gaussianDev")
				//{
				//	PostProcessRenderStage::GaussianBlurringHelper::m_gaussianDev =  StringUtil::ParseFloat(value);
				//}
				//else if (name == "gaussianMultiplier")
				//{
				//	PostProcessRenderStage::GaussianBlurringHelper::m_gaussianMultiplier =  StringUtil::ParseFloat(value);
				//}
				//else if (name == "Brightness")
				//{
				//	PostProcessRenderStage* pStage = (PostProcessRenderStage*)RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess);
				//                if (pStage)
				//	{
				//                    pStage->setSceneBrightnessThreshold(StringUtil::ParseFloat(value));
				//                }
				//}
				//#endif
				++itTemp;
				name = itTemp->name.GetString();
			}

			if (m_bUseCameraFov45)
			{
				LightmapMgr* pLmMgr = LightmapMgr::Instance();
				pLmMgr->load();
			}

			InitScene();
			prepare();
			//------------------------------second section--------------------------------------
			// ------------------------------game object list--------------------------------------
			rapidjson::Value::MemberIterator itGameObjectRootNode = doc->FindMember("GameObjectRoot");

			if (itGameObjectRootNode == doc->MemberEnd())
			{
				LordLogError("The scene file content is valid.It does not contain game object root node!");
				throw 0;
			}

			// parse sub nodes
			rapidjson::Value::MemberIterator itGameObject = itGameObjectRootNode->value.MemberBegin();
			rapidjson::Value::ValueIterator it = itGameObject->value.Begin();

			for (; it != itGameObject->value.End(); ++it)
			{
				// first decide game object type				
				String objectType = (*it)["type"].GetString();
				String objectName = (*it)["name"].GetString();

				assert(getGameObject(name) == NULL);
				// create object
				GameObject* newObject = createGameObject(StringUtil::ParseUI32(objectType));
				LordAssert(newObject);
				newObject->setName(objectName);

				i32 idname = StringUtil::ParseI32(objectName);
				if (mSceneObjectID <= idname)
					mSceneObjectID = idname + 1;

				//
				newObject->importData(*it);
				//// parse property nodes
				//xml_node<> *pPropertyNode = pGameObjectNode->first_node();
				//while(pPropertyNode)
				//{
				//	xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
				//	String propertyName = pPropertyName->value();
				//	xml_attribute<> *pPropertyValue = pPropertyName->next_attribute();
				//	String propertyValue = pPropertyValue->value();

				//	newObject->setPropertyValue(propertyName, propertyValue);

				//	pPropertyNode = pPropertyNode->next_sibling();
				//}

				// res prepare multi thread?
				if (isDisableStream)
				{
					newObject->_prepare_sys();
				}
				else if (!m_isZoomCull)
				{
					if ((newObject->m_Type != GOT_POINTLIGHT && newObject->m_Type != GOT_DIRLIGHT && newObject->m_Type != GOT_SPOTLIGHT))
					{
						newObject->_prepare_res();		//资源对象
						newObject->_prepare_io();		//磁盘数据
						newObject->_prepare_obj();		//渲染资源
					}
				}

			} // while

			rapidjson::Value::MemberIterator itZoomRootNode = doc->FindMember("ZoomRoot");
			rapidjson::Value::MemberIterator itZoomNode = itZoomRootNode->value.FindMember("Zoom");
			if (itZoomNode != doc->MemberEnd())
			{
				for (ui32 i = 0; i < itZoomNode->value.Size(); ++i)
				{
					rapidjson::Value::MemberIterator itObjectNode = itZoomNode->value[i].FindMember("Object");
					rapidjson::Value::ValueIterator itObject = itObjectNode->value.Begin();
					for (; itObject != itObjectNode->value.End(); ++itObject)
					{
						GameObject* pObject = getGameObject((*itObject)["Name"].GetString());
						if (pObject)
						{
							m_zoomList[i]->registerObjectToZoom_UnPrepare(pObject);
						}
					}
				}



				rapidjson::Value::MemberIterator itCameraRoot = doc->FindMember("CameraRoot");

				if (itCameraRoot != doc->MemberEnd())
				{
					rapidjson::Value::MemberIterator itNearClip = itCameraRoot->value.FindMember("NearClip");
					SceneManager::Instance()->getMainCamera()->setNearClip(StringUtil::ParseFloat(itNearClip->value.GetString()));

					rapidjson::Value::MemberIterator itFarClip = itCameraRoot->value.FindMember("FarClip");
					SceneManager::Instance()->getMainCamera()->setFarClip(StringUtil::ParseFloat(itFarClip->value.GetString()));

					rapidjson::Value::MemberIterator itFov = itCameraRoot->value.FindMember("Fov");
					if (itFov != itCameraRoot->value.MemberEnd())
					{
						SceneManager::Instance()->getMainCamera()->setFov(StringUtil::ParseFloat(itFov->value.GetString()));
					}

					rapidjson::Value::MemberIterator itFogRoot = doc->FindMember("FogRoot");
					if (itFogRoot != doc->MemberEnd())
					{
						Vector3 param;
						Color color;

						rapidjson::Value::MemberIterator itFogStart = itFogRoot->value.FindMember("FogStart");
						rapidjson::Value::MemberIterator itFogEnd = itFogRoot->value.FindMember("FogEnd");
						rapidjson::Value::MemberIterator itDensityEnd = itFogRoot->value.FindMember("DensityEnd");
						rapidjson::Value::MemberIterator itFogColor = itFogRoot->value.FindMember("FogColor");


						param.x = StringUtil::ParseFloat(itFogStart->value.GetString());
						param.y = StringUtil::ParseFloat(itFogEnd->value.GetString());
						param.z = StringUtil::ParseFloat(itDensityEnd->value.GetString());
						color = StringUtil::ParseColor(itFogColor->value.GetString());
						param.y = Math::Max(param.y, param.x);

						LordLogInfo("fogstart:%s, fogend:%s, fogDensity:%s \n",
							itFogStart->value.GetString(), itFogEnd->value.GetString(), itDensityEnd->value.GetString());

						SceneManager::Instance()->setFogParam(param);
						SceneManager::Instance()->setFogColor(color);


						rapidjson::Value::MemberIterator itOceanRoot = doc->FindMember("OceanRoot");
						if (itOceanRoot != doc->MemberEnd())
						{

							CreateOcean();
							m_pOceanRender->importData(itOceanRoot->value);
							m_pOceanRender->prepare_sys();

						}//if (pOceanNode)
					}//if (pFogNode)
				}//if (pCameraNode)
			}
			else
			{
				LordLogError("this scene is old format, please resave it!");
			}

			delete doc;
			LordFree(pJSONData);
			LordDelete(pDataStream);

			//if (!m_bEditorMode)
			//{
			//	String strNavFile = m_sceneName + ".nav";
			//	DataStream* pNavStream = ResourceGroupManager::Instance()->openResource(strNavFile);
			//	if(pNavStream)
			//	{
			//		m_pNavigation->importData(pNavStream);
			//		LordDelete pNavStream;
			//	}
			//}
		}
		catch (...)
		{
			LordLogError("Parse dialog file [%s] failed.", filename.c_str());
		}
	}

	void Scene::loadSceneObject(float x, float y)
	{
		const GridZoom* gridlist;
		int arraysize;
		GridZoom curZoom, indexGrid;
		curZoom.x = i16(x) / (i16)m_widthPerBlock;
		curZoom.z = i16(y) / (i16)m_heightPerBlock;

		m_loadinglist.clear();
		for (int l = 0; l<m_loadlevel; ++l)
		{
			if (l == 0) { gridlist = sFieldZoom1; arraysize = sizeof(sFieldZoom1) / sizeof(sFieldZoom1[0]); }
			else if (l == 1) { gridlist = sFieldZoom2; arraysize = sizeof(sFieldZoom2) / sizeof(sFieldZoom2[0]); }
			else if (l == 2) { gridlist = sFieldZoom3; arraysize = sizeof(sFieldZoom3) / sizeof(sFieldZoom3[0]); }
			else { gridlist = sFieldZoom4; arraysize = sizeof(sFieldZoom4) / sizeof(sFieldZoom4[0]); }

			for (int f = 0; f<arraysize; ++f)
			{
				indexGrid.x = curZoom.x + gridlist[f].x;
				indexGrid.z = curZoom.z + gridlist[f].z;
				if (indexGrid.x<0 || indexGrid.x >= m_blockNumHorizontal
					|| indexGrid.z<0 || indexGrid.z >= m_blockNumVertical)
				{
					continue;
				}
				m_zoomList[indexGrid.z*m_blockNumHorizontal + indexGrid.x]->checkLoad(m_loadinglist);
			}
		}

		GameObject* pObject = NULL;
		StreamThread* pSteamThread = StreamThread::Instance();
		for (Zoom::GameObjectList::iterator itload = m_loadinglist.begin(); itload != m_loadinglist.end(); ++itload)
		{
			pObject = *itload;
			if (!pObject->m_IOprepared && !pObject->m_bRequestPrepare)
			{
				pObject->_prepare_res();
				ObjectPrepareEvent* pEvent = LordNew ObjectPrepareEvent(pObject);
				pSteamThread->AddRequest(pEvent);
			}
		}

		m_LoadObjectNum = (ui32)m_loadinglist.size();
	}

	void Scene::loadNavMesh(const String& fileName)
	{
		DataStream* pNavStream = ResourceGroupManager::Instance()->openResource(fileName);
		if (pNavStream)
		{
			m_pNavigation->importData(pNavStream);
			LordDelete pNavStream;
		}
	}

	void Scene::preLoad(float& percent)
	{
		ui32 curCount = 0;
		for (Zoom::GameObjectList::iterator it = m_loadinglist.begin(); it != m_loadinglist.end(); ++it)
		{
			if ((*it)->m_IOprepared)
			{
				curCount++;
			}
		}
		if (m_LoadObjectNum <= 0)//解决场景无建筑无法进入
		{
			percent = -1.f;
			return;
		}//
		assert(curCount <= m_LoadObjectNum);
		percent = (float)curCount / (float)m_LoadObjectNum;
		if (percent >= 1.0f)
		{
			m_loadinglist.clear();
			m_LoadObjectNum = 0;
		}
	}

	GameObject* Scene::rayFindGameObject(const Ray& ray, ui32 queryMask)
	{
		for (GameObjectList::iterator it = m_GroupObjectList.begin(); it != m_GroupObjectList.end(); ++it)
		{
			GameObject* pObject = *it;
			if (ray.hitBox(pObject->getWorldAABB()))
			{
				return pObject;
			}
		}

		GameObjectList recivelist;
		GameObject* resultObject = NULL;
		for (GameObjectList::iterator itor = m_gameObjectList.begin(); itor != m_gameObjectList.end(); ++itor)
		{
			GameObject* pObject = *itor;
			if (!pObject->m_IOprepared || pObject->m_bRequestPrepare)
				continue;
			if (pObject->getType()&queryMask)
			{
				if (ray.hitBox(pObject->getWorldAABB()))
				{
					recivelist.push_back(pObject);
				}
			}
		}

		if (recivelist.size() == 1) //如果只拾取到一个对象，则直接返回
		{
			resultObject = *recivelist.begin();
		}
		else //如果拾取到多个对象的包围盒则做更细致的拾取查询
		{
			std::pair<bool, Real> result;
			float curDist = 1e30f;
			for (GameObjectList::iterator itor = recivelist.begin(); itor != recivelist.end(); ++itor)
			{
				GameObject* pObject = *itor;
				result = pObject->rayIntersects(ray);
				if (result.first)
				{
					Ray rayResult(ray);
					Matrix4 worldMat = pObject->m_SceneNode->getWorldMatrix();
					rayResult.transform(worldMat.detInverse());
					Vector3	p = rayResult.getPoint(result.second);//获取本地世界的坐标
					p = p * pObject->m_SceneNode->getWorldMatrix();//转换成世界坐标
					result.second = (p - ray.o).lenSqr();
					if (result.second < curDist)
					{
						resultObject = pObject;
						curDist = result.second;
					}
				}
			}
		}

		return resultObject;
	}

	GameObject* Scene::rayFindGameObject_Note(const Ray& ray)
	{
		for (GameObjectList::iterator itor = m_gameObjectList.begin(); itor != m_gameObjectList.end(); ++itor)
		{
			GameObject* pObject = *itor;
			if (pObject->m_Note != StringUtil::BLANK)
			{
				if (!pObject->m_IOprepared || pObject->m_bRequestPrepare)
					continue;
				if (ray.hitBox(pObject->getWorldAABB()))
				{
					return pObject;
				}
			}
		}

		return NULL;
	}

	void Scene::setUseCameraFov45(bool normalCamera)
	{
		m_bUseCameraFov45 = normalCamera;
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		if (!mainCamera)
			return;

		if (normalCamera)
		{
			mainCamera->setFov(Math::PI_DIV4);
			mainCamera->setNearClip(1.f);
			mainCamera->setFarClip(300.0f);
		}
		else
		{
			mainCamera->setFov(0.1123f);
			mainCamera->setNearClip(50.f);
			mainCamera->setFarClip(1000.0f);
		}
	}

	void Scene::useDefaultCamera()
	{
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		if (mainCamera)
		{
			mainCamera->setPosition(m_defaultCamraPos);
			mainCamera->setDirection(m_defaultCameraDir);
		}
	}

	void Scene::setDefaultCamera()
	{
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		if (mainCamera)
		{
			m_defaultCamraPos = mainCamera->getPosition();
			m_defaultCameraDir = mainCamera->getDirection();
		}
	}

	void Scene::DestroyScene()
	{
		SceneManager* pSceneManager = SceneManager::Instance();
		RenderQueue* pRenderQueue = NULL;
		LordAssert(m_pNavigation);
		LordSafeDelete(m_pNavigation);

		GameObjectList::iterator itorObject = m_gameObjectList.begin();
		for (; itorObject != m_gameObjectList.end(); ++itorObject)
		{
			LordSafeDelete((*itorObject));
		}
		m_gameObjectList.clear();
		m_unPreparelist.clear();
		m_unLoadlist.clear();

		for (ObjectSubsets::iterator itSet = mObjectSubsets.begin(); itSet != mObjectSubsets.end(); ++itSet)
		{
			LordDelete *itSet;
		}
		mObjectSubsets.clear();

		CameraAnimList::iterator itorca = m_CameraAnimList.begin();
		for (; itorca != m_CameraAnimList.end(); ++itorca)
		{
			CameraAnimation* pCa = *itorca;
			LordSafeDelete(pCa);
		}
		m_CameraAnimList.clear();

		for (size_t i = 0; i < m_zoomList.size(); ++i)
		{
			if (m_zoomList[i])
			{
				LordDelete m_zoomList[i];
			}
		}
		m_zoomList.clear();

		for (size_t i = 0; i < m_terrainBlockList.size(); ++i)
		{
			LordSafeDelete(m_terrainBlockList[i]);
		}
		m_terrainBlockList.clear();

		// 光照图卸载
		LightmapMgr::Instance()->clear();

		DestroyOcean();

		DestroyNavigatePoint();
	}

	void Scene::updateRenderQueue()
	{
		ui32 frameTime = Root::Instance()->getFrameTime();
		ClearSubSet();

		SceneManager* pSceneManager = SceneManager::Instance();
		pSceneManager->refleshMainPositon();

		// render zoom object
		if (m_bEditorMode || !m_isZoomCull)
		{
			for (size_t i = 0; i < m_zoomList.size(); ++i)
			{
				m_zoomList[i]->render(this, m_RenderMask);
			}

			for (ObjectSubsets::iterator itSet = mObjectSubsets.begin(); itSet != mObjectSubsets.end(); ++itSet)
			{
				ObjectSubset* subset = *itSet;
				subset->frameMove(frameTime);
			}

			CameraAnimList::iterator itAnim = m_CameraAnimList.begin();
			for (; itAnim != m_CameraAnimList.end(); ++itAnim)
			{
				(*itAnim)->update(frameTime);
				if (!m_bEditorMode)
				{
					if ((*itAnim)->isStop())
					{
						LordDelete(*itAnim);
						m_CameraAnimList.erase(itAnim++);
					}
					else
					{
						++itAnim;
					}
				}
			}
		}
		else
		{
			Real fmin;
			int arraysize;
			GridZoom indexGrid;
			const GridZoom* gridlist;
			Plane plane(Vector3::ZERO, Vector3::UNIT_Y);
			Vector3 curCentre;
			if (m_bUseCameraFov45)
			{
				curCentre = pSceneManager->getMainPosition();
			}
			else
			{
				Camera* camera = pSceneManager->getMainCamera();
				HitInfo hitinfo;
				Ray(camera->getPosition(), camera->getDirection()).hitPlane(plane, fmin, hitinfo);
				curCentre = hitinfo.hitPos;
			}

			m_CurZoom.x = i16(curCentre.x / m_widthPerBlock);
			m_CurZoom.z = i16(curCentre.z / m_heightPerBlock);

			GameObject* pObject = NULL;
			StreamThread* pSteamThread = StreamThread::Instance();
			for (Zoom::GameObjectList::iterator itload = m_unPreparelist.begin(); itload != m_unPreparelist.end(); ++itload)
			{
				pObject = *itload;
				if (!pObject->m_IOprepared && !pObject->m_bRequestPrepare)
				{
					pObject->_prepare_res();
					ObjectPrepareEvent* pEvent = LordNew ObjectPrepareEvent(pObject);
					pSteamThread->AddRequest(pEvent);
				}
			}
			m_unPreparelist.clear();
			for (int l = 0; l < m_loadlevel; ++l)
			{
				if (l == 0) { gridlist = sFieldZoom1; arraysize = sizeof(sFieldZoom1) / sizeof(sFieldZoom1[0]); }
				else if (l == 1) { gridlist = sFieldZoom2; arraysize = sizeof(sFieldZoom2) / sizeof(sFieldZoom2[0]); }
				else if (l == 2) { gridlist = sFieldZoom3; arraysize = sizeof(sFieldZoom3) / sizeof(sFieldZoom3[0]); }
				else { gridlist = sFieldZoom4; arraysize = sizeof(sFieldZoom4) / sizeof(sFieldZoom4[0]); }

				for (int f = 0; f < arraysize; ++f)
				{
					indexGrid.x = m_CurZoom.x + gridlist[f].x;
					indexGrid.z = m_CurZoom.z + gridlist[f].z;
					if (indexGrid.x < 0 || indexGrid.x >= m_blockNumHorizontal
						|| indexGrid.z < 0 || indexGrid.z >= m_blockNumVertical)
					{
						continue;
					}
					//加载查询
					m_zoomList[indexGrid.z*m_blockNumHorizontal + indexGrid.x]->checkLoad(m_unPreparelist);

					if (l < 3)
					{
						m_zoomList[indexGrid.z*m_blockNumHorizontal + indexGrid.x]->render(this, m_RenderMask);
					}
				}//for
			}//for

			 /*if (m_LoadObjectNum == 0)
			 {
			 checkObjectUnprepare();
			 }*/

			for (ObjectSubsets::iterator itSet = mObjectSubsets.begin(); itSet != mObjectSubsets.end(); ++itSet)
			{
				ObjectSubset* subset = *itSet;
				subset->frameMove(frameTime);
			}

			CameraAnimList::iterator itAnim = m_CameraAnimList.begin();
			for (; itAnim != m_CameraAnimList.end(); )
			{
				(*itAnim)->update(frameTime);
				if ((*itAnim)->isStop())
				{
					LordDelete(*itAnim);
					m_CameraAnimList.erase(itAnim++);
				}
				else
				{
					++itAnim;
				}
			}

		}

		if (m_bHasTerrain && m_isRenderTerrain)
		{
			// render terrain block
			for (size_t i = 0; i < m_terrainBlockList.size(); ++i)
			{
				if (m_terrainBlockList[i]->isNeedToBeDraw())
				{
					bool isVisble = pSceneManager->getMainCamera()->getFrustum().intersect(m_terrainBlockList[i]->getTerrainBlockAABB());
					if (isVisble)
					{
						m_terrainBlockList[i]->updateRenderQueue();
					}
				}
			}
		}

		if (m_pOceanRender)
		{
			m_pOceanRender->_updateRenderQueue(SceneManager::Instance()->getRenderQueue("Scene"));
		}
	}

	void Scene::checkObjectUnprepare()
	{
		if (m_LastZoom.x != m_CurZoom.x || m_LastZoom.z != m_CurZoom.z)
		{
			GridZoom indexGrid;
			int arraysize;
			m_unLoadlist.clear();
			for (int l = 0; l<m_loadlevel; ++l)
			{
				// const GridZoom* gridlist;
				if (l == 0) { arraysize = sizeof(sFieldZoom1) / sizeof(sFieldZoom1[0]); }
				else if (l == 1) { arraysize = sizeof(sFieldZoom2) / sizeof(sFieldZoom2[0]); }
				else if (l == 2) { arraysize = sizeof(sFieldZoom3) / sizeof(sFieldZoom3[0]); }
				else { arraysize = sizeof(sFieldZoom4) / sizeof(sFieldZoom4[0]); }

				for (int f = 0; f<arraysize; ++f)
				{
					indexGrid.x = m_LastZoom.x + sFieldZoom4[f].x;
					indexGrid.z = m_LastZoom.z + sFieldZoom4[f].z;
					if (indexGrid.x<0 || indexGrid.x >= m_blockNumHorizontal
						|| indexGrid.z<0 || indexGrid.z >= m_blockNumVertical)
					{
						continue;
					}
					m_zoomList[indexGrid.z*m_blockNumHorizontal + indexGrid.x]->checkInList(m_unLoadlist);
				}
			}
			m_LastZoom.x = m_CurZoom.x;
			m_LastZoom.z = m_CurZoom.z;

			GameObject* pObject = NULL;
			for (Zoom::GameObjectList::iterator itUnp = m_unLoadlist.begin(); itUnp != m_unLoadlist.end(); ++itUnp)
			{
				pObject = *itUnp;
				if (pObject->m_IOprepared && !pObject->m_bRequestPrepare)
				{
					pObject->_unprepare();
				}
			}
		}
	}

	void Scene::setLightDirection(const Vector3& direction)
	{
		m_lightDirection = direction;
		SceneManager::Instance()->setMainLightDir(direction);
	}

	void Scene::setLightColor(const Color& color)
	{
		m_lightColor = color;
		SceneManager::Instance()->setMainLightColor(color);
	}

	void Scene::setAmbientColor(const Color& color)
	{
		m_ambientColor = color;
		SceneManager::Instance()->setAmbientColor(color);
	}

	void Scene::getGameObjects(ui32 type, GameObjectList& lst)
	{
		GameObjectList::iterator it = m_gameObjectList.begin();
		for (; it != m_gameObjectList.end(); ++it)
		{
			if ((*it)->m_Type == type)
			{
				lst.push_back(*it);
			}
		}
	}

	void Scene::getGameObjects(GameObjectList& lst)
	{
		lst.assign(m_gameObjectList.begin(), m_gameObjectList.end());
	}

	GameObject* Scene::getStaticMeshObjectByNote(const String& note)
	{
		ObjectSubsets::iterator it = mObjectSubsets.begin();
		for (; it != mObjectSubsets.end(); ++it)
		{
			if ((*it)->m_Type == GOT_SMESH_L)
			{
				NormalMeshSubset* pSubSets = (NormalMeshSubset*)*it;
				GameObject* pObject = pSubSets->findObjectByNote(note);
				if (pObject)
				{
					return pObject;
				}
			}
			else if ((*it)->m_Type == GOT_SMESH_L_NOFOG)
			{
				NormalNoFogMeshSubset* pSubSets = (NormalNoFogMeshSubset*)*it;
				GameObject* pObject = pSubSets->findObjectByNote(note);
				if (pObject)
				{
					return pObject;
				}
			}
		}

		return NULL;
	}

	GameObject* Scene::getSMeshObjectByNoteForAll(const String& note)
	{
		for (ui32 i = 0; i < m_gameObjectList.size(); ++i)
		{
			if (m_gameObjectList[i]->m_Type == GOT_SMESH_L)
			{
				StaticMeshObject_Normal* pNormal = (StaticMeshObject_Normal*)m_gameObjectList[i];
				if (pNormal->m_Note == note)
				{
					return pNormal;
				}
			}
			else if (m_gameObjectList[i]->m_Type == GOT_SMESH_L_NOFOG)
			{
				StaticMeshObject_Normal_noFog* pNormalnoFog = (StaticMeshObject_Normal_noFog*)m_gameObjectList[i];
				if (pNormalnoFog->m_Note == note)
				{
					return pNormalnoFog;
				}
			}
		}

		return NULL;
	}

	void Scene::refleshObjectLightMap()
	{
		LightmapMgr* pLmMgr = LightmapMgr::Instance();
		pLmMgr->clear();
		GameObjectList::iterator it = m_gameObjectList.begin();
		for (; it != m_gameObjectList.end(); ++it)
		{
			if ((*it)->m_Type == GOT_SMESH_L)
			{
				StaticMeshObject_Normal* pObj = static_cast<StaticMeshObject_Normal*>(*it);
				pObj->unloadLMTexture();
			}
			else if ((*it)->m_Type == GOT_TERRAIN)
			{
				TerrainMeshObject* pTerrian = static_cast<TerrainMeshObject*>(*it);
				pTerrian->unloadLMTexture();
			}
		}
		TextureManager::Instance()->releaseAllDelayed();

		pLmMgr->load();
		it = m_gameObjectList.begin();
		for (; it != m_gameObjectList.end(); ++it)
		{
			if ((*it)->m_Type == GOT_SMESH_L)
			{
				StaticMeshObject_Normal* pObj = static_cast<StaticMeshObject_Normal*>(*it);
				pObj->refleshLMTexture();
			}
			else if ((*it)->m_Type == GOT_TERRAIN)
			{
				TerrainMeshObject* pTerrian = static_cast<TerrainMeshObject*>(*it);
				pTerrian->refleshLMTexture();
			}
		}
	}

	void Scene::EnableRenderType(ui32 type, bool bRender)
	{
		if (bRender)
		{
			m_RenderMask = m_RenderMask | type;
		}
		else
		{
			m_RenderMask = m_RenderMask ^ type;
		}

		GameObjectList::iterator it = m_gameObjectList.begin();
		for (; it != m_gameObjectList.end(); ++it)
		{
			GameObject* pObj = *it;
			if (pObj->m_Type == type)
			{
				if (pObj->m_pHoverBoxQueryObject)
				{
					pObj->m_pHoverBoxQueryObject->setVisible(bRender);
				}
				if (pObj->m_pSelectBoxQueryObject)
				{
					pObj->m_pSelectBoxQueryObject->setVisible(bRender);
				}
			}
		}
	}

	bool Scene::IsEnableRenderType(ui32 type) const
	{
		return ((m_RenderMask & type) != 0);
	}

	CameraAnimation* Scene::createCameraAnimation(const String& name)
	{
		CameraAnimation* pResult = validCameraAnimName(name);
		if (pResult == NULL)
		{
			pResult = LordNew CameraAnimation(name);
			m_CameraAnimList.insert(pResult);
		}
		return pResult;
	}

	CameraAnimation* Scene::validCameraAnimName(const String& name)
	{
		CameraAnimList::iterator it = m_CameraAnimList.begin();
		for (; it != m_CameraAnimList.end(); ++it)
		{
			if ((*it)->m_Name == name)
			{
				return *it;
			}
		}
		return NULL;
	}

	CameraAnimation* Scene::getCameraAnimation(const String& name) const
	{
		CameraAnimList::iterator it = m_CameraAnimList.begin();
		for (; it != m_CameraAnimList.end(); ++it)
		{
			if ((*it)->m_Name == name)
			{
				return *it;
			}
		}
		return NULL;
	}

	void Scene::destroyCameraAnimation(const String& name)
	{
		CameraAnimList::iterator it = m_CameraAnimList.begin();
		for (; it != m_CameraAnimList.end(); ++it)
		{
			CameraAnimation* pResult = *it;
			if (pResult->m_Name == name)
			{
				LordDelete pResult;
				m_CameraAnimList.erase(it);
				return;
			}
		}
	}

	void Scene::destroyCameraAnimation(CameraAnimation* pCameraAnim)
	{
		CameraAnimList::iterator it = m_CameraAnimList.find(pCameraAnim);
		if (it != m_CameraAnimList.end())
		{
			LordDelete *it;
			m_CameraAnimList.erase(it);
		}
	}

	std::pair<CameraAnimation*, i32> Scene::rayFindCameraAnim(const Ray& ray)
	{
		std::pair<CameraAnimation*, i32> result;
		CameraAnimList::iterator it = m_CameraAnimList.begin();
		for (; it != m_CameraAnimList.end(); ++it)
		{
			result = (*it)->rayInstersect(ray);
			if (result.first)
			{
				return result;
			}
		}
		return result;
	}

	const Scene::CameraAnimList& Scene::getCameraAnimations() const
	{
		return m_CameraAnimList;
	}

	CameraAnimation* Scene::importCameraAnimation(const String& filename)
	{
		size_t index = filename.find_last_of(".");
		String name = filename.substr(0, index);
		String ext = filename.substr(index + 1, filename.length());
		CameraAnimation* pCameraAnim = createCameraAnimation(name);
		if (pCameraAnim)
		{
			DataStream* stream = ResourceGroupManager::Instance()->openResource(filename);
			if (stream)
			{
				if (ext == "camAnim")
				{
					pCameraAnim->importData(*stream);
				}
				else
				{
					size_t fileSize = stream->size();
					char* pJSONData = (char*)LordMalloc(fileSize + 1);
					stream->read(pJSONData, fileSize);
					pJSONData[fileSize] = 0;

					rapidjson::Document* doc = new rapidjson::Document;
					doc->Parse(pJSONData);
					pCameraAnim->importData(*doc);
					delete doc;
					LordFree(pJSONData);
#if 0
					size_t fileSize = stream->size();
					char* xmlData = (char*)LordMalloc(fileSize + 1);
					stream->read(xmlData, fileSize);
					xmlData[fileSize] = 0;

					xml_document<>* doc = LordNewT(xml_document<>);        // character type defaults to cha
					doc->parse<0>(xmlData);
					pCameraAnim->importData(doc->first_node());
					LordDeleteT(doc, xml_document<>);
					LordFree(xmlData);
#endif
				}

				//pCameraAnim->_prepare_io();
				pCameraAnim->_prepare_obj();
				pCameraAnim->refleshPath();
				LordDelete stream;
			}
			else
			{
				LordLogError("Scene: CameraAnimtion file isn't exits. [%s]", filename.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				char buf[MAX_PATH];
				sprintf_s(buf, "Scene: CameraAnimtion file isn't exits. [%s]", filename.c_str());
				MessageBox(0, buf, "error", 0);
#endif
				destroyCameraAnimation(pCameraAnim);
				pCameraAnim = NULL;
			}
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			MessageBox(0, "已有同名摄像机动画", 0, 0);
#endif
		}

		return pCameraAnim;
	}

	void Scene::attachGroupObject(GroupObject* pGroup)
	{
		m_GroupObjectList.push_back(pGroup);
	}

	void Scene::detachGroupObject(GroupObject* pGroup)
	{
		GameObjectList::iterator it = std::find(m_GroupObjectList.begin(), m_GroupObjectList.end(), pGroup);
		m_GroupObjectList.erase(it);
	}

	void Scene::InsertSubSet(GameObject* object)
	{
		for (ObjectSubsets::iterator it = mObjectSubsets.begin(); it != mObjectSubsets.end(); ++it)
		{
			ObjectSubset* subset = *it;
			if (subset->m_Type == object->m_Type)
			{
				subset->addObject(object);
			}
		}
	}

	void Scene::EraseSubSet(GameObject* object)
	{
		for (ObjectSubsets::iterator it = mObjectSubsets.begin(); it != mObjectSubsets.end(); ++it)
		{
			ObjectSubset* subset = *it;
			if (subset->m_Type == object->m_Type)
			{
				subset->deleteObject(object);
			}
		}
	}

	void Scene::ClearSubSet()
	{
		for (ObjectSubsets::iterator it = mObjectSubsets.begin(); it != mObjectSubsets.end(); ++it)
		{
			ObjectSubset* subset = *it;
			subset->clearObjects();
		}
	}

	void Scene::ChangeDark(float radio)
	{
		for (ui32 i = 0; i < m_gameObjectList.size(); ++i)
		{
			if (m_gameObjectList[i]->m_Type == GOT_SMESH_L)
			{
				StaticMeshObject_Normal* pNormal = (StaticMeshObject_Normal*)m_gameObjectList[i];
				pNormal->changeDark(radio);
			}
			else if (m_gameObjectList[i]->m_Type == GOT_SMESH_L_NOFOG)
			{
				StaticMeshObject_Normal_noFog* pNormalnoFog = (StaticMeshObject_Normal_noFog*)m_gameObjectList[i];
				pNormalnoFog->changeDark(radio);
			}
			else if (m_gameObjectList[i]->m_Type == GOT_TERRAIN)
			{
				TerrainMeshObject* pTerrain = (TerrainMeshObject*)m_gameObjectList[i];
				pTerrain->changeDark(radio);
			}
		}

		if (m_pOceanRender)
		{
			m_pOceanRender->changeDark(radio);
		}
	}

	void Scene::RevertLight()
	{
		for (ui32 i = 0; i < m_gameObjectList.size(); ++i)
		{
			if (m_gameObjectList[i]->m_Type == GOT_SMESH_L)
			{
				StaticMeshObject_Normal* pNormal = (StaticMeshObject_Normal*)m_gameObjectList[i];
				pNormal->revertLight();
			}
			else if (m_gameObjectList[i]->m_Type == GOT_SMESH_L_NOFOG)
			{
				StaticMeshObject_Normal_noFog* pNormalnoFog = (StaticMeshObject_Normal_noFog*)m_gameObjectList[i];
				pNormalnoFog->revertLight();
			}
			else if (m_gameObjectList[i]->m_Type == GOT_TERRAIN)
			{
				TerrainMeshObject* pTerrain = (TerrainMeshObject*)m_gameObjectList[i];
				pTerrain->revertLight();
			}
		}
		if (m_pOceanRender)
		{
			m_pOceanRender->changeDark(1.0f);
		}
	}

	OceanRender* Scene::CreateOcean()
	{
		assert(m_pOceanRender == NULL);

		m_pOceanRender = LordNew OceanRender();
		return m_pOceanRender;
	}

	void Scene::DestroyOcean()
	{
		LordSafeDelete(m_pOceanRender);
	}

	NavigatePoint* Scene::LoadNavigatePoint(const String& fileName)
	{
		DataStream* stream = ResourceGroupManager::Instance()->openResource(fileName);
		if (stream == NULL)
		{
			LordLogError("navigate point file not found [%s]. it will be can't navigate!", fileName.c_str());
			return NULL;
		}

		size_t fileSize = stream->size();
		char* xmlData = (char*)LordMalloc(fileSize + 1);
		stream->read(xmlData, fileSize);
		xmlData[fileSize] = 0;

		xml_document<>* doc = LordNewT(xml_document<>);        // character type defaults to cha
		doc->parse<0>(xmlData);

		// size_t index = fileName.find_last_of(".");
		NavigatePoint* pNavigatePoint = CreateNewNavigatePoint();
		pNavigatePoint->importData(doc->first_node());
		pNavigatePoint->prepare();
		if (Root::Instance()->isEditorMode())
		{
			pNavigatePoint->prepare_editor();
		}

		LordFree(xmlData);
		LordDeleteT(doc, xml_document<>);
		LordDelete stream;

		return pNavigatePoint;
	}

	NavigatePoint* Scene::CreateNewNavigatePoint()
	{
		DestroyNavigatePoint();
		m_NavigatePoint = LordNew NavigatePoint(m_sceneName);
		return m_NavigatePoint;
	}

	void Scene::DestroyNavigatePoint()
	{
		LordSafeDelete(m_NavigatePoint);
	}

	void Scene::RenderNavigateMarkText()
	{
		if (m_NavigatePoint)
		{
			m_NavigatePoint->renderSpecialPointText();
		}
	}

	void Scene::SaveNavigatePoint(const String& filename)
	{
		if (m_NavigatePoint)
		{
			xml_document<>* doc = LordNewT(xml_document<>);

			xml_node<>* rot = doc->allocate_node(rapidxml::node_pi, doc->allocate_string("xml version='1.0' encoding='utf-8'"));
			doc->append_node(rot);

			m_NavigatePoint->exportData(*doc);

			std::ofstream out(filename.c_str());
			out << *doc;
			LordDeleteT(doc, xml_document<>);
		}
	}

	void Scene::EnableStaticMeshGrayRender(GameObject* pObject, bool bEnable)
	{
		if (pObject)
		{
			if (pObject->getType() == GOT_SMESH_L)
			{
				StaticMeshObject_Normal* pNormalObj = (StaticMeshObject_Normal*)pObject;
				pNormalObj->EnableGrayRender(bEnable);
			}
			else if (pObject->getType() == GOT_SMESH_L_NOFOG)
			{
				StaticMeshObject_Normal_noFog* pNormalObj = (StaticMeshObject_Normal_noFog*)pObject;
				pNormalObj->EnableGrayRender(bEnable);
			}
		}
	}

	void Scene::StopCameraAnimations()
	{
		CameraAnimList::iterator itAnim = m_CameraAnimList.begin();
		for (; itAnim != m_CameraAnimList.end(); ++itAnim)
		{
			if (!(*itAnim)->isStop())
			{
				(*itAnim)->stop();
			}
		}
	}

}