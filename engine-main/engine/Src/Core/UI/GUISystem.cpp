#include "GUISystem.h"
#include "GUIRenderManager.h"
#include "GUIWindowManager.h"
#include "GUIImagesetManager.h"
#include "GUIFontManager.h"
#include "GUIHyperlinkSystem.h"
#include "GUIWindow.h"
#include "GUIEditBox.h"
#include "GUISlider.h"
#include "GUIAnimatMgr.h"
#include "Object/Root.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "GUIRenderImage.h"
#include "GUIRenderTargetManager.h"
#include "Render/RenderTarget.h"
#include "Touch.h"
#include "rapidjson/document.h"

using namespace rapidxml;

namespace LORD
{

const int nAutoSizeMinWidth = 960;
const int nAutoSizeMinHeight = 640;

const int nAutoSizeMaxWidth = 1280;
const int nAutoSizeMaxHeight = 800;

const int nDefaultWidth = 960;
const int nDefaultHeight = 540;

const String strGUIConfigFileName = "GUIResConfig.json";
const String strGUIAnimateFileName= "GUIAnimateConfig.json";

int giState = 0;

const float fEdge = 5.0f;

void GUIManualRender::render(bool uiRender)
{
#ifndef LORD_USE_RENDERTARGET
	Renderer::Instance()->getFrameBuffer()->clear(FrameBuffer::CM_DEPTH, Renderer::BGCOLOR, 1.0f, 0);
#endif
	m_pGUISystem->RenderGUI();
}

	//---------------------------------------------------------------------
	GUISystem* GUISystem::Instance()
	{
		return ms_pSingleton;
	}

	GUISystem::GUISystem()
		: m_pRenderManager(NULL)
		, m_pWindowManager(NULL)
		, m_pImagesetManager(NULL)
		, m_pFontManager(NULL)
		, m_pRootWindow(NULL)
		, m_pModalWindow(NULL)
		, m_pActiveEditBox(NULL)
		//, m_pActiveEditboxCallback(NULL)
		//, m_pGUIEventCallback(NULL)
		, m_pHyperlinkCallback(NULL)
		, m_pRenderHitTextCallback(NULL)
		, m_pAnimateMgr(NULL)
		, m_pScriptModule(NULL)
		, m_pRenderTargetManager(NULL)
		, m_nScreenWidth(960)
		, m_nScreenHeight(640)
		, m_nLogicWidth(960)
		, m_nLogicHeight(640)
		, m_cHyperLinkClickColor(Color::YELLOW)
		, m_bLongTouchDownFlag(false)
		, m_pFocusWindow(NULL)
		, m_pFocusRenderImage(NULL)
		, m_pClickWindow(NULL)
		, m_nClickTime(0)
		, m_pScrollWindow(NULL)
		, m_pCaptureWindow(NULL)
		, m_pDraggingWindow(NULL)
		, m_enableRender( true )
		, m_RenderStartIndex(0)
		, m_RenderEndIndex(0)
		, m_bAdaptSize(true)
		, m_standardCoordinate(Vector2(960, 540))
		, m_hitTestOffset(0.0f)
		, m_currentLanguage("en_US")
	{
		memset(m_pMotionWindow, 0, sizeof(GUIWindow*));

		m_GUIManualRender.m_pGUISystem = this;
	}

	GUISystem::~GUISystem()
	{
	}

	void GUISystem::Initialize( uint nWidth, uint nHeight, lua_State* ls )
	{
		m_pRenderManager = LordNew GUIRenderManager(nWidth, nHeight);
		m_pImagesetManager = LordNew GUIImagesetManager;
		m_pFontManager = LordNew GUIFontManager;

		m_pHyperlinkManager = LordNew GUIHyperlinkManager;
		m_pAnimateMgr   = LordNew GUIAnimatMgr;
		
// 		if (ls)
// 		{
// 			m_pScriptModule = LordNew LuaScriptModule(ls);
// 			m_pScriptModule->createBindings();
// 		}

		m_pRenderTargetManager = LordNew GUIRenderTargetManager;

		// 加载字体和imagset相关的资源
		LoadGUIResource();
		
		m_pWindowManager = LordNew GUIWindowManager;

		// 加载表情动画资源
		GUIAnimatMgr *pGUIAnimateMgr= GUIAnimatMgr::Instance();
		LordAssert(pGUIAnimateMgr);
		pGUIAnimateMgr->lordConfig(strGUIAnimateFileName);

		// render set
		SceneManager* pSceneManager = SceneManager::Instance();
		LordAssert(pSceneManager);
		RenderQueue* pUIRenderQueue = pSceneManager->getRenderQueue("UI");
		LordAssert(pUIRenderQueue);
		pUIRenderQueue->setManalRenderEnd(&m_GUIManualRender);

		m_RenderStartIndex = SceneManager::Instance()->getRenderQueueIndex("Background");
		m_RenderEndIndex = SceneManager::Instance()->getRenderQueueIndex("UI");

		// 为了做适配,需要OnSize
		OnSized(nWidth, nHeight);

		// 初始化focus资源
		m_pFocusRenderImage = LordNew GUIRenderImage;

#if(LORD_PLATFORM != LORD_PLATFORM_ANDROID)
#ifdef TOUCHLOG
		m_TouchLog = _createLog();
#endif
#endif
	}

#ifdef TOUCHLOG
	Log* GUISystem::_createLog()
	{
		Log::LogConfig logConfig;

		logConfig.logName = "TouchInfo";
		logConfig.logFilename = "TouchInfo.txt";

#if (LORD_COMPILER == LORD_COMPILER_MSVC)
		logConfig.bConsoleOutput = false;
		logConfig.bVSOutput = true;
#else
		logConfig.bConsoleOutput = false;
		logConfig.bVSOutput = false;
#endif

		logConfig.bFileOutput = true;
		logConfig.logLevel = Log::LL_NONE;
		logConfig.bTimeStamp = false;
		logConfig.bLogAppend = false;

		return LordNew Log(logConfig);;
	}

