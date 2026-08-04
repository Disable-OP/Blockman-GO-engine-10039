#include "GuiRanchBuildRecipes.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Setting/RanchSetting.h"
#include "Util/LanguageManager.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/Ranch.h"

namespace BLOCKMAN
{

	void GuiRanchBuildRecipes::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchBuildRecipes::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchBuildRecipes::GuiRanchBuildRecipes(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchBuildRecipes.json"));
		renameComponents(m_window);
		m_ivIcon = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(0));
		m_ivlock = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_tvLevel = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_ivIcon->subscribeEvent(EventButtonClick, std::bind(&GuiRanchBuildRecipes::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchBuildRecipes::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return;
		}
		m_tvLevel->SetText(StringUtil::Format("Lv%d", m_productRecipe.needLevel).c_str());
		m_tvLevel->SetVisible(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level < m_productRecipe.needLevel);
		m_ivlock->SetVisible(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level < m_productRecipe.needLevel);
		m_ivIcon->SetEnabled(Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->level >= m_productRecipe.needLevel);
	}


	void GuiRanchBuildRecipes::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchBuildRecipes::setProductRecipe(ProductRecipe productRecipe)
	{
		m_productRecipe = productRecipe;
		m_ivIcon->SetNormalImage(RanchSetting::getRanchItemIcon(productRecipe.productId).c_str());
		m_ivIcon->SetPushedImage(RanchSetting::getRanchItemIcon(productRecipe.productId).c_str());
	}


	bool GuiRanchBuildRecipes::onClick(const EventArgs & args)
	{
		RanchRecipeItemClickEvent::emit(m_productRecipe);
		return true;
	}

}