#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H

#include <boost/noncopyable.hpp>

#include "../datetime/Timestamp.h"
#include "Callbacks.h"

namespace muduo 
{

class Timer: boost::noncopyable
{
public:
    Timer(const TimerCallback& cb, Timestamp when, double interval):
        m_callback(cb),
        m_expiration(when),
        m_inteval(interval),
        m_repeat(interval > 0.0)
    {

    }

    void run() const 
    {
        m_callback();
    }

    Timestamp expiration() const { return m_expiration; }
    bool repeat() const { return m_repeat; }
    void restart(Timestamp now);

private:
    const TimerCallback m_callback;
    Timestamp m_expiration;
    const double m_inteval;
    const bool m_repeat;

};

}

#endif