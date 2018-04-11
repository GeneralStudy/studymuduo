#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "../thread/Thread.h"

#include <boost/scoped_ptr.hpp>
#include <vector>

namespace muduo 
{

class Channel;
class Poller;

class EventLoop: boost::noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void updateChannel(Channel* channel);
    void assertInLoopThread() 
    {
        if (!isInLoopThread()) {
            abortNotInLoopTread();
        }
    }

    bool isInLoopThread() const 
    {
        return m_threadId == CurrentThread::tid();
    }

private:
    typedef std::vector<Channel*> ChannelList;


    void abortNotInLoopTread();
    bool m_looping;
    bool m_quit;
    const pid_t m_threadId;

    boost::scoped_ptr<Poller> m_poller;
    ChannelList m_activeChannel;
    
};

}

#endif