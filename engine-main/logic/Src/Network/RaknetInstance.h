/********************************************************************
filename: 	RaknetInstance
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\network\RaknetInstance.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __RAKNETINSTANCE_H__
#define __RAKNETINSTANCE_H__

#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <queue>
#include <mutex>  

#include "raknet/include/RakPeerInterface.h"
#include "raknet/include/BitStream.h"
#include "raknet/include/MessageIdentifiers.h"

#include "Network/protocol/DataPacket.h"
#include "Util/UThread.h"
#include "Util/UAsyncCall.h"
#include "TypeDef.h"
using namespace RakNet;

#define	 INVALID_RAKSOCKET -1
#define  MAX_CHANNEL 6
#define  MAX_RAKSSID 0xffffffff

class RaknetInstance
{

	enum GAME_DATA_ID
	{
		emNormalData = ID_USER_PACKET_ENUM+1,
		emCompressData
	};
	
	typedef struct tSession
	{
		RakNet::SystemAddress remote;
		bool isDisconnect = true;
		std::vector<std::shared_ptr<DataPacket>> sendPackets;
		ui32 lastRecviePKTime = 0;
		bool isException = false;
	}Session;

public:
	enum ConnectEventType
	{
		emConnectSuc,
		emConnectFailed,
		emConnectTimeout,
		emConnectKickOut,
		emDisconnect,
		emNewConnect
	};

	typedef struct tConnectEvent
	{
	public:
		tConnectEvent() {}
		tConnectEvent(ui64 id, ConnectEventType _type) :rakssid(id), type(_type)
		{}
		ui64 rakssid;
		ConnectEventType type;

	}ConnectEvent;

public:
	RaknetInstance(const char *ip, ui16 port);
	RaknetInstance();
	virtual ~RaknetInstance(void);
	void tearDown();

	void connect(const char * ip, ui16 port, std::function<void(ConnectEvent&)> connEventHandler);
	void listen(i32 maxPlayers, std::function<void( ConnectEvent&)> connEventHandler);
	void setHeartbeatCallback(std::function<void()> heartBeatHandler);

	void closeSession(ui64 rakSock);

	void send(std::shared_ptr<DataPacket> packet, ui64 rakssid, bool immediate = true);
	void flush(ui64 rakssid= INVALID_RAKSOCKET);

	void recv(std::function<void(std::shared_ptr<DataPacket>)> handler, bool useThreadPool);

	bool isAlive(ui64 sock);
	void printColletingPacket();

private:
	void setup(const char * ip, ui16 port);

	void tick(StopFlag shouldStop);
	int peer_recvive();
	int peer_send();

	void sendSessionData(std::shared_ptr<Session> &session);

	void onConnectResult(SystemAddress &addr,bool ret);
	void onConnectTimeout(SystemAddress &addr);
	void onConnectKickOut(SystemAddress &addr);
	void onDisconnect(SystemAddress &addr);
	void onNewIncomingConn(SystemAddress &addr);

	void handleCompressData(SystemAddress &addr, Packet *data);
	void handleNormalData(SystemAddress &addr, Packet *data);

	void regReciveTime(SystemAddress &addr);
	void count_recv_packet_num(std::shared_ptr<DataPacket> packet, ui64 rakssid);
	void count_send_packet_num(std::shared_ptr<DataPacket> packet, ui64 rakssid);

private:

	RakNet::SystemAddress m_local;

	std::shared_ptr<RakPeerInterface> m_pRakpeer;

	std::function<void(ConnectEvent&)> m_connectEventcallback = nullptr;

	std::function<void(std::shared_ptr<DataPacket>)> m_handleDataCallBack = nullptr;
	std::function<void()> m_sendHeartbeatCallback = nullptr;

	bool m_isServer = false;

	std::map<ui64, std::shared_ptr<Session>> m_sessionList;
	std::mutex m_sessionListMut;

	std::shared_ptr<UThread> m_tickThread;

	SPSCAsyncCall m_asyncCall;

	char m_localIp[20] = { 0 };
	ui16 m_localPort = 0;
	bool m_destroyed = false;

	// for collecting data packets
	bool m_bCollectingPacketData = false;
	int send_total_packet_cnt = 0;  // total amount of sending packet
	int send_total_packet_byte = 0; // total size of sending packet
	std::map<int, std::pair<int, int>> send_type_cnt_byte_map; // key: sending packetId value: pair, first: total amount of this kind of packet second: total size(byte) of this kind of packet
	std::map<ui64, int> send_user_cnt_map; // key: user raknetId value: total amount of packet sending to this user  

	int recv_total_packet_cnt = 0;  // total amount of receiving packet
	int recv_total_packet_byte = 0; // total size of receiving packet
	std::map<int, std::pair<int, int>> recv_type_cnt_byte_map; // key: receiving packetId value: pair, first: total amount of this kind of packet second: total size(byte) of this kind of packet
	std::map<ui64, int> recv_user_cnt_map; // key: user raknetId value: total amount of packet receiving from this user  
};
#endif
