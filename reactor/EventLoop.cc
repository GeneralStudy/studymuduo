#include "EventLoop.h"

#include "Channel.h"
#include "Poller.h"


#include "../logging/Logging.h"

#include <assert.h>
#include <poll.h>

using namespace muduo;

__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop::EventLoop():
    m_looping(false),
    m_quit(false),
    m_threadId(CurrentThread::tid()),
    m_poller(new Poller(this))
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
    m_quit = false;

    while (!m_quit) 
    {
        m_activeChannel.clear();
        m_poller->poll(kPollTimeMs, &m_activeChannel);
        for (ChannelList::iterator it = m_activeChannel.begin();
            it != m_activeChannel.end(); 
            ++it)
        {
            (*it)->handleEvent();
        }
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    m_looping = false;
}

void EventLoop::quit()
{
    m_quit = true;
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();

    m_poller->updateChannel(channel);
}

void EventLoop::abortNotInLoopTread() 
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId = " << m_threadId
              << ", current thread id = " << CurrentThread::tid();
}


















