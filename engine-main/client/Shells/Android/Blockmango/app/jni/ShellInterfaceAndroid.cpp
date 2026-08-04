#include "ShellInterfaceAndroid.h"
#include "game.h"
#include "TypeDef.h"
#include "EchoHelper.h"

namespace GameClient {

    ShellInterfaceAndroid::ShellInterfaceAndroid() {

    }

    ShellInterfaceAndroid::~ShellInterfaceAndroid() {

    }

    bool ShellInterfaceAndroid::isLoginFromSdk() {
        return false;
    }

    void ShellInterfaceAndroid::login() {

    }

    void ShellInterfaceAndroid::logout() {

    }

//	void	ShellInterfaceAndroid::enterUserCenter()
//	{
//
//	}
//
//	void	ShellInterfaceAndroid::enterAppBBS()
//	{
//
//	}

    void ShellInterfaceAndroid::payWithInfo(const LORD::GUIString &payInfo) {
        onCallPayJNI(payInfo.c_str());
    }

    void ShellInterfaceAndroid::onUpdatePushInfo(const LORD::GUIString &pushInfo) {

    }

    void ShellInterfaceAndroid::onClearPushInfo() {

    }

    void ShellInterfaceAndroid::onCreateRoleSuccess(const LORD::GUIString &info) {

    }

    void ShellInterfaceAndroid::onLoginRoleSuccess(const LORD::GUIString &info) {

    }

    void ShellInterfaceAndroid::onRoleLevelUp(const LORD::GUIString &info) {

    }

    void ShellInterfaceAndroid::gameExit() {
        gameExitJNI();
    }

    void ShellInterfaceAndroid::nextGame() {
        nextGameJNI();
    }

    void ShellInterfaceAndroid::onError(int errorCode) {
        onErrorJNI(errorCode);
    }


    void ShellInterfaceAndroid::usePropResults(String propId, int results) {
        usePropResultsJNI(propId.c_str(), results);
    }

    void ShellInterfaceAndroid::userIn(LORD::ui64 userId, LORD::String nickName) {
        userInJNI(userId, nickName.c_str());
    }

    void ShellInterfaceAndroid::userOut(LORD::ui64 userId) {
        userOutJNI(userId);
    }

    void ShellInterfaceAndroid::loadMapComplete() {
        loadMapCompleteJNI();
    }

    void ShellInterfaceAndroid::gameSettlement(LORD::String gameType, LORD::String gameResult) {
        gameSettlementJNI(gameType.c_str(), gameResult.c_str());
    }

    void ShellInterfaceAndroid::dataReport(LORD::String mainEvent, LORD::String childEvent) {
        dataReportJNI(mainEvent.c_str(), childEvent.c_str());
    }

    void ShellInterfaceAndroid::userChange(LORD::String userName, LORD::String teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn) {
        userChangeJNI(userName.c_str(), teamName.c_str(), userId, teamId, isUserIn);
    }

    void ShellInterfaceAndroid::onFriendOperation(int operationType, LORD::ui64 userId) {
        onFriendOperationJNI(operationType, userId);
    }

    void ShellInterfaceAndroid::resetGame(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId) {
        resetGameJNI(targetUserId, gameType.c_str(), mapId.c_str());
    }

    void ShellInterfaceAndroid::closeGame(int code) {
        closeGameJNI(code);
    }

    void ShellInterfaceAndroid::onRecharge(int type) {
        onRechargeJNI(type);
    }

    void ShellInterfaceAndroid::playVideo(int type, LORD::String path) {
        playVideoJNI(type, path.c_str());
    }

    void ShellInterfaceAndroid::playAdVideo() {
        playAdVideoJNI();
    }

    void ShellInterfaceAndroid::reportAdVideoShow() {
        reportAdVideoShowJNI();
    }

    bool ShellInterfaceAndroid::getBoolForKey(LORD::String key, bool defaultValue) {

        return getBoolForKeyJNI(key.c_str(), defaultValue);
    }

    void ShellInterfaceAndroid::putBoolForKey(LORD::String key, bool value) {
        setBoolForKeyJNI(key.c_str(), value);
    }

    void ShellInterfaceAndroid::onExpResult(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status) {
        onExpResultJNI(curLv, toLv, addExp, curExp, toExp, upExp, status);
    }
}


