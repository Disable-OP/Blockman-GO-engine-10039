#include "gui_buyGoodsTip.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderBlocks.h"
#include "cItem/cItem.h"
#include "cBlock/cBlockManager.h"
#include "cBlock/cBlock.h"
#include "GUI/GuiItemStack.h"
#include "Setting/UIDisplaySetting.h"
#include "game.h"
#include "Util/UICommon.h"

using namespace LORD;
namespace BLOCKMAN
{

	gui_buyGoodsTip::gui_buyGoodsTip()
		: gui_layout("BuyGoodsTip.json")
		, m_totalTime(0)
		, m_isPlaying(false)
	{
	}

	gui_buyGoodsTip::~gui_buyGoodsTip()
	{
	}

	void gui_buyGoodsTip::onLoad()
	{
		m_tipBg = getWindow<GUIStaticImage>("BuyGoodsTip-Bg");
		m_countText = getWindow<GUIStaticText>("BuyGoodsTip-Text"); 
		m_iconImage = getWindow<GUIStaticImage>("BuyGoodsTip-Icon");
		m_originPos = m_rootWindow->GetPosition();
		bool isShowBg = UIDisplaySetting::getUIDisplay()->isShowGetGoodsBg;
		m_originPos.d_y.d_offset = m_originPos.d_y.d_offset - (isShowBg ? 0.0f : 100.0f);
		m_totalTime = 0;
		auto rect = m_rootWindow->GetUnclippedOuterRect();
		m_moveDistance = rect.top;

		m_subscriptionGuard.add(GetMoneyEvent::subscribe(std::bind(&gui_buyGoodsTip::showContent, this, std::placeholders::_1)));
		m_subscriptionGuard.add(GetItemEvent::subscribe(std::bind(&gui_buyGoodsTip::showItemContent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(GetGoodsEvent::subscribe(std::bind(&gui_buyGoodsTip::showGoodsContent, this, std::placeholders::_1, std::placeholders::_2)));
		
	}

	void gui_buyGoodsTip::onShow()
	{

	}

	void gui_buyGoodsTip::onUpdate(ui32 nTimeElapse)
	{
		if (!m_isPlaying || !isShown())
			return;

		m_totalTime += nTimeElapse;
		if (m_totalTime < m_stayTime)
		{
			return;
		}

		float ratio = (float)(m_totalTime - m_stayTime)/ m_animateTime;
		ratio = ratio > 1.0f ? 1.0f : ratio;
		float moveY = ratio * m_moveDistance;
		LORD::UVector2 newPos = UVector2(m_originPos.d_x, UDim(m_originPos.d_y.d_scale, m_originPos.d_y.d_offset - moveY));
		m_rootWindow->SetPosition(newPos);
		m_rootWindow->SetAlpha(1.0f - ratio);
		if (ratio >= 1.0f )
		{
			if (m_moneyTips.size() > 0)
			{
				i32 sum = 0;
				for (i32 count : m_moneyTips)
				{
					sum += count;
				}
				m_moneyTips.clear();
				refreshOriginState(sum);
			}
			else
			{
				m_isPlaying = false;
				getParent()->showBuyGoodsTip(false);
			}
		}
	}

	bool gui_buyGoodsTip::showContent(int count)
	{
		m_tipBg->SetVisible(UIDisplaySetting::getUIDisplay()->isShowGetGoodsBg);		
		m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
		m_iconImage->SetOverlayImage(UICommon::getCurrencyIcon().c_str());
		m_iconImage->SetRenderAsBlock(false);
		m_countText->SetVisible(true);
		if (isShown() && GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			m_moneyTips.push_back(count);
			if (!isShown())
			{
				m_isPlaying = true;
				getParent()->showBuyGoodsTip(true);
			}
		}
		else
		{
			refreshOriginState(count);
		}
		return true;
	}

	bool gui_buyGoodsTip::showItemContent(int itemId, int meta, int count)
	{
		m_tipBg->SetVisible(UIDisplaySetting::getUIDisplay()->isShowGetGoodsBg);
		m_tipBg->SetVisible(true);
		m_countText->SetVisible(GameClient::CGame::Instance()->GetGameType() != ClientGameType::Build_War);
		refreshOriginState(count);
		updateIcon(itemId, meta);
		return true;
	}

	bool gui_buyGoodsTip::showGoodsContent(const String & icon, int count)
	{
		m_tipBg->SetVisible(UIDisplaySetting::getUIDisplay()->isShowGetGoodsBg);
		m_iconImage->SetImage(icon.c_str());
		m_iconImage->SetOverlayImage(icon.c_str());
		m_iconImage->SetRenderAsBlock(false);
		m_countText->SetVisible(count > 0);
		refreshOriginState(count);
		return true;
	}

	void gui_buyGoodsTip::refreshOriginState(int count)
	{
		m_countText->SetTextColor(m_tipBg->IsVisible() ? Color::BLACK : Color::WHITE);
		m_countText->SetTextBoader(m_tipBg->IsVisible() ? Color::TRANS : Color::BLACK);
		String text = StringUtil::Format("+%d", count);
		m_countText->SetText(text.c_str());
		m_rootWindow->SetPosition(m_originPos);
		m_rootWindow->SetAlpha(1.0f);
		m_totalTime = 0;
		m_isPlaying = true;
		getParent()->showBuyGoodsTip(true);
	}

	void gui_buyGoodsTip::updateIcon(int itemId, int meta)
	{
		ItemStack* itemStack = LordNew ItemStack(itemId, 1, meta);
		ItemStackPtr m_itemStack = itemStack->copy();
		GuiItemStack::showItemIcon(m_iconImage, m_itemStack, itemId, meta);
		LordDelete itemStack;
	}
}