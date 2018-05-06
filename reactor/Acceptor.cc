#include "Acceptor.h"
#include "../logging/Logging.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketOps.h"

#include <boost/bind.hpp>

using namespace muduo;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr):
    m_loop(loop),
    m_acceptSocket(sockets::createNonblockingOrDie()),
    m_acceptChannel(loop, m_acceptSocket.fd()),
    m_listening(false)
{
    m_acceptSocket.setReuseAddr(true);
    m_acceptSocket.bindAddress(listenAddr);
    
    m_acceptChannel.setReadCallBack(boost::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen()
{
    m_loop->assertInLoopThread();
    m_listening = true;

    m_acceptSocket.listen();
    m_acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
    m_loop->assertInLoopThread();
    InetAddress peerAddr(0);

    int connfd = m_acceptSocket.accept(&peerAddr);
    if (connfd >= 0) {
        if (m_newConnectionCallback) {
            m_newConnectionCallback(connfd, peerAddr);
        } else {
            sockets::close(connfd);
        }
    }
}