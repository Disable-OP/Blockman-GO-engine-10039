#include "LordSystem.h"
#include "Object/Root.h"
#include "Actor/ActorManager.h"
#include "Effect/EffectSystemManager.h"

namespace LORD
{
	LordSystem::LordSystem(void)
	{
	}

	LordSystem::~LordSystem(void)
	{
	}

	bool LordSystem::OnSize(unsigned int width, unsigned int height)
	{
		return Root::Instance()->onSize(width, height);
	}

	unsigned int LordSystem::GetFPS() const
	{
		return Root::Instance()->getFPS();
	}

	unsigned int LordSystem::GetFrameTime() const
	{
		return Root::Instance()->getFrameTime();
	}

	unsigned int LordSystem::GetFrameCount() const
	{
		return Root::Instance()->getFrameCount();
	}

	ActorObject* LordSystem::CreateActor(const char* actorTemplate, const char* initSkillName/*=NULL*/, int AnUserData/*=0*/, bool isUIActor/*=false*/, bool isMainActor/*=false*/, bool bSingleThread/*=false*/, bool isHide/*=false*/)
	{
		return ActorManager::Instance()->CreateActor(actorTemplate, initSkillName, AnUserData, isUIActor, isMainActor, bSingleThread, isHide);
	}

	void LordSystem::DestroyActor(const char* actorName)
	{
		ActorManager::Instance()->DestroyActorByID(actorName);
	}

	void LordSystem::FreeActors()
	{
		ActorManager::Instance()->FreeActors();
	}

	void LordSystem::EnableSkillCamera(bool enable)
	{
		ActorManager::Instance()->SetActorsSkillCameraEnable(enable);
	}

	bool LordSystem::isEnableSkillCamera() const
	{
		return ActorManager::Instance()->GetActorsSkillCameraEnable();
	}

	EffectSystem* LordSystem::createEffectSystem(const char* name, const char* templateName, bool isUIeffect/*=false*/)
	{
		return EffectSystemManager::Instance()->createEffectSystem(name, templateName, isUIeffect);
	}

	void LordSystem::destroyEffectSystem(const char* name)
	{
		EffectSystemManager::Instance()->destroyEffectSystem(EffectSystemManager::Instance()->getEffectSystem(name));
	}

	void LordSystem::destroyAllEffectSystems()
	{
		EffectSystemManager::Instance()->destroyAllEffectSystems();
	}

	Scene* LordSystem::LoadScene(const String& name, QueryObjectManager* manager /*= NULL*/, int aiLoadLv /*= 3 */)
	{
		return SceneManager::Instance()->loadScene(name, manager, aiLoadLv);
	}

	Scene* LordSystem::loadChildScene(const String& name, QueryObjectManager* manager /*= NULL*/, int aiLoadLv /*= 3 */)
	{
		return SceneManager::Instance()->loadChildScene(name, manager, aiLoadLv);
	}

	Scene* LordSystem::swapScene()
	{
		return SceneManager::Instance()->swapScene();
	}

	void LordSystem::closeScene()
	{
		SceneManager::Instance()->closeScene();
	}

	void LordSystem::RenderStaticMesh(bool bRender)
	{
		SceneManager::Instance()->RenderMaskStaticMesh(bRender);
	}

	bool LordSystem::IsRenderStaticMesh() const
	{
		return SceneManager::Instance()->IsRenderMaskStaticMesh();
	}

	void LordSystem::RenderSkeletonMesh(bool bRender)
	{
		SceneManager::Instance()->RenderMaskSkeletonMesh(bRender);
	}

	bool LordSystem::IsRenderSkeletonMesh() const
	{
		return SceneManager::Instance()->IsRenderMaskSkeletonMesh();
	}

	void LordSystem::RenderEffect(bool bRender)
	{
		SceneManager::Instance()->RenderMaskEffect(bRender);
	}

	bool LordSystem::IsRenderEffect() const
	{
		return SceneManager::Instance()->IsRenderMaskEffect();
	}
}