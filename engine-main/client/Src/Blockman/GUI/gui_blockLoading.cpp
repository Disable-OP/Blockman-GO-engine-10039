#include "gui_blockLoading.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	gui_blockLoading::gui_blockLoading()
		: gui_layout("BlockLoading.json")
	{
	}

	gui_blockLoading::~gui_blockLoading()
	{
	}

	void gui_blockLoading::onLoad()
	{
		m_curTick = 0;
		m_loadingText = getWindow<GUIStaticText>("BlockLoading-Content-Text");
		m_loadingProgressBar = getWindow<GUIProgressBar>("BlockLoading-Content-Progress");
		m_movingWindow = getWindow("BlockLoading-Content-Move");
		m_runImage = getWindow<GUIStaticImage>("BlockLoading-Content-Move-Run");
		m_originPos = m_movingWindow->GetPosition();
		m_distance = 494.0f;

		m_subscriptionGuard.add(UpdateBlockProgressEvent::subscribe(std::bind(&gui_blockLoading::RefreshProgress, this, std::placeholders::_1, std::placeholders::_2)));
	}

	void gui_blockLoading::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_blockLoading::RefreshProgress(float fProgress, bool bFirstUpdate)
	{
		if (bFirstUpdate)
		{
			m_curTick = 0;
		}

		m_curTick++;

		if (fProgress < 1.0f)
		{
			getParent()->showBlockLoading(true);
			String text = StringUtil::ToString((int)(fProgress * 100)) + "%";
			m_loadingText->SetText(text.c_str());
			auto pos = m_originPos + UVector2({0.0f, m_distance * fProgress}, {0.0f, 0.0f});
			m_movingWindow->SetPosition(pos);
			m_loadingProgressBar->SetProgress(fProgress);
		}
		else
		{
			getParent()->showBlockLoading(false);
			return true;
		}

		setRunImage();
		return true;
	}
	void gui_blockLoading::setRunImage()
	{
		int frame = m_curTick % 10; // which frame now
		int ret = frame % 5; // which image
		if (ret == 0)
			ret = 5;

		String image = StringUtil::Format("set:loadingBlock.json image:run_0%d", ret);
		m_runImage->SetImage(image.c_str());
	}
}


