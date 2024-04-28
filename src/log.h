#ifndef LOG_H
#define LoG_H

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <map>

#include "singleton.h"


#define MAKE_LOG_EVENT(level, message) \
    std::make_shared<caizi::LogEvent>(__FILE__, __LINE__, 0, 0, ::time(nullptr), message, " ", level)

#define LOG_LEVEL(logger, level, message) \
    logger->log(level, MAKE_LOG_EVENT(level, message));

#define LOG_DEBUG(logger, message) LOG_LEVEL(logger, caizi::LogLevel::DEBUG, message)
#define LOG_INFO(logger, message) LOG_LEVEL(logger, caizi::LogLevel::INFO, message)
#define LOG_WARN(logger, message) LOG_LEVEL(logger, caizi::LogLevel::WARN, message)
#define LOG_ERROR(logger, message) LOG_LEVEL(logger, caizi::LogLevel::ERROR, message)
#define LOG_FATAL(logger, message) LOG_LEVEL(logger, caizi::LogLevel::FATAL, message)

#define GET_ROOT_LOGGER() caizi::LoggerManager::getInstance()->getGlobalLogger()
#define GET_LOGGER(name) caizi::LoggerManager::getInstance()->getLogger(name)

namespace caizi{

class LogLevel;
class LogEvent;
class Logger;
class LogFormatter;
class LogAppender;
typedef std::shared_ptr<Logger> ptr;

// 日志级别
class LogLevel{
public:
    enum Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };

    static std::string levelToString(LogLevel::Level level);
    static LogLevel::Level stringToLevel(const std::string& str);
};

// 日志事件
class LogEvent{
public:
    LogEvent(const std::string& filename,
             uint32_t line,
             uint32_t threadID,
             uint32_t fiberID,
             time_t time,
             const std::string& content,
             const std::string& division = " ",
             LogLevel::Level level = LogLevel::DEBUG);

    // LogEvent(const std::string content);
    typedef std::shared_ptr<LogEvent> ptr;

    LogLevel::Level getLevel() const { return m_level; }
    const std::string& getFilename() const { return m_filename;}
    uint32_t getLine() const { return m_line; }
    uint32_t getElapse() const {return m_elapse;}
    uint64_t getTime() const { return m_time; }
    const std::string& getThreadName() const {return m_threadName;}
    uint32_t getThreadId() const { return m_threadID; }
    uint32_t getFiberId() const { return m_fiberID; }
    const std::string& getContent() const { return m_content; }
    const std::string& getDivision() const { return m_division; }

    void setLevel(LogLevel::Level level) { m_level = level; }

private:
    LogLevel::Level m_level;                  //日志等级
    const std::string m_filename = nullptr;   //文件名
    int32_t m_line = 0;                       //行号
    uint32_t m_elapse = 0;                    //程序启动流逝时间
    uint32_t m_threadID = 0;                  //线程号
    uint32_t m_fiberID = 0;                   //协程号
    uint64_t m_time = 0;                      //时间戳
    std::string m_threadName;                 //线程名称
    std::string m_content;                    //日志内容
    std::string m_division;
};

// 日志格式
// 通过format()传入LogEvent实例，返回格式化后的字符串
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) = 0;
    };

    explicit LogFormatter(const std::string& pattern);

    // @brief 日志格式化 @param[in] ev 日志事件 @return 格式化后的文本
    // std::string format(std::ostream& os, LogLevel::Level leve, LogEvent::ptr ev);
    std::string format(LogLevel::Level level, LogEvent::ptr event);
    const std::string getPatern() const {return m_format_pattern;};
private:
    // @brief 解析pattern
    void init();
    std::string m_format_pattern;                        //日志格式化的字符串
    std::vector<FormatItem::ptr> m_format_item_list;     //格式化后的日志列表
    bool m_error = false;
};

// 日志输出地 基类
class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;

    explicit LogAppender(LogLevel::Level level = LogLevel::DEBUG);
    virtual ~LogAppender() = default;
    virtual void log(LogLevel::Level level, LogEvent::ptr ev) = 0;

    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter() const {return m_formatter;}
private:
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
    bool m_hasFormatter = false;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger();
    Logger(const std::string& name, LogLevel::Level, const std::string &pattern);
    void log(LogLevel::Level level, LogEvent::ptr event);
    // void debug(LogEvent::ptr event);
    // void info(LogEvent::ptr event);
    // void warn(LogEvent::ptr event);
    // void error(LogEvent::ptr event);
    // void fatal(LogEvent::ptr event);
    
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppedners();
    LogLevel::Level getLevel(){return m_level;};
    void setLevel(LogLevel::Level level){m_level = level;};
    const std::string& getName() const {return m_name;};
    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();

private:
    std::string m_name;
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
    std::list<LogAppender::ptr> m_appenders;
    std::string m_format_pattern;
};

// 日志输出地的派生类，输出到终端
class StdoutLogAppender: public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    explicit StdoutLogAppender(LogLevel::Level level = LogLevel::DEBUG);
    void log(LogLevel::Level level, LogEvent::ptr ev) override;

};

// 日志输出地的派生类，输出到文件
class FileLogAppender: public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    explicit FileLogAppender(const std::string &filename,  LogLevel::Level level = LogLevel::DEBUG);
    void log(LogLevel::Level level, LogEvent::ptr ev) override;
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_file_stream;
};

// @brief 日志的管理器
class __LoggerManager{
public:
    typedef std::shared_ptr<__LoggerManager> ptr;
    __LoggerManager();
    // 根据日志器的名字获取日志器，如果不存在，返回全局日志器
    Logger::ptr getLogger(const std::string &name);
    Logger::ptr getGlobalLogger();

private:
    friend struct LogIniter;
    void init();
    void ensureGlobalLoggerExists();
    std::map<std::string, Logger::ptr> m_logger_map;
};

// struct LogIniter{
//     LogIniter(){

//     }
// };
typedef SingletonPtr<__LoggerManager> LoggerManager;
// typedef Singleton<__LoggerManager> test;

}

#endif