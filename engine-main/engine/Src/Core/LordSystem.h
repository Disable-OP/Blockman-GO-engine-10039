/********************************************************************
filename: 	LordSystem.h
file path:	dev\engine\Src\Core\Common\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef LORD_SYSTEM_H
#define LORD_SYSTEM_H
#include "Core.h"

namespace LORD
{
	class LORD_CORE_API LordSystem : public Singleton<LordSystem>
	{
	public:
		LordSystem(void);
		~LordSystem(void);

		static LordSystem* Instance()
		{
			return ms_pSingleton;
		}

		/*
		* resize window
		*/
		bool OnSize(unsigned int width, unsigned int height);

		/*
		* get fps
		*/
		unsigned int GetFPS() const;

		/*
		* get frameTime
		*/
		unsigned int GetFrameTime() const;

		/*
		* get frameCount
		*/
		unsigned int GetFrameCount() const;

		/*
		* create actor object
		*/
		ActorObject* CreateActor(const char* actorTemplate, const char* initSkillName = NULL, int AnUserData = 0, bool isUIActor = false, bool isMainActor = false, bool bSingleThread = false, bool isHide = false);

		/*
		* destroy actor object
		*/
		void DestroyActor(const char* actorName);

		/*
		* free all actor object
		*/
		void FreeActors();

		/*
		* enable skill camera
		*/
		void EnableSkillCamera(bool enable);

		/*
		* is enable skill camera
		*/
		bool isEnableSkillCamera() const;

		/*
		* create effect system
		*/
		EffectSystem* createEffectSystem(const char* name, const char* templateName, bool isUIeffect = false);

		/*
		* destroy effect system
		*/
		void destroyEffectSystem(const char* name);

		/*
		* destroy all effect system
		*/
		void destroyAllEffectSystems();

		/*
		* create scene
		*/
		Scene* LoadScene(const String& name, QueryObjectManager* manager = NULL, int aiLoadLv = 3);

		/*
		* create child scene
		*/
		Scene* loadChildScene(const String& name, QueryObjectManager* manager = NULL, int aiLoadLv = 3);

		/*
		* swap main scene and child scene
		*/
		Scene* swapScene();

		/*
		* close scene
		*/
		void closeScene();

		/*
		* render static mesh
		*/
		void RenderStaticMesh(bool bRender);

		/*
		*  is render static mesh
		*/
		bool IsRenderStaticMesh() const;

		/*
		* render skeleton mesh
		*/
		void RenderSkeletonMesh(bool bRender);

		/*
		* is render skeleton mesh
		*/
		bool IsRenderSkeletonMesh() const;

		/*
		* render effect
		*/
		void RenderEffect(bool bRender);

		/*
		* is render effect
		*/
		bool IsRenderEffect() const;
	};

}

#endif