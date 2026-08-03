#ifndef _SHELL_INTERFACE_ANDROID_H_
#define _SHELL_INTERFACE_ANDROID_H_

#include "ShellInterface.h"
#include "game.h"
#include "TypeDef.h"

namespace GameClient {
    
    class ShellInterfaceAndroid : public ShellInterface
    {
    public:
    	ShellInterfaceAndroid();
        
    	virtual ~ShellInterfaceAndroid();

		virtual bool	isLoginFromSdk() override;

		virtual void	login() override;

		virtual void	logout() override;

//		virtual void	enterUserCenter();
//
//		virtual void	enterAppBBS();

		virtual void	payWithInfo(const LORD::GUIString& payInfo) override;

		virtual void	onUpdatePushInfo(const LORD::GUIString& pushInfo) override;

		virtual void	onClearPushInfo() override;

		virtual void	onCreateRoleSuccess(const LORD::GUIString& info) override;

		virtual void	onLoginRoleSuccess(const LORD::GUIString& info) override;

		virtual void	onRoleLevelUp(const LORD::GUIString& info) override;

		virtual void    gameExit() override;
		
		virtual void    nextGame() override;

		virtual void	onError(int errorCode) override;

		virtual void	usePropResults(LORD::String propId, int results) override;

		virtual void	userIn(LORD::ui64 userId, LORD::String nickName) override;

		virtual void	userOut(LORD::ui64 userId) override;

		virtual void  	onFriendOperation(int operationType, LORD::ui64 userId);

        virtual void    loadMapComplete() override;

        virtual void    gameSettlement(LORD::String gameType, LORD::String gameResult) override;

		virtual void    dataReport(LORD::String mainEvent, LORD::String childEvent) override;

		virtual void	userChange(LORD::String userName, LORD::String teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn) override;

		virtual void    resetGame(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId) override;

        virtual void	closeGame(int code) override;

        virtual void    onRecharge(int type) override;

		virtual void	playVideo(int type, LORD::String path) override;

		virtual void	playAdVideo() override;

		virtual void	reportAdVideoShow() override;

		virtual bool 	getBoolForKey(LORD::String key, bool defaultValue) override;

		virtual void 	putBoolForKey(LORD::String key, bool value) override;

		virtual void 	onExpResult(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status) override;
    };
}

#endif // _SHELL_INTERFACE_ANDROID_H_

