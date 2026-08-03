#include "gui_enchantmentPanel.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "cItem/cItem.h"
#include "UI/GUISystem.h"
#include "External/cjson.h"
#include "Render/TextureAtlas.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_enchantmentPanel::gui_enchantmentPanel()
		: gui_layout("EnchantmentPanel.json")
	{
	}

	gui_enchantmentPanel::~gui_enchantmentPanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_enchantmentPanel::onLoad()
	{
		m_equipment = getWindow<GUIStaticImage>("EnchantmentPanel-Equipment");
		m_equipmentImg = getWindow<GUIStaticImage>("EnchantmentPanel-EquipmentImg");
		m_consume = getWindow<GUIStaticImage>("EnchantmentPanel-CosumeId");
		m_consumeImg = getWindow<GUIStaticImage>("EnchantmentPanel-CosumeImg");
		m_closeBtn = getWindow<GUIButton>("Enchantment-Btn-Close");
		m_enchantmentBtn = getWindow<GUIButton>("Enchantment-Btn-Enchantment");
		m_bagLayout = getWindow<GUILayout>("EnchantmentPanel-BagLayout");
		m_lackItemText = getWindow<GUIStaticText>("EnchantmentLack-text");
		m_consumeBg = getWindow<GUIStaticImage>("EnchantmentPanel-ConsumeBg");


		m_equipment->subscribeEvent(EventWindowTouchUp, std::bind(&gui_enchantmentPanel::onEquipClick, this, std::placeholders::_1));
		m_closeBtn->subscribeEvent(EventButtonClick, std::bind(&gui_enchantmentPanel::onCloseClick, this, std::placeholders::_1));
		m_enchantmentBtn->subscribeEvent(EventButtonClick, std::bind(&gui_enchantmentPanel::onEnchantmentBtn, this, std::placeholders::_1));

		m_lackItemText->SetText(getString(LanguageKey::GUI_LACK_ENCHANTMENT_ITEM));
		m_lackItemText->SetVisible(false);

		for (int i = 0; i < MAX_ENAHTMENT_EFFECT_NUM; ++i)
		{
			String button_name = StringUtil::Format("Enchantment-Btn-Effect%d", i + 1);
			m_enchantmentEffectBtn[i] = getWindow<GUIButton>(button_name.c_str());
			m_enchantmentEffectBtn[i]->subscribeEvent(EventButtonClick, std::bind(&gui_enchantmentPanel::onEnchantmentEffectBtn, this, std::placeholders::_1, i));

			String select_name = StringUtil::Format("EnchantmentPanel-EffectSelect%d", i + 1);
			m_enchantmentEffectSelect[i] = getWindow<GUIStaticImage>(select_name.c_str());
			m_enchantmentEffectSelect[i]->SetVisible(false);

			String title_name = StringUtil::Format("EnchantmentPanel-EnchantmentTitle%d", i + 1);
			m_enchantmentEffectTitle[i] = getWindow<GUIStaticText>(title_name.c_str());

			String des_name = StringUtil::Format("EnchantmentPanel-EnchantmentDes%d", i + 1);
			m_enchantmentEffectDes[i] = getWindow<GUIStaticText>(des_name.c_str());

			String img_name = StringUtil::Format("EnchantmentPanel-EnchantEquip%d", i + 1);
			m_enchantmentEffectImg[i] = getWindow<GUIStaticImage>(img_name.c_str());

			String bg_name = StringUtil::Format("EnchantmentPanel-EnchantEffectBg%d", i + 1);
			m_enchantmentEffectBg[i] = getWindow<GUIStaticImage>(bg_name.c_str());
		}

		for (int i = 0; i < MAX_ENANTMENT_BAG_NUM; ++i)
		{
			String stackName = StringUtil::Format("EnchantMentBagStack%d", i + 1);
			GUIStaticImage* stack = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, stackName.c_str());
			float aboPosX = (float)(i % MAX_ENANTMENT_BAG_COLUMN * 65 + 5.0f);
			float aboPoxY = (float)(i / MAX_ENANTMENT_BAG_COLUMN * 65 + 8.0f);

			stack->SetArea(UDim(0.f, aboPosX), UDim(0.f, aboPoxY), UDim(0.f, 64.f), UDim(0.f, 64.f));
			stack->SetImage("set:new_gui_material.json image:wupinkuang_shixin");
			stack->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			stack->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "8 8 8 8");
			stack->SetTouchable(true);
			stack->SetVisible(false);
			m_bagLayout->AddChildWindow(stack);
			m_bagItemStack[i] = stack;

			String stackImgName = StringUtil::Format("EnchantMentBagStackImg%d", i + 1);
			GUIStaticImage* stackImg = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, stackImgName.c_str());
			stackImg->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(0.75f, 0.f), UDim(0.75f, 0.f));
			stackImg->SetTouchable(true);
			stackImg->SetHorizontalAlignment(HA_CENTRE);
			stackImg->SetVerticalAlignment(VA_CENTRE);
			// stackImg->SetImage("set:new_gui_material.json image:wupinkuang");
			stack->AddChildWindow(stackImg);
			m_bagItemStackImg[i] = stackImg;
			m_bagItemStackImg[i]->subscribeEvent(EventWindowTouchUp, std::bind(&gui_enchantmentPanel::onBagStackClick, this, std::placeholders::_1, i));
		}

		getWindow<GUIStaticText>("EnchantmentPanel-Title")->SetText(getString(LanguageKey::GUI_ENCHANTMENT_TABLE_TITLE));
		getWindow<GUIStaticText>("EnchantmentPanel-EnchantmentTxt")->SetText(getString(LanguageKey::GUI_ENCHANTMENT_BTN_TXT));

		m_subscriptionGuard.add(ShowEnchantMentEvent::subscribe(std::bind(&gui_enchantmentPanel::refreshAll, this, std::placeholders::_1)));
	}

	void gui_enchantmentPanel::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{

		}
	}

	void gui_enchantmentPanel::onShow()
	{
		
	}

	bool gui_enchantmentPanel::onCloseClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		return true;
	}

	bool gui_enchantmentPanel::onEquipClick(const EventArgs & events)
	{
		if (m_equipId <= 0) return true;

		m_bag.push_back(m_equipId);
		m_equipId = 0;
		refreshAllSelf();

		return true;
	}

	bool gui_enchantmentPanel::onEnchantmentBtn(const EventArgs & events)
	{
		if (m_equipId == 0) return true;

		if (m_consumeId == 0)
		{
			if (m_need_diamond > 0)
			{
				ShowEnchantMentQuickEvent::emit(m_need_diamond, m_equipId, m_select_effect);
			}
			return true;
		}

		ClientNetwork::Instance()->getSender()->sendEnchantmentEffectClick(m_equipId, m_consumeId, m_select_effect);

		return true;
	}

	bool gui_enchantmentPanel::onEnchantmentEffectBtn(const EventArgs & events, int index)
	{
		if (index < 0 || index >= MAX_ENAHTMENT_EFFECT_NUM) return true;

		m_select_effect = index;

		for (int i = 0; i < MAX_ENAHTMENT_EFFECT_NUM; ++i)
		{
			m_enchantmentEffectSelect[i]->SetVisible(false);
		}

		m_enchantmentEffectSelect[m_select_effect]->SetVisible(true);

		return true;
	}

	bool gui_enchantmentPanel::onBagStackClick(const EventArgs & events, int index)
	{
		if (index < 0 || index >= (int)m_bag.size())
		{
			return true;
		}

		if (m_equipId > 0)
		{
			int temp = m_equipId;
			m_equipId = m_bag[index];
			m_bag[index] = temp;
		}
		else
		{
			if (index == (int)m_bag.size() - 1)
			{
				m_equipId = m_bag.back();
				m_bag.pop_back();
			}
			else
			{
				m_equipId = m_bag[index];
				auto iter = m_bag.begin();
				std::advance(iter, index);
				m_bag.erase(iter);
			}
		}
		refreshAllSelf();
		
		return true;
	}

	bool gui_enchantmentPanel::refreshAll(const String & data)
	{
		int equipId = 0;
		int consumeId = 0;
		int consumeNum = 0;
		bool haveItemEnchantment = false;
		int refreshType = RT_OPEN;
		std::vector<int> bag;
		std::vector<EffectDesInfo> des_info;
		int diamond = -1;
		String consumeBg = "";
		
		for (int i = 0; i < MAX_ENANTMENT_BAG_NUM; ++i)
		{
			m_bagItemStack[i]->SetVisible(false);
		}

		if (getAllInfo(data, equipId, consumeId, consumeNum, bag, des_info, haveItemEnchantment, refreshType, diamond, consumeBg))
		{
			m_equipId = equipId;
			m_consumeId = consumeId;
			m_consumeNum = consumeNum;
			m_need_diamond = diamond;

			String equipImg = getItemImg(equipId);
			String consumeImg = getItemImg(consumeId);

			m_equipmentImg->SetImage(equipImg.c_str());
			m_consumeImg->SetImage(consumeImg.c_str());

			m_bag.clear();

			if (refreshType == RT_OPEN)
				m_lackItemText->SetVisible(!haveItemEnchantment);
			else
				m_lackItemText->SetVisible(false);

			for (int i = 0; i < MAX_ENANTMENT_BAG_NUM; ++i)
			{
				m_bagItemStack[i]->SetVisible(false);
			}

			for (int i = 0; i < (int)bag.size() && i < MAX_ENANTMENT_BAG_NUM; ++i)
			{
				m_bagItemStack[i]->SetVisible(true);
				String img = getItemImg(bag[i]);
				m_bagItemStackImg[i]->SetImage(img.c_str());
				m_bag.push_back(bag[i]);
			}

			for (int i = 0; i < MAX_ENAHTMENT_EFFECT_NUM; ++i)
			{
				m_enchantmentEffectSelect[i]->SetVisible(false);
				m_enchantmentEffectImg[i]->SetVisible(false);
				m_enchantmentEffectBg[i]->SetVisible(false);
			}

			if (m_select_effect >= 0 && m_select_effect < MAX_ENAHTMENT_EFFECT_NUM)
			{
				m_enchantmentEffectSelect[m_select_effect]->SetVisible(true);
			}

			for (int i = 0; i < MAX_ENAHTMENT_EFFECT_NUM && i < (int)des_info.size(); ++i)
			{
				m_enchantmentEffectTitle[i]->SetText(getString(des_info[i].title.c_str()).c_str());
				m_enchantmentEffectDes[i]->SetText(getString(des_info[i].des.c_str()).c_str());
			}

			m_consumeBg->SetImage(consumeBg.c_str());

			m_effect_num = (int)des_info.size();
		}
		return true;
	}

	bool gui_enchantmentPanel::getAllInfo(const String & data, int & equptId, int & consumeId, int & consumeNum, std::vector<int>& bag, std::vector<EffectDesInfo>& effect_info,
		bool & haveItemEnchantment, int & refresh_type, int & diamond, String & consumeBg)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_enchantmentPanel::getAllInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("equipmentId") 
			|| !doc->HasMember("cosumeId")
			|| !doc->HasMember("cosumeNum")
			|| !doc->HasMember("bag")
			|| !doc->HasMember("haveItemEnchantment")
			|| !doc->HasMember("refreshType")
			|| !doc->HasMember("diamond")
			|| !doc->HasMember("consumeBg"))
		{
			LordLogError("gui_enchantmentPanel::getAllInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		int eId = doc->FindMember("equipmentId")->value.GetInt();
		int cId = doc->FindMember("cosumeId")->value.GetInt();
		int cNum = doc->FindMember("cosumeNum")->value.GetInt();
		bool hItem = doc->FindMember("haveItemEnchantment")->value.GetBool();
		int refreshType = doc->FindMember("refreshType")->value.GetInt();
		int diamond_num = doc->FindMember("diamond")->value.GetInt();
		String cBg = doc->FindMember("consumeBg")->value.GetString();

		equptId = eId;
		consumeId = cId;
		consumeNum = cNum;
		haveItemEnchantment = hItem;
		refresh_type = refreshType;
		diamond = diamond_num;
		consumeBg = cBg;

		rapidjson::Value arr = doc->FindMember("bag")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("itemId"))
			{
				LordLogError("gui_enchantmentPanel::getAllInfo The game result content bag missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int itemId = arr[i].FindMember("itemId")->value.GetInt();
			bag.push_back(itemId);
		}

		rapidjson::Value arr_effect = doc->FindMember("effect_info")->value.GetArray();
		for (size_t i = 0; i < arr_effect.Size(); ++i)
		{
			if (!arr_effect[i].HasMember("title") || !arr_effect[i].HasMember("des"))
			{
				LordLogError("gui_enchantmentPanel::getAllInfo The game result content effect_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			String title = arr_effect[i].FindMember("title")->value.GetString();
			String des = arr_effect[i].FindMember("des")->value.GetString();

			EffectDesInfo des_info;
			des_info.title = title;
			des_info.des = des;
			effect_info.push_back(des_info);
		}

		LordDelete(doc);
		return true;
	}

	String gui_enchantmentPanel::getItemImg(int itemId)
	{
		cItem* pItem = ItemClient::citemsList[itemId];
		if (pItem == nullptr)
			return StringUtil::BLANK;

		StringStream ss;
		ss << "set:";

		String ItemsJsonConfig = "items.json";
		if (itemId >= Item::itemSkillFlashBook->itemID && itemId <= Item::itemSurpriseTreasureChest_03->itemID)
		{
			ItemsJsonConfig = "throwpot.json";
		}

		ss << ItemsJsonConfig;
		ss << " image:";

		AtlasSprite* atlas = pItem->getIconFromDamageForRenderPass(0, 0);
		if (atlas != NULL)
		{
			ss << atlas->getSpriteName();
		}
		return ss.str();
	}

	void gui_enchantmentPanel::refreshAllSelf()
	{
		String equipImg = getItemImg(m_equipId);
		m_equipmentImg->SetImage(equipImg.c_str());

		for (int i = 0; i < MAX_ENANTMENT_BAG_NUM; ++i)
		{
			m_bagItemStack[i]->SetVisible(false);
		}

		for (int i = 0; i < (int)m_bag.size() && i < MAX_ENANTMENT_BAG_NUM; ++i)
		{
			m_bagItemStack[i]->SetVisible(true);
			String img = getItemImg(m_bag[i]);
			m_bagItemStackImg[i]->SetImage(img.c_str());
		}

		for (int i = 0; i < MAX_ENAHTMENT_EFFECT_NUM; ++i)
		{
			m_enchantmentEffectImg[i]->SetVisible(false);
			m_enchantmentEffectBg[i]->SetVisible(false);
		}

		if (m_equipId > 0)
		{
			String equipImg = getItemImg(m_equipId);

			for (int i = 0; i < MAX_ENAHTMENT_EFFECT_NUM && i < m_effect_num; ++i)
			{
				m_enchantmentEffectImg[i]->SetVisible(true);
				m_enchantmentEffectBg[i]->SetVisible(true);
				m_enchantmentEffectImg[i]->SetImage(equipImg.c_str());
			}
		}
		
	}

}