	void GUISystem::TouchMessage(const char* formats, ...)
	{
		if (m_TouchLog)
		{
			char szBuffer[512];
			va_list args;
			va_start(args, formats);
			vsprintf(szBuffer, formats, args);
			va_end(args);
			m_TouchLog->logMessage(Log::LL_NONE, szBuffer);
		}
	}

#endif

	void GUISystem::Destroy()
	{
		LordSafeDelete(m_pFocusRenderImage);
		//LordSafeDelete(m_pScriptModule);
		LordSafeDelete(m_pHyperlinkManager);
		LordSafeDelete(m_pWindowManager);
		LordSafeDelete(m_pFontManager);		
		LordSafeDelete(m_pRenderManager);
		LordSafeDelete(m_pAnimateMgr);
		LordSafeDelete(m_pImagesetManager);
		LordSafeDelete(m_pRenderTargetManager);
	}

	void GUISystem::UpdateGUI()
	{
		if (TouchManager::Instance())
		{
			TouchManager::Instance()->tick(Root::Instance()->getFrameTime());
		}
		
		if (m_pRootWindow)
		{
			m_pRootWindow->Update(Root::Instance()->getFrameTime());
		}
	}

	void GUISystem::RenderGUI()
	{
		GUIRenderManager::Instance()->clearQuadNumber();

		// 1. ----------------------处理逻辑----------------------------------
		if( !m_enableRender ) return;

		// 判断之前的点击事件是不是双击
		ui32 nElapsTime = Root::Instance()->getCurrentTime() - m_nClickTime;
		if (nElapsTime > 330 && m_pClickWindow && m_pClickWindow->IsMultiClicked())
		{
			if ( giState == 1 )
			{
				m_pClickWindow->TouchDown(m_touchDownPosition);
			}
			else
			{
				m_pClickWindow->TouchUp( m_touchDownPosition );				
			}					
			m_pClickWindow = NULL;
		}

		// clear hyperlink
		m_pHyperlinkManager->ClearAllHyperLink();

		
		// 2. ---------------------清理之前的渲染队列---------------------------------------------		
		// clear actor rende Queue before ui
		for (ui8 i = m_RenderStartIndex; i < m_RenderEndIndex; ++i)
		{
			RenderQueue* renderQueue = SceneManager::Instance()->getRenderQueueByIndex(i);
			if(renderQueue)
				renderQueue->beginRender();
		}
		
		// 3. ---------------------渲染开始---------------------------------------------

		//RenderTargetManager::Instance()->beginRenderTarget(RTI_DefaultBackBuffer, false, Renderer::BGCOLOR, false);
		GUIRenderManager::Instance()->SetDepth(0.0f);
		//GUIRenderManager::Instance()->BeginGUIViewport();
		GUIRenderManager::Instance()->BeginRender();

		// call render hit text call back
		if (GetRenderHitTextCallback())
		{
			GetRenderHitTextCallback()();
		}
		// gui window render
		if (m_pRootWindow)
		{
			//m_pRootWindow->Update(Root::Instance()->getFrameTime());
			m_pRootWindow->Render();

			// draw focus window
// 			if (m_pFocusWindow)
// 			{
// 				m_pFocusRenderImage->SetWholeImageName(m_strFocusImage);
// 				ERect rtArea = m_pFocusWindow->GetUnclippedOuterRect();
// 				m_pFocusRenderImage->DrawStretchImage(rtArea, 0xFFFFFFFF, 0.0f);
// 			}
		}
		GUIRenderManager::Instance()->EndRender();
		//GUIRenderManager::Instance()->EndGUIViewport();		
		//RenderTargetManager::Instance()->endRenderTarget(RTI_DefaultBackBuffer);
		
		// 4. ---------------------清理删除的window---------------------------------------------
		GUIWindowManager::Instance()->CleanDeadPool();

		// 清理贴图
		GUIImagesetManager::Instance()->handleImagesetRelease();
	}

	void GUISystem::OnSized( uint nWidth, uint nHeight )
	{
		m_nScreenWidth = nWidth;
		m_nScreenHeight = nHeight;
		
		if (m_bAdaptSize)
		{
			if (nHeight != 0 )
			{
				// 自动适配
				// 默认是960 × 540 的坐标系，其他的长宽比保证960相同，来计算另一个高度
				m_nLogicHeight = (uint)m_standardCoordinate.y;
				m_nLogicWidth = nWidth * m_nLogicHeight / nHeight;
			}
		} 
		else
		{
			m_nLogicWidth = nWidth;
			m_nLogicHeight = nHeight;
		}

		if (m_pRenderManager)
		{
			m_pRenderManager->OnSized(m_nLogicWidth, m_nLogicHeight);
		}

		if (m_pRootWindow)
		{
			m_pRootWindow->OnParentSized();
		}
	}

