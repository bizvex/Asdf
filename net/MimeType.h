//
// Created by huxuanguang on 19-2-20.
//

#ifndef ASDF_MIMETYPE_H
#define ASDF_MIMETYPE_H

#include <unordered_map>

class MimeType
{
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType &m);

    static pthread_once_t once_control;

public:
    static std::string getMime(const std::string &suffix);


};


#endif //ASDF_MIMETYPE_H
