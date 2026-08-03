#include "GUIWindowManager.h"
#include "GUIWindow.h"
#include "GUIWindowFactory.h"
#include "GUIFont.h"
#include "GUISystem.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "Object/Root.h"
#include "GUIFontManager.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidxml;

namespace LORD
{
	//---------------------------------------------------------------------
	GUIWindowManager* GUIWindowManager::Instance()
	{
		return ms_pSingleton;
	}

	GUIWindowManager::GUIWindowManager()
	{
		if (Root::Instance()->isEditorMode())
		{
			// init default value
			m_defalutValue["Material"] = "CullBack";
			m_defalutValue["MaskTexture"] = "";
			m_defalutValue["DrawColor"] = "1 1 1 1";
			m_defalutValue["Enabled"] = "true";
			m_defalutValue["Area"] = "{{0,0},{0,0},{0,0},{0,0}}";
			m_defalutValue["VerticalAlignment"] = "Top";
			m_defalutValue["HorizontalAlignment"] = "Left";
			m_defalutValue["Text"] = "";
			m_defalutValue["MaxWidth"] = "0";
			m_defalutValue["MinWidth"] = "0";
			m_defalutValue["MaxHeight"] = "0";
			m_defalutValue["MinHeight"] = "0";

			GUIFont* pfont = GUIFontManager::Instance()->GetDefaultFont();
			if (pfont)
			{
				m_defalutValue["Font"] = pfont->GetName();
			}
			else
			{
				m_defalutValue["Font"] = "FZY-14";
			}

			m_defalutValue["Touchable"] = "true";
			m_defalutValue["Visible"] = "true";
			m_defalutValue["AutoSize"] = "false";
			m_defalutValue["Level"] = "50";
			m_defalutValue["AlwaysOnTop"] = "false";
			m_defalutValue["MiddleLayoutChild"] = "false";
			m_defalutValue["Alpha"] = "1";


			m_defalutValue["ClipChild"] = "false";
			m_defalutValue["Rotate"] = "0";
			m_defalutValue["RotateY"] = "0";
			m_defalutValue["RotateX"] = "0";
			m_defalutValue["MultiClick"] = "false";

			m_defalutValue["StretchType"] = "None";
			m_defalutValue["StretchOffset"] = "0 0 0 0";
			m_defalutValue["WndOpenSound"] = "";
			m_defalutValue["WndCloseSound"] = "";
			m_defalutValue["EnableTextColorRect"] = "false";

			m_defalutValue["TextColorLeftTop"] = "1 1 1 1";
			m_defalutValue["TextColorRightTop"] = "1 1 1 1";
			m_defalutValue["TextColorLeftBottom"] = "1 1 1 1";
			m_defalutValue["TextColorRightBottom"] = "1 1 1 1";

			m_defalutValue["EffectFileName"] = "";
			m_defalutValue["EffectRotate"] = "0";

			m_defalutValue["OutputLuaVariable"] = "false";
			m_defalutValue["OutputLuaEvent"] = "";
			m_defalutValue["EnableLongTouch"] = "false";
			m_defalutValue["BackgroundColor"] = "0 0 0 0";

			// static image
			m_defalutValue["ImageName"] = "";


			// static text
			m_defalutValue["TextColor"] = "1 1 1 1";
			m_defalutValue["TextHorzAlignment"] = "Left";
			m_defalutValue["TextVertAlignment"] = "Top";
			m_defalutValue["TextWordWrap"] = "false";
			m_defalutValue["TextSelfAdaptHigh"] = "false";
			m_defalutValue["TextShadow"] = "false";
			m_defalutValue["TextShadowColor"] = "0 0 0 1";
			m_defalutValue["TextBorder"] = "false";
			m_defalutValue["TextBorderColor"] = "0 0 0 1";
			m_defalutValue["AllShowOneLine"] = "false";
			m_defalutValue["TextScale"] = "1";

			// button image
			m_defalutValue["BtnTextColor"] = "1 1 1 1";
			m_defalutValue["NormalImage"] = "";
			m_defalutValue["PushedImage"] = "";
			m_defalutValue["SoundName"] = "1";
			m_defalutValue["TextRenderOffset"] = "0 0";

				// back image
			m_defalutValue["EditBackImage"] = "";
			m_defalutValue["Password"] = "false";
			m_defalutValue["TextOffset"] = "0";
			m_defalutValue["MaxTextLength"] = "10";

			// radio
			m_defalutValue["RadioGroup"] = "0";

			// progress
			m_defalutValue["Progress"] = "1";
			m_defalutValue["ProgressBackImage"] = "";
			m_defalutValue["ProgressImage"] = "";
			m_defalutValue["ProgressHeaderImage"] = "";

				// list
			m_defalutValue["ListItemNormalImage"] = "";
			m_defalutValue["ListItemSelectImage"] = "";
			m_defalutValue["ListItemHeight"] = "30";
			m_defalutValue["ListBlinkSelect"] = "false";
			m_defalutValue["AllowScroll"] = "true";
			m_defalutValue["BetweenDistance"] = "0";

			//layout
			m_defalutValue["LayoutBackImage"] = "";

				//BubbleContain
			m_defalutValue["BubbleItemBackImage"] = "";

			//InputBox
			m_defalutValue["InputBoxBackImage"] = "";


			//PageView
			m_defalutValue["PageTurning"] = "true";
			m_defalutValue["PageInnerSlid"] = "false";
			m_defalutValue["LoopTurning"] = "true";

			// scroll pane
			m_defalutValue["HoriScrollEnable"] = "true";
			m_defalutValue["VertScrollEnable"] = "true";


			// animate
			m_defalutValue["AnimateID"] = "-1";
			m_defalutValue["Play"] = "true";


			// actor
			m_defalutValue["ActorName"] = "";
			m_defalutValue["SkillName"] = "";
			m_defalutValue["ActorRotationX"] = "0";
			m_defalutValue["ActorRotationY"] = "0";
			m_defalutValue["ActorRotationZ"] = "0";
			m_defalutValue["ActorScale"] = "1";
		}
	}

