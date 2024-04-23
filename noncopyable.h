#pragma once

/**
 * noncopyable被继承后，派生类对象可以正常构造和析构但是
 * 无法进行拷贝构造和赋值操作
*/
class noncopyable {
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;

};