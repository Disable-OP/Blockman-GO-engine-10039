#include <iostream>
#include "Core.h"
#include "AsyncClient.h"


#if(LORD_PLATFORM == LORD_PLATFORM_LINUX || LORD_PLATFORM == LORD_PLATFORM_ANDROID)

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>
#include <sys/epoll.h>
#include "helper.h"

using namespace LORD;

#define MAX_EVENT_NUM 8
#define EPOLL_WAIT_TIMEOUT 0  // milliseconds, -1 means block indefinitely, 0 means no timeout,
#define MAX_CONNECT_TIME 3 // seconds


AsyncClient::AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback)
    : m_host(host), m_port(port), m_callback(callback)
{
    m_epollFd = ::epoll_create(MAX_EVENT_NUM);
    if (m_epollFd < 0) {
        LordLogError("epoll_create fail, errno=%d, msg=%s", errno, strerror(errno));
        exit(-1);
    }

    _connect();
}

AsyncClient::~AsyncClient() {
    _close();

    ::close(m_epollFd);
    m_epollFd = -1;

        LordSafeDelete(m_callback);
}

void AsyncClient::_connect() {
    if (m_connectBeginTime == 0) {
        m_connectBeginTime = time(NULL);
    }

    m_socketFd = vcs::Helper::Socket::connect(m_host.c_str(), m_port);
    if (m_socketFd < 0) {
                LordLogError("vcs::Helper::Socket::connect fail, errno=%d, msg=%s", errno, strerror(errno));
        return;
    }

    struct epoll_event ev = {0};
    ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
    ev.data.fd = m_socketFd;
    if(epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_socketFd, &ev) < 0) {
                LordLogError("epoll_ctl add fd(%d) fail, errno=%d, msg=%s", m_socketFd, errno, strerror(errno));
        _close();
        return;
    }
    LordLogInfo("vcs::Helper::Socket::connect start, m_socketFd=%d", m_socketFd);
}

void AsyncClient::_close() {
    m_connected = false;
    m_readBuffer.shrink(m_readBuffer.size());
    m_writeBuffer.shrink(m_writeBuffer.size());

    if (m_socketFd < 0) {
        return;
    }
    ::close(m_socketFd);
    m_socketFd = -1;
}

bool AsyncClient::read(vcs::Buffer* buffer) {
    if (!m_connected) {
        return false;
    }
        
        buffer->shrink(buffer->size());
    
    buffer->append(&m_readBuffer);
    m_readBuffer.shrink(m_readBuffer.size());
    return true;
}

bool AsyncClient::write(const char* data, size_t count) {
    if (!m_connected) {
        return false;
    }

    m_writeBuffer.write(data, count);
    if (m_writeBuffer.size() > 0){
        if(!vcs::Helper::Socket::write(m_socketFd, &m_writeBuffer)) {
                        LordLogError("vcs::Helper::Socket::write fail, m_socketFd=%d, errno=%d, msg=%s", m_socketFd, errno, strerror(errno));
            _close();
        }
    }
    return true;
}

void AsyncClient::dispatch() {
    // reconnect if need
    if (m_socketFd < 0 && m_connectBeginTime == 0) {
                LordLogError("connection lost, try to reconnect, host=%s, port=%d", m_host.c_str(), m_port);
        _connect();
    }

    if (m_connectBeginTime != 0 && time(NULL) - m_connectBeginTime > MAX_CONNECT_TIME) {
                LordLogError("can not connect to room server, has tried %d seconds", MAX_CONNECT_TIME);
                m_connectBeginTime = 0;
                _close();
    }

    // epoll loop 
    struct epoll_event events[MAX_EVENT_NUM];
    int nfds = ::epoll_wait(m_epollFd, events, MAX_EVENT_NUM, EPOLL_WAIT_TIMEOUT);
    if (nfds < 0) {
        if (errno != EINTR) {
                        LordLogError("epoll_wait fail, errno=%d, msg=%s", errno, strerror(errno));
        }
        return;
    }

    for (int i = 0; i < nfds; ++i) {
        if (m_socketFd != events[i].data.fd) {
                        LordLogError("something strange wrong, fd mix, m_socketFd=%d, events[%d].data.fd=%d", m_socketFd, i, events[i].data.fd);
            ::close(events[i].data.fd);
            continue;
        }

        if (events[i].events & EPOLLIN) {
            if (!vcs::Helper::Socket::read(events[i].data.fd, &m_readBuffer)) {
                if (errno != EINPROGRESS) {
                                        LordLogError("vcs::Helper::Socket::read fail, fd=%d, errno=%d, msg=%s", events[i].data.fd, errno, strerror(errno));
                    _close();
                }
                continue;
            }
        }
        
        if (events[i].events & EPOLLOUT) {
            if (!m_connected) {
                int result;
                socklen_t result_len = sizeof(result);
                if ((getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) || (result != 0)) {
                                        LordLogError("connect fail for socket fd %d", events[i].data.fd);
                    _close();
                    continue;
                } else {
                    m_connected = true;
                    m_callback->onConnected(this);
                    m_connectBeginTime = 0;
                }
            }

            if (m_writeBuffer.size() > 0){
                if(!vcs::Helper::Socket::write(events[i].data.fd, &m_writeBuffer)) {
                                        LordLogError("vcs::Helper::Socket::write fail, m_socketFd=%d, errno=%d, msg=%s", m_socketFd, errno, strerror(errno));
                    _close();
                }
            }
        }
    }
}

