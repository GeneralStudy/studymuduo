#ifndef MUDUO_NET_SOCKET_H
#define MUDUO_NET_SOCKET_H

#include <boost/noncopyable.hpp>

namespace muduo 
{

class InetAddress;

class Socket: boost::noncopyable
{

public:
    explicit Socket(int sockfd): m_sockfd(sockfd) {}
    ~Socket();

    int fd() const { return m_sockfd; }
    void bindAddress(const InetAddress& localaddr);
    void listen();

    int accept(InetAddress* peerAddr);

    void setReuseAddr(bool on);

private:
    const int m_sockfd;

};

}

#endif