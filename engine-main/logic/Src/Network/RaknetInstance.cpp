#include "RaknetInstance.h"
#include "raknet/include/MessageIdentifiers.h"
#include "Network/protocol/DataPacket.h"
#include "Network/protocol/DataPacketFactory.h"
#include "Object/Root.h"
#include "Object/Time.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

RaknetInstance::RaknetInstance(const char * ip, ui16 port)
{
	assert(ip != NULL);
	this->setup(ip, port);
}

RaknetInstance::RaknetInstance()
{
	this->setup("", 0);
}

RaknetInstance::~RaknetInstance(void)
{
	tearDown();
}

void RaknetInstance::setup(const char * ip, ui16 port)
{
	memset(m_localIp, 0, sizeof(m_localIp));
	strcpy(m_localIp, ip);
	m_localPort = port;

	std::shared_ptr<RakPeerInterface> pPeer(RakPeerInterface::GetInstance());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	pPeer->SetTimeoutTime(180000, UNASSIGNED_SYSTEM_ADDRESS);
#else
	pPeer->SetTimeoutTime(10000, UNASSIGNED_SYSTEM_ADDRESS);
#endif
	this->m_pRakpeer = pPeer;
	m_tickThread = std::shared_ptr<UThread>(new UThread("Raknetinstance", &RaknetInstance::tick, this));
	m_bCollectingPacketData = true;   //switch collecting data packet or not
}

void RaknetInstance::tearDown()
{
	if (m_destroyed)
	{
		return;
	}
	
	m_tickThread->stopSync();
	m_pRakpeer->Shutdown(0);
	m_destroyed = true;
}

