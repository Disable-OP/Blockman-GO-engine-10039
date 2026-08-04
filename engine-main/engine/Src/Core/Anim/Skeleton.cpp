#include "Core.h"
#include "Skeleton.h"
#include "Bone.h"
#include "Animation.h"
#include "Object/Root.h"
#include "Scene/SceneManager.h"
#include "SRTTrack.h"

namespace LORD
{
	//--------------------------------------------------------------------
	Skeleton::Skeleton(const String& name, ResourceType rt)
		:Resource(name, rt)
		, m_boneCount(0)
		, m_dummyCount(0)
		, m_upRootId(0)
		, m_pRootBone(NULL)
		, m_pSkeletonStream(NULL)
	{
	}

	Skeleton::~Skeleton()
	{
		unloadImpl();
	}

	size_t Skeleton::calculateSize() const
	{
		// not implement
		return 0;
	}

	bool Skeleton::prepareImpl()
	{
		m_pSkeletonStream = ResourceGroupManager::Instance()->openResource(mName);

		if (m_pSkeletonStream == NULL)
			return false;

		int version;
		m_pSkeletonStream->read(&version, sizeof(int));

		// load each bone
		String boneName;

		uint boneNameLen;
		char szBoneName[260];
		int parentId;
		float position[3];
		float rotation[4];
		Vector3 vPos;
		Quaternion qRot;

		// bone count
		int boneCount;
		m_pSkeletonStream->read(&boneCount, sizeof(int));
		for (int i = 0; i < boneCount; ++i)
		{
			m_pSkeletonStream->read(&boneNameLen, sizeof(uint));
			m_pSkeletonStream->read(&szBoneName, boneNameLen);
			szBoneName[boneNameLen] = 0;

			boneName = szBoneName;

			m_pSkeletonStream->read(&parentId, sizeof(int));

			m_pSkeletonStream->read(&position, sizeof(position));
			vPos.set(position[0], position[1], position[2]);

			m_pSkeletonStream->read(&rotation, sizeof(rotation));
			qRot.set(rotation[0], rotation[1], rotation[2], rotation[3]);

			createBone(parentId, boneName, qRot, vPos, false);
		}

		// dummy count
		int dummyCount;
		m_pSkeletonStream->read(&dummyCount, sizeof(int));
		for (int i = 0; i < dummyCount; ++i)
		{
			m_pSkeletonStream->read(&boneNameLen, sizeof(uint));
			m_pSkeletonStream->read(&szBoneName, boneNameLen);
			szBoneName[boneNameLen] = 0;
			boneName = szBoneName;
			m_pSkeletonStream->read(&parentId, sizeof(int));

			m_pSkeletonStream->read(&position, sizeof(position));
			vPos.set(position[0], position[1], position[2]);

			m_pSkeletonStream->read(&rotation, sizeof(rotation));
			qRot.set(rotation[0], rotation[1], rotation[2], rotation[3]);

			createBone(parentId, boneName, qRot, vPos, true);
		}

		// orgnize bones hierarchy
		for (int i = 0; i < (boneCount + dummyCount); ++i)
		{
			Bone* pBone = m_bones[i];

			if (pBone->getParentId() != -1)
			{
				Bone* pBoneParent = m_bones[pBone->getParentId()];
				pBone->setParent(pBoneParent);
				pBoneParent->addChildren(pBone);
			}
		}

		LordSafeDelete(m_pSkeletonStream);

		return true;
	}

	void Skeleton::setUpper(const StringArray& bones)
	{
		for (auto pbone : m_bones)
		{
			pbone->setUpper(false);
		}

		for (auto bname : bones)
		{
			Bone* pBone = getBone(bname);
			if (!pBone)
				continue;
			pBone->setUpper(true);
		}
	}

	void Skeleton::setUpper(const String& upperRootName)
	{
		for (auto pbone : m_bones)
		{
			pbone->setUpper(false);
		}

		Bone* proot = getBone(upperRootName);
		if (!proot)
			return;
		m_upRootId = proot->getId();
		list<Bone*>::type queue;
		queue.push_back(proot);
		while (!queue.empty())
		{
			Bone* pbone = queue.front();
			pbone->setUpper(true);
			queue.pop_front();
			for (auto b : m_bones)
			{
				if (b->getParent() == pbone)
					queue.push_back(b);
			}
		}

		// print all the bones's 
		/*for (auto pbone : m_bones)
		{
			LordLogInfo("isUpper[%d] isRoot[%d] name[%s]", pbone->isUpper() ? 1 : 0, pbone->isUpRoot() ? 1 : 0, pbone->getName().c_str());
		}*/
	}

	void Skeleton::unprepareImpl()
	{
	}

	bool Skeleton::loadImpl()
	{
		return true;
	}

	void Skeleton::unloadImpl()
	{
		for (size_t i = 0; i < m_bones.size(); ++i)
			LordSafeDelete(m_bones[i]);
	}

	Bone* Skeleton::createBone(int parentId, const String& name, const Quaternion& rot, const Vector3& pos, bool bHelper)
	{
		Bone* pBone = NULL;
		try
		{
			int boneId = (int)m_bones.size();
			pBone = LordNew Bone(this, boneId, parentId, name, rot, pos, bHelper);
			m_bones.push_back(pBone);
			m_bonesByName.insert(std::make_pair(name, pBone));

			if (bHelper)
				++m_dummyCount;
			else
				++m_boneCount;

			if (!m_pRootBone)
				m_pRootBone = pBone;
		}
		catch (Exception& e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pBone;
	}
}