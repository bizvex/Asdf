//
// Created by huxuanguang on 19-1-30.
//

#include "LogFile.h"
#include <assert.h>

LogFile::LogFile(const std::string &basename, int flushPerN)
:   basename_(basename),
    flushPerN_(flushPerN),
    count_(0),
    mutex_()
{
    file_.reset(new AppendFile(basename));
}

LogFile::~LogFile()
{}

void LogFile::append_unlocked(const char *msg, size_t len)
{
    file_->append(msg,len);
    ++count_;
    if(count_>=flushPerN_)
    {
        count_=0;
        file_->flush();
    }

}

void LogFile::append(const char *msg, size_t len)
{
    std::unique_lock<std::mutex> lock(mutex_);
    append_unlocked(msg,len);
}

void LogFile::flush()
{
    std::unique_lock<std::mutex> lock(mutex_);
    file_->flush();
}
