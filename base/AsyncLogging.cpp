//
// Created by huxuanguang on 19-2-1.
//

#include "AsyncLogging.h"
#include "LogFile.h"
#include <assert.h>

AsyncLogging::AsyncLogging(std::string logFileName_,int flushInterval)
        : flushInterval_(flushInterval),
          running_(false),
          basename_(logFileName_),
          thread_(),
          mutex_(),
          cv_(),
          currentBuffer_(new Buffer),
          nextBuffer_(new Buffer),
          buffers_()
{
    assert(logFileName_.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

//前端
void AsyncLogging::append(const char* msg, int len)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (currentBuffer_->avail() > len)
        currentBuffer_->append(msg, len);
    else //currentBuffer如果满了，push进buffers_并且找一块空的buffer
    {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();//currentBuffer_丢弃控制权，为了与muduo里面一致
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else
            currentBuffer_.reset(new Buffer);
        currentBuffer_->append(msg, len);
        cv_.notify_one();
    }
}

//后端
void AsyncLogging::threadFunc()
{
    assert(running_);
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (buffers_.empty())  // unusual usage!
            {
                cv_.wait_for(lock,std::chrono::seconds(flushInterval_));
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);
            //可能会将4块缓冲区一起写出
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25)
        {
            //why?
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        for (int i = 0; i < buffersToWrite.size(); ++i)
        {
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if (buffersToWrite.size() > 2)
        {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}