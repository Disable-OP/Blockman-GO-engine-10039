#include "GUIRenderImage.h"
#include "GUIImagesetManager.h"
#include "GUIImageset.h"
#include "GUIImage.h"
#include "GUIWindow.h"

namespace LORD
{
	GUIRenderImage::GUIRenderImage()
		: m_pImageset(NULL)
		, m_pImage(NULL)
	{
	}

	GUIRenderImage::~GUIRenderImage()
	{
		if (m_pImageset)
		{
			GUIImagesetManager::Instance()->pushbackReleaseImageset(m_pImageset);
			m_pImageset = NULL;
		}

		m_actionVertex.clear();
		m_actionVertexLR.clear();
		m_actionVertexTB.clear();
		m_actionVertexNine.clear();
	}

	void GUIRenderImage::SetWholeImageName( const GUIString& strWholeName )
	{
		if (strWholeName != m_strWholeImageName)
		{
			// 贴图变了，肯定要清空
			m_pImage = NULL;

			GUIString strNewSetName;
			GUIImagesetManager::Instance()->ParseImageString(strWholeName, strNewSetName, m_strImageName);

			if (strNewSetName != m_strImagesetName)
			{
				// 如果跟老的贴图不一样，就释放老的
				if (m_pImageset)
				{
					// 加入一个队列缓存起来，保证加载线程加载完再释放
					GUIImagesetManager::Instance()->pushbackReleaseImageset(m_pImageset);
					m_pImageset = NULL;
				}

				m_strImagesetName = strNewSetName;
				
				// 新的如果不空，就创建出来，发起多线程加载
				if (!m_strImagesetName.empty() && ResourceGroupManager::Instance()->resourceExistsInAnyGroup(m_strImagesetName.c_str()))
				{
					m_pImageset = GUIImagesetManager::Instance()->createImageset(m_strImagesetName.c_str());

#ifdef MULTI_THREAD_LOAD_IMAGESET
					if (StreamThread::Instance() && StreamThread::Instance()->IsRunning())
					{
						// 是否开启了多线程模式
						if (m_pImageset && !m_pImageset->isPreparing())
						{
							m_pImageset->setPreparing(true);
							RenderImagePrepareEvent* pEvent = LordNew RenderImagePrepareEvent(m_pImageset);
							StreamThread::Instance()->AddRequest(pEvent);
						}
					}
					else
					{
						// 没开启，还是主线程加载
						// 主线程加载，判断一下是不是已经加载过了
						if (!m_pImageset->isLoaded())
						{
							prepare_io();
							prepare_image();
						}
					}
#else
					// 主线程加载，判断一下是不是已经加载过了
					if (!m_pImageset->isLoaded())
					{
						prepare_io();
						prepare_image();
					}
#endif
				}
			}
			else
			{
				// 贴图是相同的,image肯定不会相同才会走到这里
				checkImage();
			}

			m_strWholeImageName = strWholeName;
		}
	}

	void GUIRenderImage::DrawImage(const GUIWindow* window, const ERect& rtArea, unsigned int nColor, float fRotate,
								float fRotateY, float fRotateX, const ERect* rtClip)
	{
		if (!(m_pImageset && m_pImageset->isLoaded()))
		{
			return;
		}

		checkImage();
		
// 		if (m_pImage && m_pImageset && !m_pImageset->isLoaded())
// 		{
// 			__asm
// 			{
// 				int 3;
// 			}
// 		}
		// check buff

		// action动画处理
		if (m_pImage)
		{
			// 更新缓存的顶点
			m_actionVertex.clear();

			Color colorTemp;
			colorTemp.setABGR(nColor);
			ColorRect colorRect(colorTemp, colorTemp, colorTemp, colorTemp);

			m_pImage->DrawImage(m_actionVertex, rtArea, colorRect, fRotate, fRotateY, fRotateX, rtClip);
		}

		if (window)
		{
			drawCatheImage(window);
		}
	}

