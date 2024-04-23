#pragma once

#include "noncopyable.h"
#include "InetAddress.h"
#include "Callback.h"
#include "Buffer.h"
#include "Timestamp.h"

#include <memory>
#include <string>
#include <atomic>

class Channel;
class EventLoop;
class Socket;

/**
 * TcpServer => Acceptor =>新用户连接，通过accept拿到一个connfd
 * => TcpConnection 设置回调 => Channel => Poller => Channel回调
*/
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop,
                const std::string& nameArg,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }

    void send(const std::string& buf);
    // 关闭连接
    void shutdown();

    void SetConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    void setHightWaterMarkCallback(const HighWaterMarkCallback& cb)
    { hightWaterMarkCallback_ = cb; }

    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

    // 连接建立
    void connectEstablised();
    // 连接销毁
    void connectDestroyed();

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void setState(StateE state) { state_ = state; }
    void handleRead(Timestamp receiveTiem);
    void handleWrite();
    void handleClose();
    void handleError();


    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();

    EventLoop* loop_; // 绝对不是baseloop， 因为TcpConnection都是在subLoop里面的
    const std::string name_;
    std::atomic_int state_;
    bool reading_;

    // 和Acceptor类似 Acceptor=>mainLoop TcpConnection=>subLoop
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_; 

    const InetAddress localAddr_;
    const InetAddress peerAddr_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback hightWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t highWaterMark_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;
};