//
// Created by huxuanguang on 19-2-15.
//

#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
:   loop_(NULL),
    exiting_(false),
    thread_(),//thread_在后面再开启线程
    mutex_(),
    cond_()
{}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != NULL)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    /*
     * EventLoopThread是对负责IO的EventLoop的封装
     * EventLoopThread启动一个新的线程，并在新线程中运行EventLoop::loop()
     */
    thread_=thread(bind(&EventLoopThread::threadFunc, this));//开启线程
    {
        unique_lock<mutex> lock(mutex_);
        //只要loop_不为NULL就返回
        while (loop_ == NULL)
            cond_.wait(lock);
    }
    return loop_;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        unique_lock<mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();//开启loop里面的循环
    loop_ = NULL;
}