	void GUISystem::LoadGUIResource()
	{
		DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(strGUIConfigFileName);
		if (pDataStream == NULL)
		{
			LordLogError("%s not exist!", strGUIConfigFileName.c_str());
			return;
		}


		size_t nDataSize = pDataStream->size();
		char* pJSONData = (char*)LordMalloc(nDataSize + 1);
		pDataStream->read(pJSONData, nDataSize);
		pJSONData[nDataSize] = 0;
		
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(pJSONData);

		if(doc->HasParseError())
		{
			LordLogError("%s parse error", strGUIConfigFileName.c_str());
		}
		else
		{
			LordAssert(doc->HasMember("ResConfig"));
			const rapidjson::Value& resConfig = doc->FindMember("ResConfig")->value;
			bool bIsInitFont = false;

			const rapidjson::Value& Font = resConfig["Font"];
			rapidjson::Value::ConstValueIterator itFont = Font.Begin();
			for (; itFont != Font.End(); ++itFont)
			{
				GUIString strFontType = (*itFont)["Type"].GetString();
				GUIString strFontName = (*itFont)["Name"].GetString();
				GUIString strFileName = (*itFont)["Filename"].GetString();

				if (strFontType == "FreeType")
				{
					float fontSize = (*itFont)["Size"].GetFloat();
					StringArray strs = StringUtil::Split((*itFont)["Language"].GetString(), ",");
					for (int i = 0; i < int(strs.size()); ++i)
					{
						if (m_currentLanguage.compare(strs[i]) == 0) {
							strFontName = (*itFont)["Name"].GetString();
							strFileName = (*itFont)["Filename"].GetString();
							fontSize = (*itFont)["Size"].GetFloat();
							m_pFontManager->CreateFreeTypeFont(strFontName, fontSize, strFileName);
							bIsInitFont = true;
							break;
						}
					}
				}
				else
				{
					m_pFontManager->CreatePixmapFont(strFontName, strFileName);
					bIsInitFont = true;
				}

				//处理没有配置字体，初始化默认字体
				if (!bIsInitFont) {
					rapidjson::Value::ConstValueIterator itDefaultFont = Font.Begin();
					for (; itDefaultFont != Font.End(); ++itDefaultFont)
					{
						if (strFontType == "FreeType")
						{
							StringArray strs = StringUtil::Split((*itFont)["Language"].GetString(), ",");
							for (int i = 0; i < int(strs.size()); ++i)
							{
								if ((strs[i]) == "en_US") {
									m_pFontManager->CreateFreeTypeFont((*itFont)["Name"].GetString(), (*itFont)["Size"].GetFloat(), (*itFont)["Filename"].GetString());
									break;
								}
							}
						}
					}
				}
			}

			const rapidjson::Value& imageset = resConfig["Imageset"];
			rapidjson::Value::ConstValueIterator itImageset = imageset.Begin();
			for (; itImageset != imageset.End(); ++itImageset)
			{
				//使用时才创建
			}

		}
		delete doc;
		LordFree(pJSONData);
		LordDelete(pDataStream);

#if 0
		DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(strGUIConfigFileName);
		size_t nDataSize = pDataStream->size();
		char* pXMLData = (char*)LordMalloc(nDataSize+1);
		pDataStream->read(pXMLData, nDataSize);
		pXMLData[nDataSize] = 0;
		xml_document<>* doc = LordNewT(xml_document<>);
		doc->parse<0>(pXMLData);
		
		xml_node<>* pNode = doc->first_node();
		while(pNode)
		{
			GUIString strNodeName = pNode->name();
			if (strNodeName == "Font")
			{
				xml_attribute<>* pAttribute = pNode->first_attribute();
				GUIString strFontType = pAttribute->value();

				pAttribute = pAttribute->next_attribute();
				GUIString strFontName = pAttribute->value();

				pAttribute = pAttribute->next_attribute();
				GUIString strFileName = pAttribute->value();

				if (strFontType == "FreeType")
				{
					pAttribute = pAttribute->next_attribute();
					GUIString strFontSize = pAttribute->value();
					float fFontSize = StringUtil::ParseFloat(strFontSize.c_str());

					m_pFontManager->CreateFreeTypeFont(strFontName, fFontSize, strFileName);
				}
				else
				{
					// pixmap
					m_pFontManager->CreatePixmapFont(strFontName, strFileName);
				}
			}
			else if (strNodeName == "Imageset")
			{
				// 使用的时候才创建
				//xml_attribute<>* pAttribute = pNode->first_attribute();
				//GUIString strImagesetFileName = pAttribute->value();

				//m_pImagesetManager->CreateFromFile(strImagesetFileName);
			}
			else if (strNodeName == "UnderLineImage")
			{
				// 超链接下划线用到的资源
				xml_attribute<>* pAttribute = pNode->first_attribute();
				m_strUnderLineImage = pAttribute->value();
			}
			else if (strNodeName == "HyperLinkClickColor")
			{
				// 超链接点击颜色
				xml_attribute<>* pAttribute = pNode->first_attribute();
				String strColor = pAttribute->value();
				m_cHyperLinkClickColor = StringUtil::ParseColor(strColor);
			}
			else if (strNodeName == "FocusImage")
			{
				// 超链接下划线用到的资源
				xml_attribute<>* pAttribute = pNode->first_attribute();
				m_strFocusImage = pAttribute->value();
			}
			else if (strNodeName == "ScrollThumbImage")
			{
				// 滚动条的图片
				xml_attribute<>* pAttribute = pNode->first_attribute();
				m_strScrollThumbImage = pAttribute->value();
			}
			else
			{
			}
			pNode = pNode->next_sibling();
		}

		LordDeleteT(doc, xml_document<>);
		LordFree(pXMLData);
		LordSafeDelete(pDataStream);
#endif
	}

