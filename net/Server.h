//
// Created by huxuanguang on 19-2-15.
//

#ifndef ASDF_SERVER_H
#define ASDF_SERVER_H

#include <boost/noncopyable.hpp>
#include "../base/EventLoopThreadPool.h"

class Server:boost::noncopyable
{
public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server();
    EventLoop* getLoop() const { return loop_; }
    void start();
    void handNewConn();
    void handThisConn() { loop_->updatePoller(acceptChannel_); }

private:
    EventLoop *loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenFd_;
    static const int MAXFDS = 100000;
};

#endif //ASDF_SERVER_H
