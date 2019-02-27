cmake_minimum_required(VERSION 3.13)
project(TESTLOG)

set(CMAKE_CXX_STANDARD 11)

add_executable(TESTLOG TestLog.cpp ../FileUtil.cpp ../FileUtil.h ../LogFile.cpp ../LogFile.h ../LogStream.cpp ../LogStream.h ../AsyncLogging.cpp ../AsyncLogging.h ../Logging.cpp ../Logging.h)
target_link_libraries(TESTLOG pthread)