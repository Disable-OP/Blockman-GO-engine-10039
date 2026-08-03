#include "ClientSocket.h"

#define TIME_OUT_TIME 1

namespace LORD
{
	ClientSocket::ClientSocket(SOCKET socket_)
		: mSocket(socket_)
	{
		memset(&mAddr, 0, sizeof(SOCKADDR_IN));
		mAddr.sin_family = AF_INET;
	}

	ClientSocket::~ClientSocket()
	{
		Close();
	}

	void ClientSocket::setIP(Dword ip, Dword port)
	{
		mAddr.sin_addr.s_addr = htonl(ip);
		mAddr.sin_port = htons(port);
	}

	void ClientSocket::setIP(const String& ipPortStr)
	{
		StringArray ipPortStrArr = StringUtil::Split(ipPortStr, ":");
		if(ipPortStrArr.size() == 2)
		{
			mAddr.sin_addr.s_addr = inet_addr(ipPortStrArr[0].c_str());
			//inet_pton(AF_INET, ipPortStrArr[0].c_str, (void *)&mAddr.sin_addr.s_addr);
			Dword port = StringUtil::ParseI32(ipPortStrArr[1]);
			mAddr.sin_port = htons(port);
		}
	}

	void ClientSocket::setIP(const String& ipStr, Dword port)
	{
		mAddr.sin_addr.s_addr = inet_addr(ipStr.c_str());
		mAddr.sin_port = htons(port);
	}

	Dword ClientSocket::getIp() const
	{
		return ntohl(mAddr.sin_addr.s_addr);
	}

	Dword ClientSocket::getPort() const
	{
		return ntohs(mAddr.sin_port);
	}

	String ClientSocket::getIpString(bool appendPort) const 
	{
		const i32 A = IP(mAddr.sin_addr,1);
		const i32 B = IP(mAddr.sin_addr,2);
		const i32 C = IP(mAddr.sin_addr,3);
		const i32 D = IP(mAddr.sin_addr,4);
		if (appendPort)
		{
			return StringUtil::Format("%i.%i.%i.%i:%i", A, B, C, D, getPort());
		}
		else
		{
			return StringUtil::Format("%i.%i.%i.%i", A, B, C, D);
		}
	}

	bool ClientSocket::Close()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		bool ret = closesocket(mSocket) == 0;
#else 
		bool ret = close(mSocket) == 0;
#endif
		mSocket = -1;
		return ret;
	}

	bool ClientSocket::Connect()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
    
        i32 Err = connect(mSocket, (const sockaddr*)&mAddr, sizeof(struct sockaddr_in));
    if (Err == 0)
    {
        return true;
    }
    bool Return = false;

    int code = WSAGetLastError();

    LordLogError("Connect error, error code [%d]", code);

    Return = Err == WSAEWOULDBLOCK ||
        (Err == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK);
    return Return;
        
#else

#if LORD_PLATFORM == LORD_PLATFORM_MAC_IOS
        SetNonBlocking(true);
#endif
        int error = -1, len;
        timeval tm;
        fd_set set;
        unsigned long ul = 1;
        
        bool ret = false;
        if( connect(mSocket, (const sockaddr*)&mAddr, sizeof(struct sockaddr_in)) == -1)
        {
            tm.tv_sec  = TIME_OUT_TIME;
            tm.tv_usec = 0;
            FD_ZERO(&set);
            FD_SET(mSocket, &set);
            if( select(mSocket+1, NULL, &set, NULL, &tm) > 0)
            {
                getsockopt(mSocket, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
                if(error == 0) ret = true;
                else ret = false;
            } else ret = false;
        }
        else
        {
            ret = true;
        }
        
        ul = 0;
        
        SetNonBlocking(false); //设置为阻塞模式
        
        return ret;

#endif
        
	}

	bool ClientSocket::HasPendingData(ui32& PendingDataSize)
	{
		bool bHasSucceeded = false;
		PendingDataSize = 0;

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		TIMEVAL Time = {0,0};
#else
		struct timeval Time = {0,0};
#endif

		fd_set SocketSet;
		FD_ZERO(&SocketSet);
		FD_SET(mSocket, &SocketSet);
		i32 SelectStatus = select(mSocket + 1,&SocketSet,NULL,NULL,&Time);
		if (SelectStatus > 0)
		{
			i32 pending;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			if (ioctlsocket(mSocket, FIONREAD, (ULONG*)&pending ) == 0)
#else
			if(ioctl(mSocket, FIONREAD, &pending) == 0)
#endif
			{
				PendingDataSize = (ui32)pending;
				bHasSucceeded = true;
			}
		}
		return bHasSucceeded;
	}

	bool ClientSocket::Send(const Byte* Data, i32 Count, i32& BytesSent)
	{
		do 
		{
			BytesSent = send(mSocket,(const char*)Data,Count,0);
		} while ( BytesSent == SOCKET_ERROR && 
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			WSAGetLastError() == WSAEWOULDBLOCK );
#else 
			errno == EAGAIN );
