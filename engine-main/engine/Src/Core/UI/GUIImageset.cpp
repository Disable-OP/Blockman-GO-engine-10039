#include "GUIImageset.h"
#include "GUIRenderManager.h"
#include "Render/TextureManager.h"
#include "Render/Material.h"
#include "UI/GUIImagesetManager.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "rapidjson/document.h"

using namespace rapidxml;

namespace LORD
{


	GUIImageset::GUIImageset(const String& strName, ResourceType rt)
		: Resource(strName, rt)
		, m_strName(strName.c_str())
		, m_pTexture(NULL)
		, m_strTextureFileName("")
		, m_fNativeHorzRes(1.0f)
		, m_fNativeVertRes(1.0f)
		, m_fInverseNativeHorzRes(1.0f)
		, m_fInverseNativeVertRes(1.0f)
		// manual is font buffer
		, m_bIsFontBuffer(rt == RESOURCE_TYPE_MANUAL)
		, m_preparing(false)
	{
	}

	GUIImageset::~GUIImageset( void )
	{
		if (m_pTexture)
		{
			TextureManager::Instance()->releaseResource(m_pTexture, false);
			m_pTexture = NULL;
		}

		UndefineAllImage();
	}

	GUIImage* GUIImageset::DefineImage( const GUIString& strName, const ERect& rtArea, const Vector2& offset)
	{
		if (IsImageDefined(strName))
		{
			GUIString strLogInfo("GUIImageset::DefineImage - An image with the name '" + strName + "' already exists in GUIImageset '" + m_strName + "'.");
			LordLogFatal("%s", strLogInfo.c_str());
			LordAssert(0);
		}

		ERect uvRect;
		uvRect.left = rtArea.left * m_fInverseNativeHorzRes;
		uvRect.right = rtArea.right * m_fInverseNativeHorzRes;
		uvRect.top = rtArea.top * m_fInverseNativeVertRes;
		uvRect.bottom = rtArea.bottom * m_fInverseNativeVertRes;

		GUIImage* pImage = LordNew GUIImage(this, strName, uvRect, rtArea, offset);
		m_Images[strName] = pImage;

		return pImage;
	}

	void GUIImageset::UndefineImage( const GUIString& strName )
	{
		ImageRegistry::iterator itor = m_Images.find(strName);
		if (itor != m_Images.end())
		{
			LordSafeDelete(itor->second);
			m_Images.erase(itor);
		}
	}

	void GUIImageset::UndefineAllImage()
	{
		ImageRegistry::iterator itor = m_Images.begin();
		while (itor != m_Images.end())
		{
			LordSafeDelete(itor->second);
			++itor;
		}

		m_Images.clear();
	}

	void GUIImageset::ReplaceTexture(Texture* tex)
	{
		if (m_pTexture)
		{
			TextureManager::Instance()->releaseResource(m_pTexture, false);
			m_pTexture = NULL;
		}
		m_pTexture = tex;
	}

	GUIImage* GUIImageset::GetImage( const GUIString& strName ) const
	{
		ImageRegistry::const_iterator itor = m_Images.find(strName);
		if (itor != m_Images.end())
		{
			return itor->second;
		}

		return NULL;
	}

// 	void GUIImageset::DrawImage( const ERect& rtDest, const ERect& rtTexAddress, const ColorRect& color, 
// 								float fRotate, float fRotateY, float fRotateX )
// 	{
// 		if (!m_pTexture)
// 		{
// 			// lord texture
// 			m_pTexture = TextureManager::Instance()->createTexture(m_strTextureFileName.c_str());
// 			m_pTexture->load();
// 
// 			GUIRenderManager::Instance()->GetMaterial()->activeTexture(0, m_pTexture);
// 		}
// 		GUIRenderManager::Instance()->AddQuadToBuffer(rtDest, rtTexAddress, color, m_pTexture, m_bIsFontBuffer, fRotate, fRotateY, fRotateX);
// 	}
	
	void GUIImageset::DrawImage( const Vector3& lt, const Vector3& rb, const Vector3& lb, 
		const Vector3& rt, const ERect& rtTexAddress, const ColorRect& color, GUIMaterialType material, Texture* maskTexture)
	{
// 		if (!m_pTexture)
// 		{
// 			// lord texture
// 			m_pTexture = TextureManager::Instance()->createTexture(m_strTextureFileName.c_str());
// 			m_pTexture->load();
// 
// 			GUIRenderManager::Instance()->GetMaterial()->activeTexture(0, m_pTexture);
// 		}
		if (!m_pTexture)
		{
			return;
		}

		GUIRenderManager::Instance()->AddQuadToBuffer(lt, rb, lb, rt, rtTexAddress, color, m_pTexture, maskTexture, material);
	}
	
	size_t GUIImageset::calculateSize() const
	{
		return 0;
	}

