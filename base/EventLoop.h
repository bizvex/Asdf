//
// Created by huxuanguang on 19-2-8.
//

#ifndef ASDF_EVENTLOOP_H
#define ASDF_EVENTLOOP_H

#include <iostream>
#include <functional>
#include <assert.h>
#include <boost/noncopyable.hpp>
#include <thread>
#include "Channel.h"
#include "Epoll.h"
#include <mutex>


using namespace std;

class EventLoop:boost::noncopyable
{
public:
    typedef function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    //可能导致多次系统调用降低效率
    //muduo里面用的是__thread来保存变量，但__thread只能修饰POD类型
    //TODO:c++11可以用thread_local代替__thread
    bool isInLoopThread() const { return threadId_ == this_thread::get_id(); }
    void assertInLoopThread()
    {
        assert(isInLoopThread());
    }

    //跨线程调用核心
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);

    void removeFromPoller(shared_ptr<Channel> channel)
    {
        poller_->epoll_del(channel);
    }
    void updatePoller(shared_ptr<Channel> channel, int timeout = 0)
    {
        poller_->epoll_mod(channel, timeout);
    }
    void addToPoller(shared_ptr<Channel> channel, int timeout = 0)//添加监听的fd和事件
    {
        poller_->epoll_add(channel, timeout);
    }

private:
    bool looping_;
    bool quit_;
    bool eventHandling_;
    const  thread::id threadId_;
    shared_ptr<Epoll> poller_;
    int wakeupFd_;
    shared_ptr<Channel> pwakeupChannel_;

    mutable mutex mutex_;
    vector<Functor> pendingFunctors_;
    bool callingPendingFunctors_;

    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
};

#endif //ASDF_EVENTLOOP_H