#endif
		return BytesSent != SOCKET_ERROR;
	}

	bool ClientSocket::Recv(Byte* Data, i32 BufferSize,i32& BytesRead)
	{
		BytesRead = recv(mSocket,(char*)Data,BufferSize,0);

		return BytesRead >= 0;
	}

	bool ClientSocket::SetNonBlocking(bool bIsNonBlocking)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		DWORD Value = bIsNonBlocking ? TRUE : FALSE;
		return ioctlsocket(mSocket,FIONBIO,&Value) == 0;
#else
		int Flags;
		Flags = fcntl(mSocket, F_GETFL, 0);
		if (bIsNonBlocking)
			Flags |= O_NONBLOCK;
		else
			Flags &= ~O_NONBLOCK;
		return fcntl(mSocket, F_SETFL, Flags) == 0;
#endif
	}

	bool ClientSocket::SetReuseAddr(bool bAllowReuse)
	{
		const int bAllow = (int) bAllowReuse;
		return setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bAllow, sizeof(bAllow)) == 0;
	}

	bool ClientSocket::SetReceiveBufferSize(i32 Size, i32& NewSize)
	{
		i32 sizeSize = sizeof(i32);
		bool bOk = setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&Size, sizeof(i32)) == 0;

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&NewSize, &sizeSize);
#else
		getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&NewSize, (socklen_t*)&sizeSize);
#endif
		return bOk;
	}

	bool ClientSocket::SetSendBufferSize(i32 Size, i32& NewSize)
	{
		i32 sizeSize = sizeof(i32);
		bool bOk = setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&Size, sizeof(i32)) == 0;

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&NewSize, &sizeSize);
#else
		getsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&NewSize, (socklen_t*)&sizeSize);
#endif

		return bOk;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------------------------------------------------
	ClientSocketManager* ClientSocketManager::Instance()
	{
		return ms_pSingleton;
	}

	ClientSocketManager::ClientSocketManager()
	{
	}

	ClientSocketManager::~ClientSocketManager()
	{
	}

	bool ClientSocketManager::initialize()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		WSADATA WSAData;
		i32 Code = WSAStartup(0x0101, &WSAData);
		if (Code == 0)
		{
			LordLogInfo("WinSock: version %i.%i (%i.%i), MaxSocks=%i, MaxUdp=%i",
				WSAData.wVersion >> 8,WSAData.wVersion & 0xFF,
				WSAData.wHighVersion >> 8,WSAData.wHighVersion & 0xFF,
				WSAData.iMaxSockets,WSAData.iMaxUdpDg);
			return true;
		}
		return false;
#else
		return true;
#endif
	}

	void ClientSocketManager::destroy()
	{
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		WSACleanup();
#endif
	}

	ClientSocket* ClientSocketManager::createClientSocket()
	{
		ClientSocket* ret = NULL;
		SOCKET Socket = (SOCKET)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(Socket != INVALID_SOCKET)
		{
			ret = LordNew ClientSocket(Socket);
		}
		return ret;
	}


	void ClientSocketManager::destroyClientSocket(ClientSocket* socket_)
	{
		LordDelete socket_;
	}

	i32 ClientSocketManager::getLastErrorCode()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return WSAGetLastError();
#else
		return errno;
#endif
	}

}
