#include "GroupObject.h"

namespace LORD
{
	GroupObject::GroupObject( void )
		: GameObject()
	{
		m_Type = GOT_GROUP;
		editor_canRotate = true;
		editor_canScale = true;
	}

	GroupObject::~GroupObject( void )
	{
		_unprepare();
	}

	void GroupObject::addGameObject( GameObject* pObj, const Vector3& direct )
	{
		GroupObjectMap::iterator it = mGroupObjectMap.find(pObj);
		if (it == mGroupObjectMap.end())
		{
			ObjectInfo info;
			info.dir = direct;
			info.scale = pObj->getScale()/m_Scale;
			info.q = pObj->getOritation();
			mGroupObjectMap[pObj] = info;
		}
	}

	void GroupObject::UntieGameObject( GameObject* pObj )
	{
		GroupObjectMap::iterator it =  mGroupObjectMap.find(pObj);
		if (it != mGroupObjectMap.end())
		{
			mGroupObjectMap.erase(it);
		}
	}

	GroupObject::GroupObjectMap GroupObject::getGroupObjectList() const
	{
		return mGroupObjectMap;
	}

	void GroupObject::_unprepare()
	{
		mGroupObjectMap.clear();
	}
	
	void GroupObject::setPosition( const Vector3& pos )
	{
		m_Position = pos;
		GroupObjectMap::iterator it = mGroupObjectMap.begin();
		for (it = mGroupObjectMap.begin(); it!=mGroupObjectMap.end(); ++it)
		{
			GameObject* pObj = it->first;
			Vector3 pos = m_Position - it->second.dir;
			pObj->setPosition(pos);
		}
	}

	void GroupObject::getGameObjectlist( Scene::GameObjectList& list )
	{
		GroupObjectMap::iterator it = mGroupObjectMap.begin();
		for (it = mGroupObjectMap.begin(); it!=mGroupObjectMap.end(); ++it)
		{
			list.push_back(it->first);
		}
	}

	void GroupObject::updateWorldAABB()
	{
		m_WorldAABB.reset();
		GroupObjectMap::iterator it = mGroupObjectMap.begin();
		for (it = mGroupObjectMap.begin(); it!=mGroupObjectMap.end(); ++it)
		{
			m_WorldAABB.unionBox(it->first->getWorldAABB());
		}
	}

	void GroupObject::setOritation( const Quaternion& ori )
	{
		Quaternion q = Quaternion::IDENTITY;
		Matrix4 matR1, matR2;
		q.toMat4(matR1);
		ori.toMat4(matR2);
		matR1 = matR2*matR1;
		Quaternion objQ;
		objQ.fromMat4(matR1);
		m_Oritation = objQ;
		GroupObjectMap::iterator it = mGroupObjectMap.begin();
		for (it = mGroupObjectMap.begin(); it!=mGroupObjectMap.end(); ++it)
		{
			GameObject* pObj = it->first;
			it->second.q.toMat4(matR1);
			matR1 = matR2*matR1;
			Quaternion tempObjQ;
			tempObjQ.fromMat4(matR1);
			if (pObj)
			{
				pObj->setOritation(tempObjQ);
				pObj->reCaculateAngle();
			}
			Real len = it->second.dir.len();
			Vector3 dir = ori * it->second.dir; 
			dir.normalize();
			pObj->setPosition(m_Position - dir*len);
		} 
	}

	void GroupObject::setScale( const Vector3& scale )
	{
		m_Scale = scale;
		GroupObjectMap::iterator it = mGroupObjectMap.begin();
		for (it = mGroupObjectMap.begin(); it!=mGroupObjectMap.end(); ++it)
		{
			GameObject* pObj = it->first;
			pObj->setScale(scale*it->second.scale);
		}
	}

	void GroupObject::editor_refleshInfo()
	{
		GroupObjectMap::iterator it = mGroupObjectMap.begin();
		for (it = mGroupObjectMap.begin(); it!=mGroupObjectMap.end(); ++it)
		{
			GameObject* pObj = it->first;
			Vector3 direct = m_Position - it->first->getPosition();
			it->second.dir = direct;
			it->second.scale = pObj->getScale()/m_Scale;
			it->second.q = pObj->getOritation();
		}
	}

	void GroupObject::setVisible(bool visible)
	{
		m_bVisible = visible;
		GroupObjectMap::iterator it = mGroupObjectMap.begin();
		for (it = mGroupObjectMap.begin(); it != mGroupObjectMap.end(); ++it)
		{
			GameObject* pObj = it->first;
			pObj->setVisible(visible);
		}
	}

}