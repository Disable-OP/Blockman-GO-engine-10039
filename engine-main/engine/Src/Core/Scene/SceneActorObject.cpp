#include "Core.h"
#include "SceneActorObject.h"
#include "Actor/ActorManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Object/Root.h"
#include "Util/QueryObject.h"
#include "Scene/SceneNode.h"
#include "Actor/ActorManager.h"

namespace LORD
{
	const float editor_r = 0.1f;

	SceneActorObject::SceneActorObject()
		: GameObject()
		, m_pActor(NULL)
		, m_CurentChileNum(0)
		, m_pActorProVec(NULL)
	{
		GameObject::m_Type = GOT_LOGIC;
		m_Type = LOT_MONSTER;
		editor_canMirror = true;
		editor_canRotate = true;
		editor_canScale = true;
		m_isNeedSave   = false;
	}

	SceneActorObject::~SceneActorObject()
	{
		_unprepare();
	}

	void SceneActorObject::_prepare_io()
	{
	}

	void SceneActorObject::_prepare_res()
	{
	}

	void SceneActorObject::_prepare_obj()
	{
		m_pActor = ActorManager::Instance()->CreateActor(m_Resource,"idle");
		LordAssert(m_pActor);

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();
		LordAssert(m_SceneNode);

		//m_pEffectSystem->setParentSceneNode(m_SceneNode);
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

	void SceneActorObject::_prepare_editor()
	{
		if (Root::Instance()->isEditorMode())
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			QueryObjectManager* pManager = pScene->getQueryObjectManager();

			m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
			m_pHoverBoxQueryObject->setQueryFlag(0);
			m_pHoverBoxQueryObject->attachTo(m_SceneNode);

			m_pHoverBoxQueryObject->createAABBFrame(m_pActor->getActorAABB(), Color::YELLOW);

			m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
			m_pSelectBoxQueryObject->setQueryFlag(0);
			m_pSelectBoxQueryObject->attachTo(m_SceneNode);

			m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
		}
	}

	void SceneActorObject::_unprepare()
	{
		if (!m_IOprepared)
			return;

		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		if(pScene)
		{
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
		}

		TypeChileActorVec::iterator it = m_ChileActorVec.begin();
		for(;it != m_ChileActorVec.end(); ++it)
		{
			m_pActor->DetachChildActor(*it);
			ActorManager::Instance()->DestroyActor(*it);
		}
		m_ChileActorVec.clear();

		ActorManager::Instance()->DestroyActor(m_pActor);
		m_pActor = NULL;
		m_ParentNode->destroyChild(m_SceneNode);
		m_SceneNode = NULL;
		SceneManager::Instance()->getRootNode()->destroyChild(m_ParentNode);
		m_ParentNode = NULL;
		m_IOprepared = false;
	}

	std::pair<bool, Real> SceneActorObject::rayIntersects(const Ray& ray)
	{
		std::pair<bool, Real> result;
		Ray RayObject(ray);
		Matrix4 m_InversMat = m_SceneNode->getWorldMatrix();
		m_InversMat = m_InversMat.detInverse();
		RayObject.transform(m_InversMat);
		Real tMin = 0;
		result.first = RayObject.hitSphere(Sphere(Vector3::ZERO, editor_r), tMin);
		result.second = tMin;

		return result;
	}

	void SceneActorObject::_render()
	{
		LordAssert(m_pActor);

		//m_pActor

		if (Root::Instance()->isEditorMode())
		{
			m_pHoverBoxQueryObject->setVisible(getHovered());
		}
	}

	const Box& SceneActorObject::getWorldAABB() const
	{
		LordAssert(m_pActor);
		return m_worldAABB;
	}

	void SceneActorObject::updateWorldAABB()
	{
		LordAssert(m_pActor);
		//m_worldAABB = m_pActor->getActorAABB().transform(m_SceneNode->getWorldMatrix());
		m_worldAABB = m_pActor->getActorAABB();
		//m_Center = m_worldAABB.getCenter();

		if (Root::Instance()->isEditorMode())
		{
			if (m_pHoverBoxQueryObject)
			{
				m_pHoverBoxQueryObject->createAABBFrame(m_pActor->getActorAABB(), Color::BLUE);
			}
			if (m_pSelectBoxQueryObject)
			{
				m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
			}
		}
	}

