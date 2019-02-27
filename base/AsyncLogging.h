//
// Created by huxuanguang on 19-2-1.
//

#ifndef ASDF_ASYNCLOGGING_H
#define ASDF_ASYNCLOGGING_H

#include "LogStream.h"
#include <boost/noncopyable.hpp>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class AsyncLogging:boost::noncopyable
{
public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging()
    {
        if (running_)
            stop();
    }
    void append(const char* logline, int len);

    void start()
    {
        running_ = true;
        //启动线程
        thread_=std::thread(std::bind(&AsyncLogging::threadFunc, this));
    }

    void stop()
    {
        running_ = false;
        cv_.notify_one();//唤醒后端
        thread_.join();
    }

private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;
    const int flushInterval_;
    bool running_;
    std::string basename_;
    std::thread thread_;
    std::mutex mutex_;
    //前端为生产者，后端为消费者，该条件变量用来通知后端写入日志数据
    std::condition_variable cv_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};

#endif //ASDF_ASYNCLOGGING_H
