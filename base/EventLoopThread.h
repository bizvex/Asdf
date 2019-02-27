//
// Created by huxuanguang on 19-2-15.
//

#ifndef ASDF_EVENTLOOPTHREAD_H
#define ASDF_EVENTLOOPTHREAD_H

#include <boost/noncopyable.hpp>
#include "EventLoop.h"
#include <condition_variable>

class EventLoopThread:boost::noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();
    EventLoop *loop_;
    bool exiting_;
    thread thread_;
    mutex mutex_;
    condition_variable cond_;
};

#endif //ASDF_EVENTLOOPTHREAD_H
