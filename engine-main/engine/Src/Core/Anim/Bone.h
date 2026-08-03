/********************************************************************
filename: 	Bone.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_BONE_H__
#define __LORD_BONE_H__

#include "Core.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

namespace LORD
{
	class Skeleton;

	class LORD_CORE_API Bone : public ObjectAlloc
	{
	public:
		typedef vector<Bone*>::type BoneList;

	public:
		Bone(Skeleton* pSkel, int id, int parentId, const String& name, const Quaternion& rot, const Vector3& pos, bool bHelper = false);
		virtual ~Bone();

	public:
		void			setParent(Bone* pBone);
		void			addChildren(Bone* pBone);

		Skeleton*		getSkeleton() const;
		bool			isUpper() const { return m_bUpper; }
		void			setUpper(bool bupper) { m_bUpper = bupper; if (!bupper) m_bUpRoot = false; }
		bool			isUpRoot() const { return m_bUpRoot; }
		void			setUpRoot(bool bUpRoot) { m_bUpRoot = bUpRoot; }
		int				getId() const;
		int				getParentId() const;
		const String&	getName() const;
		bool			isDummy() const;
		Bone*			getParent() const;
		const BoneList&	getChildren() const;
		const Quaternion&	getRotation() const;
		const Vector3&		getPosition() const;
		const Matrix4&		getMatrix() const;

	protected:
		Skeleton*	m_pSkel;
		Bone*		m_pParent;
		int			m_id;
		int			m_parentId;
		uint		m_shaderIdx;
		String		m_name;
		bool		m_bDummy;
		bool		m_bUpper;
		bool		m_bUpRoot;
		BoneList	m_children;
		Quaternion	m_rotation;
		Vector3		m_position;
		Matrix4		m_matrix;
	};
}

#endif