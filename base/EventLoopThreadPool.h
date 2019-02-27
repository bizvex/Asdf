//
// Created by huxuanguang on 19-2-15.
//

#ifndef ASDF_EVENTLOOPTHREADPOOL_H
#define ASDF_EVENTLOOPTHREADPOOL_H

#include <boost/noncopyable.hpp>
#include "EventLoopThread.h"
#include "Logging.h"

class EventLoopThreadPool:boost::noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop, int numThreads);
    ~EventLoopThreadPool()
    {
        LOG << "~EventLoopThreadPool()";
    }
    void start();
    EventLoop *getNextLoop();

private:
    EventLoop* baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

#endif //ASDF_EVENTLOOPTHREADPOOL_H
