#pragma once

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

class Channel;
class Poller;

// 时间循环类，包括两大模块 Channel Poller
class EventLoop : noncopyable 
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    // 开启事件循环
    void loop();
    // 退出事件循环
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    // 在当前loop中执行cb
    void runInLoop(Functor cb);
    // 把cb放入队列中，唤醒loop所在的线程执行cb
    void queueInLoop(Functor cb);

    // 用来唤醒loop所在的线程
    void wakeup();

    // EventLoop的方法 -> Poler的方法
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    // 判断EventLoop的对象是否在自己的线程里面
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    // wake up
    void handleRead();
    // 执行回调
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;
    std::atomic_bool looping_; // 原子操作，通过CAS实现
    std::atomic_bool quit_; // 标志退出loop循环
    std::atomic_bool callingPendingFunctors_; // 标识当前loop是否有需要执行的回调操作

    const pid_t threadId_; // 记录当前loop所在线程的id

    Timestamp pollReturnTime_; // 记录Poller返回发生事件的Channels的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupfd_; // 主要作用：当mainLoop获取一个新用户的Channel通过轮询算法选择一个subloop，通过该成员唤醒subloop处理channel
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;
    std::vector<Functor> pendingFunctors_; // 存贮loop需要执行的所有的回调操作
    std::mutex mutex_; // 互斥锁，用来保护上面vector容器的线程安全操作
};