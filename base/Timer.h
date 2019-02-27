//
// Created by huxuanguang on 19-2-8.
//

#ifndef ASDF_TIMER_H
#define ASDF_TIMER_H

#include <memory>
#include <vector>
#include <queue>

class HttpData;

class TimerNode
{
private:
    bool deleted_;
    long expiredTime_;
    std::shared_ptr<HttpData> SPHttpData;
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    TimerNode(TimerNode& tn);
    ~TimerNode();
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted() { deleted_ = true; }
    bool isDeleted() const { return deleted_; }
    long getExpTime() const { return expiredTime_; }
};

struct TimerCmp
{
    bool operator()(std::shared_ptr<TimerNode>& l, std::shared_ptr<TimerNode>& r) const
    {
        return l->getExpTime() > r->getExpTime();//小顶堆
    }
};

class TimerManager
{
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::vector<SPTimerNode>, TimerCmp> timerNodeQueue;
};

#endif //ASDF_TIMER_H
