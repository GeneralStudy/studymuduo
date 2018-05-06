#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "../datetime/Timestamp.h"
#include "../thread/Mutex.h"
#include "../thread/Thread.h"
#include "Callbacks.h"
#include "TimerId.h"

#include <boost/scoped_ptr.hpp>
#include <vector>

namespace muduo 
{

class Channel;
class Poller;
class TimerQueue;

class EventLoop: boost::noncopyable
{
public:
    typedef boost::function<void()> Functor;

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

    Timestamp pollReturnTime() const { return m_pollReturnTime; }
    TimerId runAt(const Timestamp& time, const TimerCallback& cb);
    TimerId runAfter(double delay, const TimerCallback& cb);
    TimerId runEvery(double interval, const TimerCallback& cb);

    void runInLoop(const Functor& cb);

private:
    typedef std::vector<Channel*> ChannelList;


    void abortNotInLoopTread();
    bool m_looping;
    bool m_quit;
    const pid_t m_threadId;

    boost::scoped_ptr<Poller> m_poller;
    ChannelList m_activeChannel;

    Timestamp m_pollReturnTime;
    boost::scoped_ptr<TimerQueue> m_timerQueue;
};

}

#endif