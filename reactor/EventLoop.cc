#include "EventLoop.h"

#include "Channel.h"
#include "Poller.h"


#include "../logging/Logging.h"

#include <assert.h>
#include <poll.h>

using namespace muduo;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop():
    m_looping(false),
    m_threadId(CurrentThread::tid())
{
    LOG_TRACE << "EventLoop created" << this << " in thread " << m_threadId;
    if (t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread 
                  << " exists in this thread" << m_threadId;
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() 
{
    assert(!m_looping);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!m_looping);
    assertInLoopThread();
    m_looping = true;

    ::poll(NULL, 0, 5 * 1000);

    LOG_TRACE << "EventLoop " << this << " stop looping";
    m_looping = false;
}

void EventLoop::abortNotInLoopTread() 
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId = " << m_threadId
              << ", current thread id = " << CurrentThread::tid();
}


