#else

using namespace LORD;
AsyncClient::AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback)
        : m_host(host), m_port(port), m_callback(callback)
{
        WSADATA wsaData;
        int iResult;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
                LordLogError("WSAStartup failed: %d on windows, abort", iResult);
                std::abort();
        }

        m_socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socketFd == INVALID_SOCKET) {
                LordLogError("Error at socket(): %ld on windows, abort", WSAGetLastError());
                std::abort();
        }

        SOCKADDR_IN server;
        memset(&server, 0, sizeof(SOCKADDR_IN));
        server.sin_family = AF_INET;
        server.sin_addr.S_un.S_addr = inet_addr(m_host.c_str());
        server.sin_port = htons(m_port);

        iResult = connect(m_socketFd, (struct sockaddr *)&server, sizeof(SOCKADDR_IN));
        if (iResult == SOCKET_ERROR) {
                LordLogError("connect to %s:%d failed on windows, abort", m_host.c_str(), (int32_t)m_port);
                std::abort();
        }

        m_connected = true;

        unsigned long ul = 1;
        iResult = ioctlsocket(m_socketFd, FIONBIO, &ul); //ÉèÖÃÎª·Ç×èÈûÄ£Ê½
        if (iResult != NO_ERROR) {
                LordLogError("ioctlsocket failed with error: %ld on windows, abort", iResult);
                std::abort();
        }
}

AsyncClient::~AsyncClient() {
        closesocket(m_socketFd);
        WSACleanup();

        m_readBuffer.shrink(m_readBuffer.size());
        m_writeBuffer.shrink(m_writeBuffer.size());
        LordSafeDelete(m_callback);
}

bool AsyncClient::_read(int fd, vcs::Buffer* buffer)
{
        char buf[1024];
        int curr_read = 0;
        int total_read = 0;
        while (true) {
                curr_read = recv(fd, buf, 1024, 0);
                if (curr_read == SOCKET_ERROR || curr_read == 0) {
                        break;
                }
                buffer->write(buf, curr_read);
                total_read += curr_read;
        }

        if ((curr_read == SOCKET_ERROR)) {
                int iError = WSAGetLastError();
                if (iError == WSAEWOULDBLOCK) {
                        return true;
                }
        }

        if ((curr_read == SOCKET_ERROR) || (total_read == 0)) {
                return false;
        }
        return true;
}

bool AsyncClient::read(vcs::Buffer* buffer) {
        buffer->shrink(buffer->size());

        buffer->append(&m_readBuffer);
        m_readBuffer.shrink(m_readBuffer.size());
        return true;
}

bool AsyncClient::_write(int fd, vcs::Buffer* buffer)
{
        bool blocked = false;
        char buf[1024];
        int curr_read = 0;
        while (!blocked && ((curr_read = buffer->peek(buf, 1024)) > 0)) {
                int n_written = 0;
                int n_left = curr_read;
                while (n_left > 0) {
                        int written = send(fd, buf + n_written, n_left, 0);
                        if (written == SOCKET_ERROR) {
                                int iError = WSAGetLastError();
                                if (iError == WSAEWOULDBLOCK) {
                                        blocked = true;
                                        break;
                                }
                                else {
                                        LordLogError("send fail with error %d", iError);
                                        return false;
                                }
                        }
                        else {
                                if (written > 0) {
                                        n_written += written;
                                        n_left -= written;
                                }
                        }
                }

                if (n_written > 0) {
                        buffer->shrink(n_written);
                }
        }
        return true;
}

bool AsyncClient::write(const char* data, size_t count) {
        m_writeBuffer.write(data, count);
        return true;
}

void AsyncClient::dispatch() {
        if (!m_connected) {
                return;
        }

        if (!m_lastConnected && m_connected) {
                m_callback->onConnected(this);
                m_lastConnected = true;
        }

        if (!_read(m_socketFd, &m_readBuffer)) {
                m_connected = false;
                //LordLogError("in dispatch, AsyncClient::_read fail on windows, abort");
                //std::abort();
        }

        if (!_write(m_socketFd, &m_writeBuffer)) {
                m_connected = false;
                //LordLogError("in dispatch, AsyncClient::_write fail on windows, abort");
                //std::abort();
        }
}

#endif
