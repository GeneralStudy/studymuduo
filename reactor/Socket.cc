#include "Socket.h"
#include "InetAddress.h"
#include "SocketOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

using namespace muduo;

Socket::~Socket()
{
    sockets::close(m_sockfd);
}

void Socket::bindAddress(const InetAddress& addr)
{
    sockets::bindOrDie(m_sockfd, addr.getSockAddrInet());
}

void Socket::listen()
{
    sockets::listenOrDie(m_sockfd);
}

int Socket::accept(InetAddress* peerAddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(m_sockfd, &addr);
    if (connfd >= 0) {
        peerAddr->setSockAddrInet(addr);
    }

    return connfd;
}

void Socket::setReuseAddr(bool on) 
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}