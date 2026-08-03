#include "GUIRenderHexagon.h"
#include "GUIRenderImage.h"
#include "GUIImagesetManager.h"
#include "GUIImageset.h"
#include "GUIImage.h"
#include "GUIWindow.h"
#include "GUICoordConverter.h"

namespace LORD
{
	GUIRenderHexagon::GUIRenderHexagon()
		: m_pImagesetBlock(NULL)
		, m_pImageTop(NULL)
		, m_pImageSide(NULL)
		, m_pImageOverlayer(NULL)
		, m_bActive(false)
		, m_colorTop(Color::WHITE)
		, m_colorSide(Color::WHITE)
		, m_colorOverlayer(Color::WHITE)
	{
	}

	GUIRenderHexagon::~GUIRenderHexagon()
	{
		if (m_pImagesetBlock)
		{
			GUIImagesetManager::Instance()->pushbackReleaseImageset(m_pImagesetBlock);
			m_pImagesetBlock = NULL;
			m_pImageSide = NULL;
			m_pImageOverlayer = NULL;
		}

		m_actionVertex.clear();
	}

	void GUIRenderHexagon::SetImageOverlayerName(const GUIString& strName)
	{
		if (strName == m_imageOverlayerWholeName)
			return;

		m_pImageOverlayer = NULL;

		GUIString strNewSetName;
		GUIImagesetManager::Instance()->ParseImageString(strName, strNewSetName, m_strImageOverlayerName);
		if (strNewSetName != m_strImagesetName)
		{
			LordLogWarning("Hexagon must set the same imageset");
			return;
		}
		m_imageOverlayerWholeName = strName;
		checkImage();
	}

	void GUIRenderHexagon::SetImageSideName(const GUIString& strName)
	{
		if (strName == m_imageSideWholeName)
			return;

		m_pImageSide = NULL;

		GUIString strNewSetName;
		GUIImagesetManager::Instance()->ParseImageString(strName, strNewSetName, m_strImageSideName);

		if (strNewSetName != m_strImagesetName)
		{
			LordLogWarning("Hexagon must set the same imageset");
			return; 
		}
		m_imageSideWholeName = strName;
		checkImage();
	}