	GUIWindowManager::~GUIWindowManager()
	{
		DestroyAllGUIWindows();
		CleanDeadPool();
	}

	GUIWindow* GUIWindowManager::CreateGUIWindow( GUIWindowType nType, const GUIString& strName )
	{
		if (strcmp(strName.c_str(), "SwitchablePropsPanel") == 0) {
			printf("");
		}
		if (IsGUIWindowPresent(strName))
		{
			LordLogFatal("Window with id : %s is already exist!", strName.c_str());
		}

		GUIWindow* pWindow = nullptr;
		try
		{
			pWindow = GUIWindowFactory::Instance()->create(nType, strName);
		}
		catch (...)
		{
			const auto& e = std::current_exception();
			LordAssert(0);
			// TODO: log warn
			pWindow = LordNew GUIWindow(GWT_DEFAULT_WINDOW, strName);
		}

		m_WindowRegistry[strName] = pWindow;

		return pWindow;
	}

	GUIWindow* GUIWindowManager::CreateGUIWindow( const GUIString& strType, const GUIString& strName )
	{
		GUIWindowType nType = GWT_DEFAULT_WINDOW;
		for (const auto& typeCreatorPair : GUIWindowFactory::Instance()->registry())
		{
			if (strType == typeCreatorPair.first.value)
			{
				nType = typeCreatorPair.first;
			}
		}
		return CreateGUIWindow(nType, strName);
	}

	void GUIWindowManager::DestroyGUIWindow( GUIWindow* window )
	{
		LordAssert(window);
		DestroyGUIWindow(window->GetName());
	}

	void GUIWindowManager::DestroyGUIWindow( const GUIString& strName )
	{
		WindowRegistry::iterator wndpos = m_WindowRegistry.find(strName);

		if (wndpos != m_WindowRegistry.end())
		{
			GUIWindow* wnd = wndpos->second;

			// remove entry from the WindowRegistry.
			m_WindowRegistry.erase(wndpos);

			wnd->Destroy();

			GUISystem::Instance()->NotifyWindowDestroy(wnd);

			// add window to dead pool
			m_Deathrow.push_back(wnd);
		}
	}