	void GUIRenderImage::DrawStretchImage(const GUIWindow* window, const ERect& rtArea, unsigned int nColor, StretchType stretchType, const StretchOffset& offset, float fRotate,
											float fRotateY, float fRotateX, const ERect* rtClip)
	{
		if (!(m_pImageset && m_pImageset->isLoaded()))
		{
			return;
		}

		checkImage();

		if (m_pImage)
		{
			m_actionVertex.clear();

			// action动画处理
			if (window)
			{
				if (stretchType == ST_TOP_BOTTOM)
				{
					m_pImage->DrawStretchImageTB(m_actionVertex, rtArea, nColor, fRotate, fRotateY, fRotateX, rtClip, offset);
				}
				else if (stretchType == ST_LEFT_RIGHT)
				{
					m_pImage->DrawStretchImageLR(m_actionVertex, rtArea, nColor, fRotate, fRotateY, fRotateX, rtClip, offset);
				}
				else if (stretchType == ST_NINE)
				{
					m_pImage->DrawStretchImage(m_actionVertex, rtArea, nColor, fRotate, fRotateY, fRotateX, rtClip, offset);
				}
				else
				{
				}
				drawCatheImage(window);
			}			
		}
	}

	LORD::Vector2 GUIRenderImage::GetImageSize()
	{
		Vector2 temp(Vector2::ZERO);
		if (m_pImage)
		{
			temp.x = m_pImage->GetWidth();
			temp.y = m_pImage->GetHeight();
		}

		return temp;
	}

	void GUIRenderImage::prepare_io()
	{
		if (m_pImageset)
		{
			m_pImageset->prepare();
		}
	}

	void GUIRenderImage::prepare_image()
	{
		if (m_pImageset)
		{
			m_pImageset->load();
			
			if (m_pImageset)
			{
				m_pImage = m_pImageset->GetImage(m_strImageName);
			}
		}
	}

	void GUIRenderImage::checkImage()
	{
		if (m_pImageset && !m_pImage && m_pImageset->isLoaded())
		{
			m_pImage = m_pImageset->GetImage(m_strImageName);
		}
	}

	void GUIRenderImage::drawCatheImage(const GUIWindow* window)
	{
		for (ImageBufferListType::iterator iter = m_actionVertex.begin(); iter != m_actionVertex.end(); ++iter)
		{
			if (iter->imageset)
			{
				// 要乘以变幻矩阵
				iter->imageset->DrawImage(iter->lt * window->getActionMatrix(), iter->rb * window->getActionMatrix(),
					iter->lb * window->getActionMatrix(), iter->rt * window->getActionMatrix(), iter->uv, iter->color, window->getMaterial(), window->getMaskTexture());
			}
		}
	}

	RenderImagePrepareEvent::RenderImagePrepareEvent(GUIImageset* imageset)
		: m_imageset(imageset)
	{

	}

	RenderImagePrepareEvent::~RenderImagePrepareEvent()
	{

	}

	bool RenderImagePrepareEvent::ProcessEvent()
	{
		if (m_imageset)
		{
			m_imageset->prepare();
		}

		return true;
	}

	bool RenderImagePrepareEvent::RespondEvent()
	{
		if (m_imageset)
		{
			m_imageset->load();
		}
		
		return true;
	}

	ImagesetReleaseEvent::ImagesetReleaseEvent(GUIImageset* imageset)
		: m_imageset(imageset)
	{

	}

	ImagesetReleaseEvent::~ImagesetReleaseEvent()
	{

	}

	bool ImagesetReleaseEvent::ProcessEvent()
	{
		return true;
	}

	bool ImagesetReleaseEvent::RespondEvent()
	{
		if (m_imageset)
		{
			GUIImagesetManager::Instance()->releaseResource(m_imageset);
			m_imageset = NULL;
		}
		return true;
	}
}
