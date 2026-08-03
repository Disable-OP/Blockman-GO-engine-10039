#include "GUIRadioButton.h"
#include "GUISystem.h"
#include "GUIEventArgs.h"

#include "Audio/SoundSystem.h"

namespace LORD
{
	GUIRadioButton::GUIRadioButton( GUIWindowType nType, const GUIString& strName )
		: GUIButton(nType, strName)
		, m_bSelected(false)
		, m_nGroupID(0)
	{
	}

	GUIRadioButton::~GUIRadioButton()
	{
	}

	bool GUIRadioButton::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIButton::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_RADIO_GROUP)
		{
			SetGroupID(StringUtil::ParseUI32(strValue.c_str()));
			return true;
		}

		return false;
	}

	bool GUIRadioButton::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIButton::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_RADIO_GROUP)
		{
			strValue = StringUtil::ToString(m_nGroupID).c_str();
			return true;
		}
		
		return false;
	}

	void GUIRadioButton::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIButton::GetPropertyList(properlist);
		properlist.push_back(GWP_RADIO_GROUP);
	}

	bool GUIRadioButton::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIButton::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_RADIO_GROUP)
		{
			type = GPT_INT;
			return true;
		}

		return false;
	}

	void GUIRadioButton::SetSelected( bool bSelected )
	{
		if (bSelected != m_bSelected)
		{
			m_bSelected = bSelected;
			m_eState = m_bSelected ? BS_PUSHED : BS_NORMAL;

			Invalidate();

			if (m_bSelected)
			{
				DeselectOtherButtonInGroup();
			}

			//GUISystem::Instance()->GetEventCallback()(GET_RADIO_STATE_CHANGED, GetOwnerDialogName(), GetName(), m_bSelected, 0);

			WindowEventArgs args(this);
			fireEvent(EventRadioStateChanged, args);
		}
	}

	void GUIRadioButton::SetGroupID( uint nGroupID )
	{
		if (nGroupID != m_nGroupID)
		{
			m_nGroupID = nGroupID;
			if (m_bSelected)
			{
				DeselectOtherButtonInGroup();
			}
		}
	}

	void GUIRadioButton::DeselectOtherButtonInGroup()
	{
		if (m_pParent)
		{
			size_t nChildCount = m_pParent->GetChildCount();

			for (size_t i = 0; i < nChildCount; ++i)
			{
				if (m_pParent->GetChildByIndex(i)->GetType() == GetType())
				{
					GUIRadioButton* pRadio = (GUIRadioButton*)m_pParent->GetChildByIndex(i);

					if (pRadio->IsSelected() && (pRadio != this) && (pRadio->GetGroupID() == m_nGroupID))
					{
						pRadio->SetSelected(false);
					}
				}
			}
		}
	}

	bool GUIRadioButton::TouchDown( const Vector2& position )
	{
		
		return true;
	}

	bool GUIRadioButton::TouchUp( const Vector2& position )
	{
		SetSelected(true);
		WindowEventArgs args(this);
		fireEvent(EventWindowTouchUp, args);
		SoundSystem::Instance()->playEffectByType((SoundType)m_nSoundType);
		return true;
	}

	bool GUIRadioButton::MotionRelease()
	{
		return true;
	}

}
