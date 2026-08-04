#include "GUICheckBox.h"
#include "GUIFont.h"
#include "GUISystem.h"
#include "GUIEventArgs.h"

#include "Audio/SoundSystem.h"

namespace LORD
{
	GUICheckBox::GUICheckBox( GUIWindowType nType, const GUIString& strName )
		: GUIButton(nType, strName)
		, m_bChecked(false)
	{
	}

	GUICheckBox::~GUICheckBox()
	{
	}

	bool GUICheckBox::TouchDown( const Vector2& position )
	{
		GUIWindow::TouchDown(position);
		
		return true;
	}

	bool GUICheckBox::TouchUp( const Vector2& position )
	{
		GUIWindow::TouchUp(position);

		SetChecked(!GetChecked());
		Invalidate();
		SoundSystem::Instance()->playEffectByType((SoundType)m_nSoundType);
		return true;
	}

	bool GUICheckBox::MotionRelease()
	{
		return true;
	}

	void GUICheckBox::SetChecked( bool bChecked )
	{
		if (m_bChecked != bChecked)
		{
			m_bChecked = bChecked;
			m_eState = m_bChecked ? BS_PUSHED : BS_NORMAL;

			WindowEventArgs args(this);
			fireEvent(EventCheckStateChanged, args);
		}
	}

	void GUICheckBox::SetCheckedNoEvent( bool bChecked )
	{
		if (m_bChecked != bChecked)
		{
			m_bChecked = bChecked;
			m_eState = m_bChecked ? BS_PUSHED : BS_NORMAL;
		}
	}

}
