#include "GLES2RenderStd.h"
#include "GLES2TextureManager.h"
#include "GLES2Texture.h"

namespace LORD
{
	GLES2TextureManager::GLES2TextureManager()
		: TextureManager()
	{
	}

	GLES2TextureManager::~GLES2TextureManager()
	{
	}

	Resource* GLES2TextureManager::createImpl(const String& name, ResourceType rt)
	{
		Texture *pTexture = LordNew GLES2Texture(name, rt);
		return pTexture;
	}
}
