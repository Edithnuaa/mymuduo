#pragma once

#include <functional>
#include <memory>

#include "noncopyable.h"

class EventLoop; //类型的前置声明
class Timestamp;

/**
 * Channel为通过，封装了Sockfd和其感兴趣的event，如EPOLLIN、EPOLLOUT事件
 * 还绑定了Poller监听返回的具体事件
*/
class Channel : noncopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // 处理事件，调用相应的方法
    void handleEvent(Timestamp receiveTime);

    void setReadEventCallback(ReadEventCallback cb) {
        readCallback_ = std::move(cb); //把一个左值转成右值
    }
    void setWriteCallback(EventCallback cb) {
        writeCallback_ = std::move(cb);
    }
    void setCloseCallback(EventCallback cb) {
        closeCallback_ = std::move(cb);
    }
    void setErrorCallback(EventCallback cb) {
        errorCallback_ = std::move(cb);
    }
    
    // 防止当Channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    // 设置fd相应的事件状态
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kReadEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    int index() const { return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop* ownerLoop() const {return loop_; }
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp reveiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_; // 文件描述符, Poller监听的对象
    int events_; // 注册fd感兴趣的事件
    int revents_; // Poller返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    // Channel通道能够获得fd最终发生的事件，所以它负责调用具体事件的回调操作
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

};