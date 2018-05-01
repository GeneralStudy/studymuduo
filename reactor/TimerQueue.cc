#define __STDC_LIMIT_MACROS
#include "TimerQueue.h"

#include "../logging/Logging.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"

#include <boost/bind.hpp>
#include <sys/timerfd.h>

namespace muduo 
{

namespace detail 
{

int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        LOG_SYSFATAL << "Failed in timerfd_create";
    }

    return timerfd;
}   

struct timespec howMuchTimeFromNow(Timestamp when) 
{
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);

    return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));

    LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
    if (n != sizeof(howmany)) {
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));

    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        LOG_SYSERR << "timerfd_settime()";
    }
}

}

}

using namespace muduo;
using namespace muduo::detail;


TimerQueue::TimerQueue(EventLoop* loop):
    m_loop(loop),
    m_timerfd(createTimerfd()),
    m_timerfdChannel(loop, m_timerfd),
    m_timers()
{
    m_timerfdChannel.setReadCallBack(boost::bind(&TimerQueue::handleRead, this));
    m_timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue() 
{
    ::close(m_timerfd);
    for (TimerList::iterator it = m_timers.begin(); it != m_timers.end(); ++it) {
        delete it->second;
    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(cb, when, interval);
    m_loop->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged) {
        resetTimerfd(m_timerfd, timer->expiration());
    }

    return TimerId(timer);
}

void TimerQueue::handleRead()
{
    m_loop->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(m_timerfd, now);

    std::vector<Entry> expired = getExpired(now);
    for (std::vector<Entry>::iterator it = expired.begin(); it != expired.end(); ++it) {
        it->second->run();
    }

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = m_timers.lower_bound(sentry);

    assert(it == m_timers.end() || now < it->first);

    std::copy(m_timers.begin(), it, back_inserter(expired));
    
    m_timers.erase(m_timers.begin(), it);

    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now) 
{
    Timestamp nextExpire;

    for (std::vector<Entry>::const_iterator it = expired.begin(); it != expired.end(); ++it) {
        if (it->second->repeat()) {
            it->second->restart(now);
            insert(it->second);
        } else {
            delete it->second;
        }
    }

    if (!m_timers.empty()) {
        nextExpire = m_timers.begin()->second->expiration();
    }

    if (nextExpire.valid()) {
        resetTimerfd(m_timerfd, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer) 
{
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = m_timers.begin();
    if (it == m_timers.end() || when < it->first) {
        earliestChanged = true;
    }

    std::pair<TimerList::iterator, bool> result = m_timers.insert(std::make_pair(when, timer));

    assert(result.second);

    return earliestChanged;
}