	void GUISystem::NotifyWindowDestroy( GUIWindow* pWindow )
	{
		if (m_pRootWindow == pWindow)
		{
			m_pRootWindow = NULL;
		}

		if (m_pModalWindow == pWindow)
		{
			m_pModalWindow = NULL;
		}

		if (m_pActiveEditBox == pWindow)
		{
			m_pActiveEditBox = NULL;
		}

		for (size_t i = 0; i < MAX_MOTION_TARGET_COUNT; ++i)
		{
			if (m_pMotionWindow[i] == pWindow)
			{
				m_pMotionWindow[i] = NULL;
			}
		}

		if (m_pFocusWindow == pWindow)
		{
			m_pFocusWindow = NULL;
		}

		if (m_pClickWindow == pWindow)
		{
			m_pClickWindow = NULL;
			giState = 1;
		}

		if (m_pScrollWindow == pWindow)
		{
			m_pScrollWindow = NULL;
		}

		if (m_pCaptureWindow == pWindow)
		{
			m_pCaptureWindow = NULL;
		}

		if (m_pDraggingWindow == pWindow)
		{
			m_pDraggingWindow = NULL;
		}
	}

	GUIWindow* GUISystem::GetTargetGUIWindow( const Vector2& point ) const
	{
		// if there is no GUI sheet visible, then there is nowhere to send input
		if (!m_pRootWindow || !m_pRootWindow->IsVisible())
			return NULL;

		GUIWindow* pTarget = m_pRootWindow->GetTargetChildAtPosition(point);

		if (!pTarget)
			pTarget = m_pRootWindow;


		// modal target overrules
		if (m_pModalWindow && pTarget != m_pModalWindow)
			if (!pTarget->IsAncestor(m_pModalWindow))
				pTarget = m_pModalWindow;

		if (pTarget == m_pRootWindow)
		{
			return NULL;
		}
		
		return pTarget;
	}

// 	bool GUISystem::TouchDown( size_t nIndex, const Vector2& position )
// 	{
// 		m_touchDownPosition = position;
// 
// 		// 清除长按的标志位
// 		m_bLongTouchDownFlag = false;
// 
// 		Vector2 outPosition;
// 		AdaptPosition(position, outPosition);
// 		m_lastPosition = outPosition;
// 
// 		// 没处理返回false
// 		if (nIndex >= MAX_MOTION_TARGET_COUNT)
// 		{
// 			return false;
// 		}
// 
// 		ui32 nCurrentTime = Root::Instance()->getCurrentTime();
// 
// 		GUIWindow* pTarget = GetTargetGUIWindow(outPosition);
// 
// #ifdef TOUCHLOG
// 		if (pTarget)
// 		{
// 			TouchMessage("%s:%s", pTarget->GetName().c_str(), StringUtil::ToString(outPosition).c_str());
// 		}
// #endif
// 
// 		// 处理超链接的点击
// 		if (m_pHyperlinkManager->OnClickHyperLink(outPosition, pTarget))
// 		{
// 			return true;
// 		}
// 
// 		m_pCaptureWindow = pTarget;
// 
// 		if (pTarget)
// 		{
// 			if (pTarget != m_pActiveEditBox)
// 			{
// 				SetActiveEditBox(NULL);
// 			}
// 
// 			m_pMotionWindow[nIndex] = pTarget;
// 
// 			if (pTarget->GetInScroll())
// 			{
// 				pTarget->GetInScroll()->TouchDown(outPosition);
// 
// 				m_pScrollWindow = pTarget->GetInScroll();
// 			}
// 
// 			// 判断是单击还是双击
// 			ui32 nElapsTime = nCurrentTime - m_nClickTime;
// 			// bool bHandle = false;
// 
// 			if (pTarget->IsMultiClicked())
// 			{
// 				if (nElapsTime < 330 && pTarget == m_pClickWindow)
// 				{
// 					//bHandle = pTarget->DoubleClick();
// 					pTarget->DoubleClick();
// 					m_pClickWindow = NULL;
// 					giState = 0;
// 				}
// 				else
// 				{
// 					m_pClickWindow = pTarget;
// 					m_nClickTime = nCurrentTime;
// 					// bHandle = true;
// 					giState = 1;
// 				}
// 			}
// 			else
// 			{
// 				// bHandle = pTarget->TouchDown(outPosition);
// 				pTarget->TouchDown(outPosition);
// 			}
// 
// 			return true;
// 		}
// 		else
// 		{
// 			SetActiveEditBox(NULL);
// 			return false;
// 		}
// 	}
// 
// 	bool GUISystem::TouchUp( size_t nIndex, const Vector2& position )
// 	{
// 		m_pHyperlinkManager->SetClickLink(false);
// 
// 		Vector2 outPosition;
// 		AdaptPosition(position, outPosition);
// 
// 		if (nIndex >= MAX_MOTION_TARGET_COUNT)
// 		{
// 			return false;
// 		}
// 
// 		GUIWindow* pTarget = GetTargetGUIWindow(outPosition);
// 
// 		if (m_pDraggingWindow)
// 		{
// 			MouseEventArgs args(m_pDraggingWindow);
// 			args.position = outPosition;
// 
// 			if (pTarget)
// 			{
// 				pTarget->TouchUp(outPosition);
// 			}
// 
// 			if (m_pMotionWindow[nIndex] && pTarget != m_pMotionWindow[nIndex])
// 			{
// 				m_pMotionWindow[nIndex]->MotionRelease();
// 
// 				m_pMotionWindow[nIndex] = NULL;
// 			}
// 
// 			m_pDraggingWindow->fireEvent(EventSystemDropDragging, args);
// 
// 			m_pDraggingWindow = NULL;
// 
// 			return true;
// 		}
// 
// 
// 		if (pTarget)
// 		{
// 			// up的时候 跟 down的时候不是一个window
// 			bool lbUpOk = true;
// 			if (m_pMotionWindow[nIndex] && pTarget != m_pMotionWindow[nIndex])
// 			{
// 				// 如果不是 那么就把 down的up掉？ NO! add MontionRelease Event.
// 				//m_pMotionWindow[nIndex]->TouchUp(outPosition);
// 				m_pMotionWindow[nIndex]->MotionRelease();
// 
// 				lbUpOk = false;
// 			}
// 
// 			m_pMotionWindow[nIndex] = NULL;
// 			
// 			//bool lbTouchUp = false;
// 			if (pTarget->GetInScroll() != pTarget)
// 			{
// 				if ( lbUpOk )
// 				{
// 					if ( pTarget->IsMultiClicked() )
// 					{
// 						ui32 nCurrentTime = Root::Instance()->getCurrentTime();
// 						ui32 nElapsTime = nCurrentTime - m_nClickTime;
// 						if ( nElapsTime >= 330 && giState != 0 )
// 						{
// 							//lbTouchUp = pTarget->TouchUp( outPosition );
// 							pTarget->TouchUp(outPosition);
// 						}
// 						else
// 						{
// 							giState = 2;
// 						}					
// 					}
// 					else
// 					{
// 						//lbTouchUp = pTarget->TouchUp(outPosition);
// 						pTarget->TouchUp(outPosition);
// 					}	
// 				}
// 			}
// 
// 			if (pTarget->GetInScroll())
// 			{
// 				pTarget->GetInScroll()->TouchUp(outPosition);
// 			}
// 
// 			return true;
// 		}
// 		else
// 		{
// 			if (m_pScrollWindow && m_pMotionWindow[nIndex] == m_pScrollWindow)
// 			{
// 				m_pScrollWindow->TouchUp(outPosition);
// 				m_pScrollWindow = NULL;
// 			}
// 
// 			if (m_pCaptureWindow)
// 			{
// 				m_pCaptureWindow->MotionRelease();
// 
// 				m_pCaptureWindow = NULL;
// 			}
// 
// 			if (m_pMotionWindow[nIndex])
// 			{
// 				m_pMotionWindow[nIndex]->MotionRelease();
// 				m_pMotionWindow[nIndex] = NULL;
// 			}
// 
// 			return false;
// 		}
// 	}
// 
// 	bool GUISystem::TouchMove(size_t nIndex, const Vector2& position )
// 	{
// // 		// 移到屏幕边缘的处理
// // 		if (position.x < fEdge ||
// // 			position.y < fEdge || 
// // 			position.x > (m_nScreenWidth - fEdge) ||
// // 			position.y > (m_nScreenHeight - fEdge ))
// // 		{
// // 			if (m_pCaptureWindow)
// // 			{
// // 				m_pCaptureWindow->MotionRelease();
// // 				m_pCaptureWindow = NULL;
// // 			}
// // 
// // 			m_pMotionWindow[nIndex] = NULL;
// // 		}
// 
// 		m_pClickWindow = NULL;
// 		giState = 0;
// 
// 		Vector2 outPosition;
// 		AdaptPosition(position, outPosition);
// 
// 		GUIWindow* pTarget = GetTargetGUIWindow(outPosition);
// 
// 		if (m_pDraggingWindow)
// 		{
// 			// 处理拖拽
// 			Vector2 moveDelta = outPosition - m_lastPosition;
// 
// 			UVector2 newPosition = m_pDraggingWindow->GetPosition() + UVector2(UDim(0, moveDelta.x), UDim(0, moveDelta.y));
// 			m_pDraggingWindow->SetPosition(newPosition);
// 
// 			m_lastPosition = outPosition;
// 
// 			if (pTarget)
// 			{
// 				pTarget->TouchMoveOver(outPosition);
// 			}
// 
// 			return true;
// 		}
// 
// 		Vector2 vOffset = position - m_touchDownPosition;
// 
// 		if ( Math::Abs(vOffset.x) >5 && !m_bLongTouchDownFlag && pTarget && pTarget == m_pCaptureWindow)
// 		{
// 			m_pCaptureWindow->OnTouchDrag(outPosition);
// 			m_bLongTouchDownFlag = true;
// 		}
// 
// 		if (vOffset.lenSqr() < 49)
// 		{
// // 			ui32 nElapseTime = Root::Instance()->getCurrentTime() - m_nClickTime;
// // 
// // 			if (nElapseTime > 300 && !m_bLongTouchDownFlag
// // 				&& pTarget && pTarget == m_pCaptureWindow )
// // 			{
// // 				m_pCaptureWindow->OnTouchDrag(outPosition);
// // 				m_bLongTouchDownFlag = true;
// // 			}
// 
// 			return true;
// 		}
// 
// 		if ( pTarget != NULL && pTarget->IsTouchSlide() )
// 		{
// 			pTarget->TouchSlide( outPosition );
// 		}
// 
// 		if (m_pMotionWindow[nIndex])
// 		{
// 			GUIWindow* pScrollWindow = m_pMotionWindow[nIndex]->GetInScroll();
// 			if (pScrollWindow)
// 			{
// 				if (pScrollWindow != m_pMotionWindow[nIndex])
// 				{
// 					m_pMotionWindow[nIndex]->MotionRelease();
// 					m_pMotionWindow[nIndex] = pScrollWindow;
// 				}
// 			}
// 
// 			m_pMotionWindow[nIndex]->TouchMove(outPosition);
// 			
// 			return true;
// 		}
// 		else
// 		{			
// 			return false;
// 		}
// 	}

