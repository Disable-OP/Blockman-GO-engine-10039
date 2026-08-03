#include "GuiUrlImage.h"

#include "Render/TextureAtlas.h"
#include "GUI/GuiDef.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/ImageManager.h"

namespace BLOCKMAN
{
	GuiUrlImage::GuiUrlImage(const GUIWindowType& nType, const GUIString& strName)
		: BaseClass(nType, strName)
		, m_window(GUIWindowManager::Instance()->LoadWindowFromJSON(JSON_NAME))
	{
		renameComponents(m_window);
		m_iconImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_iconBack = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		AddWindowToDrawList(*m_window);
	}

	vector<GUIWindow*>::type GuiUrlImage::getPrivateChildren()
	{
		return { m_window };
	}

	void GuiUrlImage::onShowUrlImage()
	{
		if (m_iconImage)
		{
			m_isShowHttpIcon = true;
			m_iconImage->changeTexture((GameClient::ImageManager::Instance()->getUrlImageRootPath() + GameClient::ImageManager::Instance()->getImageName(m_currUrl)).c_str());
		}
	}

	void GuiUrlImage::onShowDefaultImage()
	{
		if (m_iconImage && !m_isShowHttpIcon)
		{
			m_iconImage->SetImage(m_defaultImage.c_str());
		}
	}

	void GuiUrlImage::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());

		if (m_oldUrl != m_currUrl)
		{

			String imageName = GameClient::ImageManager::Instance()->getImageName(m_currUrl);
			if (imageName == "")
			{
				GameClient::ImageManager::Instance()->setUrlImageDownloading(m_currUrl);
				BLOCKMAN::HttpPromise httpPromise = GameClient::ImageManager::Instance()->downloadImage(m_currUrl);
			}
			else if (imageName == "downloading")
			{
			}
			else
			{
				m_oldUrl = m_currUrl;
				onShowUrlImage();
			}
		}
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiUrlImage::Destroy()
	{
		RemoveWindowFromDrawList(*m_window);
		GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		BaseClass::Destroy();
	}

	void GuiUrlImage::Clone(const GUIString & strNamePreffix, GUIWindow * pWindow)
	{
		auto guiUrlImage = dynamic_cast<GuiUrlImage*>(pWindow);
		if (!guiUrlImage)
		{
			LordLogError("cannot clone GuiUrlImage from GUIWindow that isn't a GuiUrlImage");
			return;
		}
		BaseClass::Clone(strNamePreffix, guiUrlImage);
	}
}
