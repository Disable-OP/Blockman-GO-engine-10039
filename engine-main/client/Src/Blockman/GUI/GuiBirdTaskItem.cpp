#include "GuiBirdTaskItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/BirdSimulator.h"

namespace BLOCKMAN
{
	
	void GuiBirdTaskItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdTaskItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdTaskItem::GuiBirdTaskItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdTaskItem.json"));
		renameComponents(m_window);


		m_title = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_content = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1));

		m_tvTitleName = dynamic_cast<GUIStaticText*>(m_title->GetChildByIndex(0));
		m_progressBar = dynamic_cast<GUIProgressBar*>(m_content->GetChildByIndex(0));
		m_tvTaskInfo = dynamic_cast<GUIStaticText*>(m_content->GetChildByIndex(1));
		

		AddWindowToDrawList(*m_window);
	}

	void GuiBirdTaskItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (UICommon::checkBirdParam() && m_taskId >0 && m_taskItemId >0)
		{
			auto task = Blockman::Instance()->m_pPlayer->m_birdSimulator->getTaskById(m_taskId);
			if (task.taskId > 0)
			{
				for (	auto& item : task.contents)
				{
					if (item.id == m_taskItemId)
					{
						float progress = item.curValue * 1.f / item.needValue * 1.f;
						String desc = getString(item.desc).c_str();
						if (desc.find("%s") < desc.find("%d"))
						{
							desc = StringUtil::Format(desc.c_str(), getString(item.name).c_str(), item.needValue);
							String value = StringUtil::Format("%d/%d", item.curValue, item.needValue);
							setTaskInfo(StringUtil::Format("%s %s", desc.c_str(), value.c_str()));
							setProgress(progress);
						}
						else
						{
							desc = StringUtil::Format(desc.c_str(), item.needValue, getString(item.name).c_str());
							String value = StringUtil::Format("%d/%d", item.curValue, item.needValue);
							setTaskInfo(StringUtil::Format("%s %s", desc.c_str(), value.c_str()));
							setProgress(progress);
						}
						break;
					}
				}
			}
		}

	}

	
	void GuiBirdTaskItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBirdTaskItem::setTitle(const String & title)
	{
		m_title->SetVisible(true);
		m_content->SetVisible(false);
		m_tvTitleName->SetText(title.c_str());

	}

	void GuiBirdTaskItem::setTaskInfo(const String & info)
	{
		m_title->SetVisible(false);
		m_content->SetVisible(true);
		m_tvTaskInfo->SetText(GUIString(info.c_str()));
	}

	void GuiBirdTaskItem::setProgress(float progress)
	{
		m_title->SetVisible(false);
		m_content->SetVisible(true);
		m_progressBar->SetProgress(progress);
	}


}