	void GUISystem::SetActiveEditBox( GUIEditBox* pEditBox )
	{
		if (pEditBox != m_pActiveEditBox)
		{
			if (!pEditBox && m_pActiveEditBox)
			{
				m_pActiveEditBox->CloseKeyboard();
			}

			m_pActiveEditBox = pEditBox;
		}
	}

	void GUISystem::SetRootWindow( GUIWindow* pRoot )
	{
		m_pRootWindow = pRoot;
		if (pRoot)
		{
			pRoot->OnParentSized();
		}
	}

	void GUISystem::AdaptSize( uint nWidth, uint nHeight, uint& outWidth, uint& outHeight )
	{
		if (nWidth < nAutoSizeMinWidth || nHeight < nAutoSizeMinHeight)
		{
			float fWidthScale = nAutoSizeMinWidth * 1.0f / nWidth;
			float fHeightScale = nAutoSizeMinHeight * 1.0f / nHeight;
			if (fWidthScale > fHeightScale)
			{
				outWidth = (uint)(nWidth*fWidthScale);
				outHeight = (uint)(nHeight*fWidthScale);
			}
			else
			{
				outWidth = (uint)(nWidth*fHeightScale);
				outHeight = (uint)(nHeight*fHeightScale);
			}
		}
		else if (nWidth > nAutoSizeMaxWidth || nHeight > nAutoSizeMaxHeight)
		{
			float fWidthScale = nAutoSizeMaxWidth * 1.0f / nWidth;
			float fHeightScale = nAutoSizeMaxHeight * 1.0f / nHeight;
			if (fWidthScale < fHeightScale)
			{
				outWidth = (uint)(nWidth*fWidthScale);
				outHeight = (uint)(nHeight*fWidthScale);
			} 
			else
			{
				outWidth = (uint)(nWidth*fHeightScale);
				outHeight = (uint)(nHeight*fHeightScale);
			}
		}
		else
		{
			// 屏幕尺寸就是最终尺寸
			outWidth = nWidth;
			outHeight = nHeight;
		}
	}

