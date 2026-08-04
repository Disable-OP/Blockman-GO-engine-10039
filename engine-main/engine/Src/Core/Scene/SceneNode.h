/********************************************************************
filename: 	SceneNode.h
file path:	dev\engine\Src\Core\Scene\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SCENENODE_H__
#define __LORD_SCENENODE_H__

#include "Core.h"

namespace LORD
{

	class LORD_CORE_API SceneNode : public ObjectAlloc
	{
		friend class SceneManager;
		friend class ActorManager;
		typedef set<SceneNode*>::type ChildNodeSet;

	public:
		SceneNode();
		virtual ~SceneNode();

	public:
		SceneNode*			createChild();
		void				destroyChild(SceneNode* pChild);
		void				destroyAllChildren();

		void				setParent(SceneNode* pParent);
		SceneNode*			getParent() const;

		void				setVisible(bool bVisible);
		bool				isVisible() const;

		void				scale(const Vector3& scl);
		void				roll(const Real randian);
		void				pitch(const Real randian);
		void				yaw(const Real randian);
		void				rotate(const Vector3& vAxis, const Real randian);
		void				rotate(const Quaternion& rot);
		void				translate(const Vector3& d);

		void				setLocalScaling(const Vector3& scl);
		void				setLocalOrientation(const Quaternion& ort);
		void				setLocalPosition(const Vector3& pos);

		void				setWorldOrientation(const Quaternion& ort);
		void				setWorldPosition(const Vector3& pos);

		// only update this level scene node.
		void				update(bool bUpdateChildren = false);

		const Vector3&		getLocalScaling() const;
		const Quaternion&	getLocalOrientation() const;
		const Vector3&		getLocalPosition() const;
		const Vector3&		getWorldScaling() const;
		const Quaternion&	getWorldOrientation() const;
		const Vector3&		getWorldPosition() const;

		// this function oly to Accelerate the sceneNode.
		// if you dont setPos/ori/scale you will get the tempMatrix when you call getWorldMatrix function.
		void				setWorldMatrix(const Matrix4& tempMatrix);
		const Matrix4&		getWorldMatrix();
		Matrix4*			getWorldMatrixPtr();
		Matrix4				getInverseWorldMatrix() const;

	protected:
		void			convertWorldToLocalPosition(Vector3& posLocal, const Vector3& posWorld);
		void			convertWorldToLocalOrientation(Quaternion& ortLocal, const Quaternion& ortWorld);
		inline void		needUpdate();
		void			updateImpl(bool bUpdateChildren);

	protected:
		bool			m_bVisible;

		bool			m_bModify;      //for caculate. ie: getWorldPostion
		bool			m_bMatrixDirty; //for rendering.

		SceneNode*		m_pParent;
		// only to notify the parent is modifyed.
		ChildNodeSet    m_children;

		Vector3			m_posLocal;
		Vector3			m_sclLocal;
		Vector3			m_posWorld;
		Vector3			m_sclWorld;
		Quaternion		m_ortWorld;
		Quaternion		m_ortLocal;

		Matrix4			m_matWorld;/// Cached derived transform as a 4x4 matrix
	};
}

#endif