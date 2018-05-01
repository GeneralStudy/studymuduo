#ifndef MUDUO_NET_TIMEQUEUE_H
#define MUDUO_NET_TIMEQUEUE_H

#include <set>
#include <vector>
#include <boost/noncopyable.hpp>

#include "../datetime/Timestamp.h"
#include "../thread/Mutex.h"
#include "Callbacks.h"
#include "Channel.h"

namespace muduo 
{

class EventLoop;
class Timer;
class TimerId;

class TimerQueue: boost::noncopyable
{

public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb, Timestamp when, double inteval);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    void handleRead();
    
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);
    bool insert(Timer* timer);

    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerfdChannel;
    TimerList m_timers;

};

}

#endif