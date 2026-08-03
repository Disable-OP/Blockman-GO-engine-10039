#include "GUIImagesetManager.h"
#include "GUIImageset.h"
#include "GUIRenderManager.h"
#include "Render/Material.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "GUIRenderImage.h"

using namespace rapidxml;

namespace LORD
{
	//---------------------------------------------------------------------
	GUIImagesetManager* GUIImagesetManager::Instance()
	{
		return ms_pSingleton;
	}

	GUIImagesetManager::GUIImagesetManager()
	{

	}

	GUIImagesetManager::~GUIImagesetManager()
	{
	}

	GUIImageset* GUIImagesetManager::CreateFromFile(const GUIString& strName)
	{
		GUIImageset* pImageset = (GUIImageset*)ResourceManager::createResource(strName.c_str(), RESOURCE_TYPE_NORMAL);
		pImageset->load();
		return pImageset;
	}

	GUIImageset* GUIImagesetManager::CreateManual( const GUIString& strName, const int nTextureSize )
	{
		GUIImageset* pImageset = (GUIImageset*)ResourceManager::createNewResource(strName.c_str(), RESOURCE_TYPE_MANUAL);
		if (!pImageset)
		{
			return NULL;
		}

		int nBufferSize = nTextureSize * nTextureSize;
		unsigned char* pTempBuffer = (unsigned char*)LordMalloc(nBufferSize);
		memset(pTempBuffer, 0, nBufferSize);
		Buffer buff(nBufferSize, pTempBuffer);

		Texture* pTexture = TextureManager::Instance()->createManual(strName.c_str(), Texture::TT_2D, PF_A8_UNORM, Texture::TU_GPU_READ, nTextureSize, nTextureSize, 1, 1, buff);

		pImageset->m_pTexture = pTexture;
		pImageset->m_fNativeHorzRes = (float)nTextureSize;
		pImageset->m_fNativeVertRes = (float)nTextureSize;
		pImageset->m_fInverseNativeHorzRes = 1.0f / nTextureSize;
		pImageset->m_fInverseNativeVertRes = 1.0f / nTextureSize;

		LordFree(pTempBuffer);
		pTempBuffer = NULL;

		SamplerState* pState = GUIRenderManager::Instance()->GetMaterial()->getSamplerStateByTexStage(0);

		if(pTexture)
			pTexture->setSamplerState(pState);

		return pImageset;
	}

	void GUIImagesetManager::ParseImageString( const GUIString& strWholeName, GUIString& strSetName, GUIString& strImageName )
	{
		if (strWholeName.empty())
			return;

		char strImageSet[128];
		char strImage[128];
		memset(strImageSet, 0, 128);
		memset(strImage, 0, 128);

		sscanf(strWholeName.c_str(), " set:%127s image:%127s", strImageSet, strImage);

		strSetName = strImageSet;
		strImageName = strImage;
	}

	Resource* GUIImagesetManager::createImpl( const String& name, ResourceType rt)
	{
		return LordNew GUIImageset(name, rt);
	}

	GUIImageset* GUIImagesetManager::createImageset(const GUIString& strName, ResourceType rs)
	{
		GUIImageset* pImageset = (GUIImageset*)ResourceManager::createResource(strName.c_str(), rs);
		return pImageset;
	}

	GUIImageset* GUIImagesetManager::createOrRetrieveImageset(const GUIString& strName, ResourceType rs)
	{
		ResourceCreateOrRetrieveResult res = ResourceManager::createOrRetrieve(strName.c_str(), rs);
		GUIImageset* pImageset = (GUIImageset*)res.first;
		return pImageset;
	}

	void GUIImagesetManager::handleImagesetRelease()
	{
		vector<GUIImageset*>::type::iterator iter = m_deleteList.begin();

		while (iter != m_deleteList.end())
		{
			if ((*iter)->isLoaded())
			{
				// 收集起来的队列，如果是已经加载完成了才释放，否则的话不处理
#ifdef MULTI_THREAD_LOAD_IMAGESET
				if (StreamThread::Instance() && StreamThread::Instance()->IsRunning() && (*iter)->getRefrenceCount() == 1)
				{
					ImagesetReleaseEvent* pEvent = LordNew ImagesetReleaseEvent((*iter));
					StreamThread::Instance()->AddRequest(pEvent);
				}
				else
				{
					GUIImagesetManager::Instance()->releaseResource((*iter));
				}
#else
				GUIImagesetManager::Instance()->releaseResource((*iter));
#endif
				iter = m_deleteList.erase(iter);
			}
			else
			{
				++iter;
			}

		}

	}

	void GUIImagesetManager::pushbackReleaseImageset(GUIImageset* imageset)
	{
		m_textureToImageSetMap.erase(imageset->GetTexture());
		m_deleteList.push_back(imageset);
	}

	GUIImageset* GUIImagesetManager::findImageSet(const Texture * texture) const
	{
		auto iter = m_textureToImageSetMap.find(texture);
		if (iter == m_textureToImageSetMap.end())
		{
			return nullptr;
		}
		return iter->second;
	}

	void GUIImagesetManager::registerGUIImageset(GUIImageset * imageset)
	{
		m_textureToImageSetMap[imageset->GetTexture()] = imageset;
	}

}
