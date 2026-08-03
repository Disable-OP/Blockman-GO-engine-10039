#include "GUIRenderTargetManager.h"
#include "Render/RenderTarget.h"
#include "GUIRenderManager.h"
#include "Render/Material.h"
#include "GUISystem.h"

namespace LORD
{
	//---------------------------------------------------------------------
	GUIRenderTargetManager* GUIRenderTargetManager::Instance()
	{
		return ms_pSingleton;
	}

	GUIRenderTargetManager::GUIRenderTargetManager()
	{
		m_RenderTargetFlagList.reserve(MAX_GUI_RT_COUNT);
		m_RenderTargetFlagList.resize(MAX_GUI_RT_COUNT);
		for (size_t i = 0; i < MAX_GUI_RT_COUNT; ++i)
		{
			m_RenderTargetFlagList[i].m_bFlag = false;
			m_RenderTargetFlagList[i].m_nOldID = 0;		
			m_RenderTargetFlagList[i].m_nOldWidth = 0;
			m_RenderTargetFlagList[i].m_nOldHeight = 0;
		}
	}

	GUIRenderTargetManager::~GUIRenderTargetManager()
	{
	}

	int GUIRenderTargetManager::GetUnusedID()
	{
		for (int i = 0; i < MAX_GUI_RT_COUNT; ++i)
		{
			if (!m_RenderTargetFlagList[i].m_bFlag)
			{
				return i+RTI_GUI_BEGIN;
			}
		}

		return -1;
	}

	int GUIRenderTargetManager::CreateRenderTarget(float fWidth, float fHeight)
	{
		int nID = GetUnusedID();
		if (nID > 0)
		{
			RenderTarget* pRenderTarget = RenderTargetManager::Instance()->createRenderTarget(RenderTargetID(nID), RTCF_Fixed, fWidth, fHeight, PF_RGBA8_UNORM, false, false );
			if (!pRenderTarget)
			{
				LordLogInfo( "create gui render target failed, %d", nID);
				return -1;
			}
			else
			{
				m_RenderTargetFlagList[nID-RTI_GUI_BEGIN].m_bFlag = true;
				m_RenderTargetFlagList[nID-RTI_GUI_BEGIN].m_nOldID = 0;
				m_RenderTargetFlagList[nID-RTI_GUI_BEGIN].m_nOldWidth = 0;
				m_RenderTargetFlagList[nID-RTI_GUI_BEGIN].m_nOldHeight = 0;

				SamplerState* pState = GUIRenderManager::Instance()->GetMaterial()->getSamplerState(0);
				pRenderTarget->getBindTexture()->setSamplerState(pState);

				LordLogInfo( "create gui render target succeeded, %d", nID);
			}
		} 

		return nID;
	}

	void GUIRenderTargetManager::DestroyRenderTarget( int nID )
	{
		size_t nIndex = nID - RTI_GUI_BEGIN;
		if (nIndex >= MAX_GUI_RT_COUNT)
		{
			LordLogError("GUIRenderTargetManager::DestroyRenderTarget  failed, GUI rt index is bigger than cout! nindex: %d", nIndex);
			return;
		}

		if (m_RenderTargetFlagList[nIndex].m_bFlag)
		{
			RenderTargetManager::Instance()->destroyRenderTarget(RenderTargetID(nID));
			
			LordLogInfo("destroy gui render target succeeded, %d", nID);

			m_RenderTargetFlagList[nIndex].m_bFlag = false;
		}
	}

	void GUIRenderTargetManager::BeginRenderTarget( int nID, uint nWidth, uint nHeight)
	{
		size_t nIndex = nID - RTI_GUI_BEGIN;
		if (nIndex >= MAX_GUI_RT_COUNT)
		{
			LordLogError("GUIRenderTargetManager::BeginRenderTarget  failed, GUI rt index is bigger than cout! nindex: %d", nIndex);
			return;
		}

		m_RenderTargetFlagList[nIndex].m_nOldID = RenderTargetManager::Instance()->getInUsingRenderTargetID();
		
		m_RenderTargetFlagList[nIndex].m_nOldWidth = (uint)GUIRenderManager::Instance()->GetWidth();
		m_RenderTargetFlagList[nIndex].m_nOldHeight = (uint)GUIRenderManager::Instance()->GetHeight();


		RenderTargetManager::Instance()->endRenderTarget((RenderTargetID)m_RenderTargetFlagList[nIndex].m_nOldID);

		RenderTargetManager::Instance()->beginRenderTarget((RenderTargetID)nID, true, Color(0.f, 0.f, 0.f, 0.f), false);

		GUIRenderManager::Instance()->OnSized(nWidth, nHeight);

		// set viewport
		Viewport* pViewport = Renderer::Instance()->getMainViewport(); //getFrameBuffer()->getViewport();
		LordAssert(pViewport);
		pViewport->resize(0, 0, nWidth, nHeight);
		Renderer::Instance()->setViewport(pViewport);

	}

	void GUIRenderTargetManager::EndRenderTarget()
	{
		int nNowID = RenderTargetManager::Instance()->getInUsingRenderTargetID();
		size_t nIndex = nNowID - RTI_GUI_BEGIN;
		if (nIndex >= MAX_GUI_RT_COUNT)
		{
			LordLogError("GUIRenderTargetManager::EndRenderTarget  failed, GUI rt index is bigger than cout! nindex: %d", nIndex);
			return;
		}

		RenderTargetManager::Instance()->endRenderTarget(RenderTargetID(nNowID));
		RenderTargetManager::Instance()->beginRenderTarget((RenderTargetID)m_RenderTargetFlagList[nIndex].m_nOldID, false, Renderer::BGCOLOR, false);
		// 投影矩阵刷回来
		GUIRenderManager::Instance()->OnSized(m_RenderTargetFlagList[nIndex].m_nOldWidth, m_RenderTargetFlagList[nIndex].m_nOldHeight);


		// 设置viewport
		// 如果老的是屏幕的话，需要设置成屏幕的viewport，而不是逻辑的viewport
		uint nWidth = 0;
		uint nHeight = 0;

		if (m_RenderTargetFlagList[nIndex].m_nOldID == RTI_DefaultBackBuffer)
		{
			nWidth = GUISystem::Instance()->GetScreenWidth();
			nHeight = GUISystem::Instance()->GetScreenHeight();
			if (nWidth == 0 ||
				nHeight == 0)
			{
				LordLogFatal("GUIRenderTargetManager::EndRenderTarget() ERROR, the screen size is 0!");
			}
		} 
		else
		{
			nWidth = m_RenderTargetFlagList[nIndex].m_nOldWidth;
			nHeight = m_RenderTargetFlagList[nIndex].m_nOldHeight;
		}

		// set viewport
		Viewport* pViewport = Renderer::Instance()->getMainViewport();// getFrameBuffer()->getViewport();
		LordAssert(pViewport);
		pViewport->resize(0, 0, nWidth, nHeight);
		Renderer::Instance()->setViewport(pViewport);
	}

}