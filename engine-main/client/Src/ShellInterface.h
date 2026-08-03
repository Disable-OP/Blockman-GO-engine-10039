#ifndef _SHELL_INTERFACE_H_
#define _SHELL_INTERFACE_H_

#include "Core.h"
#include "TypeDef.h"
#include "UI/GUIString.h"

namespace GameClient
{	
	class ShellInterface : public LORD::ObjectAlloc
	{
	public:  
		ShellInterface();
		virtual ~ShellInterface();
		
		virtual bool	isLoginFromSdk();

		virtual bool	islogin();

		virtual void	login();

		virtual void	logout();

		virtual void	enterUserCenter();

		virtual void	enterAppBBS();

		virtual void	payWithInfo(const LORD::GUIString& payInfo);

		virtual void	onUpdatePushInfo(const LORD::GUIString& pushInfo);

		virtual void	onClearPushInfo();

		virtual LORD::GUIString getDeviceInfo();
        
        virtual LORD::GUIString getPlatformID() {return "";}

		virtual void	enableRecord(bool setting) {}

		virtual void    gameExit();

		virtual void	nextGame();

		virtual void	closeGame(int code);

		virtual void	onError(int errorCode);

		virtual void	usePropResults(LORD::String propId, int results);
	
		virtual void	userIn(LORD::ui64 userId, LORD::String nickName);

		virtual void	userOut(LORD::ui64 userId);

		virtual void	loadMapComplete();
		
		virtual void	gameSettlement(LORD::String gameType, LORD::String gameResult);

		virtual void	dataReport(LORD::String mainEvent, LORD::String childEvent);

		virtual void	userChange(LORD::String userName, LORD::String teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn);

		virtual void	onFriendOperation(int operationType, LORD::ui64 userId);

		virtual void	resetGame(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId);

		virtual void	onRecharge(int type);

		virtual void	playVideo(int isLocal, LORD::String path);

		virtual bool	getBoolForKey(LORD::String key, bool defaultValue);

		virtual void	putBoolForKey(LORD::String key, bool value);

		virtual void	playAdVideo();

		virtual void	reportAdVideoShow();

		// android sdk
		virtual void	onCreateRoleSuccess(const LORD::GUIString& info) {}

		virtual void	onLoginRoleSuccess(const LORD::GUIString& info) {}

		virtual void	onRoleLevelUp(const LORD::GUIString& info) {}
        
        // ios weixin sdk
        virtual void    onShareToWeiXin(const LORD::GUIString& info) {}

		virtual void	onExpResult(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status) {}
	}; 
}

#endif
