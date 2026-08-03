#include "GUIAnimateController.h"
#include "GUIWindow.h"
#include "GUIRenderManager.h"

namespace LORD
{


	GUIAnimateController::GUIAnimateController(GUIWindow* pWindow)
		: m_nAnimateType(AT_NONE)
		, m_pWindow(pWindow)
		, m_bStarted(false)
		, m_nElapseTime(0)
		, m_nWholeTime(300)
		, m_fRotateAngle(360.0f)
		, m_NeedRT(true)
	{
		if (m_pWindow)
		{
			m_TargetArea = m_pWindow->GetArea();
//			m_fTargetRotateY = m_pWindow->GetSavedRotateY();
		}
	}

	GUIAnimateController::~GUIAnimateController()
	{

	}

	void GUIAnimateController::Update( ui32 nTimeElapse )
	{
		if (m_bStarted)
		{
			m_nElapseTime += nTimeElapse;

			if (m_nElapseTime > m_nWholeTime)
			{
				// 动画停止
				m_bStarted = false;
				m_nElapseTime = 0;

				if (m_pWindow)
				{
					m_pWindow->SetArea(m_TargetArea);
					m_pWindow->SetAlpha(1.0f);
					if (m_nAnimateType == AT_ROTATE)
					{
						m_pWindow->SetRotateY(m_fTargetRotateY);
					}
					else if(m_nAnimateType == AT_POPUP)
					{
//						m_pWindow->SetRenderTarget(false);
					}
				}
			}
			else
			{
				DoAnimate();
			}
		}
	}

	void GUIAnimateController::Start()
	{
		// 在setrendertarget之前先set false
		// 之后再set 成 true
		m_bStarted = false;
		if(m_nAnimateType == AT_NONE)
			return;

		if (m_pWindow)
		{
//			m_TargetArea = m_pWindow->GetSavedArea();
		}
		switch (m_nAnimateType)
		{
		case AT_FLY_FROM_LEFT:
			{
				m_nWholeTime = 500;
				UDim startX = UDim(0 ,0) - m_TargetArea.getWidth();
				UDim endX   = m_TargetArea.getPosition().d_x;
				UDim dx     = endX - startX;
				float _time = m_nWholeTime * 0.01f* 0.75f;
				m_AnimParam[0] = 2*dx.d_offset / (_time * _time);
				m_AnimParam[1] = startX.d_offset;
			}
			break;
		case AT_POPUP:
			{
				m_AnimParam[1] = -1.f;
				m_nWholeTime = 600;
				float _time = m_nWholeTime * 0.01f * 0.75f;
				m_AnimParam[0] = 2.0f / (_time * _time);
				m_NeedRT = false;
//				m_pWindow->SetRenderTarget(true);
			}
			break;
		}

		m_bStarted = true;
		m_nElapseTime = 0;
	}

	void GUIAnimateController::DoAnimate()
	{
		if (!m_pWindow)
		{
			return;
		}

		float fTimePercent = m_nElapseTime * 1.0f / m_nWholeTime;

		switch (m_nAnimateType)
		{
		case AT_FLY_FROM_LEFT:
			{
				float _time = m_nElapseTime * 0.01f;
				float Dscale;
				UDim startX;
				if(m_nElapseTime < 300)
				{
					Dscale = 0.5f * m_AnimParam[0] * _time * _time;
					startX = UDim(0 ,0) - m_TargetArea.getWidth();
					
				}
				else  if(m_nElapseTime < 400)
				{
					_time = _time - 3.0f;
					Dscale = -0.5f * m_AnimParam[0] * _time * _time;
					startX = m_TargetArea.d_min.d_x;
				}
				else
				{
					_time = _time - 4.0f;
					Dscale = 0.5f * m_AnimParam[0] * _time * _time;
					startX.d_offset = m_TargetArea.d_min.d_x.d_offset - 0.5f * m_AnimParam[0];
					startX.d_scale  = 0.0f;
				}

				UDim newX = startX + UDim(0.0f,Dscale);
				m_pWindow->SetXPosition(newX);
				m_pWindow->SetAlpha(fTimePercent);
			}
			break;
		case AT_FLY_FROM_TOP:
			{
				UDim startY = UDim(0 ,0) - m_TargetArea.getHeight();
				UDim endY = m_TargetArea.getPosition().d_y;
				UDim newY = startY + UDim(fTimePercent, fTimePercent) * (endY - startY);

				m_pWindow->SetYPosition(newY);
			}
			break;
		case AT_FLY_FROM_RIGHT:
			{
				UDim startX(0.0f, (float)GUIRenderManager::Instance()->GetWidth());
				UDim endX = m_TargetArea.getPosition().d_x;
				UDim newX = startX + UDim(fTimePercent, fTimePercent) * (endX - startX);

				m_pWindow->SetXPosition(newX);
			}
			break;
		case AT_FLY_FROM_BOTTOM:
			{
				UDim startY(0.0f, (float)GUIRenderManager::Instance()->GetHeight());
				UDim endY = m_TargetArea.getPosition().d_y;
				UDim newY = startY + UDim(fTimePercent, fTimePercent) * (endY - startY);

				m_pWindow->SetYPosition(newY);
			}
			break;
		case AT_POPUP:
			{
				if(m_AnimParam[1] == -1.f)
				{
					m_AnimParam[1] = -2.f;
					m_nElapseTime = 0;
					break;
				}
				else if(m_AnimParam[1] == -2.f)
				{
					m_AnimParam[1] = -3.f;
					m_NeedRT = true;
				}
					
				float _time = m_nElapseTime * 0.01f;
//				float Dscale;
				if(_time < 4.f)
				{
//					Dscale = 0.5f * m_AnimParam[0] * _time * _time;
//					m_pWindow->SetScale(Dscale, m_pWindow->GetSavedArea());
					m_pWindow->SetAlpha(fTimePercent);
					//if(Dscale > 1.0f)
					//{
					//	int aaa =0;
					//}
				}
				else  if(_time < 5.f)
				{
					_time = _time - 4.0f;
//					Dscale = (m_AnimParam[0]*_time -  0.5f * m_AnimParam[0] * _time * _time) * 2.f;
//					m_pWindow->SetScale(1.0f - Dscale, m_pWindow->GetSavedArea());
					m_pWindow->SetAlpha(fTimePercent);
				}
				else
				{
					_time = _time - 5.0f;
//					Dscale = 0.5f * m_AnimParam[0] * _time * _time * 2.f;
//					float d = 1.0f + Dscale - 0.5f * m_AnimParam[0];
//					m_pWindow->SetScale(1.0f - Dscale, m_pWindow->GetSavedArea());
					m_pWindow->SetAlpha(fTimePercent);
				}
			}
			break;
		case AT_ROTATE:
			{
				float fStart = m_fTargetRotateY - m_fRotateAngle;
				m_pWindow->SetRotateY(fStart + fTimePercent*m_fRotateAngle);
			}
			break;
		}
	}

	ui32 GUIAnimateController::GetWholeTime()
	{
		ui32 ret = 0;
		if(m_nAnimateType == AT_POPUP)
			ret = 600;
		return ret;
	}

}
