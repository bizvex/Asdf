//
// Created by huxuanguang on 19-1-30.
//

#ifndef ASDF_LOGFILE_H
#define ASDF_LOGFILE_H

#include <boost/noncopyable.hpp>
#include "FileUtil.h"
#include <string>
#include <mutex>
#include <memory>

class LogFile:boost::noncopyable
{
public:
    LogFile(const std::string& basename, int flushPerN = 1024);
    ~LogFile();

    void append(const char* msg, size_t len);
    void flush();

private:
    void append_unlocked(const char* msg, size_t len);

    const std::string basename_;
    const int flushPerN_;//每被append flushPerN次，flush一下

    int count_;

    std::mutex mutex_;
    std::unique_ptr<AppendFile> file_;
};

#endif //ASDF_LOGFILE_H