	void GUISystem::AdaptPosition( const Vector2& position, Vector2& outPosition )
	{
		if (m_nScreenWidth == 0 ||
			m_nScreenHeight == 0)
		{
			return;
		}

		Vector2 temp = position;
		// 加入黑边的处理，所以屏幕坐标要重新算一下
		if (Root::Instance()->isFixedAspect())
		{
			int deltaHeight = (int)Root::Instance()->getRealHeight() - (int)m_nScreenHeight;
			temp.y = temp.y - (deltaHeight>0 ? deltaHeight / 2 : deltaHeight);
		}

		float xScale = temp.x / m_nScreenWidth;
		float yScale = temp.y / m_nScreenHeight;

		outPosition.x = xScale * m_nLogicWidth;
		outPosition.y = yScale * m_nLogicHeight;
	}

	void GUISystem::AdaptPositionEx(const Vector2& position, Vector2& outPosition)
	{
		if (m_nScreenWidth == 0 ||
			m_nScreenHeight == 0)
		{
			return;
		}

		Vector2 temp = position;

		float xScale = temp.x * 1.0f / m_nScreenWidth;
		float yScale = temp.y * 1.0f / m_nScreenHeight;

		outPosition.x = xScale * m_nLogicWidth;
		outPosition.y = yScale * m_nLogicHeight;
	}

	void GUISystem::LogicRectToScreenRect( const ERect& inRect, ERect& outRect )
	{
		Vector2 leftTop;
		Vector2 rightBottom;
		LogicPositinToScreenPosition(Vector2(inRect.left, inRect.top), leftTop);
		LogicPositinToScreenPosition(Vector2(inRect.right, inRect.bottom), rightBottom);
		outRect.left = leftTop.x;
		outRect.top = leftTop.y;
		outRect.right = rightBottom.x;
		outRect.bottom = rightBottom.y;
	}

