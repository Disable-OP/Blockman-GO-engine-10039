#include "Core.h"
#include "Bone.h"
#include "Skeleton.h"
#include "Object/Exception.h"
#include "Render/Renderer.h"

namespace LORD
{
	Bone::Bone(Skeleton* pSkel, int id, int parentId, const String& name, const Quaternion& rot, const Vector3& pos, bool bHelper)
		:m_pSkel(pSkel)
		, m_pParent(NULL)
		, m_id(id)
		, m_parentId(parentId)
		, m_name(name)
		, m_bDummy(bHelper)
		, m_bUpRoot(false)
		, m_bUpper(false)
		, m_rotation(rot)
		, m_position(pos)
	{
		if (!pSkel)
			LordException("Skeleton is NULL.");

		if (pSkel->getBoneById(id))
			LordException("The Bone [%d] is already exist.", id);

		/*
		if(parentId != -1)
		{
		m_pParent = pSkel->getBoneById(parentId);
		if(!m_pParent)
		LordException(_T("The parent of Bone [%d] is not exist."), parentId);

		m_pParent->m_children.push_back(this);
		}
		*/

		m_matrix.fromQuan(rot);
		m_matrix.translate(pos);
	}

	Bone::~Bone()
	{
	}

	void Bone::setParent(Bone* pBone)
	{
		LordAssert(pBone);
		m_pParent = pBone;
	}

	void Bone::addChildren(Bone* pBone)
	{
		LordAssert(pBone);
		m_children.push_back(this);
	}

	Skeleton* Bone::getSkeleton() const
	{
		return m_pSkel;
	}

	int Bone::getId() const
	{
		return m_id;
	}

	int Bone::getParentId() const
	{
		return m_parentId;
	}

	const String& Bone::getName() const
	{
		return m_name;
	}

	bool Bone::isDummy() const
	{
		return m_bDummy;
	}

	Bone* Bone::getParent() const
	{
		return m_pParent;
	}

	const Bone::BoneList& Bone::getChildren() const
	{
		return m_children;
	}

	const Quaternion& Bone::getRotation() const
	{
		return m_rotation;
	}

	const Vector3& Bone::getPosition() const
	{
		return m_position;
	}

	const Matrix4& Bone::getMatrix() const
	{
		return m_matrix;
	}
}