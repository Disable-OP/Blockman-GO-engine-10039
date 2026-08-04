#include "GUIAnimate.h"
#include "GUIImage.h"
#include "Object/Root.h"
#include "GUIRenderImage.h"
#include "GUIWindow.h"

namespace LORD
{	
	GUIAnimate::GUIAnimate()
		:m_id(0)		
		,m_playTime(0)
		,m_loop(false)
		,m_hasRenderTime(0)
		,m_index(0)
		,m_playContinue(true)
	{

	}

	GUIAnimate::~GUIAnimate()
	{		
		m_vecImages.clear();
	}

	Vector2 GUIAnimate::getCurrentImageVect2()
	{
		m_renderImage.SetWholeImageName(m_vecImages[m_index].strWholeImageName);
		return m_renderImage.GetImageSize();
	}

	void GUIAnimate::doPlay(const ERect &rt, const GUIWindow* window)
	{		
		if (m_playContinue)
		{		
			m_renderImage.SetWholeImageName(m_vecImages[m_index].strWholeImageName);

			Color color;
			color.setABGR(m_vecImages[m_index].color);
			if (m_index-1 >=0)
			{
				// 跟前一帧插值
				Color colorTemp;
				colorTemp.setABGR(m_vecImages[m_index-1].color);

				color = colorTemp + (color - colorTemp) * (1.0f * m_hasRenderTime/m_playTime);
			}

			m_renderImage.DrawImage(window, rt, color, 0.0f);
			m_hasRenderTime=m_hasRenderTime + Root::Instance()->getFrameTime();//getFrameTime:当前渲染距离上次渲染间隔多少毫秒
			if (m_hasRenderTime>=m_playTime)
			{
				m_hasRenderTime=0;
				m_index=(m_index + 1)% m_vecImages.size();
				if (0==m_index)
					m_playContinue=false;
			}
			if (m_loop)
				m_playContinue=true;
		}
		else
		{
			m_renderImage.SetWholeImageName(m_vecImages[m_vecImages.size()-1].strWholeImageName);			
			m_renderImage.DrawImage(window, rt, m_vecImages[m_vecImages.size() - 1].color, 0.0f);
		}
		
	}

	void GUIAnimate::clearImage()
	{
		m_renderImage.SetWholeImageName("");
	}

	void GUIAnimate::doPlayEx(const ERect &rt, const GUIWindow* window, GUIRenderImage& render, ui32& timeStamp, int& index, bool& play)
	{
		if (play)
		{
			render.SetWholeImageName(m_vecImages[index].strWholeImageName);

			Color color;
			color.setABGR(m_vecImages[index].color);
			if (index - 1 >= 0)
			{
				// 跟前一帧插值
				Color colorTemp;
				colorTemp.setABGR(m_vecImages[index - 1].color);

				color = colorTemp + (color - colorTemp) * (1.0f * timeStamp / m_playTime);
			}

			//color.a = color.a * window->GetAlpha();
			
			const_cast<GUIWindow*>(window)->SetAlpha(color.a);

			render.DrawImage(window, rt, color, 0.0f);
			timeStamp = timeStamp + Root::Instance()->getFrameTime();//getFrameTime:当前渲染距离上次渲染间隔多少毫秒
			if (timeStamp >= m_playTime)
			{
				timeStamp = 0;
				index = (index + 1) % m_vecImages.size();
				if (0 == index)
					play = false;
			}
			if (m_loop)
				play = true;
		}
		else
		{
			render.SetWholeImageName(m_vecImages[m_vecImages.size() - 1].strWholeImageName);
			render.DrawImage(window, rt, m_vecImages[m_vecImages.size() - 1].color, 0.0f);
		}
	}

}