	void GUISystem::LogicPositinToScreenPosition( const Vector2 position, Vector2& outPosition )
	{
		if (m_nLogicWidth == 0 ||
			m_nLogicHeight == 0)
		{
			return;
		}

		float xScale = position.x * 1.0f / m_nLogicWidth;
		float yScale = position.y * 1.0f / m_nLogicHeight;

		outPosition.x = xScale * m_nScreenWidth;
		outPosition.y = yScale * m_nScreenHeight;

		// 加入黑边的处理，所以屏幕坐标要重新算一下
		if (Root::Instance()->isFixedAspect())
		{
			outPosition.y = outPosition.y + (Root::Instance()->getRealHeight() - m_nScreenHeight) / 2;
		}
	}

// 	void GUISystem::MoveFocusForward()
// 	{
// 		if (m_pFocusWindow == NULL)
// 		{
// 			m_pFocusWindow = m_pRootWindow;
// 		}
// 		if (m_pFocusWindow == NULL)
// 		{
// 			return;
// 		}
// 
// 		GUIWindow* pParent = m_pFocusWindow->GetParent();
// 		if (pParent)
// 		{
// 			size_t nCount = pParent->GetChildCount();
// 			size_t nFocusIndex = 0;
// 			for (size_t i = 0; i < nCount; ++i)
// 			{
// 				GUIWindow* pWindow = pParent->GetChildByIndexInDrawlist(i);
// 				if (pWindow == m_pFocusWindow)
// 				{
// 
// 					nFocusIndex = i;
// 					break;
// 				}
// 			}
// 
// 			if (nFocusIndex == nCount-1)
// 			{
// 				m_pFocusWindow = pParent->GetChildByIndexInDrawlist(0);
// 			}
// 			else
// 			{
// 				m_pFocusWindow = pParent->GetChildByIndexInDrawlist(nFocusIndex+1);
// 			}
// 		}
// 	}
// 
// 	void GUISystem::MoveFocusBackward()
// 	{
// 		if (m_pFocusWindow == NULL)
// 		{
// 			m_pFocusWindow = m_pRootWindow;
// 		}
// 		if (m_pFocusWindow == NULL)
// 		{
// 			return;
// 		}
// 
// 		GUIWindow* pParent = m_pFocusWindow->GetParent();
// 		if (pParent)
// 		{
// 			size_t nCount = pParent->GetChildCount();
// 			size_t nFocusIndex = 0;
// 			for (size_t i = 0; i < nCount; ++i)
// 			{
// 				GUIWindow* pWindow = pParent->GetChildByIndexInDrawlist(i);
// 				if (pWindow == m_pFocusWindow)
// 				{
// 					nFocusIndex = i;
// 					break;
// 				}
// 			}
// 
// 			if (nFocusIndex == 0)
// 			{
// 				m_pFocusWindow = pParent->GetChildByIndexInDrawlist(nCount-1);
// 			}
// 			else
// 			{
// 				m_pFocusWindow = pParent->GetChildByIndexInDrawlist(nFocusIndex-1);
// 			}
// 		}
// 	}
// 
// 	void GUISystem::EnterFocus()
// 	{
// 		if (m_pFocusWindow == NULL)
// 		{
// 			m_pFocusWindow = m_pRootWindow;
// 		}
// 		if (m_pFocusWindow == NULL)
// 		{
// 			return;
// 		}
// 
// 		size_t nCount = m_pFocusWindow->GetChildCount();
// 		if (nCount > 0)
// 		{
// 			m_pFocusWindow = m_pFocusWindow->GetChildByIndexInDrawlist(0);
// 		}
// 	}
// 
// 	void GUISystem::BackFocus()
// 	{
// 		if (m_pFocusWindow == NULL)
// 		{
// 			m_pFocusWindow = m_pRootWindow;
// 		}
// 		if (m_pFocusWindow == NULL)
// 		{
// 			return;
// 		}
// 
// 		GUIWindow* pParent = m_pFocusWindow->GetParent();
// 		if (pParent)
// 		{
// 			m_pFocusWindow = pParent;
// 		}
// 	}
// 
// 	void GUISystem::FocusDown()
// 	{
// 		if (m_pFocusWindow == NULL)
// 		{
// 			m_pFocusWindow = m_pRootWindow;
// 		}
// 		if (m_pFocusWindow == NULL)
// 		{
// 			return;
// 		}
// 
// 		ERect rtOuter = m_pFocusWindow->GetUnclippedOuterRect();
// 
// 		if (m_pFocusWindow->GetInScroll())
// 		{
// 			m_pFocusWindow->GetInScroll()->TouchDown(Vector2(rtOuter.left+1, rtOuter.top+1));
// 		}
// 
// 		m_pFocusWindow->TouchDown(Vector2(rtOuter.left+1, rtOuter.top+1));
// 	}
// 
// 	void GUISystem::FocusUp()
// 	{
// 		if (m_pFocusWindow == NULL)
// 		{
// 			m_pFocusWindow = m_pRootWindow;
// 		}
// 		if (m_pFocusWindow == NULL)
// 		{
// 			return;
// 		}
// 
// 		ERect rtOuter = m_pFocusWindow->GetUnclippedOuterRect();
// 		if (m_pFocusWindow->GetInScroll())
// 		{
// 			m_pFocusWindow->GetInScroll()->TouchUp(Vector2(rtOuter.left+1, rtOuter.top+1));
// 		}
// 		m_pFocusWindow->TouchUp(Vector2(rtOuter.left+1, rtOuter.top+1));
// 	}

	LORD::Vector2 GUISystem::WorldPostionToScreen(const Vector3& worldpos)
	{
		Vector3 screenpos(Vector3::ZERO);

		Renderer::Instance()->project(screenpos, worldpos, SceneManager::Instance()->getMainCamera()->getViewProjMatrix());

		Vector2 logicpos(Vector2::ZERO);
		GUISystem::Instance()->AdaptPosition(Vector2(screenpos.x, screenpos.y), logicpos);
		return logicpos;
	}

