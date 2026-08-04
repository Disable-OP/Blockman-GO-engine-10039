#include "Core.h"
#include "SkeletonMeshObject.h"
#include "Model/MeshManager.h"
#include "SkinedEntity.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Object/Root.h"
#include "Util/QueryObject.h"
#include "Anim/AnimBlender.h"
#include "Anim/AnimManager.h"
#include "Anim/Animation.h"
#include "Anim/SkeletonManager.h"
#include "Anim/Skeleton.h"
#include "Anim/AnimState.h"
#include "Model/SkinnedMesh.h"
#include "SceneNode.h"
#include "Render/ShaderProgramManager.h"

namespace LORD
{
	SkeletonMeshObject::SkeletonMeshObject()
		: GameObject()
		, m_isPlayAnim(false)
		, m_SkinEntity(NULL)
		, m_Skeleton(NULL)
		, m_SkinMesh(NULL)
		, m_Animation(NULL)
		, m_AnimationState(NULL)
		, m_AnimationBlender(NULL)
		, m_InnerAnimationTime(0)
	{
		m_Type = GOT_DMESH;
		editor_canMirror = true;
		editor_canRotate = true;
		editor_canScale = true;

		SamplerState::SamplerDesc desc = m_Material->getSamplerState(0)->getDesc();
		desc.mipFilter = SamplerState::FO_NONE;
		m_Material->setSamplerState(0, desc);

		m_ShaderDefine = "SkinedMeshForScene";
	}

	SkeletonMeshObject::~SkeletonMeshObject()
	{
		_unprepare();
	}

	void SkeletonMeshObject::_prepare_io()
	{
		m_Skeleton->prepare();
		m_SkinMesh->prepare();
		m_Animation->prepare();
	}

	void SkeletonMeshObject::_prepare_res()
	{
		m_Material->setShaderProgram(ShaderProgramManager::Instance()->getShaderProgram(m_ShaderDefine));

		m_SkinMesh = MeshManager::Instance()->createSkinnedMesh(m_Resource, false, true);
		if (m_SkeletonName.empty())
		{
			m_SkeletonName = m_Resource.substr(0, m_Resource.length()-4);
			m_SkeletonName += "skel";
		}
		if (m_AnimationName.empty())
		{
			m_AnimationName = m_Resource.substr(0, m_Resource.length()-5);
			m_AnimationName += "_idle.anim";
		}
		m_Skeleton = SkeletonManager::Instance()->createSkeleton(m_SkeletonName);

		m_Animation = AnimManager::Instance()->createAnim(m_AnimationName);
	}

	void SkeletonMeshObject::_prepare_obj()
	{
		m_Skeleton->load();
		m_SkinMesh->load();
		if (!m_Animation->load())
		{
			AnimManager::Instance()->releaseResource(m_Animation);
			m_Animation = NULL;
		}

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();

		if (m_Animation)
		{
			m_AnimationState = LordNew AnimState(m_Animation);
			m_AnimationState->enableLoop(true);
			m_AnimationState->enable(true);
			m_AnimationState->setTimePosition(0);
			m_AnimationBlender = LordNew AnimBlender(m_Skeleton);
			m_AnimationBlender->setBlendMode(AnimBlender::BM_REPLACE);
			m_AnimationBlender->blend(m_AnimationState, 0);
		}

		m_SkinEntity = LordNew SkinedEntity;

		m_SkinEntity->setSkeleton(m_Skeleton);
		m_SkinEntity->setMesh(m_SkinMesh);
		m_SkinEntity->setMaterial(m_Material);
		m_SkinEntity->setRenderType(ModelEntity::RT_FOR_SCENE);
		m_SkinEntity->setAnimBlender(m_AnimationBlender);
		m_SkinEntity->attachTo(m_SceneNode);
		m_SkinEntity->createRenderable();
		// ¼ÓÔØÄ£ÐÍÌùÍ¼
		m_SkinEntity->prepareTexture(Root::Instance()->isEditorMode(), __FUNCTION__);

		//updateAnimation();

		// ----update scene node
		m_SceneNode->setWorldPosition(m_Position);
		m_SceneNode->setWorldOrientation(m_Oritation);
		m_SceneNode->setLocalScaling(m_Scale);
		m_SceneNode->update();
		updateWorldAABB();

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		scene->registerGameObjectToZoom(this);

		m_IOprepared = true;
	}

