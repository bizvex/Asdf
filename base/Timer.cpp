//
// Created by huxuanguang on 19-2-8.
//

#include <sys/time.h>
#include "Timer.h"
#include "../net/HttpData.h"


TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout)
:   deleted_(false),
    SPHttpData(requestData)
{
    timeval now;
    gettimeofday(&now, NULL);
    // 以毫秒计
    expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

TimerNode::TimerNode(TimerNode &tn)
:   SPHttpData(tn.SPHttpData)
{}

TimerNode::~TimerNode()
{
    if (SPHttpData)
        SPHttpData->handleClose();
}

void TimerNode::update(int timeout)
{
    timeval now;
    gettimeofday(&now, NULL);
    expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid()
{
    timeval now;
    gettimeofday(&now, NULL);
    long temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if (temp < expiredTime_)
        return true;
    else
    {
        this->setDeleted();
        return false;
    }
}

void TimerNode::clearReq()
{
    SPHttpData.reset();
    this->setDeleted();
}


TimerManager::TimerManager()
{ }

TimerManager::~TimerManager()
{ }

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout)
{
    SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
    timerNodeQueue.push(new_node);
    SPHttpData->linkTimer(new_node);
}

void TimerManager::handleExpiredEvent()
{
    /* 对于被置为deleted的时间节点，会延迟到它超时 或它前面的节点都被删除时，它才会被删除。
     * 这样做有两个好处：
     * (1) 第一个好处是不需要遍历优先队列，省时。
     * (2) 第二个好处是给超时时间一个容忍的时间，就是设定的超时时间是删除的下限(并不是一到超时时间就立即删除)，
     * 如果监听的请求在超时后的下一次请求中又一次出现了，就不用再重新申请RequestData节点了，
     * 这样可以继续重复利用前面的RequestData，减少了一次delete和一次new的时间。
     */
    while (!timerNodeQueue.empty())
    {
        SPTimerNode ptimer_now = timerNodeQueue.top();
        if (ptimer_now->isDeleted())
            timerNodeQueue.pop();
        else if (!ptimer_now->isValid())
            timerNodeQueue.pop();
        else
            break;//前面的是早过期的
    }
}
