#include "Core.h"
#include "Zoom.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Object/Root.h"
#include "Scene/Camera.h"
#include "StaticMeshObject_Normal.h"

#define SQUALDOTCULL 0.00161315886f

namespace LORD
{

	Zoom::Zoom(Scene* scene, ui16 xUnitOffset, ui16 zUnitOffset)
		: m_ownerScene(scene)
		, m_xUnitOffset(xUnitOffset)
		, m_zUnitOffset(zUnitOffset)
	{
	}

	Zoom::~Zoom()
	{
		m_zoomObjectList.clear();
	}

	void Zoom::registerObjectToZoom( GameObject* object )
	{
		//assert(m_zoomObjectList.find(object)==m_zoomObjectList.end());
		m_zoomObjectList.insert(object);

		object->setOwnerZoom(this);
	}

	void Zoom::unregisterObjectFromZoom( GameObject* object )
	{
		object->setOwnerZoom(NULL);

		//assert(m_zoomObjectList.find(object)!=m_zoomObjectList.end());
		GameObjectList::iterator it = m_zoomObjectList.find(object);
		if (it!=m_zoomObjectList.end())
		{
			m_zoomObjectList.erase(it);
		}
	}

	void Zoom::registerObjectToZoom_UnPrepare( GameObject* object )
	{
		//assert(m_zoomObjectList_unPrepare.find(object)==m_zoomObjectList_unPrepare.end());
		m_zoomObjectList_unPrepare.insert(object);
	}

	void Zoom::unregisterObjectFromZoom_UnPrepare( GameObject* object )
	{
		//assert(m_zoomObjectList_unPrepare.find(object)!=m_zoomObjectList_unPrepare.end());
		GameObjectList::iterator it = m_zoomObjectList.find(object);
		if (it!=m_zoomObjectList_unPrepare.end())
		{
			m_zoomObjectList_unPrepare.erase(it);
		}
	}

	bool Zoom::_cullSmallerObject(StaticMeshObject_Normal* obj, float& ratio)
	{
		StaticMeshObject_Normal* pSmObjet = static_cast<StaticMeshObject_Normal*>(obj);
		float r = pSmObjet->m_Extent*pSmObjet->m_Extent / pSmObjet->m_SquaredLength;
		ratio = r;
		return (r > SQUALDOTCULL);
	}

	void Zoom::render(Scene* scene, ui32 mask)
	{
		const Frustum& frustum = SceneManager::Instance()->getMainCamera()->getFrustum();
		GameObjectList::iterator itorObject = m_zoomObjectList.begin();
		Camera* pCamera = SceneManager::Instance()->getMainCamera();
		for (; itorObject != m_zoomObjectList.end(); ++itorObject )
		{
			GameObject* object = *itorObject;
			//object->setInsideList(true);
			if (!object->getIOprepared() || object->getRequestPrepare())
				return;

			if (mask & object->getType())
			{
				if (frustum.intersect(object->getWorldAABB()))
				{
					if (object->m_Type == GOT_SMESH_L)
					{
						StaticMeshObject_Normal* pSm = static_cast<StaticMeshObject_Normal*>(object);
						pSm->m_SquaredLength = (pSm->GetPosition() - pCamera->getPosition()).lenSqr();
						float ratio = 0;
						if (_cullSmallerObject(pSm, ratio) || !scene->isEnableZoomCull())
						{
							scene->InsertSubSet(object);
						}
					}
					else
					{
						scene->InsertSubSet(object);
					}
				}
			}
		}
	}

	void Zoom::checkInList( GameObjectList& checklist )
	{
		const Frustum& frustum = SceneManager::Instance()->getMainCamera()->getFrustum();
		GameObjectList::iterator itorObject = m_zoomObjectList.begin();
		for (; itorObject != m_zoomObjectList.end(); ++itorObject )
		{
			GameObject* obj = *itorObject;
			if (!obj->m_bInsideList)
			{
				checklist.insert(obj);
			}
		}
	}

	void Zoom::checkLoad(GameObjectList& Loadlist)
	{
		bool bEditor = Root::Instance()->isEditorMode();
		// StreamThread* pThread = StreamThread::Instance();
		GameObjectList::iterator itorObject = m_zoomObjectList_unPrepare.begin();
		for (; itorObject != m_zoomObjectList_unPrepare.end();)
		{
			GameObject* obj = *itorObject;
			if ((obj->m_Type == GOT_POINTLIGHT||obj->m_Type == GOT_DIRLIGHT||obj->m_Type == GOT_SPOTLIGHT) && !bEditor)
			{
				++itorObject;
				continue;
			}
			if (!obj->m_IOprepared && !obj->m_bRequestPrepare)
			{
				Loadlist.insert(obj);
			}
			m_zoomObjectList_unPrepare.erase(itorObject++);
		}
	}

}