void RaknetInstance::connect(const char * ip, ui16 port, std::function<void(ConnectEvent&)> connEventHandler)
{
	assert(connEventHandler != NULL);
	assert(ip != NULL);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.port = m_localPort;
	strcpy(socketDescriptor.hostAddress, m_localIp);

	StartupResult b = m_pRakpeer->Startup(1, &socketDescriptor, 1);
	if (StartupResult::RAKNET_STARTED != b)
	{
		ConnectEvent connEvvent(INVALID_RAKSOCKET, emConnectFailed);
		connEventHandler(connEvvent);
		return;
	}

	bool ok = m_pRakpeer->Connect(ip, port, NULL, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	if(ok)
	{
		m_connectEventcallback = connEventHandler;
		m_isServer  = false; 
		m_tickThread->start();
	}
	else
	{
		ConnectEvent connEvvent(INVALID_RAKSOCKET, emConnectFailed);
		connEventHandler(connEvvent);
	}
}

void RaknetInstance::listen(i32 maxPlayers, std::function<void(ConnectEvent&)> connEventHandler)
{
	assert(connEventHandler != NULL);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.port = m_localPort;
	strcpy(socketDescriptor.hostAddress, m_localIp);

	StartupResult b = m_pRakpeer->Startup(maxPlayers, &socketDescriptor, 1);
	if (StartupResult::RAKNET_STARTED != b)
	{
		return;
	}
	m_pRakpeer->SetMaximumIncomingConnections(maxPlayers);// must call this for server
	m_connectEventcallback = connEventHandler;
	m_isServer = true;
	m_tickThread->start();
}

void RaknetInstance::setHeartbeatCallback(std::function<void()> heartBeatHandler)
{
	m_sendHeartbeatCallback = heartBeatHandler;
}

void RaknetInstance::closeSession(ui64 ssid)
{
	auto fuc = [this, ssid]()
	{
		std::lock_guard<std::mutex> lk(m_sessionListMut);
		auto session = this->m_sessionList.find(ssid);
		if(session == this->m_sessionList.end())
			return;

		this->m_pRakpeer->CloseConnection(session->second->remote, true);
		this->m_sessionList.erase(session);
	};

	m_asyncCall.noblockCall(fuc);
}

void RaknetInstance::sendSessionData(std::shared_ptr<Session> &session)
{
	RakNet::BitStream sendStreams[MAX_CHANNEL];
	for (int j = 0; j < MAX_CHANNEL; j++)
	{
		sendStreams[j].Write((char)emCompressData);
	}

	auto& listTmp = session->sendPackets;

	auto dataPacket = listTmp.begin();
	while (dataPacket != listTmp.end())
	{
		int channel = (*dataPacket)->channel();
		sendStreams[channel].Write((ui16)((*dataPacket)->pid()));
		sendStreams[channel].Write((short)((*dataPacket)->GetNumberOfBytesUsed()));
		sendStreams[channel].Write((const  char*)(*dataPacket)->GetData(), (*dataPacket)->GetNumberOfBytesUsed());

		dataPacket++;
	}

	for (int i = 0; i<MAX_CHANNEL; i++)
	{
		if (sendStreams[i].GetNumberOfBytesUsed() > 1)
		{
			PacketReliability reliability = i == 0 ? RELIABLE : RELIABLE_ORDERED;
			uint32_t ret = this->m_pRakpeer->Send(&sendStreams[i], HIGH_PRIORITY, reliability, i, session->remote, false);
		}
	}

	listTmp.clear();
}

void RaknetInstance::flush(ui64 rakssid /*= INVALID_RAKSOCKET*/)
{
	// TODO compress

	auto fuc = [this, rakssid](void)
	{
		if (INVALID_RAKSOCKET != rakssid)
		{
			auto session = this->m_sessionList.find(rakssid);
			if (session != this->m_sessionList.end())
			{
				sendSessionData(session->second);
			}

		}
		else
		{
			auto session = this->m_sessionList.begin();
			while (session != this->m_sessionList.end())
			{
				sendSessionData(session->second);
				session++;
			}
		}
	};

	m_asyncCall.noblockCall(fuc);
}

void RaknetInstance::count_send_packet_num(std::shared_ptr<DataPacket> packet, ui64 rakssid)
{
	if (!m_bCollectingPacketData)
		return;

	if (send_type_cnt_byte_map.find(packet->pid()) == send_type_cnt_byte_map.end()) {
		send_type_cnt_byte_map[packet->pid()] = {0, 0};
	}

	if (send_user_cnt_map.find(rakssid) == send_user_cnt_map.end()) {
		send_user_cnt_map[rakssid] = 0;
	}
	
	int packetSize = packet->GetNumberOfBytesUsed();
	send_total_packet_cnt++;
	send_total_packet_byte += packetSize;
	send_type_cnt_byte_map[packet->pid()].first++;
	send_type_cnt_byte_map[packet->pid()].second += packetSize;
	send_user_cnt_map[rakssid]++;
}

void RaknetInstance::count_recv_packet_num(std::shared_ptr<DataPacket> packet, ui64 rakssid) 
{
	if (!m_bCollectingPacketData)
		return;

	if (recv_type_cnt_byte_map.find(packet->pid()) == recv_type_cnt_byte_map.end()) {
		recv_type_cnt_byte_map[packet->pid()] = {0, 0};
	}

	if (recv_user_cnt_map.find(rakssid) == recv_user_cnt_map.end()) {
		recv_user_cnt_map[rakssid] = 0;
	}

	int packetSize = packet->GetNumberOfBytesUsed();
	recv_total_packet_cnt++;
	recv_total_packet_byte += packetSize;
	recv_type_cnt_byte_map[packet->pid()].first++;
	recv_type_cnt_byte_map[packet->pid()].second += packetSize;
	recv_user_cnt_map[rakssid]++;
}

void RaknetInstance::send(std::shared_ptr<DataPacket> packet,ui64 rakssid, bool immediate /* = false */)
{
	count_send_packet_num(packet, rakssid);
	auto fuc = [this, packet, rakssid, immediate]()
	{
		ui64 sock = rakssid;
		auto session = this->m_sessionList.find(sock);
		if(session != this->m_sessionList.end())
		{
			if(immediate)
			{
				RakNet::BitStream sendData;
				sendData.Write((char)emNormalData);
				sendData.Write((ui16)packet->pid());
				sendData.Write((const char *)packet->GetData(), packet->GetNumberOfBytesUsed());

				PacketReliability reliability = packet->channel()==0? RELIABLE: RELIABLE_ORDERED;
				uint32_t rt = this->m_pRakpeer
					->Send(&sendData, HIGH_PRIORITY, reliability, packet->channel(), session->second->remote, false);
			}
			else
			{
				session->second->sendPackets.push_back(packet);
			}

		}
	};

	m_asyncCall.noblockCall(fuc);
}


void RaknetInstance::tick(StopFlag shouldStop)
{
	while(!shouldStop)
	{
		int recvNum = peer_recvive();
		int sendNum = peer_send();
		int limit = 100;
		if (m_isServer) {
			limit = 500;
		}

		if (recvNum + sendNum < limit) {
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			Sleep(8);
#else
			usleep(8000);
#endif
		}

		if (m_isServer && m_sendHeartbeatCallback)
		{
			m_sendHeartbeatCallback();
		}
	}
}

int RaknetInstance::peer_send()
{
	bool hasSentPacket = true;
	int sentPacketCnt = 0;
	int maxSentPacketNum = 100;
	if (m_isServer) {
		maxSentPacketNum = 500;
	}

	do {
		hasSentPacket = m_asyncCall.realCall();
	} while (hasSentPacket && ++sentPacketCnt < maxSentPacketNum);

	return sentPacketCnt;
}

int RaknetInstance::peer_recvive()
{
	ui64 sessionId = 0;
	int handlePk = 0;
	int maxHandlePkNum = 100;
	if (m_isServer) {
		maxHandlePkNum = 500;
	}

	Packet *pk = m_pRakpeer->Receive();
	while(pk)
	{
		sessionId = SystemAddress::ToInteger(pk->systemAddress);
		switch (pk->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			onConnectResult(pk->systemAddress, true);
			LordLogInfo("recv raknet message ID_CONNECTION_REQUEST_ACCEPTED, sessionId=" U64FMT, sessionId);
			break;

		case ID_CONNECTION_LOST://Time Out
			onConnectTimeout(pk->systemAddress);
			LordLogInfo("recv raknet message ID_CONNECTION_LOST, sessionId=" U64FMT, sessionId);
			break;

		case ID_DISCONNECTION_NOTIFICATION: // server kickOut
			onDisconnect(pk->systemAddress);
			LordLogInfo("recv raknet message ID_DISCONNECTION_NOTIFICATION, sessionId=" U64FMT, sessionId);
			break;

		case ID_NEW_INCOMING_CONNECTION:
			onNewIncomingConn(pk->systemAddress);
			LordLogInfo("recv raknet message ID_NEW_INCOMING_CONNECTION, sessionId=" U64FMT, sessionId);
			break;

		case ID_CONNECTION_ATTEMPT_FAILED:
			onConnectResult(pk->systemAddress, false);
			LordLogInfo("recv raknet message ID_CONNECTION_ATTEMPT_FAILED, sessionId=" U64FMT, sessionId);
			break;

		case emNormalData:
			handleNormalData(pk->systemAddress, pk);
			break;

		case emCompressData:
			handleCompressData(pk->systemAddress, pk);
			break;

		default:
			LordLogWarning("recv raknet message unknown, type=%d, sessionId=" U64FMT, int(pk->data[0]), sessionId);
			onDisconnect(pk->systemAddress);
			break;
		}

		m_pRakpeer->DeallocatePacket(pk);
		handlePk++;

		if (handlePk >= maxHandlePkNum) {
			break;
		}
		pk = m_pRakpeer->Receive();
	}

	return handlePk;

}


void  RaknetInstance::recv(std::function<void(std::shared_ptr<DataPacket>)> handler, bool useThreadPool)
{
	this->m_handleDataCallBack = handler;
}

void RaknetInstance::onConnectResult(SystemAddress &addr,bool ret)
{
	ui64 sock = INVALID_RAKSOCKET;
	if(ret)
	{
		std::shared_ptr<Session> session(new Session());
		session->remote = addr;
		session->isDisconnect = false;

		sock = addr.ToInteger(addr);
		m_sessionList[sock] = session;

		ConnectEvent connEvvent(sock, emConnectSuc);
		m_connectEventcallback(connEvvent);

	}
	else
	{
		ConnectEvent connEvvent(sock, emConnectFailed);
		m_connectEventcallback(connEvvent);
	}


}

void RaknetInstance::onConnectTimeout(SystemAddress &addr)
{
	std::lock_guard<std::mutex> lk(m_sessionListMut);

	ui64 sock = addr.ToInteger(addr);

	auto session = m_sessionList.find(sock);
	if (session != m_sessionList.end())
	{
		m_sessionList.erase(session);
	}

	ConnectEvent connEvvent(sock, emConnectTimeout);
	m_connectEventcallback(connEvvent);
}

void RaknetInstance::onConnectKickOut(SystemAddress & addr)
{
	std::lock_guard<std::mutex> lk(m_sessionListMut);
	ui64 sock = addr.ToInteger(addr);

	auto session = m_sessionList.find(sock);
	if (session != m_sessionList.end())
	{
		m_sessionList.erase(session);
	}

	ConnectEvent connEvvent(sock, emConnectKickOut);
	m_connectEventcallback(connEvvent);
}

void RaknetInstance::onDisconnect(SystemAddress &addr)
{
	std::lock_guard<std::mutex> lk(m_sessionListMut);
	ui64 sock = addr.ToInteger(addr);

	auto session = m_sessionList.find(sock);
	if(session != m_sessionList.end())
	{
		m_sessionList.erase(session);
	}

	ConnectEvent connEvvent(sock, emDisconnect);
	m_connectEventcallback(connEvvent);
}

void RaknetInstance::onNewIncomingConn(SystemAddress &addr)
{
	std::unique_ptr<Session> session(new Session());
	session->remote = addr;
	session->isDisconnect = false;

	ui64 sock = addr.ToInteger(addr);
	m_sessionList[sock] = std::move(session);

	ConnectEvent connEvvent(sock, emNewConnect);
	m_connectEventcallback(connEvvent);
}

#define  GET_PK_ID_compress(data) ntohs(((ui16*)(data))[0])
void RaknetInstance::handleCompressData( SystemAddress &addr, Packet *data)
{
	this->regReciveTime(data->systemAddress);

	uint32_t nextByte = 1;
	while (nextByte < data->length)
	{
		std::shared_ptr<DataPacket> pDataPacket(
			DataPacketFactory::getInstance(m_isServer)->create(GET_PK_ID_compress(data->data + nextByte)));

		nextByte = nextByte + 2;
		int dataLength = ntohs (*(short*)(data->data+nextByte));
		nextByte = nextByte + 2;

		pDataPacket->Write((const char *)(data->data + nextByte), dataLength);
		pDataPacket->decode();
		pDataPacket->setSender(data->systemAddress.ToInteger(data->systemAddress));
		if (m_handleDataCallBack)
		{
			m_handleDataCallBack(pDataPacket);
		}

		nextByte = nextByte + dataLength;
	}
}

static std::string netDataToHex(const char *data, int len)
{
	std::string rt;
	for (int i = 0; i < len; i++)
	{
		char buf[10];
		memset(buf, 0, 10);
		sprintf(buf, " %x", data[i]);

		rt = rt + buf;
	}

	return rt;
}



#define  GET_PK_ID(data) ntohs(((ui16*)(data +1))[0])
void RaknetInstance::handleNormalData(SystemAddress &addr, Packet *data)
{
	this->regReciveTime(data->systemAddress);
	std::shared_ptr<DataPacket> pDataPacket(DataPacketFactory::getInstance(m_isServer)->create(GET_PK_ID(data->data)));
	if (!pDataPacket)
		return;

	pDataPacket->Write((const char *)(data->data)+3, data->length - 3);

	try 
	{
		pDataPacket->decode();
	}
	catch (std::exception e)
	{
		LordLogError("packet decode error, rakid: %d, id: %d, error: %s ", int(addr.ToInteger(addr)), GET_PK_ID(data->data), e.what());
		
		LordLogError("packet data=[%s]", netDataToHex((const char*)data->data, data->length).c_str());

		ui64 sock = addr.ToInteger(addr);

		auto session = m_sessionList.find(sock);
		if (session != m_sessionList.end())
		{
			session->second->isException = true;
		}
		return;
	}

	count_recv_packet_num(pDataPacket, addr.ToInteger(addr));
	pDataPacket->setSender(data->systemAddress.ToInteger(data->systemAddress));
	if(m_handleDataCallBack)
	{
		m_handleDataCallBack(pDataPacket);
	}
}

bool RaknetInstance::isAlive(ui64 sock)
{
	std::lock_guard<std::mutex> lk(m_sessionListMut);

	auto session = m_sessionList.find(sock);
	if (session != m_sessionList.end() && Root::Instance())
	{
		if (session->second->lastRecviePKTime == 0 ||
			Root::Instance()->getCurrentTime() - session->second->lastRecviePKTime < m_pRakpeer->GetTimeoutTime(UNASSIGNED_SYSTEM_ADDRESS) * 2)
		{
			return true;
		}
	}

	return false;
}

void RaknetInstance::regReciveTime(SystemAddress & addr)
{
	ui64 sock = addr.ToInteger(addr);

	auto session = m_sessionList.find(sock);
	if (session != m_sessionList.end() && Root::Instance())
	{
			session->second->lastRecviePKTime = Root::Instance()->getCurrentTime();
	}
}

void RaknetInstance::printColletingPacket()
{
	LordLogInfo("###########################################################");
	LordLogInfo("#################send packet count begin###################");
	LordLogInfo("## Total number of sending packet: %d", send_total_packet_cnt);
	LordLogInfo("## Total size(byte) of sending packet: %d", send_total_packet_byte);
	for (auto it : send_type_cnt_byte_map) {
		LordLogInfo("##Sending Packet Id %3d --> Total Number: %8d, Total Size: %10d, Percentage: %0.5f", it.first, it.second.first, it.second.second, it.second.second / (float)send_total_packet_byte);
	}
	for (auto it : send_user_cnt_map) {
		LordLogInfo("---send user %lld --> %d", it.first, it.second);
	}
	LordLogInfo("################send packet count end######################");
	LordLogInfo("###########################################################\n");

	LordLogInfo("----------I---am---a---dividing---line--------------------\n");

	LordLogInfo("###########################################################");
	LordLogInfo("#################recv packet count begin###################");
	LordLogInfo("## Total number of receiving packet: %d", recv_total_packet_cnt);
	LordLogInfo("## Total size(byte) of receiving packet: %d", recv_total_packet_byte);
	for (auto it : recv_type_cnt_byte_map) {
		LordLogInfo("##Receiving Packet Id %3d --> Total Number: %8d Total Size: %10d, Percentage: %0.5f", it.first, it.second.first, it.second.second, it.second.second / (float)recv_total_packet_byte);
	}
	for (auto it : recv_user_cnt_map) {
		LordLogInfo("---send user %lld --> %d", it.first, it.second);
	}
	LordLogInfo("################recv packet count end######################");
	LordLogInfo("###########################################################\n");
}


