#include "ClientDataReport.h"



namespace BLOCKMAN {
	const String ClientDataReport::ENTER_GAME_STATUS = "enter_game_status";
	const String ClientDataReport::ENTER_GAME_SUCCESS = "enter_game_success";
	const String ClientDataReport::QUIT_GAME = "quit_game";
	const String ClientDataReport::GAME_ERROR_CODE = "game_error_code";
	const String ClientDataReport::GAME_DISCONNECT = "game_disconnect";


	const String ClientDataReport::ERROR_CODE_ENTER_GAME_SUCCESS = "0";
	const String ClientDataReport::ERROR_CODE_VALIDATION_FAILS = "1";
	const String ClientDataReport::ERROR_CODE_VALIDATION_TIMEOUT = "2";
	const String ClientDataReport::ERROR_CODE_MAP_DOWNLOAD_FAILS = "3";
	const String ClientDataReport::ERROR_CODE_NETWORK_TIMEOUT = "4";
	const String ClientDataReport::ERROR_CODE_NETWORK_CONNECT_ERROR = "5";
	const String ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS = "6";
	const String ClientDataReport::ERROR_CODE_GAME_OVER = "7";
	const String ClientDataReport::ERROR_CODE_BE_KICKED = "8";
	const String ClientDataReport::ERROR_CODE_UNKNOWN = "9";
	const String ClientDataReport::ERROR_CODE_ENGINE_VERSION_VALIDATION_FAILS = "10";
	const String ClientDataReport::ERROR_CODE_ENGINE_VERSION_VALIDATION_TIMEOUT = "11";
	const String ClientDataReport::ERROR_CODE_ROOM_FULL = "12";
	const String ClientDataReport::ERROR_CODE_GAME_START = "13";
	const String ClientDataReport::ERROR_CODE_GAME_STOP = "14";
	const String ClientDataReport::ERROR_CODE_ENGINE_GET_USER_ATTR_FAIL = "15";
	const String ClientDataReport::ERROR_CODE_NOT_RECV_GAME_INFO = "16";
	const String ClientDataReport::ERROR_CODE_CONNECTION_LOST = "17";
	const String ClientDataReport::ERROR_CODE_SERVER_LOAD_DB_ERROR = "18";
	const String ClientDataReport::ERROR_CODE_ENGINE_WAIT_ROLE_ATTR_FAIL = "19";
	const String ClientDataReport::ERROR_CODE_ROOM_FULL_RETRY = "20";
	const String ClientDataReport::ERROR_CODE_GAME_START_RETRY = "21";
	const String ClientDataReport::ERROR_CODE_CHEAT_DETECTED = "22";


	const String ClientDataReport::GAME_DISCONNECT_NORMAL	= "1";
	const String ClientDataReport::GAME_DISCONNECT_TIMEOUT	= "2";
	const String ClientDataReport::GAME_DISCONNECT_UNKNOWN  = "3";
}
