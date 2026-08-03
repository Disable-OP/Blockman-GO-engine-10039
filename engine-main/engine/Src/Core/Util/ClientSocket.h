/********************************************************************
filename: 	ClientSocket.h
file path:	dev\engine\Src\Core\Util\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include "CoreDef.h"
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include <winsock2.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)
#endif

#include "Core.h"
#include "Object/Singleton.h"
#include "Util/StringUtil.h"

namespace LORD
{

	typedef int					SOCKET;
	typedef struct hostent		HOSTENT;
	typedef in_addr				IN_ADDR;
	typedef struct sockaddr		SOCKADDR;
	typedef struct sockaddr_in	SOCKADDR_IN;
	typedef struct linger		LINGER;

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define IP(sin_addr,n) sin_addr.S_un.S_un_b.s_b##n
#else
#define IP(sin_addr,n) ((Byte*)&sin_addr.s_addr)[n-1]
#endif

	// just used for game client.
	class LORD_CORE_API ClientSocket : public ObjectAlloc
	{
	protected:
		SOCKADDR_IN mAddr;
		SOCKET  mSocket;

	public:
		ClientSocket(SOCKET socket_);
		virtual ~ClientSocket();

		void setIP(Dword ip, Dword port);
		void setIP(const String& ipPortStr);
		void setIP(const String& ipStr, Dword port);

		Dword getIp() const;
		Dword getPort() const;
		String getIpString(bool appendPort) const;

		virtual bool Close(void);

		virtual bool Connect();

		virtual bool HasPendingData(ui32& PendingDataSize);

		virtual bool Send(const Byte* Data, i32 Count, i32& BytesSent);

		virtual bool Recv(Byte* Data, i32 BufferSize, i32& BytesRead);

		virtual bool SetNonBlocking(bool bIsNonBlocking = true);

		virtual bool SetReuseAddr(bool bAllowReuse = true);

		virtual bool SetSendBufferSize(i32 Size, i32& NewSize);

		virtual bool SetReceiveBufferSize(i32 Size, i32& NewSize);
	};

	class LORD_CORE_API ClientSocketManager : public ObjectAlloc, public Singleton<ClientSocketManager>
	{
	public:
		static ClientSocketManager* Instance();

	public:
		ClientSocketManager();
		~ClientSocketManager();

		bool initialize();
		void destroy();
		ClientSocket* createClientSocket();
		void destroyClientSocket(ClientSocket* socket_);
		i32 getLastErrorCode();

	protected:
	};
}

#endif