	GUIWindow* GUIWindowManager::GetGUIWindow( const GUIString& strName ) const
	{
		WindowRegistry::const_iterator wndpos = m_WindowRegistry.find(strName);

		if (wndpos != m_WindowRegistry.end())
		{
			return wndpos->second;
		}
		else
		{
			LordLogInfo("window does not exist with name:  %s", strName.c_str());
			return NULL;
		}
	}

	bool GUIWindowManager::IsGUIWindowPresent(const GUIString& strName ) const
	{
		return (m_WindowRegistry.find(strName) != m_WindowRegistry.end());
	}

	void GUIWindowManager::DestroyAllGUIWindows( void )
	{
		GUIString strName;
		while (!m_WindowRegistry.empty())
		{
			strName = m_WindowRegistry.begin()->first;
			DestroyGUIWindow(strName);
		}
	}

	void GUIWindowManager::CleanDeadPool( void )
	{
		WindowVector::reverse_iterator curr = m_Deathrow.rbegin();
		for (; curr != m_Deathrow.rend(); ++curr)
		{
			LordSafeDelete(*curr);
		}

		// all done here, so clear all pointers from dead pool
		m_Deathrow.clear();
	}

	void GUIWindowManager::WriteXML( GUIWindow* pWindow, const GUIString& strFileName )
	{
		LordAssert(pWindow);
		xml_document<>* doc = LordNewT(xml_document<>);

		xml_node<>* rot = doc->allocate_node(rapidxml::node_pi, doc->allocate_string("xml version='1.0' encoding='utf-8'"));
		doc->append_node(rot);
		xml_node<>* root = doc->allocate_node(node_element, "Dialog", NULL);
		doc->append_node(root);

		pWindow->ExportXMLData(*doc, root);

		std::ofstream out(strFileName.c_str());
		out << *doc;

		LordDeleteT(doc, xml_document<>);
	}

	GUIWindow* GUIWindowManager::LoadWindowFromXML( const GUIString& strFileName )
	{
		DataStream* pXMLStream = ResourceGroupManager::Instance()->openResource(strFileName.c_str());
		if(pXMLStream == NULL)
		{
			LordLogError("dialog file not found [%s].", strFileName.c_str());
			return NULL;
		}
		size_t fileSize = pXMLStream->size();
		char* xmlData = (char*)LordMalloc(fileSize + 1);
		pXMLStream->read(xmlData, fileSize);
		xmlData[fileSize] = 0;

		xml_document<>* doc = LordNewT(xml_document<>);        // character type defaults to cha
		doc->parse<0>(xmlData);

		// root window
		xml_node<>* pDialogNode = doc->first_node();
		LordAssert(pDialogNode);
		xml_node<>* pWindowNode = pDialogNode->first_node();
		LordAssert(pWindowNode);

		xml_attribute<>* pAttri = pWindowNode->first_attribute();
		GUIString strWindowType = pAttri->value();
		pAttri = pAttri->next_attribute();
		GUIString strWindowName = pAttri->value();

		GUIWindow* pRootWindow = CreateGUIWindow(strWindowType, strWindowName);
		LordAssert(pRootWindow);

		pRootWindow->SetThisWindowDialog();

		pRootWindow->ImportXMLData(pWindowNode);

		//pRootWindow->playAction();

		LordDeleteT(doc, xml_document<>);
		LordFree(xmlData);
		LordDelete pXMLStream;

		return pRootWindow;
	}

	void GUIWindowManager::WriteJSON(GUIWindow* pWindow, const GUIString& strFileName)
	{
		LordAssert(pWindow);
		rapidjson::Document* doc = new rapidjson::Document;
		doc->SetObject();

		rapidjson::Value dialogObj(rapidjson::kObjectType);
		doc->AddMember("Dialog", dialogObj, doc->GetAllocator());
		
		rapidjson::Value windowObj(rapidjson::kObjectType);
		doc->FindMember("Dialog")->value.AddMember("Window", windowObj, doc->GetAllocator());

		pWindow->ExportJSONData(doc, doc->FindMember("Dialog")->value["Window"]);


		//输出到文件
		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		doc->Accept(writer);


		rapidjson::Value& dialog = (*doc)["Dialog"];
		rapidjson::Value& window = dialog["Window"];
		rapidjson::Value& property = window["Property"];
		

		std::wofstream out(strFileName.c_str());
		String s = sb.GetString();
		out << sb.GetString();
		out.close();
		
		delete doc;
	}

