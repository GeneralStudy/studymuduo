#include "Poller.h"
#include "Channel.h"
#include "../logging/Logging.h"

#include <assert.h>
#include <poll.h>

using namespace muduo;

Poller::Poller(EventLoop* loop): 
    m_ownerLoop(loop)
{

}

Poller::~Poller()
{

}

Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::poll(&*m_pollfds.begin(), m_pollfds.size(), timeoutMs);
    Timestamp now(Timestamp::now());

    if (numEvents > 0) {
        LOG_TRACE << numEvents << " events happend";
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        LOG_TRACE << " nothing happended";
    } else {
        LOG_SYSERR << "Poller::poll";
    }

    return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const 
{
    for (PollFdList::const_iterator pfd = m_pollfds.begin(); 
        pfd != m_pollfds.end() && numEvents > 0;
        ++pfd)
    {
        if (pfd->revents > 0) {
            --numEvents;
            
        }
    }
}