	void SkeletonMeshObject::_prepare_editor()
	{
		if (Root::Instance()->isEditorMode())
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			QueryObjectManager* pManager = pScene->getQueryObjectManager();

			m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
			m_pHoverBoxQueryObject->setQueryFlag(0);
			m_pHoverBoxQueryObject->attachTo(m_SceneNode);

			m_pHoverBoxQueryObject->createAABBFrame(m_SkinEntity->getLocalAABB(), Color::BLUE);

			m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
			m_pSelectBoxQueryObject->setQueryFlag(0);
			m_pSelectBoxQueryObject->attachTo(m_SceneNode);

			m_pSelectBoxQueryObject->createAABBFrame(m_SkinEntity->getLocalAABB(), Color::YELLOW);
		}
	}

	void SkeletonMeshObject::_unprepare()
	{
		if (m_IOprepared)
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			pScene->unregisterGameObjectFromZoom(this);
			pScene->registerGameObjectToZoom_unprepare(this);

			if (Root::Instance()->isEditorMode())
			{
				QueryObjectManager* pManager = pScene->getQueryObjectManager();

				pManager->destroyQueryObject(m_pHoverBoxQueryObject);
				pManager->destroyQueryObject(m_pSelectBoxQueryObject);
				m_pHoverBoxQueryObject = NULL;
				m_pSelectBoxQueryObject = NULL;
			}

			LordSafeDelete(m_SkinEntity);
			m_ParentNode->destroyChild(m_SceneNode);
			m_SceneNode = NULL;

			m_IOprepared = false;
		}

		LordSafeDelete(m_AnimationBlender);
		LordSafeDelete(m_AnimationState);
		if(m_Animation)
		{
			AnimManager::Instance()->releaseResource(m_Animation);
			m_Animation = NULL;
		}
		if(m_SkinMesh)
		{
			MeshManager::Instance()->releaseResource(m_SkinMesh);
			m_SkinMesh = NULL;
		}

		if(m_Skeleton)
		{
			SkeletonManager::Instance()->releaseResource(m_Skeleton);
			m_Skeleton = NULL;
		}
	}

	void SkeletonMeshObject::_render()
	{
		LordAssert(m_SkinEntity);
		m_SkinEntity->updateRenderQueue(m_RenderQueue);

		if (Root::Instance()->isEditorMode())
		{
			m_pHoverBoxQueryObject->setVisible(getHovered());
			m_pSelectBoxQueryObject->setVisible(getSelected());
		}
	}

	const Box& SkeletonMeshObject::getWorldAABB() const
	{
		LordAssert(m_SkinEntity);
		return m_worldAABB;
	}

	void SkeletonMeshObject::updateWorldAABB()
	{
		LordAssert(m_SkinEntity);
		m_worldAABB = m_SkinEntity->getLocalAABB().transform(m_SceneNode->getWorldMatrix());
		//m_Center = m_worldAABB.getCenter();

		if (Root::Instance()->isEditorMode())
		{
			if (m_pHoverBoxQueryObject)
			{
				m_pHoverBoxQueryObject->createAABBFrame(m_SkinEntity->getLocalAABB(), Color::BLUE);
			}
			if (m_pSelectBoxQueryObject)
			{
				m_pSelectBoxQueryObject->createAABBFrame(m_SkinEntity->getLocalAABB(), Color::YELLOW);
			}
		}
	}

	void SkeletonMeshObject::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);

		lst.push_back(std::pair<String, bool>(objToken[TOKEN_DM_SKELETON], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_DM_ANIMATION], true));
	}

	bool SkeletonMeshObject::getPropertyType( const String & name, PropertyType &type )
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}

		if(name == objToken[TOKEN_DM_SKELETON])
		{
			type = PT_STRING; return true;
		}
		else if (name == objToken[TOKEN_DM_ANIMATION])
		{
			type = PT_STRING; return true;
		}

		return false;
	}

	bool SkeletonMeshObject::getPropertyValue( const String & name, String &value )
	{
		if (GameObject::getPropertyValue(name, value))
		{
			return true;
		}

		if(name == objToken[TOKEN_DM_SKELETON])
		{
			value = m_SkeletonName;
			return true;
		}
		else if(name == objToken[TOKEN_DM_ANIMATION])
		{
			value = m_AnimationName;
			return true;
		}

		return false;
	}

	bool SkeletonMeshObject::setPropertyValue( const String & name,const String & value )
	{
		if (GameObject::setPropertyValue(name, value))
		{
			return true;
		}

		if(name == objToken[TOKEN_DM_SKELETON])
		{
			m_SkeletonName = value;
			if (m_IOprepared)
			{
				updateSkeleton();
			}
			return true;
		}
		else if(name == objToken[TOKEN_DM_ANIMATION])
		{
			m_AnimationName = value;

			if (m_IOprepared)
			{
				updateAnimation();
			}
			return true;
		}

		return false;
	}

	void SkeletonMeshObject::updateAnimation()
	{
		AnimManager* pAnimMgr = AnimManager::Instance();
		if (m_Animation)
		{
			pAnimMgr->releaseResource(m_Animation);
		}
		m_Animation = pAnimMgr->createAnim(m_AnimationName);
		if (m_Animation->load())
		{
			AnimState* pOldState = m_AnimationState;
			m_AnimationState = LordNew AnimState(m_Animation);
			m_AnimationState->enableLoop(true);
			m_AnimationState->enable(true);
			m_AnimationState->setTimePosition(0);
			if (!m_AnimationBlender)
			{
				m_AnimationBlender = LordNew AnimBlender(m_Skeleton);
				m_AnimationBlender->setBlendMode(AnimBlender::BM_REPLACE);
			}
			m_AnimationBlender->blend(m_AnimationState, 0);
			LordSafeDelete(pOldState);
		}
	}

	void SkeletonMeshObject::frameMove( i32 deltaTime )
	{
		if(m_AnimationBlender)
		{
			m_SceneNode->update();

			m_AnimationBlender->frameMove(deltaTime);

			m_SkinEntity->update();

			//updateWorldAABB();
		}
	}

	void SkeletonMeshObject::updateSkeleton()
	{
		SkeletonManager::Instance()->releaseResource(m_Skeleton);
		m_Skeleton = SkeletonManager::Instance()->createSkeleton(m_SkeletonName);
		m_Skeleton->load();

		LordSafeDelete(m_AnimationBlender);
		m_AnimationBlender = LordNew AnimBlender(m_Skeleton);
		m_AnimationBlender->setBlendMode(AnimBlender::BM_REPLACE);

		updateAnimation();

		m_SkinEntity->setSkeleton(m_Skeleton);
		m_SkinEntity->setAnimBlender(m_AnimationBlender);
	}
}
