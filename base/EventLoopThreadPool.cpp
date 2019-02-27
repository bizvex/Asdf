//
// Created by huxuanguang on 19-2-15.
//

#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int numThreads)
        :   baseLoop_(baseLoop),
            started_(false),
            numThreads_(numThreads),
            next_(0)
{
    if (numThreads_ <= 0)
    {
        LOG << "numThreads_ <= 0";
        abort();
    }
}

void EventLoopThreadPool::start()
{
    baseLoop_->assertInLoopThread();
    started_ = true;
    for (int i = 0; i < numThreads_; ++i)
    {
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
        /*
         * 会开启numThreads_个线程和numThreads_个loop里面的循环
         * 但此时只监听pwakeupChannel_而没有其他Channel
         * 其他Channel通过addToPoller添加
         * 如果pwakeupChannel_没有被唤醒，则会阻塞在epoll_wait
         */
    }
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop *loop = baseLoop_;
    if (!loops_.empty())
    {
        loop = loops_[next_];
        next_ = (next_ + 1) % numThreads_;
    }
    return loop;
}
