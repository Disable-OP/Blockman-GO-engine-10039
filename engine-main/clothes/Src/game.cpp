#include "game.h"
#include "ShellInterface.h"
#include "backgroundRenderable.h"

#include "Core.h"
#include "Util/QueryObject.h"
#include "Object/Root.h"
#include "GLES2Render.h"
#include "UI/GUISystem.h"

#include "Memory/MemTracker.h"

#include "Render/Material.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Resource/ResourcePack.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIWindow.h"
#include "UI/Touch.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIRenderText.h"
#include "UI/GUIRenderManager.h"
#include "ShellInterface.h"

#if (LORD_PLATFORM != LORD_PLATFORM_WINDOWS)
#include <unistd.h>
#endif

using namespace LORD;
unsigned int g_framedt = 0;

namespace ClothesPlug
{
	CGame* CGame::Instance()
	{
		return ms_pSingleton;
	}

	CGame::CGame()
		: m_bInited(false)
		, m_enterBackFlag(false)
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		, m_fps(1000)
#else
		, m_fps(40)
#endif
		, m_shellInterface(NULL)
		, m_Root(NULL)
		, m_tickCount(0)
		, m_yaw(0.f)
		, m_rotating(false)
		, m_lastX(0)
		, m_isBoy(false)
	{
		maxFrameTime = 1000 / m_fps;
		m_shiftIndex = 0;
		m_chooseIndex[0] = 1;
		m_chooseIndex[1] = 1;
		m_chooseIndex[2] = 1;
		m_chooseIndex[3] = 1;
		m_isBoy = false;
	}

	CGame::~CGame()
	{
#ifdef LORD_MEMORY_TRACKER
		LORD::MemTracker::get().reportLeaks();
#endif
#ifdef TRACK_EVENT_SUBSCRIPTION
		BLOCKMAN::EventManager::reportLeaks();
#endif
	}


	void CGame::onError(int errorCode)
	{
		if (m_shellInterface)
		{
			m_shellInterface->onError(errorCode);
		}
	}

	bool CGame::exitGame()
	{
		if (m_shellInterface)
		{
			m_shellInterface->gameExit();
		}

		return true;
	}

	void CGame::initEngine(const char* strRootPath, const char* strConfigPath, int w, int h, unsigned int wnd, int sex)
	{
		m_width = w;
		m_height = h;
		m_hWnd = wnd;
		m_isBoy = sex == 1;

		m_Root = LordNew LORD::Root;
		m_Root->setWriteablePath(strConfigPath);
		LORD::Root::RootCfg rootcfg;
		rootcfg.rootPath = strRootPath;
		rootcfg.resCfgFile = rootcfg.rootPath + "resource.cfg";
		m_Root->initialize(rootcfg);
		m_Root->enableCalcThread(false);

		LORD::Renderer::RenderCfg renderCfg;
		renderCfg.screenWidth = m_width;
		renderCfg.screenHeight = m_height;
		renderCfg.windowHandle = (unsigned int)m_hWnd;
		m_Root->initRenderer(LordNew LORD::GLES2Renderer, renderCfg);
		m_Root->onSize(w, h);

		LordNew QueryObjectManager;
		QueryObjectManager::Instance()->Initialize();

		m_MainCamera = SceneManager::Instance()->getMainCamera();
		LordAssert(m_MainCamera);
		m_actorNode = SceneManager::Instance()->getRootNode()->createChild();
		LordAssert(m_actorNode);
		m_mainActor = ActorManager::Instance()->CreateActor(m_isBoy ? "boy.actor" : "girl.actor", "idle", 0, false, false, true);
		LordAssert(m_mainActor);
		m_mainActor->AttachToParentNode(m_actorNode);

//        m_actorNode->setLocalPosition(Vector3(-0.6f, 0.f, -0.7f));
        
		m_MainCamera->setPosition(Vector3(0.f, 1.1f, 3.f));
		m_MainCamera->setTarget(Vector3(0.f, 1.f, 0.f));

		m_actorPosition = Vector3(0.f, 0.f, 0.f);
		m_mainActor->SetPosition(m_actorPosition);
		m_mainActor->SetBrightness(Color::WHITE);

		mpBackgroundImage = LordNew LORD::BackGroundRenderable("decorate_default_bg.png");
	}

	void CGame::DestroyGame()
	{
		LordDelete mpBackgroundImage;
		m_Root->destroy();
		LordDelete m_Root;
	}