	void GUISystem::set2DBillBoardPosition(GUIWindow* window, const Vector3& worldPosition, const Real width, const Real height)
	{
		if (window && SceneManager::Instance() && SceneManager::Instance()->getMainCamera())
		{
			Camera* camera = SceneManager::Instance()->getMainCamera();
			Vector3 cameraPosition = camera->getPosition();
			Quaternion cameraOritation = camera->getOritation();

			// 计算摄像机在billboard坐标系下的位置
			cameraPosition = cameraOritation*(cameraPosition - worldPosition);

			// 计算摄像机朝向
			// 默认billboard在0 0 0点
			Vector3 look = (Vector3::ZERO-cameraPosition).normalizedCopy();
			Vector3 right = cameraOritation * Vector3::UNIT_X;
			Vector3 up = cameraOritation * Vector3::UNIT_Y;

			// 四个点的偏移量
			Vector3 leftOffset = -right * 0.5f * width;
			Vector3 rightOffset = right * 0.5f * width;
			Vector3 topOffset = up * 0.5f * height;
			Vector3 bottomOffset = -up * 0.5f * height;

			// 计算billboard左上，和右下点坐标本地坐标
			Vector3 leftTop = leftOffset + topOffset;
			Vector3 rightBottom = rightOffset + bottomOffset;
			Matrix4 worldMat = Matrix4::IDENTITY;
			worldMat.transform(worldPosition);
			leftTop = leftTop * worldMat;
			rightBottom = rightBottom * worldMat;

			// 计算左上，右下点投影
			Vector3 screenLeftTop = Vector3::ZERO;
			Vector3 screenRightBottom = Vector3::ZERO;
			Renderer::Instance()->project(screenLeftTop, leftTop, SceneManager::Instance()->getMainCamera()->getViewProjMatrix());
			Renderer::Instance()->project(screenRightBottom, rightBottom, SceneManager::Instance()->getMainCamera()->getViewProjMatrix());

			// 计算中心点投影
			Vector2 screenCenter = WorldPostionToScreen(worldPosition);

			// 计算ui位置
			float originalWidth = window->GetWidth().asAbsolute((float)GUIRenderManager::Instance()->GetWidth());
			float originalHeight = window->GetHeight().asAbsolute((float)GUIRenderManager::Instance()->GetHeight());
			window->SetPosition(UVector2(UDim(0, PixelAligned(screenCenter.x - originalWidth * 0.5f)), UDim(0, PixelAligned(screenCenter.y - originalHeight * 0.5f))));

			Vector3 scale = Vector3::ONE;
			scale.x = PixelAligned(screenRightBottom.x - screenLeftTop.x) / originalWidth;
			scale.y = PixelAligned(screenRightBottom.y - screenLeftTop.y) / originalHeight;

			window->SetActionMatrixScale(scale);
			
			//window->SetArea(URect(UVector2(UDim(0, screenLeftTop.x), UDim(0, screenLeftTop.y)), UVector2(UDim(0, screenRightBottom.x), UDim(0, screenRightBottom.y))));
		}
	}

	void GUISystem::handleTouchDown(const Touch* touch)
	{
		GUIWindow* window = GUIWindowManager::Instance()->GetGUIWindow(touch->getWindowName());
		if (window == NULL)
		{
			return;
		}

		// 处理超链接的点击
		if (m_pHyperlinkManager->OnClickHyperLink(touch->getTouchPoint(), window))
		{
			return;
		}

		if (window != m_pActiveEditBox)
		{
			SetActiveEditBox(NULL);
		}

		if (window->GetInScroll())
		{
			window->GetInScroll()->TouchDown(touch->getTouchPoint());
		}

		window->TouchDown(touch->getTouchPoint());
	}

	void GUISystem::handleTouchMove(Touch* touch)
	{
		GUIWindow* window = GUIWindowManager::Instance()->GetGUIWindow(touch->getWindowName());
		if (window == NULL)
		{
			return;
		}

		// 如果在dragging的就只处理dragging
		if (window->isEnableDrag())
		{
			window->onDragging(touch);
			return;
		}

		GUIWindow* scroll = window->GetInScroll();
		GUIWindow* nowWindow = GetTargetGUIWindow(touch->getTouchPoint());
		if (nowWindow != window)
		{
			window->MotionRelease();
			return;
		}

		if (scroll)
		{
			scroll->TouchMove(touch->getTouchPoint());
		}
		else
		{
			window->TouchMove(touch->getTouchPoint());
		}
		return;
	}

	void GUISystem::handleTouchUp(const Touch* touch)
	{
		GUIWindow* window = GUIWindowManager::Instance()->GetGUIWindow(touch->getWindowName());
		if (window == NULL)
		{
			return;
		}
		
		m_pHyperlinkManager->SetClickLink(false);

		// 处理scroll
		GUIWindow* scroll = window->GetInScroll();
		if (scroll)
		{
			//LordLogDebug("up in scroll");
			scroll->TouchUp(touch->getTouchPoint());

			// 如果位置变了，说明滚动了，那么后面的事件都不处理了
			Vector2 offset = touch->getTouchPoint() - touch->getStartPoint();
			if (offset.len() > 20.0f)
			{
				//LordLogDebug("up in scroll return");
				 window->MotionRelease();
				return;
			}
		}

		GUIWindow* nowWindow = GetTargetGUIWindow(touch->getTouchPoint());

		if (nowWindow != window)
		{
			//LordLogDebug("up window MotionRelease [%s]", window->GetName().c_str());
			window->MotionRelease();
			if (nowWindow)
			{
				//LordLogDebug("up nowWindow TouchUp [%s]", nowWindow->GetName().c_str());
				nowWindow->TouchUp(touch->getTouchPoint());
			}
		}
		else
		{
			/*if (window == GetTargetGUIWindow(touch->getStartPoint()))
			{
				if (touch->getLongTouchFlag())
				{
					window->MotionRelease();
					window->onLongTouchEnd(touch);
				}
				else
				{
					window->TouchUp(touch->getTouchPoint());
				}
			}
			else
			{
				window->MotionRelease();
			}*/
			if (touch->getLongTouchFlag())
			{
				window->MotionRelease();
				window->onLongTouchEnd(touch);
			}
			else
			{
				window->TouchUp(touch->getTouchPoint());
			}
		}

		// 最后处理，因为需要在up的时候做释放的逻辑
		if (window->isEnableDrag())
		{
			window->onDragEnd(touch);
		}
	}

	void GUISystem::handleTouchCancel(const Touch* touch)
	{
		GUIWindow* window = GUIWindowManager::Instance()->GetGUIWindow(touch->getWindowName());
		if (window == NULL)
		{
			return;
		}

		m_pHyperlinkManager->SetClickLink(false);

		// 处理scroll
		GUIWindow* scroll = window->GetInScroll();
		if (scroll)
		{
			scroll->TouchUp(touch->getTouchPoint());
		}

		window->MotionRelease();

		// 最后处理，因为需要在up的时候做释放的逻辑
		if (window->isEnableDrag())
		{
			window->onDragEnd(touch);
		}
	}

}
