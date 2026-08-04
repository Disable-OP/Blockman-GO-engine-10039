/*
from Ogre
*/
#include "Core.h"
#include "TextureManager.h"
#include "Object/Exception.h"
#include "Object/Root.h"
#include "Image/PixelFormat.h"

namespace LORD
{
	//---------------------------------------------------------------------
	TextureManager* TextureManager::Instance()
	{
		return ms_pSingleton;
	}

	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
		// subclasses should unregister with resource group manager
	}

	Texture* TextureManager::createTexture(const String& name, Dword usage, ResourceType rt)
	{
		Texture* pTexture =(Texture*)ResourceManager::createResource(name, rt);
		if(pTexture)
		{
			pTexture->m_usage = usage;
		}

		return pTexture;
	}

	Texture* TextureManager::createManual(const String& name, Texture::TexType texType, PixelFormat format, Dword usage,
		uint width, uint height, uint depth, int numMipmaps, const Buffer& buff)
	{
		Texture *pTexture = (Texture*)createNewResource(name, RESOURCE_TYPE_MANUAL);
		pTexture->m_texType = texType;
		pTexture->m_pixFmt = format;
		pTexture->m_usage = usage;
		pTexture->m_width = width;
		pTexture->m_height = height;
		pTexture->m_depth = depth;
		pTexture->m_numMipmaps = numMipmaps;

		if(pTexture->m_numMipmaps > Texture::MAX_MINMAPS)
		{
			pTexture->m_numMipmaps = Texture::MAX_MINMAPS;
			LordLogWarning("Over the max support mipmaps, using the max mipmaps num.");
		}
		else
		{
			pTexture->m_numMipmaps = (numMipmaps > 0? numMipmaps : 1);
		}

		if(!pTexture->create2D(format, usage, width, height, numMipmaps, buff))
		{
			LordSafeDelete(pTexture);
		}
		else
		{
			pTexture->m_size = pTexture->m_uploadedSize;
			if (Root::Instance()->getEnableFrameProfile())
			{
				Root::Instance()->frameState().incrUploadTextureSizeInBytes(pTexture->m_uploadedSize);
			}
		}

		return pTexture;
	}
}