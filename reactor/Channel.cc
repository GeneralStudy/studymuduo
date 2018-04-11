#include "Channel.h"
#include "EventLoop.h"
#include "../logging/Logging.h"

#include <sstream>
#include <poll.h>

using namespace muduo;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fdArg):
    m_loop(loop),
    m_fd(fdArg),
    m_events(0),
    m_revents(0),
    m_index(-1)
{

}

void Channel::update()
{
    m_loop->updateChannel(this);
}