	GUIWindow* GUIWindowManager::LoadWindowFromJSON(const GUIString& strFileName)
	{
		DataStream* pJSONStream = ResourceGroupManager::Instance()->openResource(strFileName.c_str());
		if (pJSONStream == NULL)
		{
			LordLogError("dialog file not found [%s].", strFileName.c_str());
			return NULL;
		}

		size_t fileSize = pJSONStream->size();
		char* jsonData = (char*)LordMalloc(fileSize + 1);
		pJSONStream->read(jsonData, fileSize);
		jsonData[fileSize] = 0;

		GUIWindow* pWindow = NULL;
		rapidjson::Document* doc = new rapidjson::Document; 
		doc->Parse(jsonData);

		//LordLogError("json error");

		if (doc->HasParseError())
		{
			LordLogError("[%s] parse error", strFileName.c_str());			
		}
		else
		{
			rapidjson::Value::ConstMemberIterator dialog = doc->MemberBegin();
			rapidjson::Value::ConstMemberIterator window = dialog->value.MemberBegin();


			LordAssert(window->value["Type"].IsString());
			LordAssert(window->value["Name"].IsString());

			GUIString strWindowType = window->value["Type"].GetString();
			GUIString strWindowName = window->value["Name"].GetString();

			GUIWindow* pRootWindow = CreateGUIWindow(strWindowType, strWindowName);
			LordAssert(pRootWindow);

			pRootWindow->SetThisWindowDialog();
			pRootWindow->ImportJSONData(window->value);			
			pWindow = pRootWindow;
		}

		delete doc;
		LordFree(jsonData);
		LordDelete(pJSONStream);

		return pWindow;
	}

	void GUIWindowManager::RenameWindow( GUIWindow* pWindow, const GUIString& strNewName )
	{
		if (pWindow)
		{
			WindowRegistry::iterator pos = m_WindowRegistry.find(pWindow->GetName());
			if (pos != m_WindowRegistry.end())
			{
				m_WindowRegistry.erase(pos);

				pWindow->Rename(strNewName);

				m_WindowRegistry[strNewName] = pWindow;
			}
		}
	}

	GUIWindow* GUIWindowManager::CreateWindowFromTemplate( const GUIString& strNamePreffix, const GUIString& strTemplateFileName )
	{
		DataStream* pXMLStream = ResourceGroupManager::Instance()->openResource(strTemplateFileName.c_str());
		if(pXMLStream == NULL)
		{
			LordLogError("template file not found [%s].", strTemplateFileName.c_str());
			return NULL;
		}
		size_t fileSize = pXMLStream->size();
		char* xmlData = (char*)LordMalloc(fileSize + 1);
		pXMLStream->read(xmlData, fileSize);
		xmlData[fileSize] = 0;

		xml_document<>* doc = LordNewT(xml_document<>);        // character type defaults to cha
		doc->parse<0>(xmlData);

		// root window
		xml_node<>* pDialogNode = doc->first_node();
		LordAssert(pDialogNode);
		xml_node<>* pWindowNode = pDialogNode->first_node();
		LordAssert(pWindowNode);

		xml_attribute<>* pAttri = pWindowNode->first_attribute();
		GUIString strWindowType = pAttri->value();
		pAttri = pAttri->next_attribute();
		GUIString strWindowName = pAttri->value();

		GUIWindow* pRootWindow = CreateGUIWindow(strWindowType, strNamePreffix+"_"+strWindowName);
		LordAssert(pRootWindow);

		pRootWindow->ImportTemplateData(strNamePreffix, pWindowNode);

		LordDeleteT(doc, xml_document<>);
		LordFree(xmlData);
		LordDelete pXMLStream;

		return pRootWindow;
	}

	GUIWindow* GUIWindowManager::CloneWindow(const GUIString& strNamePreffix, GUIWindow* pWindow)
	{
		GUIWindow* pRootWindow = CreateGUIWindow(pWindow->GetType(), strNamePreffix + pWindow->GetName());
		LordAssert(pRootWindow);

		pRootWindow->Clone(strNamePreffix, pWindow);

		return pRootWindow;
	}

}