	void CGame::MainTick()
	{
		m_tickCount++;
		try
		{
			static LORD::ui32 lastTime = m_Root->getCurrentTime();
			LORD::ui32 curTime = m_Root->getCurrentTime();
			g_framedt = int(LORD::Math::Min(curTime - lastTime, (LORD::ui32)100) /** ActorManager::Instance()->GetSpeedUp()*/);
			lastTime = curTime;

			RenderTick();

			LORD::ui32 endTime = LORD::Root::Instance()->getCurrentTime();
			g_framedt = endTime - curTime;
			if (g_framedt < maxFrameTime)
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				/*Sleep(maxFrameTime - g_framedt);*/
				Sleep(maxFrameTime - g_framedt);
#else
				usleep((maxFrameTime - g_framedt) * 1000);
#endif
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
			//LuaEngine::Instance()->exceptionHandle(e);
		}
	}

	void CGame::RenderTick()
	{
		if (m_enterBackFlag) {
			return;
		}

		m_Root->resetFrameState();
		// update
		m_Root->frameMove(g_framedt);

		if (mpBackgroundImage)
		{
			mpBackgroundImage->update();
			mpBackgroundImage->render();
		}

		m_mainActor->SetOrientation(Quaternion(Vector3::UNIT_Y, m_yaw));

		// render
		Renderer::Instance()->renderScene();

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		{
			// use one batch to render all the debug strings.
			vector<FontBuffer>::type buffer;

			GUIFont* pFont = GUIFontManager::Instance()->GetFont("HT28");
			char buff[256];
			uint len;
			StringUtil::GBKToUTF8(buff, len, "鼠标左键拖拽可以旋转角色。", 64); buff[len] = 0;
			Renderer::Instance()->renderText(buff, Vector2(10, 20), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);
			StringUtil::GBKToUTF8(buff, len, "数字键1,2,3,4,5,6可以切换着装效果", 64); buff[len] = 0;
			Renderer::Instance()->renderText(buff, Vector2(10, 40), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);
			StringUtil::GBKToUTF8(buff, len, "Tab键可以改变时装,盔甲,脸型,发型模式", 64); buff[len] = 0;
			Renderer::Instance()->renderText(buff, Vector2(10, 60), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);
			StringUtil::GBKToUTF8(buff, len, "左Shift按键可以切换男女角色", 64); buff[len] = 0;
			Renderer::Instance()->renderText(buff, Vector2(10, 80), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			String statistics;
			String armorNamePrefix[6] = { "布甲", "皮甲", "锁甲", "铁甲", "钻石甲", "金甲" };
			String shiftPrefix[4] = { "时装", "盔甲", "脸型", "发型" };
			if (m_shiftIndex == 1)
				statistics = StringUtil::Format("M%s 当前盔甲套装: %s. 当前脸型: %d", shiftPrefix[m_shiftIndex].c_str(), armorNamePrefix[m_chooseIndex[1]].c_str(), m_chooseIndex[2]);
			else
				statistics = StringUtil::Format("M%s 当前时装套装: %d. 当前脸型: %d. 当前发型: %d.", shiftPrefix[m_shiftIndex].c_str(), m_chooseIndex[0], m_chooseIndex[2], m_chooseIndex[3]);
			StringUtil::GBKToUTF8(buff, len, statistics.c_str(), statistics.length()); buff[len] = 0;
			Renderer::Instance()->renderText(buff, Vector2(10.f, m_height - 30.f), pFont, Color(1.f, 0.6f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			GUIRenderText::DrawTextBuffer(pFont, buffer);
		}
#endif

		Renderer::Instance()->present();
	}

	void CGame::refreshActor()
	{
		m_mainActor->EnableAllBodyParts(false);
		// 脸型，发型选择模型下，默认使用时装外观。
		int faceid = m_chooseIndex[2];
		int hairid = m_chooseIndex[3];
		int chothesid = m_shiftIndex == 1 ? m_chooseIndex[1] : m_chooseIndex[0];

		faceid = Math::Clamp(faceid, 1, 6);
		hairid = Math::Clamp(hairid, 1, 6);
		chothesid = Math::Clamp(chothesid, 1, 5);

		// 脸总是需要渲染的
		m_mainActor->AttachSlave("custom_face", StringUtil::ToString(faceid));
		if (m_shiftIndex == 1) //如果是盔甲预览模式，需要渲染头盔，鞋子，胸甲，护腿
		{
			String armorNamePrefix[6] = { "clothes", "leather", "chain", "iron", "diamond", "gold" };
			m_mainActor->AttachSlave("armor_head", armorNamePrefix[chothesid]);		// 头盔
			m_mainActor->AttachSlave("armor_chest", armorNamePrefix[chothesid]);	// 胸甲
			m_mainActor->AttachSlave("armor_thigh", armorNamePrefix[chothesid]);	// 护腿
			m_mainActor->AttachSlave("armor_foot", armorNamePrefix[chothesid]);		// 鞋子
		}
		else // 时装模式
		{
			// 时装模式下需要渲染发型
			m_mainActor->AttachSlave("custom_hair", StringUtil::ToString(hairid));
			m_mainActor->AttachSlave("clothes_tops", StringUtil::ToString(chothesid));
			m_mainActor->AttachSlave("clothes_pants", StringUtil::ToString(chothesid));
			m_mainActor->AttachSlave("custom_shoes", StringUtil::ToString(chothesid));
			map<LORD::String, LORD::String>::iterator  iter;
			for (iter = m_actorPartsMap.begin(); iter != m_actorPartsMap.end(); iter++)
			{
				m_mainActor->AttachSlave(iter->first.c_str(), iter->second.c_str());
			}
		}
	}

    void CGame::setPosition(float x, float y = 0.f, float z = -0.7f) {
        m_actorPosition = Vector3(x, y, z);
        if (m_mainActor != NULL) {
            m_mainActor->SetPosition(m_actorPosition);
        }
    }

	void CGame::changeDefaultIdle(int defaultIdle)
	{
		String idleName = "idle";
		if (defaultIdle > 0)// 0 is idle
		{
			idleName = "idle_" + StringUtil::Format("%d", defaultIdle);
			if (!m_mainActor->IsSkillExist(idleName))
			{
				idleName = "idle";
			}
		}
		m_mainActor->setDefaultIdle(idleName);
		m_mainActor->PlaySkill(m_mainActor->getDefaultIdle());
	}

    void CGame::changeBackgroundImage(LORD::String imageName) {
        if (mpBackgroundImage != NULL) {
            mpBackgroundImage->changeTexture(imageName);
        }
    }

	void CGame::changeSex()
	{
		m_isBoy = !m_isBoy;
		ActorManager::Instance()->DestroyActor(m_mainActor);
		m_mainActor = ActorManager::Instance()->CreateActor(m_isBoy ? "boy.actor" : "girl.actor", "idle", 0, false, false, true);
		m_mainActor->SetPosition(m_actorPosition);
		m_mainActor->SetBrightness(Color::WHITE);
	}

	void CGame::changeParts(String masterName, String slaveName)
	{
		m_actorPartsMap[masterName] = slaveName;
		refreshActor();
		LordLogInfo("changeParts masterName %s  slaveName %s", masterName.c_str(), slaveName.c_str());
	}

	void CGame::changeSex(int sex)
	{
		m_isBoy = sex == 1;
		ActorManager::Instance()->DestroyActor(m_mainActor);
		m_mainActor = ActorManager::Instance()->CreateActor(m_isBoy ? "boy.actor" : "girl.actor", "idle", 0, false, false, true);
		m_mainActor->SetPosition(m_actorPosition);
		m_mainActor->SetBrightness(Color::WHITE);
		m_actorPartsMap.clear();
	}

	void CGame::changeSkinColor(float r, float g, float b, float a)
	{
		if (m_mainActor)
		{
			m_mainActor->SetCustomColor(Color(r, g, b, a));
		}
	}

	void CGame::OnSize(int nWidth, int nHeight)
	{
		if (nWidth != 0 && nHeight != 0)
		{
			m_width = nWidth;
			m_height = nHeight;
			m_Root->onSize(nWidth, nHeight);
		}
	}

	void CGame::OnPause()
	{
		m_enterBackFlag = true;
		GUIWindowManager::Instance()->SetEnterBack(m_enterBackFlag);
	}

	void CGame::OnResume()
	{
		m_enterBackFlag = false;
		GUIWindowManager::Instance()->SetEnterBack(m_enterBackFlag);
	}

	bool CGame::handleTouchesBegin(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool touchOnUI = false;
		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;
			int unusedIndex = 0;

			//LordLogError("handleTouchesBegin ---------!");

			// 			LORD::vector<Touch*>::type sceneTouchList;
			// 			sceneTouchList.clear();

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);

				if (touch == NULL)
				{
					// it is a new touch
					ui32 time = Root::Instance()->getCurrentTime();
					Vector2 outPosition;

					GUISystem::Instance()->AdaptPosition(Vector2(x, y), outPosition);

					GUIWindow* pTarget = GUISystem::Instance()->GetTargetGUIWindow(outPosition);
					if (pTarget)
					{
						touchOnUI = true;
						touch = TouchManager::Instance()->createTouch(id, TOUCH_TYPE_UI, time);

						if (touch)
						{
							touch->setWindowName(pTarget->GetName());
							touch->setTouchPoint(outPosition);
							GUISystem::Instance()->handleTouchDown(touch);
						}
					}
					else
					{
						int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();
						if (sceneTouchCount > 0) {
							LordLogInfo("handleTouchesBegin, sceneTouchCount > 0");
							return touchOnUI;
						}
						touch = TouchManager::Instance()->createTouch(id, TOUCH_TYPE_SCENE, time);
						if (touch)
						{
							static float tolerance = m_displayDensity * 10;
							touch->setTouchPoint(Vector2(x, y), tolerance);

							// begin rotate actor.
							m_rotating = true;
							m_lastX = x;
						}
					}
				}
				else
				{
					LordLogError("handleTouchesBegin error touch already exist!");
				}
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
		}
		return touchOnUI;
	}

	bool CGame::handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool nResult = false;
		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;

			//LordLogError("handleTouchesMove ---------!");

			// 			LORD::vector<Touch*>::type sceneTouchList;
			// 			sceneTouchList.clear();

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);
				if (touch)
				{
					Vector2 uiPosition;
					GUISystem::Instance()->AdaptPosition(Vector2(x, y), uiPosition);
					if (touch->getType() == TOUCH_TYPE_UI)
					{
						nResult = true;
						touch->setTouchPoint(uiPosition);

						Vector2 offset = (touch->getTouchPoint() - touch->getStartPoint());
						if (offset.len() < 20)
						{
							continue;
						}

						Vector2 outPosition;
						GUISystem::Instance()->AdaptPosition(Vector2(x, y), outPosition);
						GUIWindow* pTarget = GUISystem::Instance()->GetTargetGUIWindow(outPosition);
						if (pTarget) //&& pTarget->GetName() != touch->getWindowName())
						{
							GUISystem::Instance()->handleTouchMove(touch);
							touch->setWindowName(pTarget->GetName());
							touch->setTouchPoint(outPosition);
							GUISystem::Instance()->handleTouchDown(touch);
						}
						else
						{
							Touch* sceneTouch1 = TouchManager::Instance()->getSceneTouch1();
							GUISystem::Instance()->handleTouchUp(touch);
							if (!sceneTouch1)
							{
								touch->setTouchType(TOUCH_TYPE_SCENE);
							}
						}
					}
					else if (touch->getType() == TOUCH_TYPE_SCENE)
					{
						static float tolerance = m_displayDensity * 8;
						touch->setTouchPoint(Vector2(x, y), tolerance);

						// rotate the actor's yaw.
						if (m_rotating)
						{
							m_yaw += (x - m_lastX) * 0.01f;
							m_lastX = x;
						}
					}
					else
					{
						LordLogError("handleTouchesMove error, unknown touch type occur, type=%d", (int)(touch->getType()));
					}
				}
			}

			int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();

			if (sceneTouchCount == 1 && TouchManager::Instance()->getSceneTouch1())
			{
				Vector2 offset = TouchManager::Instance()->getSceneTouch1()->getTouchPoint() - TouchManager::Instance()->getSceneTouch1()->getStartPoint();
				if (offset.len() > 10)
				{
					//LuaEngine::Instance()->handleTouchMove(TouchManager::Instance()->getSceneTouch1());
				}
			}
			else if (sceneTouchCount > 1)
			{
				//LuaEngine::Instance()->handleMultiTouchMove(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
			}
			else
			{
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
			// LuaEngine::Instance()->exceptionHandle(e);
		}

		return nResult;
	}

	bool CGame::handleTouchesEnd(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool nResult = false;
		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;

			//LordLogError("handleTouchesEnd ---------!");

			bool deleteflag[TouchManager::MAX_TOUCHES] = { false };

			int sceneEndCount = 0;
			Touch* sceneEndTouch = NULL;

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);
				if (touch)
				{
					deleteflag[i] = true;
					Vector2 uiPosition;

					//LordLogDebug("touch end?x %f, y %f", x, y);

					GUISystem::Instance()->AdaptPosition(Vector2(x, y), uiPosition);
					if (touch->getType() == TOUCH_TYPE_UI)
					{
						nResult = true;
						touch->setTouchPoint(uiPosition);
						GUISystem::Instance()->handleTouchUp(touch);
					}
					else if (touch->getType() == TOUCH_TYPE_SCENE)
					{
						static float tolerance = m_displayDensity * 10;
						touch->setTouchPoint(Vector2(x, y), tolerance);

						m_rotating = false;

						++sceneEndCount;
						sceneEndTouch = touch;
					}
					else
					{
						LordLogError("handleTouchesEnd error, unknown touch type occur, type=%d", (int)(touch->getType()));
					}
				}
				else
				{
					LordLogError("handleTouchesEnd error touch not exist!");
				}
			}

			int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();

			if (sceneEndCount == 1 && sceneTouchCount == 1 && TouchManager::Instance()->getSceneTouch1())
			{

			}
			else if (sceneTouchCount > 1)
			{
				LordLogError("handleTouchesEnd error, sceneTouchCount > 1, should not occur");
				if (sceneEndCount == 1)
				{
					if (sceneEndTouch == TouchManager::Instance()->getSceneTouch1())
					{
						//LuaEngine::Instance()->handleMultiTouchUpOne(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
					}
					else
					{
						//LuaEngine::Instance()->handleMultiTouchUpOne(TouchManager::Instance()->getSceneTouch2(), TouchManager::Instance()->getSceneTouch1());
					}
				}
				else
				{
					//LuaEngine::Instance()->handleMultiTouchUp(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
				}
			}

			for (int i = 0; i < num; ++i)
			{
				if (deleteflag[i])
				{
					deleteflag[i] = false;
					id = ids[i];
					TouchManager::Instance()->destroyTouch(id);
				}
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
			//LuaEngine::Instance()->exceptionHandle(e);
		}

		return nResult;
	}

	bool CGame::handleTouchesCancel(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool nResult = false;

		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;

			bool deleteflag[TouchManager::MAX_TOUCHES] = { false };

			int sceneEndCount = 0;
			Touch* sceneEndTouch = NULL;

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);

				if (touch)
				{
					deleteflag[i] = true;
					Vector2 uiPosition;
					GUISystem::Instance()->AdaptPosition(Vector2(x, y), uiPosition);
					if (touch->getType() == TOUCH_TYPE_UI)
					{
						nResult = true;
						touch->setTouchPoint(uiPosition);
						GUISystem::Instance()->handleTouchCancel(touch);
					}
					else if (touch->getType() == TOUCH_TYPE_SCENE)
					{
						touch->setTouchPoint(Vector2(x, y));

						sceneEndTouch = touch;
						++sceneEndCount;
					}
					else
					{
					}
				}
			}

			int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();

			if (sceneEndCount == 1 && sceneTouchCount == 1)
			{
				//LuaEngine::Instance()->handleTouchCancel(TouchManager::Instance()->getSceneTouch1());
			}
			else if (sceneTouchCount > 1)
			{
				if (sceneEndCount == 1)
				{
					if (sceneEndTouch == TouchManager::Instance()->getSceneTouch1())
					{
						//LuaEngine::Instance()->handleMultiTouchCancelOne(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
					}
					else
					{
						//LuaEngine::Instance()->handleMultiTouchCancelOne(TouchManager::Instance()->getSceneTouch2(), TouchManager::Instance()->getSceneTouch1());
					}
				}
				else
				{
					//LuaEngine::Instance()->handleMultiTouchCancel(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
				}
			}
			else
			{
			}

			for (int i = 0; i < num; ++i)
			{
				if (deleteflag[i])
				{
					deleteflag[i] = false;
					id = ids[i];
					TouchManager::Instance()->destroyTouch(id);
				}
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
			//LuaEngine::Instance()->exceptionHandle(e);
		}

		return nResult;
	}

	void CGame::handleKeyDown(int key)
	{
		KeyEventManager::Instance()->onKeyDown(key);
	}

	void CGame::handleKeyUp(int key)
	{
		KeyEventManager::Instance()->onKeyUp(key);
		switch (key)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			m_chooseIndex[m_shiftIndex] = key - 48;  refreshActor(); break;
		case 9:
			m_shiftIndex = (m_shiftIndex + 1) % 4; refreshActor(); break;
		case 16:
			changeSex(); break;
		default:
			break;
		}
	}
}
