/********************************************************************
filename: 	Scene.h
file path:	dev\engine\Src\Core\Scene\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __SCENE_H__
#define __SCENE_H__

#include "Core.h"
#include "Image/Color.h"
#include "Zoom.h"
#include "TerrainBlock.h"

namespace LORD
{
	// 1m * 1m == 57 pixel * 28 pixel
	// 512 pixel * 512 pixel per terrain block
	const float MAX_TERRAIN_BLOCK_HEIGHT = 0.1f;

	class Material;
	class QueryObjectManager;
	class Navigation;
	class CameraAnimation;
	class GroupObject;
	class OceanRender;
	class NavigatePoint;
	class LORD_CORE_API Scene : public ObjectAlloc
	{
	public:
		typedef vector<TerrainBlock*>::type		TerrainBlockList;
		typedef vector<GameObject*>::type			GameObjectList;
		typedef vector<Zoom*>::type				ZoomList;
		typedef set<CameraAnimation*>::type CameraAnimList;
		typedef vector<ObjectSubset*>::type ObjectSubsets;
		typedef map<String, NavigatePoint*>::type NavigatePointList;

		struct GridZoom
		{
			i16 x, z;
		};

		struct rayinfo
		{
			GameObject* o;
			Vector3 p;
		};

		Scene(const String& name, bool hasTerrain = true, bool bSphere = false);
		virtual ~Scene();

		// -------------------------initilize ---------------------------------------
		// init the default scene, all terrain block will be renderable
		// just for editor create a new scene
		void		InitScene();

		void		prepare();

		void		DestroyScene();

		void setBlockCount(ui16 width, ui16 height)
		{
			m_blockNumHorizontal = width;
			m_blockNumVertical = height;
		}

		void getBlockCount(ui16& width, ui16& height)
		{
			width = m_blockNumHorizontal;
			height = m_blockNumVertical;
		}

		float getWidthPerBlock() const
		{
			return m_widthPerBlock;
		}

		float getHeightPerBlock() const
		{
			return m_heightPerBlock;
		}

		void setSizeofBlock(float width, float height)
		{
			m_widthPerBlock = width;
			m_heightPerBlock = height;
		}

		void setSphereParam(float radius, bool bAuto)
		{
			if (bAuto) m_SphereRadius = (float)m_blockNumHorizontal*m_widthPerBlock*0.5f;
			else m_SphereRadius = radius;

			float scenewidht = (float)m_blockNumHorizontal*m_widthPerBlock;
			float sceneheight = (float)m_blockNumVertical*m_heightPerBlock;
			m_SphereTerrainCenter.x = scenewidht*0.5f;
			m_SphereTerrainCenter.y = -m_SphereRadius;
			m_SphereTerrainCenter.z = sceneheight*0.5f;

			m_AutoCalc = bAuto;
		}

		void getSphereParam(float& radius, bool& bAuto)
		{
			radius = m_SphereRadius;
			bAuto = m_AutoCalc;
		}

		void enableZoomCull(bool bEnable) { m_isZoomCull = bEnable; }
		bool isEnableZoomCull() const { return m_isZoomCull; }

		inline String& getSceneName() { return m_sceneName; }

		void		updateRenderQueue();
		void		checkObjectUnprepare();

		void		setIsRenderTerrain(bool setting) { m_isRenderTerrain = setting; }
		bool		isRenderTerrain() { return m_isRenderTerrain; }
		bool		hasTerrain() { return m_bHasTerrain; }
		bool		isSphereTerrain() { return m_bSphere; }

		void		editor_modifySphereBlockHeight(const Vector3& pos, const TerrainBlock::TerrainBrushInfo& info, float escapeTime);
		void		editor_enableBlockMeshVisible(bool bVisible);
		void		modifySphereRadius(float radius);
		float		getSphereRadius() const { return m_SphereRadius; }

		void		editor_saveSphereTerrain(DataStream* stream);
		void		loadSphereTerrain(DataStream* stream);
		const Vector3& getSphereTerrainCenter() const { return m_SphereTerrainCenter; }
		// --------------------------object operation -----------------------------
		// 创建场景对象 
		GameObject* createGameObject(ui32 type);
		// 销毁场景对象
		void destroyGameObject(GameObject* obj);
		void destroyGameObjectByType(GameObjectType type);
		// 获得场景对象
		GameObject* getGameObject(const String& name);

		size_t		GetGameObjectCount() { return m_gameObjectList.size(); }
		GameObject* GetGameObjectByIndex(size_t nIndex);

		void getGameObjects(ui32 type, GameObjectList& lst);
		void getGameObjects(GameObjectList& lst);

		GameObject* getStaticMeshObjectByNote(const String& note);
		GameObject* getSMeshObjectByNoteForAll(const String& note);
		void EnableStaticMeshGrayRender(GameObject* pObject, bool bEnable);

		inline const Vector3&	getLightDirection() { return m_lightDirection; }
		inline const Color& getLightColor() { return m_lightColor; }
		inline const Color& getAmbientColor() { return m_ambientColor; }
		void		setLightDirection(const Vector3& direction);
		void		setLightColor(const Color& color);
		void		setAmbientColor(const Color& color);
		inline bool	getUseCameraFov45() const { return m_bUseCameraFov45; }
		void		setUseCameraFov45(bool normalCamera);
		void		useDefaultCamera();
		void		setDefaultCamera();

		// register to scene
		void		registerGameObjectToZoom(GameObject* object);
		void		unregisterGameObjectFromZoom(GameObject* object);
		void		registerGameObjectToZoom_unprepare(GameObject* object);
		void		unregisterGameObjectFromZoom_unprepare(GameObject* object);

		String		getAutoObjectName();

		// ----------------------------query method-----------------------------------
		TerrainBlock*	getRayIntersectTerrainBlock(const Ray& ray);
		bool			getRayIntersectPoint(const Ray& ray, Vector3& point, GameObjectList* culllist = NULL, ui32 queryMask = 0xFFFFFFFF);
		bool			getRayIntersectPointForBlock(const Ray& ray, Vector3& point, float& tmin);
		GameObject*		rayFindGameObject(const Ray& ray, ui32 queryMask = 0xFFFFFFFF);
		GameObject*		rayFindGameObject_Note(const Ray& ray);

		// -----------------------------file operation -------------------------------
		void		saveSceneToXml(const String& filename);
		void		loadSceneFromXml();
		void		saveSceneToJson(const String& filename);
		void		loadSceneFromJson();

		// -----------------------------game mode load object ------------------------------------------------
		void		loadSceneObject(float x, float y);
		void		loadNavMesh(const String& fileName);
		void		preLoad(float& percent);
		inline void		setLoadLevel(ui8 level) { m_loadlevel = level; }

		// ---------------------------------------for editor only----------------------------------------
		inline void		setQueryObjectManager(QueryObjectManager* manager) { m_queryObjectManager = manager; }
		inline QueryObjectManager* getQueryObjectManager() { return m_queryObjectManager; }

		inline Navigation*	getNavigation() { return m_pNavigation; }
		inline TerrainBlockList& getTerrainBlockList() { return m_terrainBlockList; }

		void		refleshObjectLightMap();

		void		EnableRenderType(ui32 type, bool bRender);
		bool		IsEnableRenderType(ui32 type) const;

		//-----------------------------------------CameraAnimation----------------------------------------
		CameraAnimation* importCameraAnimation(const String& filename);
		CameraAnimation* createCameraAnimation(const String& name);
		CameraAnimation* validCameraAnimName(const String& name);
		CameraAnimation* getCameraAnimation(const String& name) const;
		const CameraAnimList& getCameraAnimations() const;
		void destroyCameraAnimation(const String& name);
		void destroyCameraAnimation(CameraAnimation* pCameraAnim);
		std::pair<CameraAnimation*, i32> rayFindCameraAnim(const Ray& ray);
		void StopCameraAnimations();

		//-----------------------------------------Group Object-------------------------------------------
		void attachGroupObject(GroupObject* pGroup);
		void detachGroupObject(GroupObject* pGroup);

		void InsertSubSet(GameObject* object);
		void EraseSubSet(GameObject* object);
		void ClearSubSet();

		void ChangeDark(float radio);
		void RevertLight();

		//-----------------------------------------Ocean--------------------------------------------------
		OceanRender* GetOceanRender() { return m_pOceanRender; }
		OceanRender* CreateOcean();
		void DestroyOcean();

		//-----------------------------------------NavigatePoint------------------------------------------
		NavigatePoint* LoadNavigatePoint(const String& fileName);
		NavigatePoint* CreateNewNavigatePoint();
		NavigatePoint* GetNavigatePoint() { return m_NavigatePoint; }
		void DestroyNavigatePoint();
		void RenderNavigateMarkText();
		void SaveNavigatePoint(const String& filename);

	protected:
		bool						m_bEditorMode;
		bool						m_bHasTerrain;
		bool						m_bSphere;
		bool						m_bUseCameraFov45;
		bool						m_isRenderTerrain;
		bool						m_AutoCalc;
		bool						m_isZoomCull;
		ui8							m_loadlevel;
		ui16						m_blockNumHorizontal;
		ui16						m_blockNumVertical;
		ui32						m_RenderMask;
		ui32						m_LoadObjectNum;
		i32							mSceneObjectID;
		float						m_widthPerBlock;
		float						m_heightPerBlock;
		float						m_SphereRadius;

		Vector3						m_lightDirection;
		Vector3						m_defaultCamraPos;
		Vector3						m_defaultCameraDir;
		Vector3						m_SphereTerrainCenter;
		Color						m_lightColor;
		Color						m_ambientColor;

		QueryObjectManager*			m_queryObjectManager;
		LightmapMgr*				m_pLightmapMgr;				// 光照图管理器
		Navigation*					m_pNavigation;
		OceanRender*				m_pOceanRender;
		NavigatePoint*				m_NavigatePoint;

		TerrainBlockList			m_terrainBlockList;
		GameObjectList				m_gameObjectList;
		GameObjectList				m_GroupObjectList;
		ZoomList					m_zoomList;
		CameraAnimList				m_CameraAnimList;
		ObjectSubsets				mObjectSubsets;
		Zoom::GameObjectList		m_unPreparelist;
		Zoom::GameObjectList		m_unPreparelist_copy;
		Zoom::GameObjectList		m_unLoadlist;
		Zoom::GameObjectList		m_loadinglist;
		String						m_sceneName;

	private:
		static const GridZoom sFieldZoom1[9];
		static const GridZoom sFieldZoom2[16];
		static const GridZoom sFieldZoom3[24];
		static const GridZoom sFieldZoom4[32];
		GridZoom					m_CurZoom;
		GridZoom					m_LastZoom;
	};
}

#endif