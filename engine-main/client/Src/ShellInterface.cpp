#include "ShellInterface.h"
#include "TypeDef.h"
#include "game.h"
#ifdef WIN32
#include "shellapi.h"
#endif

namespace GameClient
{
	ShellInterface::ShellInterface()
	{

	}

	ShellInterface::~ShellInterface()
	{

	}

	void ShellInterface::login()
	{

	}

	void ShellInterface::logout()
	{

	}

	void ShellInterface::enterUserCenter()
	{

	}

	void ShellInterface::enterAppBBS()
	{

	}

	void ShellInterface::payWithInfo(const LORD::GUIString& payInfo)
	{
		//LORD::String gbkUrl = StringUtil::UTF8ToGBK();
#ifdef WIN32
		ShellExecute(0, "open", "iexplore.exe", payInfo.c_str(), NULL, SW_HIDE);
#endif
	}

	bool ShellInterface::islogin()
	{
		return true;
	}

	bool ShellInterface::isLoginFromSdk()
	{
		return false;
	}

	void ShellInterface::onUpdatePushInfo(const LORD::GUIString& pushInfo)
	{

	}

	void ShellInterface::onClearPushInfo()
	{

	}

	LORD::GUIString ShellInterface::getDeviceInfo()
	{
		LORD::String phonetype = CGame::Instance()->GetPhoneInfo();
		LORD::String gouinfo = CGame::Instance()->GetGPUInfo();
		LORD::GUIString ret((phonetype + "|" + gouinfo).c_str());
		return ret;
	}

	void ShellInterface::gameExit()
	{

	}

	void ShellInterface::nextGame()
	{
	}

	void ShellInterface::closeGame(int code)
	{
	}

	void ShellInterface::onError(int errorCode)
	{
	}

	void ShellInterface::usePropResults(LORD::String propId, int results)
	{
	}

	void ShellInterface::userIn(LORD::ui64 userId, LORD::String nickName)
	{
	}

	void ShellInterface::userOut(LORD::ui64 userId)
	{
	}

	void ShellInterface::loadMapComplete()
	{
	}

	void ShellInterface::gameSettlement(LORD::String gameType, LORD::String gameResult)
	{
	}

	void ShellInterface::dataReport(LORD::String mainEvent, LORD::String childEvent)
	{
	}

	void ShellInterface::userChange(LORD::String userName, LORD::String teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn)
	{
	}

	void ShellInterface::onFriendOperation(int operationType, LORD::ui64 userId)
	{
	}

	void ShellInterface::resetGame(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId)
	{
	}

	void ShellInterface::onRecharge(int type)
	{
		
	}

	void ShellInterface::playVideo(int type, LORD::String path)
	{
	}

	bool ShellInterface::getBoolForKey(LORD::String key, bool defaultValue)
	{
		return !defaultValue;
	}

	void ShellInterface::putBoolForKey(LORD::String key, bool value)
	{
	}

	void ShellInterface::playAdVideo()
	{

	}

	void ShellInterface::reportAdVideoShow()
	{

	}

}

