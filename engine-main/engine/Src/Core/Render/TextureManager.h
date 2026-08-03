/********************************************************************
filename: 	TextureManager.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_TEXTUREMANAGER_H__
#define __LORD_TEXTUREMANAGER_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API TextureManager : public ResourceManager, public Singleton<TextureManager>
	{
	public:
		static TextureManager* Instance();

	public:
		TextureManager();
		virtual ~TextureManager();

		virtual Texture* createTexture(const String& name, Dword usage = Texture::TU_STATIC, ResourceType rt = RESOURCE_TYPE_NORMAL);
		virtual Texture* createManual(const String& name, Texture::TexType texType, PixelFormat format, Dword usage, uint width, uint height, uint depth,
			int num_mips, const Buffer& buff);

	};
}

#endif