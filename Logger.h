#pragma once
#include "noncopyable.h"

#include <string>

// LOG_INFO("%s, %d", arg1, arg2)
#define LOG_INFO(logmsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)

#define LOG_ERROR(logmsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)

#define LOG_FATAL(logmsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0)

#ifdef MUDUBUG
#define LOG_DEBUG(logmsgFormat, ...) \
    do { \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(DEBUG); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)
#else
#define LOG_DEBUG(logmsgFormat, ...)
#endif

// 定义日志的级别
// INFO    ERROR     FATAL    DEBUG
// 正常     错误       失败     调试
enum LogLevel {
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger : noncopyable {
// 禁止拷贝和赋值
public:
    // 获取日志的实例对象
    static Logger& instance();

    // 定义文件级别
    void setLogLevel(int level);

    // 写日志
    void log(std::string msg);

private:
    int logLevel_;

    // 私有构造函数确保不能外部创建实例
    Logger(){}

};