#include "GuiRanchItem.h"
#include "UI/GUIWindowManager.h"
#include "Setting/RanchSetting.h"
#include "Util/LanguageManager.h"
#include "Util/FriendManager.h"
#include "game.h"

namespace BLOCKMAN
{
	
	void GuiRanchItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GuiRanchItem::init(const String & jsonName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON(jsonName.c_str()));
		renameComponents(m_window);
		m_tvName = nullptr;
		m_tvDesc = nullptr;
		m_helperIcon = nullptr;

		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		AddWindowToDrawList(*m_window);
	}

	vector<GUIWindow*>::type GuiRanchItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchItem::GuiRanchItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		init("RanchItem.json");
	}

	void GuiRanchItem::changeInfoUi()
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchItemInfo.json"));
		renameComponents(m_window);
		m_icon = nullptr;
		m_tvNum = nullptr;
		m_helperIcon = nullptr;

		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_tvDesc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchItem::initOrderReward()
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchOrderRewardItem.json"));
		renameComponents(m_window);
		m_tvName = nullptr;
		m_helperIcon = nullptr;

		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_helperIcon = dynamic_cast<GuiUrlImage*>(m_window->GetChildByIndex(2));
		m_helperIcon->SetVisible(false);
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchItem::resetWindow(const String & jsonName)
	{
		removeComponents();
		init(jsonName);
	}

	void GuiRanchItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiRanchItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchItem::setItemId(i32 itemId)
	{
		m_itemId = itemId;
		setIcon(RanchSetting::getRanchItemIcon(itemId).c_str() );
		setName(LanguageManager::Instance()->getString(RanchSetting::getRanchItemName(itemId)).c_str());
		setDesc(LanguageManager::Instance()->getString(RanchSetting::getRanchItemDesc(itemId).c_str()));
	}

	void GuiRanchItem::setNum(String strNum)
	{
		if (m_tvNum)
		{
			m_num = strNum;
			m_tvNum->SetText(m_num.c_str());
		}
	}

	void GuiRanchItem::setIcon(const GUIString& strIconImage)
	{
		m_iconImage = strIconImage.c_str();
		if (m_icon)
		{
			m_icon->SetImage(m_iconImage);
		}
	}

	void GuiRanchItem::setName(String name)
	{
		if (m_tvName)
		{
			m_tvName->SetText(name.c_str());
		}
	}

	void GuiRanchItem::setDesc(String desc)
	{
		if (m_tvDesc)
		{
			m_tvDesc->SetText(desc.c_str());
		}
	}

	void GuiRanchItem::setHelper(i64 userId)
	{
		m_helperIcon->SetVisible(userId > 0 && userId != GameClient::CGame::Instance()->getPlatformUserId());
		if (m_helperIcon && userId > 0 && userId != GameClient::CGame::Instance()->getPlatformUserId())
		{
			auto pal = FriendManager::findFriendById(userId);
			m_helperIcon->setUrl(pal ? pal->picUrl : "");
			m_helperIcon->setDefaultImage(pal && pal->sex == 1 ? "set:ranch_boy_icon.json image:ranch_boy_icon" : "set:ranch_girl_icon.json image:ranch_girl_icon");
		}
	}

	void GuiRanchItem::setTextScale(float scale)
	{
		if (m_tvNum)
		{
			m_tvNum->SetTextScale(scale);
		}
	}

	bool GuiRanchItem::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (strName == GWP_WINDOW_TEXT)
		{
			setNum(m_num);
		}

		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			setIcon(strValue);
			return true;
		}

		return false;
	}

	bool GuiRanchItem::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			strValue = m_iconImage.c_str();
			return true;
		}
		return false;
	}

	void GuiRanchItem::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_SI_IMAGE_NAME);
	}

	bool GuiRanchItem::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}
		if (strName == GWP_SI_IMAGE_NAME)
		{
			type = GPT_IMAGE;
			return true;
		}

		return false;
	}

}