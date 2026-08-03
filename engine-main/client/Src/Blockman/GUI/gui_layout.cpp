#include "GUI/gui_layout.h"
#include <functional>
#include "UI/GUIWindowManager.h"
#include "UI/GUIWindowFactory.h"
#include "UI/GUISystem.h"
#include "GUI/RootGuiLayout.h"
#include "Functional/LordNewOp.h"

namespace BLOCKMAN
{

	const GUIWindowType gui_layout::GWT_ROOT_WINDOW = "RootGuiWindow"_gwt;

	bool gui_layout::rootGuiExists = false;

	gui_layout::gui_layout()
		: m_isRootLayout(true)
	{
		LordAssertX(!rootGuiExists, "root layout is created more than once!");
		rootGuiExists = true;
	}

	gui_layout::~gui_layout()
	{
		unload();
		if (m_isRootLayout)
		{
			rootGuiExists = false;
		}
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_layout::load()
	{
		if (!isLoaded())
		{
			if (m_isRootLayout)
			{
				loadRootWindow();
			}
			else
			{
				m_rootWindow = GUIWindowManager::Instance()->LoadWindowFromJSON(m_jsonName.c_str());
			}
			LordAssert(m_rootWindow != nullptr);
			onLoad();
		}
	}

	void gui_layout::unload()
	{
		if (isLoaded())
		{
			if (m_isRootLayout)
			{
				hide();
			}
			else
			{
				detach();
			}
			onUnload();
			while (!m_childLayouts.empty())
			{
				m_childLayouts.front()->detach();
			}
			GUIWindowManager::Instance()->DestroyGUIWindow(m_rootWindow);
			m_rootWindow = nullptr;
		}
	}

	void gui_layout::attachTo(gui_layout* parent, GUIWindow * container)
	{
		LordAssert(!m_isRootLayout);
		if (isAttached())
		{
			LordLogError("this layout is already attached to a container");
			return;
		}
		load();
		m_container = container;
		m_parentLayout = parent;
		container->AddChildWindow(m_rootWindow);
		parent->m_childLayouts.push_back(this);
		onAttach();
	}

	void gui_layout::detach()
	{
		LordAssert(!m_isRootLayout);
		if (!isAttached())
		{
			LordLogWarning("this layout is not attached to a container");
			return;
		}
		hide();
		onDetach();
		m_container->RemoveChildWindow(m_rootWindow);
		m_container = nullptr;
		auto& children = m_parentLayout->m_childLayouts;
		auto pos = std::find(children.begin(), children.end(), this);
		if (pos != children.end())
		{
			children.erase(pos);
		}
		m_parentLayout = nullptr;
	}

	void gui_layout::show()
	{
		if (isShown())
		{
			return;
		}
		if (m_isRootLayout)
		{
			load();
		}
		else if (!isAttached())
		{
			LordLogWarning("this layout is not attached to a container");
			RootGuiLayout::Instance()->addLayout(this);
		}
		m_rootWindow->SetVisible(true);
		onShow();
	}

	void gui_layout::hide()
	{
		if (isShown())
		{
			onHide();
			m_rootWindow->SetVisible(false);
		}
	}

	void gui_layout::update(ui32 nTimeElapse)
	{
		onUpdate(nTimeElapse);
		std::for_each(m_childLayouts.begin(), m_childLayouts.end(), std::bind(&gui_layout::update, std::placeholders::_1, nTimeElapse));
	}

	void gui_layout::removeLayout(gui_layout * layout)
	{
		if (std::find(m_childLayouts.begin(), m_childLayouts.end(), layout) == m_childLayouts.end())
		{
			LordLogWarning("specified layout is not a child of this layout");
			return;
		}
		layout->detach();
	}

	void gui_layout::setText(const String & viewName, const String &  key)
	{
		auto view = getWindow(viewName.c_str());
		if (view)
		{
			view->SetText(getString(key));
		}
	}

	void gui_layout::setText(GUIWindow * window, const String & key)
	{
		if (window)
		{
			window->SetText(getString(key));
		}
	}

	void gui_layout::loadRootWindow()
	{
		static bool registered = (GUIWindowFactory::Instance()->registerWindowType(GWT_ROOT_WINDOW, lord_new_op<RootGuiWindow>()), true);
		if (!isLoaded())
		{
			m_rootWindow = GUIWindowManager::Instance()->CreateGUIWindow(GWT_ROOT_WINDOW, GUIString("RootWindow"));
			m_rootWindow->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
			m_rootWindow->SetSize(UVector2(UDim(1, 0), UDim(1, 0)));
			GUISystem::Instance()->SetRootWindow(m_rootWindow);
		}
	}

	void gui_layout::RootGuiWindow::Update(ui32 nTimeElapse)
	{
		GUIWindow::Update(nTimeElapse);
		RootGuiLayout::Instance()->update(nTimeElapse);
	}

	void gui_layout::playSoundByType(SoundType soundType)
	{
		SoundSystem::Instance()->playEffectByType(soundType);
	}

	void gui_layout::stopOrCloseBackgroundMusic()
	{
		SoundSystem::Instance()->stopBackgroundMusic();
	}
}