	void GUIRenderHexagon::SetImageTopName(const GUIString& strName)
	{
		if (strName == m_imageTopWholeName)
			return;
		
		m_pImageTop = NULL;

		GUIString strNewSetName;
		GUIImagesetManager::Instance()->ParseImageString(strName, strNewSetName, m_strImageTopName);

		if (strNewSetName != m_strImagesetName)
		{
			m_strImageSideName.clear();
			m_imageSideWholeName.clear();
			m_strImageSideName.clear();
			m_imageOverlayerWholeName.clear();
			m_strImageOverlayerName.clear();

			// 如果跟老的贴图不一样，就释放老的
			if (m_pImagesetBlock)
			{
				// 加入一个队列缓存起来，保证加载线程加载完再释放
				GUIImagesetManager::Instance()->pushbackReleaseImageset(m_pImagesetBlock);
				m_pImagesetBlock = NULL;
			}

			m_strImagesetName = strNewSetName;

			// 新的如果不空，就创建出来，发起多线程加载
			if (!m_strImagesetName.empty() && ResourceGroupManager::Instance()->resourceExistsInAnyGroup(m_strImagesetName.c_str()))
			{
				m_pImagesetBlock = GUIImagesetManager::Instance()->createImageset(m_strImagesetName.c_str());

#ifdef MULTI_THREAD_LOAD_IMAGESET
				if (StreamThread::Instance() && StreamThread::Instance()->IsRunning())
				{
					// 是否开启了多线程模式
					if (m_pImagesetBlock && !m_pImagesetBlock->isPreparing())
					{
						m_pImagesetBlock->setPreparing(true);
						RenderImagePrepareEvent* pEvent = LordNew RenderImagePrepareEvent(m_pImagesetBlock);
						StreamThread::Instance()->AddRequest(pEvent);
					}
				}
				else
				{
					// 没开启，还是主线程加载
					// 主线程加载，判断一下是不是已经加载过了
					if (!m_pImagesetBlock->isLoaded())
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

		m_imageTopWholeName = strName;
		m_bActive = m_imageTopWholeName != "";
	}

	void GUIRenderHexagon::SetVertexData(ImageBufferListType &vertex)
	{
		m_blockVertex.clear();
		for (ImageBufferListType::iterator it = vertex.begin(); it != vertex.end(); ++it)
		{
			//it->imageset = m_pImageset;				
			m_blockVertex.push_back(*it);
		}
	}

	void GUIRenderHexagon::DrawHexagon(const GUIWindow* window, const ERect& rtArea, const ERect* rtClip)
	{
		if (!(m_pImagesetBlock && m_pImagesetBlock->isLoaded()))
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
		if (m_pImageTop)
		{
			// 更新缓存的顶点
			m_actionVertex.clear();

			m_pImageTop->DrawHexagon(m_actionVertex, m_pImageSide, m_pImageOverlayer, rtArea, m_colorTop, m_colorSide, m_colorOverlayer, m_height, rtClip);
		}

		if (window)
		{
			drawCacheHexagon(window);
		}
	}

	void GUIRenderHexagon::DrawHexagon2(GUIWindow *window, const ERect& rtArea, const ERect& rtClip)
	{
		m_actionVertex.clear();
		
		for (ImageBufferListType::iterator it = m_blockVertex.begin(); it != m_blockVertex.end(); ++it)
		{
			m_actionVertex.push_back(*it);
		}
		
		//将局部坐标转换成屏幕坐标
		for (ImageBufferListType::iterator it = m_actionVertex.begin(); it != m_actionVertex.end(); ++it)
		{
			it->lt.x = CoordConverter::windowToScreenX(*window, it->lt.x);
			it->lt.y = CoordConverter::windowToScreenY(*window, it->lt.y);

			it->lb.x = CoordConverter::windowToScreenX(*window, it->lb.x);
			it->lb.y = CoordConverter::windowToScreenY(*window, it->lb.y);

			it->rb.x = CoordConverter::windowToScreenX(*window, it->rb.x);
			it->rb.y = CoordConverter::windowToScreenY(*window, it->rb.y);

			it->rt.x = CoordConverter::windowToScreenX(*window, it->rt.x);
			it->rt.y = CoordConverter::windowToScreenY(*window, it->rt.y);
		}

		//clip
		/*for (ImageBufferListType::iterator it = m_actionVertex.begin(); it != m_actionVertex.end();)
		{
			if (it->lt.y < rtClip.top || it->rt.y < rtClip.top || it->lb.y > rtClip.bottom || it->rb.y > rtClip.bottom)
			{
				it = m_actionVertex.erase(it);
			}
			else
			{
				++it;
			}
		}*/

		drawCacheHexagon(window);
	}

	LORD::Vector2 GUIRenderHexagon::GetImageSize()
	{
		Vector2 temp(Vector2::ZERO);
		if (m_pImageTop)
		{
			temp.x = m_pImageTop->GetWidth();
			temp.y = m_pImageTop->GetHeight();
		}

		return temp;
	}

	void GUIRenderHexagon::prepare_io()
	{
		if (m_pImagesetBlock)
		{
			m_pImagesetBlock->prepare();
		}
	}

	void GUIRenderHexagon::prepare_image()
	{
		if (m_pImagesetBlock)
		{
			m_pImagesetBlock->load();

			if (m_pImagesetBlock)
			{
				m_pImageTop = m_pImagesetBlock->GetImage(m_strImageTopName);
			}
		}
	}

	void GUIRenderHexagon::checkImage()
	{
		if (m_pImagesetBlock && m_pImagesetBlock->isLoaded())
		{
			if (!m_pImageTop && !m_strImageTopName.empty())
				m_pImageTop = m_pImagesetBlock->GetImage(m_strImageTopName);
			if (!m_pImageSide && !m_strImageSideName.empty())
				m_pImageSide = m_pImagesetBlock->GetImage(m_strImageSideName);
			if (!m_pImageOverlayer && !m_strImageOverlayerName.empty())
				m_pImageOverlayer = m_pImagesetBlock->GetImage(m_strImageOverlayerName);
		}
	}

	void GUIRenderHexagon::drawCacheHexagon(const GUIWindow* window)
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
}
