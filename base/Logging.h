//
// Created by huxuanguang on 19-2-1.
//

#ifndef ASDF_LOGGING_H
#define ASDF_LOGGING_H

#include <string>
#include "LogStream.h"

class AsyncLogging;

class Logger
{
public:
    Logger(const char *fileName, int line);
    ~Logger();
    LogStream& stream() { return impl_.stream_; }

    static void setLogFileName(std::string fileName)
    {
        logFileName_ = fileName;
    }
    static std::string getLogFileName()
    {
        return logFileName_;
    }

private:
    class Impl
    {
    public:
        Impl(const char *fileName, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;
};

//创建临时的Logger对象并调用stream
#define LOG Logger(__FILE__, __LINE__).stream()

#endif //ASDF_LOGGING_H
