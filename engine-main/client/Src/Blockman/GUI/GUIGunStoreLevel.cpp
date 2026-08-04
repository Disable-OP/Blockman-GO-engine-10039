#include "GUIGunStoreLevel.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	GUIGunStoreLevel::GUIGunStoreLevel(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("GunStoreLevel.json"));
		renameComponents(m_window);
		
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUIGunStoreLevel::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIGunStoreLevel::initChilds(bool forse)
	{
		if (m_init && !forse)
			return;
		m_init = true;
		m_window->CleanupChildren();
		float Width = 70.0f;
		Width = Math::Min(Width, GetPixelSize().x / m_maxLevel);
		float Height = Width * 0.6285f;
		float Offset = Width * 0.25f;
		static unsigned level_index = 0;
		float PositionX = GetPixelSize().x - (GetPixelSize().x - (Width - Offset) * m_maxLevel) / 2 - Width;
		for (int i = 0; i < m_maxLevel; i++)
		{
			level_index++;
			GUIString LevelName = StringUtil::Format("GUIGunStoreLevel-%d", level_index).c_str();
			GUIStaticImage* pLevelChild = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, LevelName);
			pLevelChild->SetWidth({ 0.0f, Width });
			pLevelChild->SetHeight({ 0.0f, Height });
			pLevelChild->SetImage(m_maxLevel - i <= m_curLevel ? "set:gun_store.json image:detail_level_pre" : "set:gun_store.json image:detail_level_nor");
			pLevelChild->SetXPosition({ 0.0f, PositionX });
			PositionX -= Width - Offset;
			m_window->AddChildWindow(pLevelChild);
		}
	}

	void GUIGunStoreLevel::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x }, { 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		initChilds(false);
	}

	void GUIGunStoreLevel::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIGunStoreLevel::InitLevel(int curLevel, int maxLevel)
	{
		m_curLevel = curLevel;
		if (m_maxLevel != maxLevel)
		{
			m_maxLevel = maxLevel;
			initChilds(true);
		}
		else
		{
			size_t count = m_window->GetChildCount();
			for (size_t i = 0; i < count; i++)
			{
				GUIStaticImage* child = (GUIStaticImage*)m_window->GetChildByIndex(i);
				if (child)
				{
					child->SetImage(m_maxLevel - i <= m_curLevel ? "set:gun_store.json image:detail_level_pre" : "set:gun_store.json image:detail_level_nor");
				}
			}
		}
	}

}
