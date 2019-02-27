cmake_minimum_required(VERSION 3.13)
project(Asdf)

add_subdirectory(base/test_log)

set(CMAKE_CXX_STANDARD 11)

add_executable(Asdf main.cpp base/FileUtil.cpp base/FileUtil.h base/LogFile.cpp base/LogFile.h base/LogStream.cpp base/LogStream.h base/AsyncLogging.cpp base/AsyncLogging.h base/Logging.cpp base/Logging.h base/EventLoop.cpp base/EventLoop.h base/Channel.cpp base/Channel.h base/Timer.cpp base/Timer.h base/Epoll.cpp base/Epoll.h base/Util.cpp base/Util.h base/EventLoopThread.cpp base/EventLoopThread.h base/EventLoopThreadPool.cpp base/EventLoopThreadPool.h net/Server.cpp net/Server.h net/HttpData.cpp net/HttpData.h net/MimeType.cpp net/MimeType.h)

target_link_libraries(Asdf pthread)