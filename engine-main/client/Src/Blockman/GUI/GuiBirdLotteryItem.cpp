#include "GuiBirdLotteryItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	
	void GuiBirdLotteryItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdLotteryItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdLotteryItem::GuiBirdLotteryItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdLotteryItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_pro = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_quality = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));

		AddWindowToDrawList(*m_window);
	}

	void GuiBirdLotteryItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBirdLotteryItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdLotteryItem::setBirdEgg(const BirdEgg & birdEgg)
	{
		String imageName = birdEgg.isHave ? StringUtil::Format("set:bird_icon.json image:body_%s", birdEgg.icon.c_str()) : "set:bird_atlas.json image:unknown_bird";
		m_pro->SetText(StringUtil::Format("%.1f%%", birdEgg.prob * 100).c_str());
		m_image->SetImage(imageName.c_str());
		setNameColor(birdEgg.quality);
	}

	void GuiBirdLotteryItem::setAtlasItem(const BirdEgg & birdEgg)
	{
		m_window->SetBackImage("set:bird_pack.json image:fuseFrame");
		if (birdEgg.isHave)
		{
			m_image->SetImage(birdEgg.icon.c_str());
		}
		setNameColor(birdEgg.quality);
		m_pro->SetVisible(false);
	}

	void GuiBirdLotteryItem::setNameColor(i32 quality)
	{
		switch (quality) {
		case 0:
		case 1:
			m_quality->SetText(LanguageManager::Instance()->getString("gui_bird_quality_1").c_str());
			m_quality->SetTextColor(Color(88.f / 255.f, 44.f / 255.f, 34.f / 255.f));
			break;
		case 2:
			m_quality->SetText(LanguageManager::Instance()->getString("gui_bird_quality_2").c_str());
			m_quality->SetTextColor(Color(209.f / 255.f, 252.f / 255.f, 242.f / 255.f));
			break;
		case 3:
			m_quality->SetText(LanguageManager::Instance()->getString("gui_bird_quality_3").c_str());
			m_quality->SetTextColor(Color(255.f / 255.f, 255.f / 255.f, 0.f / 255.f));
			break;
		case 4:
			m_quality->SetText(LanguageManager::Instance()->getString("gui_bird_quality_4").c_str());
			m_quality->SetTextColor(Color(0.f / 255.f, 123.f / 255.f, 255.f / 255.f));
			break;
		}

	}



}