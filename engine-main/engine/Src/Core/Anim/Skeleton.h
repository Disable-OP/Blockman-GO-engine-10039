/********************************************************************
filename: 	Skeleton.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SKELETON_H__
#define __LORD_SKELETON_H__

#include "Core.h"
#include "Resource/Resource.h"

namespace LORD
{
	class LORD_CORE_API Skeleton : public Resource
	{
	public:
		friend class SkeletonManager;
		Skeleton(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);
		virtual ~Skeleton();

		typedef vector<Bone*>::type			BoneList;
		typedef map<String, Bone*>::type	BoneListByName;

	public:
		virtual Bone*		createBone(int parentId, const String& name, const Quaternion& rot, const Vector3& pos, bool bHelper = false);
		Bone* getBoneById(int boneId) const
		{
			if (boneId < 0 || boneId >= (int)m_bones.size())
				return NULL;

			return m_bones[boneId];
		}
		Bone* getBone(const String& boneName)
		{
			BoneListByName::iterator it = m_bonesByName.find(boneName);
			return (it != m_bonesByName.end()) ? it->second : NULL;
		}

		void setUpper(const StringArray& bones);
		void setUpper(const String& upperRootName);

		uint getUpRootId() const
		{
			return m_upRootId;
		}

		uint getBoneCount() const
		{
			return m_boneCount;
		}

		uint getDummyCount() const
		{
			return m_dummyCount;
		}

		uint getTotalCount() const
		{
			return (uint)m_bones.size();
		}

		const BoneList& getBones() const
		{
			return m_bones;
		}

	protected:
		virtual size_t			calculateSize() const;
		virtual bool			prepareImpl();
		virtual void			unprepareImpl();
		virtual bool			loadImpl();
		virtual void			unloadImpl();

	protected:
		BoneList		m_bones;
		BoneListByName	m_bonesByName;
		uint			m_boneCount;		// without dummy
		uint			m_upRootId;
		uint			m_dummyCount;
		Bone			*m_pRootBone;

		DataStream*		m_pSkeletonStream;
	};
}

#endif