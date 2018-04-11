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
            ChannelMap::const_iterator ch = m_channels.find(pfd->fd);
            assert(ch != m_channels.end());

            Channel* channel = ch->second;
            assert(channel->fd() == pfd->fd);

            channel->set_revents(pfd->revents);

            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel* channel)
{
    assertInLoopThread();

    LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
    if (channel->index() < 0) {
        assert(m_channels.find(channel->fd()) == m_channels.end());

        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        m_pollfds.push_back(pfd);

        int idx = static_cast<int>(m_pollfds.size()) - 1;
        channel->set_index(idx);
        m_channels[pfd.fd] = channel;
    } else {
        assert(m_channels.find(channel->fd()) != m_channels.end());
        assert(m_channels[channel->fd()] == channel);

        int idx = channel->index();
        assert(0 <= idx && idx < static_cast<int>(m_pollfds.size()));

        struct pollfd& pfd = m_pollfds[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == 1);

        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            pfd.fd = 1;
        }
    }
}
