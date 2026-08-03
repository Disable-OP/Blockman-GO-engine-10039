#include "gui_birdPackAndFuse.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiBirdPack.h"
#include "GUI/GuiBirdFuse.h"
#include "GUI/GuiBirdPackAndFuseItem.h"
#include "World/BirdSimulator.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_birdPackAndFuse::gui_birdPackAndFuse()
		: gui_layout("BirdPackAndFuse.json")
	{
	}

	gui_birdPackAndFuse::~gui_birdPackAndFuse()
	{

	}

	void gui_birdPackAndFuse::onHide()
	{
		m_birdPackView->hideView();
	}

	void gui_birdPackAndFuse::onShow()
	{
		if (m_isPackUpdate)
		{
			changeSortBirdItem();
			m_isPackUpdate = false;
		}
	}

	void gui_birdPackAndFuse::onLoad()
	{

		setText("BirdPackAndFuse-Sort-Tab-Text", LanguageKey::GUI_BIRD_PACK_SORT);
		m_radioMap[ViewId::PACK] = getWindow<GUIRadioButton>("BirdPackAndFuse-ChoosePack");
		m_radioMap[ViewId::FUSE] = getWindow<GUIRadioButton>("BirdPackAndFuse-ChooseFuse");

		m_pack = getWindow<GUILayout>("BirdPackAndFuse-Detil-Pack");
		m_fuse = getWindow<GUILayout>("BirdPackAndFuse-Detil-Fuse");

		m_radioContentMap[ViewId::PACK] = getWindow<GUILayout>("BirdPackAndFuse-Detil-Pack");
		m_radioContentMap[ViewId::FUSE] = getWindow<GUILayout>("BirdPackAndFuse-Detil-Fuse");

		setText(m_radioMap[ViewId::PACK], "gui_bird_pack");
		setText(m_radioMap[ViewId::FUSE], "gui_bird_fuse");

		m_radioSortMap[ViewId::QUALITY] = getWindow<GUIRadioButton>("BirdPackAndFuse-Choose-Quality");
		m_radioSortMap[ViewId::LEVEL] = getWindow<GUIRadioButton>("BirdPackAndFuse-Choose-Level");
		m_radioSortMap[ViewId::QUALITY]->SetSelected(true);

		setText(m_radioSortMap[ViewId::QUALITY], "gui_bird_pack_quality");
		setText(m_radioSortMap[ViewId::LEVEL], "gui_bird_pack_level");

		m_gvSort = getWindow<GUIGridView>("BirdPackAndFuse-Sort-GridView");
		m_gvSort->InitConfig(13.f, 13.f, 5);
		m_carryTab = getWindow<GUILayout>("BirdPackAndFuse-Carry-Tab");
		m_icon = getWindow<GUIStaticImage>("BirdPackAndFuse-PackOrFuseImage");
		m_sollBar = getWindow<GUISlider>("BirdPackAndFuse-ListRollBar");

		initPackItem();
		initFuseItem();

		m_sollBar->subscribeEvent(EventWindowTouchMove, std::bind(&gui_birdPackAndFuse::onViewSoll, this, std::placeholders::_1, ViewId::SOLL_BAR));
		m_sollBar->subscribeEvent(EventWindowTouchDown, std::bind(&gui_birdPackAndFuse::onViewSoll, this, std::placeholders::_1, ViewId::SOLL_BAR));
		m_gvSort->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdPackAndFuse::onViewSoll, this, std::placeholders::_1, ViewId::HIDE_VIEW));
		m_gvSort->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdPackAndFuse::onViewSoll, this, std::placeholders::_1, ViewId::GRID_VIEW));
		m_gvSort->subscribeEvent(EventWindowTouchMove, std::bind(&gui_birdPackAndFuse::onViewSoll, this, std::placeholders::_1, ViewId::GRID_VIEW));
		m_gvSort->subscribeEvent(EventWindowTouchDown, std::bind(&gui_birdPackAndFuse::onViewSoll, this, std::placeholders::_1, ViewId::GRID_VIEW));


		for (auto item : m_radioMap)
		{
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_birdPackAndFuse::onRadioChange, this, std::placeholders::_1, item.first));
		}

		for (auto item : m_radioSortMap)
		{
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_birdPackAndFuse::onRadioSortChange, this, std::placeholders::_1, item.first));
		}

		getWindow("BirdPackAndFuse")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::MAIN));
		getWindow("BirdPackAndFuse-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BirdPackAndFuse-Carry-Tab-Button")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::CARRAY));
		getWindow("BirdPackAndFuse-Capacity-Tab-Button")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::CAPACITY));

		m_subscriptionGuard.add(UpdateBirdPackEvent::subscribe(std::bind(&gui_birdPackAndFuse::onPackUpdate, this)));
		m_subscriptionGuard.add(ShowBirdInfoClickEvent::subscribe(std::bind(&gui_birdPackAndFuse::onShowBirdInfo, this, std::placeholders::_1, false)));
		m_subscriptionGuard.add(BirdFuseRemoveItemEvent::subscribe(std::bind(&gui_birdPackAndFuse::onFuseRemoveItem, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BirdFusePutItemEvent::subscribe(std::bind(&gui_birdPackAndFuse::onFusePutItem, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BirdNestExpandEvent::subscribe(std::bind(&gui_birdPackAndFuse::openExpandCarry, this)));
	}


	void gui_birdPackAndFuse::initPackItem()
	{
		m_birdPackView = (GuiBirdPack*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_PACK, "BirdPackAndFuse-BirdPackView");
		m_birdPackView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.0f), UDim(1.0f, 0.0f));
		m_birdPackView->SetTouchable(false);
		m_pack->AddChildWindow(m_birdPackView);
	}

	void gui_birdPackAndFuse::initFuseItem()
	{
		m_birdFuseView = (GuiBirdFuse*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_FUSE, "BirdPackAndFuse-BirdFuseView");
		m_birdFuseView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.0f), UDim(1.0f, 0.0f));
		m_birdFuseView->SetTouchable(false);
		m_fuse->AddChildWindow(m_birdFuseView);
	}

	void gui_birdPackAndFuse::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			if (m_gvSortXSize != m_gvSort->GetPixelSize().x)
			{
				m_gvSortXSize = m_gvSort->GetPixelSize().x;
				sortItemSizeChange();
			}
		}
	}

	bool gui_birdPackAndFuse::onPackUpdate()
	{
		m_isPackUpdate = true;
		if (isShown())
		{
			onShow();
		}
		return false;
	}

	void gui_birdPackAndFuse::setShowPackOrFuse(bool isPack)
	{
		m_radioMap[isPack ? ViewId::PACK : ViewId::FUSE]->SetSelected(true);
	}

	bool gui_birdPackAndFuse::isShowPack()
	{
		return m_radioMap[ViewId::PACK]->IsSelected();
	}

	bool gui_birdPackAndFuse::onShowBirdInfo(i64 birdId, bool isRefresh)
	{
		if (!UICommon::checkBirdBagParam())
		{
			return false;
		}
		bool isChange = false;
		auto birds = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag()->birds;
		for (auto bird : birds)
		{
			if (bird.id == birdId)
			{
				isChange = true;
				m_birdInfo = bird;
				m_birdPackView->setBird(bird);
				break;
			}
		}

		if (!isChange && birds.size() > 0)
		{
			m_birdInfo = birds[0];
			m_birdPackView->setBird(m_birdInfo);
		}

		for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
		{
			dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvSort->GetItem(i))->setSelect(birdId);
		}
		if (isRefresh)
		{
			m_birdPackView->changeView();
		}
		else
		{
			m_birdPackView->hideView();
		}
		return true;
	}

	bool gui_birdPackAndFuse::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioMap[viewId]->IsSelected())
		{
			m_carryTab->SetVisible(viewId == ViewId::PACK);
			m_icon->SetImage(viewId == ViewId::FUSE ? "set:bird_pack.json image:fuseIcon" : "set:bird_pack.json image:packIcon");

			for (auto item : m_radioContentMap)
			{
				item.second->SetVisible(item.first == viewId);
			}
			changeSortBirdItem();
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_birdPackAndFuse::onRadioSortChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioSortMap[viewId]->IsSelected())
		{
			playSoundByType(ST_Click);
			changeSortBirdItem();
		}
		return true;
	}

	bool gui_birdPackAndFuse::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::CARRAY:
			if (UICommon::checkBirdBagParam())
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
				if (bag->maxCarry >= bag->maxCarryLevel)
				{
					ShowBirdCommonTipEvent::emit("bird_carry_already_max_level");
				}
				else
				{
					ShowBirdExpandCarryTipEvent::emit(bag->expandCarryPrice, bag->expandCurrencyType);
				}
			}
			break;
		case ViewId::CAPACITY:
			if (UICommon::checkBirdBagParam())
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
				if (bag->maxCapacity >= bag->maxCapacityLevel)
				{
					ShowBirdCommonTipEvent::emit("bird_capacity_already_max_level");
				}
				else
				{
					ShowBirdExpandCapacityTipEvent::emit(bag->expandCapacityPrice, bag->expandCurrencyType);
				}
			}
			break;
		}
		m_birdPackView->hideView();
		return true;
	}

	bool gui_birdPackAndFuse::openExpandCarry()
	{
		RootGuiLayout::Instance()->showBirdPackAndFuse(true);
		onClick(EventArgs(), ViewId::CARRAY);
		return false;
	}

	void gui_birdPackAndFuse::changeSortBirdItem()
	{
		if (!UICommon::checkBirdBagParam())
		{
			return;
		}
		auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
		float oldOffset = m_gvSort->GetScrollOffset();
		m_gvSort->RemoveAllItems();
		m_birdFuseView->removeAllItems();
		getWindow("BirdPackAndFuse-Carry-Tab-Text")->SetText(StringUtil::Format("%d/%d", bag->curCarry, bag->maxCarry).c_str());
		getWindow("BirdPackAndFuse-Capacity-Tab-Text")->SetText(StringUtil::Format("%d/%d", bag->curCapacity, bag->maxCapacity).c_str());
		static int count = 0;
		auto birds = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBirdsSort(m_radioSortMap[ViewId::LEVEL]->IsSelected());
		for (auto item : birds)
		{
			String iterLayout = StringUtil::Format("BirdPackAndFuse-Sort-GridView-Item-%d", count++).c_str();
			GuiBirdPackAndFuseItem* itemView = (GuiBirdPackAndFuseItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_PACK_AND_FUSE_ITEM, iterLayout.c_str());
			float w = (m_gvSort->GetPixelSize().x  - 52) / 5;
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 4.f));
			itemView->setBirdInfo(item);
			itemView->SetTouchable(true);
			itemView->setShowPackView(isShowPack());
			m_gvSort->AddItem(itemView);
		}
		m_gvSort->ResetPos();
		m_gvSort->SetScrollOffset(oldOffset);
		if (isShowPack() && birds.size() > 0)
		{
			if (m_birdInfo.id > 0)
			{
				onShowBirdInfo(m_birdInfo.id);
			}
			else
			{
				onShowBirdInfo(birds[0].id);
			}
		}

		if (birds.size() == 0)
		{
			m_birdPackView->setBird(BirdInfo());
		}
		onViewSoll(EventArgs(), ViewId::GRID_VIEW);
	}

	bool gui_birdPackAndFuse::onFuseRemoveItem(i64 birdId)
	{
		for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
		{
			dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvSort->GetItem(i))->unSelect(birdId);
		}
		return true;
	}

	bool gui_birdPackAndFuse::onFusePutItem(i64 birdId, const String & image)
	{
		if (m_birdFuseView->onPutBird(birdId, image))
		{
			for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
			{
				dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvSort->GetItem(i))->setSelect(birdId);
			}
		}
		return false;
	}

	bool gui_birdPackAndFuse::onViewSoll(const EventArgs &, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::GRID_VIEW:
		{
			float offset = m_gvSort->GetScrollOffset();
			float height = m_gvSort->GetContainerHeight();
			float progres = offset / (m_gvSort->GetPixelSize().y - height);
			m_sollBar->SetProgress(progres);
		}
			break;
		case ViewId::SOLL_BAR:
		{
			float progres = m_sollBar->GetProgress();
			float height = m_gvSort->GetContainerHeight();
			float offset = (m_gvSort->GetPixelSize().y - height) * progres;
			m_gvSort->SetScrollOffset(offset > 0 ? 0 : offset);
		}
			break;
		case ViewId::HIDE_VIEW:
			m_birdPackView->hideView();
		break;
		}
		return true;
	}

	void gui_birdPackAndFuse::sortItemSizeChange()
	{
		for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
		{
			float w = (m_gvSort->GetPixelSize().x - 52) / 5;
			m_gvSort->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 4.f));
		}
	}

}
