#ifndef ASYNC_CLIENT_H
#define ASYNC_CLIENT_H

#include "CoreDef.h"
#if(LORD_PLATFORM == LORD_PLATFORM_LINUX)
#include <sys/epoll.h>
#endif

#include <string>
#include "buffer.h"

class AsyncClient;

class AsyncClientCallback {
public:
    virtual void onConnected(AsyncClient* client) = 0;
    virtual ~AsyncClientCallback(){}
};

#if(LORD_PLATFORM == LORD_PLATFORM_LINUX || LORD_PLATFORM == LORD_PLATFORM_MAC_OSX)

class AsyncClient {
public:
    AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback);
    ~AsyncClient();
    
    bool read(vcs::Buffer* buffer);
    bool write(const char* data, size_t count);
    void dispatch();
	std::string getHost() { return m_host; }
	unsigned short getPort() { return m_port; }

private:
    void _connect();
    void _close();
    
private:
    std::string m_host;
    unsigned short m_port;
    AsyncClientCallback* m_callback = nullptr;

    int m_epollFd   = -1;
    int m_socketFd  = -1;
    bool m_connected = false;
    time_t m_connectBeginTime = 0;

    vcs::Buffer m_readBuffer;
    vcs::Buffer m_writeBuffer;

};

#else


#include <WinSock2.h>

class AsyncClient {
public:
    AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback);
    ~AsyncClient();
    
    bool read(vcs::Buffer* buffer);
    bool write(const char* data, size_t count);
    void dispatch();
	std::string getHost() { return m_host; }
	unsigned short getPort() { return m_port; }

private:
	bool _write(int fd, vcs::Buffer* buffer);
	bool _read(int fd, vcs::Buffer* buffer);

private:
	std::string m_host;
	unsigned short m_port;
	AsyncClientCallback* m_callback = nullptr;

	SOCKET      m_socketFd = INVALID_SOCKET;

	vcs::Buffer m_readBuffer;
	vcs::Buffer m_writeBuffer;

	bool m_lastConnected = false;
	bool m_connected = false;
};

#endif


#endif


