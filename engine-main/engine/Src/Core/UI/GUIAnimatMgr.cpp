#include "GUIAnimatMgr.h"
#include "GUIAnimate.h"
#include "GUIImagesetManager.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "rapidjson/document.h"

using namespace rapidxml;
namespace LORD
{
	GUIAnimatMgr* GUIAnimatMgr::Instance()
	{
		return ms_pSingleton;
	}

	GUIAnimatMgr::GUIAnimatMgr()
	{

	}

	GUIAnimatMgr::~GUIAnimatMgr()
	{
		GUIAnimatMap::iterator it;
		for(it=m_mapAnimations.begin();it!=m_mapAnimations.end();++it)
			LordSafeDelete(it->second);
		m_mapAnimations.clear();		
	}

	GUIAnimate* GUIAnimatMgr::GetAnimatById(i32 id)
	{
		GUIAnimatMap::const_iterator itor=m_mapAnimations.find(id);
		if (itor!=m_mapAnimations.end())					
			return itor->second;					
		else
			return NULL;
	}

	void GUIAnimatMgr::lordConfig(String cfgFileName)
	{
		if (m_configFileName != cfgFileName)
		{
			m_configFileName = cfgFileName;

			DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(cfgFileName);
			if(pDataStream == NULL)
			{
				LordLogError("%s not exist!", cfgFileName.c_str());
				return;
			}

			size_t nDataSize = pDataStream->size();
			char* pJSONData = (char*)LordMalloc(nDataSize + 1);
			pDataStream->read(pJSONData, nDataSize); 
			pJSONData[nDataSize] = 0;

			rapidjson::Document* doc = new rapidjson::Document;
			doc->Parse(pJSONData);
			if(doc->HasParseError())
			{
				LordLogError("%s parse error!", cfgFileName.c_str());
			}
			else
			{
				
				const rapidjson::Value& animateConfig = doc->FindMember("AnimateConfig")->value;
				rapidjson::Value::ConstValueIterator itAnimate = animateConfig.Begin();

				for (; itAnimate != animateConfig.End(); ++itAnimate)
				{
					GUIAnimate* pGUIAnimate = LordNew(GUIAnimate);
					LordAssert(pGUIAnimate);

					pGUIAnimate->m_id = (*itAnimate)["Id"].GetInt();
					
					pGUIAnimate->m_name = (*itAnimate)["Name"].GetString();

					pGUIAnimate->m_playTime = (*itAnimate)["PlayTime"].GetUint();

					pGUIAnimate->m_loop = (*itAnimate)["Loop"].GetBool();

					if (itAnimate->HasMember("Image"))
					{
						const rapidjson::Value& images = itAnimate->FindMember("Image")->value;
						rapidjson::Value::ConstValueIterator itImage = images.Begin();
						for(; itImage != images.End(); ++itImage)
						{
							GUIAnimate::GUIImageInfo imageInfo;
							imageInfo.strWholeImageName = (*itImage)["Name"].GetString();
							float alpha = (*itImage)["Alpha"].GetFloat();
							imageInfo.color = Color(1, 1, 1, alpha).getARGB();
							pGUIAnimate->m_vecImages.push_back(imageInfo);
						}

					}

					m_mapAnimations.insert(GUIAnimatMap::value_type(pGUIAnimate->m_id, pGUIAnimate));
				}



			}
			delete doc;
			LordFree(pJSONData);
			LordDelete(pDataStream);
		}
#if 0
		if (m_configFileName!=cfgFileName)
		{
			m_configFileName=cfgFileName;	

			DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(cfgFileName);
			size_t nDataSize = pDataStream->size();
			char* pXMLData = (char*)LordMalloc(nDataSize+1);
			pDataStream->read(pXMLData, nDataSize);
			pXMLData[nDataSize] = 0;
			xml_document<>* doc = LordNewT(xml_document<>);
			doc->parse<0>(pXMLData);

			// AnimateMgr node
			xml_node<>* pGUIAnimateMgrNode = doc->first_node();
			LordAssert(pGUIAnimateMgrNode);
			//Animate node
			xml_node<>* pGUIAnimateNode = pGUIAnimateMgrNode->first_node();
			
			while(pGUIAnimateNode != NULL)
			{
				GUIAnimate * pGUIAnimate = LordNew GUIAnimate;
				LordAssert(pGUIAnimate);

				xml_attribute<>* pAnimateId = pGUIAnimateNode->first_attribute();
				GUIString strAnimateId = pAnimateId->value();
				pGUIAnimate->m_id=StringUtil::ParseI32(strAnimateId.c_str());

				xml_attribute<>* pName = pAnimateId->next_attribute();
				pGUIAnimate->m_name = pName->value();
				
				xml_attribute<>* pPlayTime = pName->next_attribute();
				GUIString strpPlayTime = pPlayTime->value();
				pGUIAnimate->m_playTime = StringUtil::ParseUI32(strpPlayTime.c_str());

				xml_attribute<>* pLoop = pPlayTime->next_attribute();
				GUIString strLoop = pLoop->value();
				pGUIAnimate->m_loop = StringUtil::ParseBool(strLoop.c_str());		
				//Image node
				xml_node<>* pImage=pGUIAnimateNode->first_node();				
				while(pImage != NULL)
				{
					//pImageInfo=(GUIAnimate::GUIImageInfo*)LordMalloc(sizeof(GUIAnimate::GUIImageInfo));
					GUIAnimate::GUIImageInfo imageInfo;					
					xml_attribute<>* pImageName = pImage->first_attribute();
					imageInfo.strWholeImageName = pImageName->value();

					xml_attribute<>* pAlpha = pImageName->next_attribute();
					float alpha=StringUtil::ParseFloat(pAlpha->value());
					imageInfo.color=Color(1,1,1,alpha).getARGB();
					pGUIAnimate->m_vecImages.push_back(imageInfo);

					pImage = pImage->next_sibling();
				}				
				m_mapAnimations.insert(GUIAnimatMap::value_type(pGUIAnimate->m_id,pGUIAnimate));

				pGUIAnimateNode = pGUIAnimateNode->next_sibling();
			}

			LordDeleteT(doc, xml_document<>);
			LordFree(pXMLData);
			LordSafeDelete(pDataStream);
		}
#endif

	}
}