/********************************************************************
filename: 	GLES2TextureManager.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2TEXTUREMANAGER_H__
#define __LORD_GLES2TEXTUREMANAGER_H__

#include "Render/TextureManager.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2TextureManager : public TextureManager
	{
	public:
		GLES2TextureManager();
		~GLES2TextureManager();

	protected:
		Resource*	createImpl(const String& name, ResourceType rt);
	};
}

#endif
