#pragma once
#include <memory>
#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiUrlImage : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		constexpr static char* JSON_NAME = "UrlImage.json";
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_iconImage = nullptr;
		GUIStaticImage* m_iconBack = nullptr;
		String m_defaultImage = "";// set:ranch_boy_icon.json image:ranch_boy_icon
		String m_urlImage = "";
		String m_oldUrl = "";
		String m_currUrl = "";

		bool m_isShowHttpIcon = false;

	public:
		GuiUrlImage(const GUIWindowType& nType, const GUIString& strName);
		~GuiUrlImage() = default;

		void setDefaultImage(const String& defaultImage)
		{
			m_defaultImage = defaultImage;
			onShowDefaultImage();
		}
		void setUrl(const String& imageUrl) { m_currUrl = imageUrl; }
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void Clone(const GUIString& strNamePreffix, GUIWindow* pWindow) override;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	private:
		void onShowUrlImage();
		void onShowDefaultImage();
	};
}