#ifndef _MERGEMESHMANAGER_H
#define	_MERGEMESHMANAGER_H

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API MergeMeshManager : public ObjectAlloc , public Singleton<MergeMeshManager>
	{
	public:
		MergeMeshManager();
		virtual ~MergeMeshManager();

		TextureSet* GetTextureSet(String texturehash);
		TextureSet* AllocTextureSet(String texturehash, int width, int height);
		SkinnedMesh* GetSkinnedMesh(TextureSet* ptexSet);
		SkinnedMesh* AllocSkinnedMesh(TextureSet* ptexSet);
	protected:
		map<String, TextureSet*>::type mTextureSet;
		map<TextureSet*, SkinnedMesh*>::type mMergeMeshes;
	};
}
#endif