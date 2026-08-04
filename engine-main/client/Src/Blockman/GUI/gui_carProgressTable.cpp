#include "gui_carProgressTable.h"
#include "ShellInterface.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_carProgressTable::gui_carProgressTable() :
		gui_layout("CarProgressTable.json")
	{

	}

	gui_carProgressTable::~gui_carProgressTable()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_carProgressTable::onLoad()
	{
		m_progress_list = getWindow<GUILayout>("CarProgressTable-ProgressList");
		m_yellow_progress = getWindow<GUICarProgress>("CarProgressTable-Yellow");
		m_green_progress = getWindow<GUICarProgress>("CarProgressTable-Green");
		m_red_progress = getWindow<GUICarProgress>("CarProgressTable-Red");

		m_progress_list->SetVisible(false);
		m_yellow_progress->SetStyle(CarProgressStyle::Yellow);
		m_green_progress->SetStyle(CarProgressStyle::Green);
		m_red_progress->SetStyle(CarProgressStyle::Red);

		m_subscriptionGuard.add(UpdateCarDirectionEvent::subscribe(std::bind(&gui_carProgressTable::onCarDirectionUpdate, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateCarProgressEvent::subscribe(std::bind(&gui_carProgressTable::onCarProgressUpdate, this, std::placeholders::_1, std::placeholders::_2)));
	
	}

	void gui_carProgressTable::onUpdate(ui32 nTimeElapse)
	{
		if (!m_visiable)
			return;
		float positionY = RootGuiLayout::Instance()->canPersonalShop() ? 118.0f : 55.0f;
		if (m_recordPosY != positionY)
		{
			m_recordPosY = positionY;
			m_progress_list->SetYPosition({ 0, m_recordPosY });
		}
	}

	bool gui_carProgressTable::onCarDirectionUpdate(int type, int direction)
	{
		m_visiable = true;
		m_progress_list->SetVisible(true);
		CarProgressDirection dir = direction == 1 ? CarProgressDirection::Left : CarProgressDirection::Right;
		switch(type)
		{
		case 1:
			m_yellow_progress->SetDirection(dir);
			break;
		case 2:
			m_green_progress->SetDirection(dir);
			break;
		case 3:
			m_red_progress->SetDirection(dir);
			break;
		default:
			break;
		}
		return true;
	}

	bool gui_carProgressTable::onCarProgressUpdate(int type, float progress)
	{
		m_visiable = true;
		m_progress_list->SetVisible(true);
		switch (type)
		{
		case 1:
			m_yellow_progress->SetProgress(progress);
			break;
		case 2:
			m_green_progress->SetProgress(progress);
			break;
		case 3:
			m_red_progress->SetProgress(progress);
			break;
		default:
			break;
		}
		return true;
	}

}

