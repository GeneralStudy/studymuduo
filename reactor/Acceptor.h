#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "Channel.h"
#include "Socket.h"

namespace muduo 
{

class EventLoop;
class InetAddress;

class Acceptor: boost::noncopyable 
{

public: 
    typedef boost::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr);
    void setNewConnectionCallback(const NewConnectionCallback& cb) { m_newConnectionCallback = cb; }

    bool listenning() const { return m_listening; }
    void listen();

private: 
    void handleRead();

    EventLoop* m_loop;
    Socket m_acceptSocket;
    Channel m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback;
    bool m_listening;
};

}

#endif