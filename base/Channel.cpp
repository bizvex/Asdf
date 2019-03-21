//
// Created by huxuanguang on 19-2-8.
//

#include "Channel.h"
#include <sys/epoll.h>

Channel::Channel(EventLoop *loop)
:   loop_(loop),
    events_(0),
    lastEvents_(0)
{}

Channel::Channel(EventLoop *loop, int fd)
:   loop_(loop),
    fd_(fd),
    events_(0),
    lastEvents_(0)
{}

Channel::~Channel() {}

int Channel::getFd()
{
    return fd_;
}
void Channel::setFd(int fd)
{
    fd_ = fd;
}

void Channel::handleEvents()
{
    events_ = 0;

    if (revents_ & EPOLLIN)
    {
        handleRead();
    }
    if (revents_ & EPOLLOUT)
    {
        handleWrite();
    }
    handleConn();
}

void Channel::handleRead()
{
    if (readHandler_)
    {
        readHandler_();
    }
}

void Channel::handleWrite()
{
    if (writeHandler_)
    {
        writeHandler_();
    }
}

void Channel::handleConn()
{
    if (connHandler_)
    {
        connHandler_();
    }
}
