//
// Created by huxuanguang on 19-1-30.
//

#ifndef ASDF_FILEUTIL_H
#define ASDF_FILEUTIL_H

#include <boost/noncopyable.hpp>
#include <string>

class AppendFile:boost::noncopyable
{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();

    //写出到缓冲区
    void append(const char *msg, const size_t len);
    //将数据从缓冲区刷新到文件
    void flush();

private:
    size_t write(const char *msg, size_t len);
    FILE* fp_;
    char buffer_[64*1024];

};

#endif //ASDF_FILEUTIL_H
