#include "MergeMeshManager.h"
#include "Model/MeshManager.h"
#include "Model/SkinnedMesh.h"

namespace LORD
{
	static int s_MergeIndex = 0;
	MergeMeshManager::MergeMeshManager()
		:Singleton()
	{
	}
	
	MergeMeshManager::~MergeMeshManager()
	{
		for (auto& it : mMergeMeshes)
		{
			MeshManager::Instance()->releaseResource(it.second);
		}
		mMergeMeshes.clear();
		for (auto& it : mTextureSet)
		{
			LordDelete(it.second);
		}
		mTextureSet.clear();
	}

	TextureSet * MergeMeshManager::GetTextureSet(String texturehash)
	{
		auto it = mTextureSet.find(texturehash);
		if (it != mTextureSet.end())
		{
			return it->second;
		}
		return nullptr;
	}

	TextureSet* MergeMeshManager::AllocTextureSet(String texturehash, int width, int height)
	{
		//LordAssert(mTextureSet.find(texturehash) == mTextureSet.end());
		TextureSet* ret = LordNew TextureSet(width, height);
		ret->CreateTexture();
		//mTextureSet.insert(std::make_pair(texturehash, ret));

		return ret;
	}
	SkinnedMesh* MergeMeshManager::GetSkinnedMesh(TextureSet * ptexSet)
	{
		SkinnedMesh* pSkinnedMesh = nullptr;
		auto it = mMergeMeshes.find(ptexSet);
		if (it != mMergeMeshes.end())
		{
			return it->second;
		}
		return nullptr;
	}
	SkinnedMesh * MergeMeshManager::AllocSkinnedMesh(TextureSet * ptexSet)
	{
		//LordAssert(mMergeMeshes.find(ptexSet) == mMergeMeshes.end());
		SkinnedMesh* pSkinnedMesh = MeshManager::Instance()->createSkinnedMesh("MergeMesh_" + LORD::StringUtil::ToString(s_MergeIndex++));
		//mMergeMeshes.insert(std::make_pair(ptexSet, pSkinnedMesh));
		return pSkinnedMesh;
	}
}