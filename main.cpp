#include <iostream>
#include "base/Logging.h"
#include "base/EventLoop.h"
#include "net/Server.h"

using namespace std;

int main()
{
    int threadNum = 4;
    int port = 9999;
    string logPath = "./Asdf_WebServer.log";

    Logger::setLogFileName(logPath);

    LOG<<"!!!SERVER START!!!";

    EventLoop mainLoop;
    Server myHTTPServer(&mainLoop, threadNum, port);
    myHTTPServer.start();
    mainLoop.loop();

    return 0;
}