	bool GUIImageset::loadOutSide(Texture* texture, const ImageSetUnitArr& defines)
	{
		LordAssert(!m_pTexture);
		m_pTexture = texture;
		// this is point copy, so need add refrence count
		m_pTexture->addRefrenceCount();

		m_strName = mName.c_str();
		m_fNativeHorzRes = float(m_pTexture->getWidth());
		m_fNativeVertRes = float(m_pTexture->getHeight());
		m_fInverseNativeHorzRes = 1.0f / m_fNativeHorzRes;
		m_fInverseNativeVertRes = 1.0f / m_fNativeVertRes;
		m_strTextureFileName = texture->getName().c_str();

		for (auto it : defines)
		{
			const ImageSetUnit& unit = it;
			GUIString strImageName = unit.name.c_str();

			float fLeft = float(unit.pos.x);
			float fTop = float(unit.pos.y);
			float fWidth = float(unit.size.x);
			float fHeight = float(unit.size.y);

			Vector2 vOffset(Vector2::ZERO);

			DefineImage(strImageName, ERect(fLeft, fTop, fLeft + fWidth, fTop + fHeight), vOffset);
		}

		return true;
	}

	bool GUIImageset::prepareImpl()
	{
		if (mResourceType == RESOURCE_TYPE_NORMAL)
		{
			//add by maxicheng
			DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(mName);
			if (pDataStream == NULL)
			{
				LordLogError("%s not exist", mName.c_str());
				return false;
			}

			size_t size = pDataStream->size();
			char* pJSONData = (char*)LordMalloc(size + 1);
			pDataStream->read(pJSONData, size);
			pJSONData[size] = 0;

			rapidjson::Document* doc = new rapidjson::Document;
			doc->Parse(pJSONData);

			if (doc->HasParseError())
			{
				LordLogError("%s parse error", mName.c_str());
			}
			else
			{

				LordAssert(doc->HasMember("meta"));
				const rapidjson::Value& meta = (*doc)["meta"];
				m_strName = mName.c_str();//
				m_strTextureFileName = ResourceGroupManager::Instance()->FindFileArchive(mName)->getFileName(meta["image"].GetString()).c_str();
				m_fNativeHorzRes = meta["size"]["w"].GetFloat();
				m_fNativeVertRes = meta["size"]["h"].GetFloat();
				m_fInverseNativeHorzRes = 1.0f / m_fNativeHorzRes;
				m_fInverseNativeVertRes = 1.0f / m_fNativeVertRes;


				DefineImage("FullImage", ERect(0.0f, 0.0f, m_fNativeHorzRes, m_fNativeVertRes));

				LordAssert(doc->HasMember("frames"));
				const rapidjson::Value& frames = (*doc)["frames"];
				rapidjson::Value::ConstMemberIterator it = frames.MemberBegin();
				for (; it != frames.MemberEnd(); ++it)
				{
					const rapidjson::Value& frame = it->value["frame"];
					GUIString strImageName = it->name.GetString();

					float fLeft = frame["x"].GetFloat();
					float fTop = frame["y"].GetFloat();
					float fWidth = frame["w"].GetFloat();
					float fHeight = frame["h"].GetFloat();


					Vector2 vOffset(Vector2::ZERO);
					const rapidjson::Value& offset = it->value["offset"];
					vOffset.y = offset["y"].GetFloat();

					DefineImage(strImageName, ERect(fLeft, fTop, fLeft + fWidth, fTop + fHeight), vOffset);
				}


			}

			delete doc;
			LordFree(pJSONData);
			LordDelete(pDataStream);
		}

		//暂时挪到主线程去
// 		// prepare texture
// 		m_pTexture = TextureManager::Instance()->createTexture(m_strTextureFileName.c_str());
// 		m_pTexture->prepare();

		return true;
	}

	void GUIImageset::unprepareImpl()
	{

	}

	bool GUIImageset::loadImpl()
	{
		if (mResourceType == RESOURCE_TYPE_NORMAL)
		{
			// 暂时挪到主线程来处理
			LordAssert(!m_pTexture);
			if (m_pTexture != NULL)
			{
				LordException("GUIImageset::loadImpl() error texture not null!");
			}

			m_pTexture = TextureManager::Instance()->createTexture(m_strTextureFileName.c_str());
			if (m_pTexture)
			{
				m_pTexture->prepare();

				// load texture
				m_pTexture->load();
				SamplerState* pState = GUIRenderManager::Instance()->GetMaterial()->getSamplerStateByTexStage(0);
				m_pTexture->setSamplerState(pState);
			}

			GUIImagesetManager::Instance()->registerGUIImageset(this);
		}
		return true;
	}

	void GUIImageset::unloadImpl()
	{
		if (m_pTexture)
		{
			TextureManager::Instance()->releaseResource(m_pTexture, false);
			m_pTexture = NULL;
		}
	}

// 	bool ImagesetPrepareEvent::ProcessEvent()
// 	{
// 		if (m_imageset)
// 		{
// 			return m_imageset->prepare();
// 		}
// 
// 		return true;
// 	}
// 
// 	bool ImagesetPrepareEvent::RespondEvent()
// 	{
// 		if (m_imageset)
// 		{
// 			return m_imageset->load();
// 		}
// 
// 		return true;
// 	}
// 
// 	ImagesetPrepareEvent::ImagesetPrepareEvent(GUIImageset* imageset)
// 		: m_imageset(imageset)
// 	{
// 
// 	}
// 
// 	ImagesetPrepareEvent::~ImagesetPrepareEvent()
// 	{
// 
// 	}

}
