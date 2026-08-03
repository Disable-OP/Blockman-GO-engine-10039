
#ifndef __ZOOM_H__
#define __ZOOM_H__

#include "GameObject.h"

namespace LORD
{
	class Scene;
	class StaticMeshObject_Normal;

	class LORD_CORE_API Zoom: public ObjectAlloc
	{
	public:
		typedef set<GameObject*>::type		GameObjectList;

	public:
		Zoom(Scene* scene, ui16 xUnitOffset, ui16 zUnitOffset);
		virtual ~Zoom();

		void		registerObjectToZoom(GameObject* object);

		void		unregisterObjectFromZoom(GameObject* object);

		void		registerObjectToZoom_UnPrepare(GameObject* object);

		void		unregisterObjectFromZoom_UnPrepare(GameObject* object);
		
		void		render(Scene* scene, ui32 mask);

		void		checkInList(GameObjectList& checklist);

		void		checkLoad(GameObjectList& Loadlist);

		const GameObjectList& getGameObjecs() const
		{
			return m_zoomObjectList;
		}
	protected:
		bool		_cullSmallerObject(StaticMeshObject_Normal* obj, float& ratio);
	protected:
		ui16			m_xUnitOffset;
		ui16			m_zUnitOffset;
		Scene*			m_ownerScene;
		// this just the reference of scene objects
		// zoom do not control the alloc and release of the objects
		GameObjectList	m_zoomObjectList;
		GameObjectList	m_zoomObjectList_unPrepare;

	};
}

#endif

