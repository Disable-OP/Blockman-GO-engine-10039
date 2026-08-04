#include "GUIChestRewardDetail.h"
#include "UI/GUILayout.h"

namespace BLOCKMAN
{
	GUIChestRewardDetail::GUIChestRewardDetail() :
		gui_layout("ChestRewardDetail.json")
	{
	}

	GUIChestRewardDetail::~GUIChestRewardDetail()
	{
	}

	void GUIChestRewardDetail::onLoad()
	{
		GUILayout* ChestRewardDetail = getWindow<GUILayout>("ChestRewardDetail");
		ChestRewardDetail->subscribeEvent(EventWindowTouchUp, std::bind(&GUIChestRewardDetail::onContentClick, this, std::placeholders::_1));

		m_title = getWindow<GUIStaticText>("ChestRewardDetail-Title");
		m_image = getWindow<GUIStaticImage>("ChestRewardDetail-Image");
		m_name = getWindow<GUIStaticText>("ChestRewardDetail-Name");
		m_desc = getWindow<GUIStaticText>("ChestRewardDetail-Desc");
	}

	void GUIChestRewardDetail::onUpdate(ui32 nTimeElapse)
	{
	}

	void GUIChestRewardDetail::ShowLotteryChestReward(LotteryChestReward * reward)
	{
		show();
		m_title->SetText(getString(reward->Title));
		m_image->SetImage(reward->Image.c_str());
		m_name->SetText(getString(reward->Name));
		m_desc->SetText(getString(reward->Desc));
	}

	bool GUIChestRewardDetail::onContentClick(const EventArgs & events)
	{
		hide();
		return true;
	}

}
