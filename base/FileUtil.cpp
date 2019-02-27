//
// Created by huxuanguang on 19-1-30.
//

#include "FileUtil.h"

AppendFile::AppendFile(std::string filename)
:   fp_(fopen(filename.c_str(), "ae"))
{
    //创建一个块缓冲区
    setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile()
{
    fclose(fp_);
}

size_t AppendFile::write(const char *msg, size_t len)
{
    return fwrite_unlocked(msg, 1, len, fp_);
}

void AppendFile::append(const char *msg, const size_t len)
{
    size_t n = this->write(msg, len);
    size_t remain = len - n;
    while (remain > 0)
    {
        size_t x = this->write(msg + n, remain);
        if (x == 0)
        {
            int err = ferror(fp_);
            if (err)
                fprintf(stderr, "AppendFile::append() failed !\n");
            break;
        }
        n += x;
        remain = len - n;
    }
}

void AppendFile::flush()
{
    fflush(fp_);
}
