#ifndef _GAME_H_
#define _GAME_H_

#include "Core.h"
#include "stdio.h"

namespace LORD
{
	class SoundEngine;
	class BackGroundRenderable;
}

namespace ClothesPlug
{
	class ShellInterface;

	class CGame : public LORD::Singleton<CGame>
	{

	public:
		CGame();
		virtual ~CGame();

		static CGame* Instance();
				
		void initEngine(const char* strRootPath, const char* strConfigPath, int w, int h, unsigned int wnd , int sex = 2);
		void RenderTick();
		void DestroyGame();
		
		void onError(int errorCode);
		bool exitGame();
		void MainTick();

		void OnSize(int nWidth, int nHeight);

		bool handleTouchesBegin(int num, intptr_t ids[], float xs[], float ys[]);
		bool handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[]);
		bool handleTouchesEnd(int num, intptr_t ids[], float xs[], float ys[]);
		bool handleTouchesCancel(int num, intptr_t ids[], float xs[], float ys[]);

		void handleKeyDown(int key);
		void handleKeyUp(int key);
		void OnPause();
		void OnResume();

		void SetMaxFps(unsigned int fps)
		{
			m_fps = fps;
			maxFrameTime = 1000 / m_fps;
		}

		void setShellInterface(ShellInterface* shellInterface) { m_shellInterface = shellInterface; }

		ShellInterface* getShellInterface() { return m_shellInterface; }
				
		LORD::String getRootPath() { return m_rootPath; }
		LORD::ui64 getGameTimestamp() { return m_gameTimestamp; }

		void setDisplayDensity(float displayDensity) { this->m_displayDensity = displayDensity; }
		float getDisplayDensity() { return m_displayDensity; }

		void refreshActor();
		void changeSex();

		void changeParts(LORD::String masterName, LORD::String slaveName);
		void changeSex(int sex);
		void changeBackgroundImage(LORD::String imageName);
		void changeSkinColor(float r, float g, float b, float a);
		void setAction(int actionId) {};
		void setActorSize(float w, float h) {};
		void setPosition(float x, float y, float z);
		void changeDefaultIdle(int defaultIdle);

	protected:
		bool	m_bInited;
		bool    m_enterBackFlag;

		float   m_yaw;				
		bool	m_rotating;			
		int		m_lastX;			
		int		m_shiftIndex;
		int		m_chooseIndex[4];
		bool	m_isBoy;	
	private:
		unsigned int        m_fps;
		unsigned int		maxFrameTime;

		LORD::String		m_nickName;
		LORD::ui64			m_gameTimestamp;
		LORD::String		m_rootPath;
		LORD::String		m_configPath;
		
		int					m_width;
		int					m_height;
		unsigned int		m_hWnd;
		LORD::Root*			m_Root;
		LORD::Camera*		m_MainCamera;
		LORD::SceneNode*	m_actorNode;
		LORD::ActorObject*  m_mainActor;
		LORD::BackGroundRenderable*	mpBackgroundImage;

		ShellInterface*		m_shellInterface;

		LORD::ui64			m_tickCount;
		
		float				m_displayDensity = 1.0f;

		LORD::map<LORD::String, LORD::String>::type m_actorPartsMap;
        
		LORD::Vector3 m_actorPosition = LORD::Vector3(0.f, 0.f, 0.f);
	};
}

#endif
