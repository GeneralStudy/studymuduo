#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace muduo 
{

class EventLoop;

class Channel: boost::noncopyable 
{
public:
    typedef boost::function<void()> EventCallback;

    Channel(EventLoop* loop, int fd);


    void handleEvent();
    void setReadCallBack(const EventCallback& cb)
        { m_readCallBack = cb; }
    void setWriteCallBack(const EventCallback& cb)
        { m_writeCallBack = cb; }
    void setErrorCallBack(const EventCallback& cb) 
        { m_errorCallBack = cb; }

    int fd() const { return m_fd; }
    int events() const { return m_events; }
    void set_revents(int rect) { m_revents = rect; }
    bool isNoneEvent() const { return m_events == kNoneEvent; }

    void enableReading() 
    { 
        m_events |= kReadEvent;
        update();
    }

    int index() { return m_index; }
    void set_index(int idx) { m_index = idx; }

    EventLoop* ownerLoop() { return m_loop; }


private: 
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* m_loop;
    const int m_fd;
    int m_events;
    int m_revents;
    int m_index;

    EventCallback m_readCallBack;
    EventCallback m_writeCallBack;
    EventCallback m_errorCallBack;

};


}

#endif