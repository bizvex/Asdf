//
// Created by huxuanguang on 19-2-8.
//

#include "EventLoop.h"
#include "Logging.h"
#include "Util.h"
#include <sys/eventfd.h>

__thread EventLoop* t_loopInThisThread = NULL;

int createEventfd()
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
:   looping_(false),
    quit_(false),
    eventHandling_(false),
    threadId_(this_thread::get_id()),
    poller_(new Epoll()),
    wakeupFd_(createEventfd()),
    pwakeupChannel_(new Channel(this, wakeupFd_)),
    callingPendingFunctors_(false)
{
    if (t_loopInThisThread)
    {
        LOG << "Another EventLoop exists in this thread " << threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }

    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    pwakeupChannel_->setReadHandler(bind(&EventLoop::handleRead, this));
    pwakeupChannel_->setConnHandler(bind(&EventLoop::handleConn, this));
    poller_->epoll_add(pwakeupChannel_, 0);
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread=NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_=true;
    quit_ = false;

    vector<shared_ptr<Channel>> ret;
    //主循环
    while (!quit_)
    {
        ret.clear();
        ret = poller_->poll();
        eventHandling_ = true;
        for (auto &it : ret)
            it->handleEvents();
        eventHandling_ = false;
        doPendingFunctors();
        poller_->handleExpired();
    }

    looping_=false;
}

//唤醒IO线程
//对wakeupFd_写入数据
void EventLoop::wakeup()
{
    //通过向wakeupFd_写入一个字节，使IO线程从epoll_wait中返回
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_, (char*)(&one), sizeof one);
    if (n != sizeof one)
    {
        LOG<< "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}
//从wakeupFd_读出数据
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::handleConn()
{
    updatePoller(pwakeupChannel_, 0);
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor&& cb)
{
    if (isInLoopThread())
        cb();
    else
        queueInLoop(std::move(cb));
}

void EventLoop::queueInLoop(Functor&& cb)
{
    /*
     * 某个线程调用了runInLoop()，如果该线程就是本EventLoop的IO线程，直接执行cb；
     * 若该线程不是当前EventLoop的IO线程（EventLoop的threadId_与this_thread的id不一样）则会调用queueInLoop
     * queueInLoop()将要执行的cb收集起来，并唤醒EventLoop的IO线程。
     */
    {
        unique_lock<mutex> lock(mutex_);
        pendingFunctors_.emplace_back(move(cb));
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        //不是当前EventLoop的IO线程，或callingPendingFunctors_为true则需要唤醒

        //callingPendingFunctors_为true表示正在执行传入IO线程的cb（doPendingFunctors()）
        //doPendingFunctors()有可能会调用queueInLoop()
        //若不加这个会导致cb没法被及时收集并执行
        wakeup();
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        unique_lock<mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (size_t i = 0; i < functors.size(); ++i)
        functors[i]();//执行传入本IO线程的函数
    callingPendingFunctors_ = false;
}