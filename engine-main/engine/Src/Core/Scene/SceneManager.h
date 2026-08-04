/********************************************************************
filename: 	SceneManager.h
file path:	dev\engine\Src\Core\Scene\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SCENEMGR_H__
#define __LORD_SCENEMGR_H__

#include "Core.h"
#include "Object/Singleton.h"
#include "Render/RenderQueue.h"
#include "Image/Color.h"

namespace LORD
{
	class Scene;
	class QueryObjectManager;
	class TerrainMesh;

	class LORD_CORE_API SceneManager : public ObjectAlloc, public Singleton<SceneManager>
	{
	public:
		inline static SceneManager* Instance()
		{
			return ms_pSingleton;
		}

	public:
		SceneManager();
		virtual ~SceneManager();

		typedef map<String, Animation*>::type	AnimMap;

		struct LightParam
		{
			Vector4			mainLightDir;
			Vector4			mainLightColor;
			Vector4			subLightDir;
			Vector4			subLightColor;
			Vector4			ambientColor;
		};
		struct FogParam
		{
			Vector4			fogParam;//start, end, globalDensity
			Vector4			fogColor;
			Vector4			mainPosition;
		};

	public:
		bool			initialize();
		void			destroy();

		inline Camera* getMainCamera() const
		{
			return m_pMainCamera;
		}

		inline Camera* get2DCamera() const
		{
			return m_p2DCamera;
		}

		inline Camera* getGUICamera() const
		{
			return m_pGUICamera;
		}

		void addCameraShake(float beginTime, float scale, float durationTime, int shakeTimes);

		bool		isPlayingCameraAnimate();

		inline void setGUICamera(Camera* pCamera) { m_pGUICamera = pCamera; }

		void			setMainLightDir(const Vector3& dir);
		void			setMainLightColor(const Color& color);
		void			setSubLightColor(const Vector4& color);
		void			setAmbientColor(const Color& color);
		void			setFogParam(const Vector3& param);
		void			setFogColor(const Color& color);

		const Vector3&  getMainLightDir() const
		{
			return *(Vector3*)(&m_LightParam.mainLightDir);
		}

		Vector3* getMainLightDirPtr()
		{
			return (Vector3*)(&m_LightParam.mainLightDir);
		}

		const Color& getMainLightColor() const
		{
			return *(Color*)(&m_LightParam.mainLightColor);
		}

		const Color& getSubLightColor() const
		{
			return *(Color*)(&m_LightParam.subLightColor);
		}
		
		Color*	getMainLightColorPtr()
		{
			return (Color*)(&m_LightParam.mainLightColor);
		}

		Color* getSubLightColorPtr()
		{
			return (Color*)(&m_LightParam.subLightColor);
		}

		const Vector3& getSubLightDir() const
		{
			return *(Vector3*)(&m_LightParam.subLightDir);
		}

		Vector3* getSubLightDirPtr()
		{
			return (Vector3*)(&m_LightParam.subLightDir);
		}

		const Color& getAmbientColor() const
		{
			return *(Color*)(&m_LightParam.ambientColor);
		}

		Color*	getAmbientColorPtr()
		{
			return (Color*)(&m_LightParam.ambientColor);
		}

		inline const Vector4* getLightParamPtr() const
		{
			return (Vector4*)&m_LightParam;
		}

		const Vector4& getFogParam() const
		{
			return *(Vector4*)(&m_FogParam.fogParam);
		}

		inline const Vector4* getFogParamVec4Ptr()
		{
			return (Vector4*)&m_FogParam;
		}

		inline const Vector4* getForParamFarVec4Ptr()
		{
			return (Vector4*)&m_FogParam_far;
		}

		Vector3* getFogParamPtr()
		{
			return (Vector3*)(&m_FogParam.fogParam);
		}

		const Color& getFogColor() const
		{
			return *(Color*)(&m_FogParam.fogColor);
		}

		Color* getFogColorPtr()
		{
			return (Color*)(&m_FogParam.fogColor);
		}

		inline SceneNode* getRootNode() const
		{
			return m_pRootNode;
		}

		RenderQueue*	getRenderQueue(const String& strQueueName) const
		{
			RenderQueue* pRenderQueue = NULL;
			size_t nCount = m_RenderQueueGroup.size();
			for (size_t i = 0; i < nCount; ++i)
			{
				if (strQueueName == m_RenderQueueGroup[i]->getName())
				{
					pRenderQueue = m_RenderQueueGroup[i];
					break;
				}
			}

			return pRenderQueue;
		}

		inline RenderQueue*	getRenderQueueByIndex(ui32 nIndex) const
		{
			if (nIndex >= m_RenderQueueGroup.size())
			{
				return NULL;
			}
			else
			{
				return m_RenderQueueGroup[nIndex];
			}
		}

		ui8 getRenderQueueIndex(const String& strQueueName) const
		{
			ui8 nIndex = 0;
			ui32 nCount = (ui32)m_RenderQueueGroup.size();
			for (ui32 i = 0; i < nCount; ++i)
			{
				if (strQueueName == m_RenderQueueGroup[i]->getName())
				{
					nIndex = i;
					break;
				}
			}

			return nIndex;
		}

		inline ui32 getRenderQueueCount() { return (ui32)m_RenderQueueGroup.size(); }

		void			update();
		void			render();
		void			renderScene();

		Scene*			createScene(const String& name, bool hasTerrain, bool bSphere, float blockWidth, float blockHeight, ui16 BlockNumH, ui16 BlockNumV, bool bNormalFov, float radius, bool autocalc);
		Scene*			loadScene(const String& name, QueryObjectManager* manager = NULL, int aiLoadLv = 3);
		void			loading(float x, float y);
		float			getLoadingPercent();
		Vector3			getCameraRayIntersectPoint();

		Scene*			loadScene(const String& name, int aiLoadLv = 3);
		Scene*			loadChildScene(const String& name, QueryObjectManager* manager = NULL, int aiLoadLv = 3);
		Scene*          swapScene();
		void			saveScene(const String& filename);
		void			closeScene();
		Scene*			getCurrentScene();

		void			setDeltaPosition(const Vector3& pos);
		void			refleshMainPositon();
		inline const Vector3&	getMainPosition() const
		{
			return *(Vector3*)(&m_FogParam.mainPosition);
		}

		inline bool		isRenderActorHigh() { return m_bIsRenderActorHigh; }
		inline void		setRenderActorHigh(bool bHigh) { m_bIsRenderActorHigh = bHigh; }

		inline bool		isComputeActorFog() { return m_bisComputeActorFog; }
		inline void		setComputeActorFog(bool bHigh) { m_bisComputeActorFog = bHigh; }

		inline bool		isUseXRay() { return m_isUseXRay; }
		inline void		setUseXRay(bool isUseXRay) { m_isUseXRay = isUseXRay; }

		void			stopCameraAnimations();
		void			enableCameraShake(bool bEnable);

		void			RenderMaskStaticMesh(bool bRender);			// 2D插片儿的静态模型（比如不败战神场景模型）
		bool			IsRenderMaskStaticMesh() const;
		void			RenderMaskStaticMesh_Light(bool bRender);	// 3D场景静态模型，接受光照的静态模型(比如冰火项目场景模型)
		bool			IsRenderMaskStaticMesh_Light() const;
		void			RenderMaskSkeletonMesh(bool bRender);		// 场景动态模型
		bool			IsRenderMaskSkeletonMesh() const;
		void			RenderMaskEffect(bool bRender);				// 场景特效
		bool			IsRenderMaskEffect() const;
		void			RenderMaskTerrain(bool bRender);			// 地形
		bool			IsRenderMaskTerrain() const;

	protected:
		void			initRenderQueueGroup();
		void			destroyRenderQueueGroup();

	protected:
		bool				m_bNeedUpate;
		bool				m_isAnotherSceneLoading;
		bool				m_bIsRenderActorHigh;
		bool				m_isUseXRay;
		bool				m_bisComputeActorFog;
		Camera*			    m_pMainCamera;
		Camera*				m_p2DCamera;
		Camera*				m_pGUICamera;
		SceneNode*			m_pRootNode;
		Scene*				m_pCurrentScene;
		Scene*              m_pAnotherScene;
		Vector3				m_DeltaPosition;
		LightParam			m_LightParam;
		FogParam			m_FogParam;
		FogParam			m_FogParam_far;
		RenderQueueGroup	m_RenderQueueGroup;
	};

}

#endif