//
// Created by huxuanguang on 19-2-9.
//

#ifndef ASDF_EPOLL_H
#define ASDF_EPOLL_H

#include <boost/noncopyable.hpp>
#include <memory>
#include <vector>
#include <sys/epoll.h>
#include "Timer.h"
#include "Channel.h"

class Epoll:boost::noncopyable
{
public:
    typedef std::shared_ptr<Channel> SP_Channel;
    Epoll();
    ~Epoll();
    void epoll_add(SP_Channel request, int timeout);
    void epoll_mod(SP_Channel request, int timeout);
    void epoll_del(SP_Channel request);
    std::vector<SP_Channel> poll();
    std::vector<SP_Channel> getEventsRequest(int events_num);
    void add_timer(SP_Channel request_data, int timeout);
    int getEpollFd()
    {
        return epollFd_;
    }
    void handleExpired();

private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_;
    SP_Channel fd2chan_[MAXFDS];
    std::shared_ptr<HttpData> fd2http_[MAXFDS];
    TimerManager timerManager_;
};

#endif //ASDF_EPOLL_H
