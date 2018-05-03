#ifndef MUDUO_NET_INETADDRESS_H
#define MUDUO_NET_INETADDRESS_H

#include "../datetime/copyable.h"
#include "string"
#include <netinet/in.h>

namespace muduo 
{

class InetAddress: public muduo::copyable
{

public: 
    explicit InetAddress(uint16_t port);
    InetAddress(const std::string& ip, uint16_t port);
    InetAddress(const struct sockaddr_in& addr):m_addr(addr) {}

    std::string toHostPort() const;
    
    const struct sockaddr_in& getSockAddrInet() const { return m_addr; }
    void setSockAddrInet(const struct sockaddr_in& addr) { m_addr = addr; }


private: 
    struct sockaddr_in m_addr;

};

}

#endif 