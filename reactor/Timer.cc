#include "Timer.h"

using namespace muduo;

void Timer::restart(Timestamp now)
{
    if (m_repeat) {
        m_expiration = addTime(now, m_inteval);
    } else {
        m_expiration = Timestamp::invalid();
    }
}