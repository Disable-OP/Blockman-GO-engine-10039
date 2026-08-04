#include "CustomGuiWindow.h"
#include "Util/LanguageManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	UVector2 CustomGuiWindow::getAbsolutePositionFromRoot()
	{
		GUIWindow* curWindow = this;
		GUIWindow* parentWindow = this->GetParent();
		Vector2 position(0, 0);
		while (parentWindow)
		{
			auto curWindowSize = curWindow->GetPixelSize();
			auto parentWindowSize = parentWindow->GetPixelSize();
			auto curPosition = curWindow->GetPosition().asAbsolute(parentWindowSize);
			switch (curWindow->GetHorizontalAlignment())
			{
			case HA_LEFT:
				position.x += curPosition.x;
				break;
			case HA_CENTRE:
				position.x += curPosition.x + (parentWindowSize.x - curWindowSize.x) / 2;
				break;
			case HA_RIGHT:
				position.x += curPosition.x + parentWindowSize.x - curWindowSize.x;
				break;
			}
			switch (curWindow->GetVerticalAlignment())
			{
			case VA_TOP:
				position.y += curPosition.y;
				break;
			case VA_CENTRE:
				position.y += curPosition.y + (parentWindowSize.y - curWindowSize.y) / 2;
				break;
			case VA_BOTTOM:
				position.y += curPosition.y + parentWindowSize.y - curWindowSize.y;
				break;
			}
			curWindow = parentWindow;
			parentWindow = parentWindow->GetParent();
		}
		return curWindow->GetPosition() + UVector2({ 0, position.x }, { 0, position.y });
	}

	void CustomGuiWindow::subscribeToBubbleUpMouseEvents()
	{
		for (GUIWindow* child : getPrivateChildren())
		{
			child->subscribeEvent(EventWindowTouchDown, SubscriberSlot(&CustomGuiWindow::onChildTouchDown, this));
			child->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&CustomGuiWindow::onChildTouchUp, this));
			child->subscribeEvent(EventWindowTouchMove, SubscriberSlot(&CustomGuiWindow::onChildTouchMove, this));
			child->subscribeEvent(EventMotionRelease, SubscriberSlot(&CustomGuiWindow::onChildMotionRelease, this));
			child->subscribeEvent(EventWindowDragging, SubscriberSlot(&CustomGuiWindow::onChildDragging, this));
			child->subscribeEvent(EventWindowDragEnd, SubscriberSlot(&CustomGuiWindow::onChildDragEnd, this));
			child->subscribeEvent(EventWindowLongTouchStart, SubscriberSlot(&CustomGuiWindow::onChildLongTouchStart, this));
			child->subscribeEvent(EventWindowLongTouchEnd, SubscriberSlot(&CustomGuiWindow::onChildLongTouchEnd, this));
		}
	}

	GUIString CustomGuiWindow::getString(String key)
	{
		if (!LanguageManager::Instance())
		{
			return key.c_str();
		}
		return LanguageManager::Instance()->getString(key).c_str();
	}

	void CustomGuiWindow::Update(ui32 nTimeElapse)
	{
		GUIWindow::Update(nTimeElapse);
		// TODO: currently, private children is updated after the normal children. This might be cause bugs in some cases
		for (GUIWindow* child : getPrivateChildren())
		{
			child->Update(nTimeElapse);
		}
	}

	bool CustomGuiWindow::onChildTouchDown(const EventArgs & events)
	{
		auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
		TouchDown(mouseEventArgs.position);
		return true;
	}

	bool CustomGuiWindow::onChildTouchUp(const EventArgs & events)
	{
		auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
		TouchUp(mouseEventArgs.position);
		return true;
	}

	bool CustomGuiWindow::onChildTouchMove(const EventArgs & events)
	{
		auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
		TouchMove(mouseEventArgs.position);
		return true;
	}

	bool CustomGuiWindow::onChildMotionRelease(const EventArgs & events)
	{
		MotionRelease();
		return true;
	}

	bool CustomGuiWindow::onChildDragging(const EventArgs & events)
	{
		auto touchEventArgs = dynamic_cast<const TouchEventArgs&>(events);
		onDragging(touchEventArgs.touch);
		return true;
	}

	bool CustomGuiWindow::onChildDragEnd(const EventArgs & events)
	{
		auto touchEventArgs = dynamic_cast<const TouchEventArgs&>(events);
		onDragEnd(touchEventArgs.touch);
		return true;
	}

	bool CustomGuiWindow::onChildLongTouchStart(const EventArgs & events)
	{
		auto touchEventArgs = dynamic_cast<const TouchEventArgs&>(events);
		onLongTouchStart(touchEventArgs.touch);
		return true;
	}

	bool CustomGuiWindow::onChildLongTouchEnd(const EventArgs & events)
	{
		auto touchEventArgs = dynamic_cast<const TouchEventArgs&>(events);
		onLongTouchEnd(touchEventArgs.touch);
		return true;
	}

	void CustomGuiWindow::addPrefixToAllWindowNames(const GUIString& prefix, GUIWindow * rootWindow)
	{
		rootWindow->Rename(prefix + rootWindow->GetName());
		auto pCustomGuiWindow = dynamic_cast<CustomGuiWindow*>(rootWindow);
		if (pCustomGuiWindow)
		{
			for (GUIWindow* child : pCustomGuiWindow->getPrivateChildren())
			{
				addPrefixToAllWindowNames(prefix, child);
			}
		}
		for (int i = 0; i < int(rootWindow->GetChildCount()); ++i)
		{
			addPrefixToAllWindowNames(prefix, rootWindow->GetChildByIndex(i));
		}
	}
}