	void SceneActorObject::frameMove( i32 deltaTime )
	{
		LordAssert(m_pActor);
		LORD::Vector3 pos = m_SceneNode->getWorldPosition();
		m_Oritation.z = 0.f;
		m_Oritation.x = 0.f;
		m_pActor->SetPosition(pos);
		m_pActor->SetOrientation(m_Oritation);
		//m_pActor->Update(deltaTime);

		size_t size = m_pActorProVec->size();
		for(size_t i = 0;i < size; ++i)
		{
			if(m_pActorProVec->at(i).name == "position")
			{
				String x = StringUtil::ToString(pos.x);
				String y = StringUtil::ToString(pos.y);
				String z = StringUtil::ToString(pos.z);
				String v = x + " " + y + " " + z; 
				m_pActorProVec->at(i).velue = v;
			}
			else if(m_pActorProVec->at(i).name == "num")
			{
				 int tnum = StringUtil::ParseInt(m_pActorProVec->at(i).velue) - 1;
				 if(tnum != m_CurentChileNum)
				 {
					 m_CurentChileNum = tnum;
					 creatChileActor(m_CurentChileNum);
				 }
			}
			else if(m_pActorProVec->at(i).name == "face")
			{
				String face = StringUtil::ToString(int(m_Yaw >= 0 ? m_Yaw:360 + m_Yaw));
				m_pActorProVec->at(i).velue = face;
			}
		}
	}

	bool SceneActorObject::getPropertyValue( const String & name, String &value )
	{
		TypePropertyVec::iterator it = m_pActorProVec->begin();
		for(; it != m_pActorProVec->end(); ++it)
		{
			if((*it).name  == name)
			{
				value = (*it).velue;
				return true;
			}
		}
		return false;
	}

	bool SceneActorObject::setPropertyValue( const String & name,const String & value )
	{
		TypePropertyVec::iterator it = m_pActorProVec->begin();
		for(; it != m_pActorProVec->end(); ++it)
		{
			if((*it).name  == name)
			{
				(*it).velue = value;
				if(name == "position")
				{
					m_SceneNode->setWorldPosition(StringUtil::ParseVec3(value));
					m_SceneNode->update(true);
					m_Position = StringUtil::ParseVec3(value);
				}
				else if (name == "face")
				{
					int face = StringUtil::ParseInt(value); 
					if(face >-1 && face < 360)
					{
						m_Yaw =  float(face < 180 ?  face : face-360);
						Quaternion q = reCaculateOritation(); 
						setOritation(q);
					}
				}
				return true;
			}
		}
		return false;
	}

	void SceneActorObject::creatChileActor( int num )
	{
		//数量有改变先释放原来的
		TypeChileActorVec::iterator it = m_ChileActorVec.begin();
		for(;it != m_ChileActorVec.end(); ++it)
		{
			m_pActor->DetachChildActor(*it);
			ActorManager::Instance()->DestroyActor(*it);
		}
		m_ChileActorVec.clear();

		float x,z,rad;
		String radStr;
		x = z = rad = 1.f;

		if(getPropertyValue("radius",radStr))
		{
			rad = StringUtil::ParseReal(radStr);
		}

		for(int i = 0; i< num; i++)
		{
			ActorObject* pAtor = ActorManager::Instance()->CreateActor(m_Resource, "idle");
			m_pActor->AttachChildActor("BuSuiFu",pAtor);
			x = Math::IntervalRandom(-rad,rad);
			float radx = x>0.f ? (rad-x):(rad+x);
			z = Math::IntervalRandom(-radx,radx);
			pAtor->GetSceneNodeMaster()->setLocalPosition(Vector3(x,0,z));
			m_ChileActorVec.push_back(pAtor);
		}
	}

}