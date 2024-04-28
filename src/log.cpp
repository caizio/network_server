#include "log.h"
#include <functional>
#include <map>

namespace caizi{

const std::string cs_foramt = "%p%n%t%f%l%m"; 

std::string LogLevel::levelToString(LogLevel::Level level){
    std::string res;
    switch (level){
        case UNKNOW:
            res = "UNKONW";
            break;
        case DEBUG:
            res = "DEBUG";
            break;
        case INFO:
            res = "INFO";
            break;
        case WARN:
            res = "WARN";
            break;
        case ERROR:
            res = "ERROR";
            break;
        case FATAL:
            res = "FATAL";
            break;
    }
    return res;
}

LogLevel::Level LogLevel::stringToLevel(const std::string& str){
    #define XX(level, v) {if(str == #v){return LogLevel::level;}}
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return Level::UNKNOW;
    #undef XX
}

LogEvent::LogEvent(const std::string& filename,
            uint32_t line,
            uint32_t threadID,
            uint32_t fiberID,
            time_t time,
            const std::string& content,
            const std::string& division,
            LogLevel::Level level):
        m_filename(filename),
        m_line(line),
        m_threadID(threadID),
        m_fiberID(fiberID),
        m_time(time),    
        m_content(content),
        m_division(division),
        m_level(level){};


class MessageFormatItem: public LogFormatter::FormatItem{
    public:
    MessageFormatItem(const std::string $str){};
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << ev->getContent();
    }
};

class LevelFormatItem: public LogFormatter::FormatItem{
    public:
    LevelFormatItem(const std::string $str){};
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << LogLevel::levelToString(level);
    }    
};

class ThreadIDFormatItem: public LogFormatter::FormatItem{
    public:
    ThreadIDFormatItem(const std::string $str){};
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << ev->getThreadId();
    }    
};

class DivisionFormatItem: public LogFormatter::FormatItem{
    public:
    DivisionFormatItem(const std::string $str){};        
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << ev->getDivision();
    }        
};

class FiberIDFormatItem: public LogFormatter::FormatItem{
    public:
    FiberIDFormatItem(const std::string $str){};        
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << ev->getFiberId();
    }    
};
class FilenameFormatItem: public LogFormatter::FormatItem{
    public:
    FilenameFormatItem(const std::string $str){};     
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        auto p = ev->getFilename();
        out << ev->getFilename();
    }    
};

class LineFormatItem: public LogFormatter::FormatItem{
    public:
    LineFormatItem(const std::string $str){};   
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << ev->getLine();
    }    
};

class DateTimeFormatItem: public LogFormatter::FormatItem{
    public:
    DateTimeFormatItem(const std::string $str){};   
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << ev->getTime();
    }    
};

class ThreadNameFormatItem: public LogFormatter::FormatItem{
    public:
    ThreadNameFormatItem(const std::string $str){};   
    void format(std::ostream& out, LogLevel::Level level, LogEvent::ptr ev) override{
        out << ev->getThreadName();
    }    
};

LogFormatter::LogFormatter(const std::string& pattern):m_format_pattern(pattern){
    init();
}

std::string LogFormatter::format(LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for(auto &p:m_format_item_list){
        p->format(ss,level,event);
    }
    return ss.str();
}


void LogFormatter::init(){
    std::vector<std::string> vec;
    enum PARSE_STATUS{
        SCAN_STATUS,
        CREATE_STATUS,
    };

    PARSE_STATUS status = SCAN_STATUS;
    size_t str_start, str_end;
    // 解析格式
    for(size_t str_start = 0; str_start < m_format_pattern.size(); str_start++){
        if(m_format_pattern[str_start] == '%'){
            str_end = str_start + 1;
            for(str_end = str_start + 1; str_end < m_format_pattern.size(); str_end++){
                if(m_format_pattern[str_end] == '%'){
                    vec.push_back(m_format_pattern.substr(str_start + 1,str_end - str_start - 1));
                    str_start = str_end;
                    str_start --;
                    break;
                }else if(str_end == m_format_pattern.size() - 1){
                    vec.push_back(m_format_pattern.substr(str_start + 1,str_end - str_start));
                }
            }
        }
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& s)>> s_format_items = {
    #define XX(str,C) {#str, [](const std::string& fc) {return std::make_shared<C>(fc);}}
        XX(m, MessageFormatItem),           //m:消息 
        XX(p, LevelFormatItem),             //p:日志级别
        // XX(r, ElapseFormatItem),            //r:累计毫秒数
        // XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIDFormatItem),          //t:线程id
        // XX(n, NewLineFormatItem),           //n:换行
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        // XX(T, TabFormatItem),               //T:Tab
        // XX(F, FiberIdFormatItem),           //F:协程id
        XX(n, ThreadNameFormatItem),        //n:线程名称
        XX(d, DivisionFormatItem), //分割符
    #undef XX
    };

    for(auto &p:vec){
        m_format_item_list.push_back(s_format_items[p](""));
    }
}

Logger::Logger():m_name("default"),m_level(LogLevel::DEBUG), m_format_pattern("%p%d%n%d%t%d%f%d%l%d%m")
{
    m_formatter.reset(new LogFormatter(m_format_pattern));
}

Logger::Logger(const std::string& name,  LogLevel::Level level, const std::string &pattern): m_name(name), m_level(level){
    m_formatter.reset(new LogFormatter(pattern));
}

// 遍历输出器，输出日志
void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        if(!(m_appenders.empty())){
            for(auto& p: m_appenders){
                p->log(level, event);
            }
        }
    }
}

void Logger::addAppender(LogAppender::ptr appender){
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
    for(auto it = m_appenders.begin(); it != m_appenders.end(); it++){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppedners(){
    m_appenders.clear();
}

void Logger::setFormatter(LogFormatter::ptr val){
    m_formatter = val;
    for(auto &p:m_appenders){
        if(!p->m_hasFormatter){
            p->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& val){
    LogFormatter::ptr new_val(new LogFormatter(val));
}

LogAppender::LogAppender(LogLevel::Level level){

}

void LogAppender::setFormatter(LogFormatter::ptr val){
    m_formatter = val;
    if(m_formatter){
        m_hasFormatter = true;
    }else{
        m_hasFormatter = false;
    }
}

StdoutLogAppender::StdoutLogAppender(LogLevel::Level level){

}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
    if(level < m_level) return;
    std::cout << m_formatter->format(level, event);
    std::cout.flush();
}

FileLogAppender::FileLogAppender(const std::string &filename,  LogLevel::Level level):
    LogAppender(level), m_filename(filename){
    reopen();
}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr ev) {
    if(level < m_level) return;
    m_file_stream <<  m_formatter->format(level,ev); 
    m_file_stream.flush();
}
// 
bool FileLogAppender::reopen(){
    if(!m_file_stream){
        m_file_stream.close();
    }
    m_file_stream.open(m_filename, std::ios_base::out | std::ios_base::app);
    return !!m_file_stream;
}

__LoggerManager::__LoggerManager(){
    init();
}

Logger::ptr __LoggerManager::getLogger(const std::string &name){
    auto iter = m_logger_map.find(name);
    if(iter == m_logger_map.end()){
        return m_logger_map.find("global")->second;
    }
    return iter->second;
}

Logger::ptr __LoggerManager::getGlobalLogger(){
    return getLogger("Global");
}

void __LoggerManager::init(){
    m_logger_map.erase("global");
    Logger::ptr logger = std::make_shared<Logger>("global",LogLevel::DEBUG,"%p%d%n%d%t%d%f%d%l%d%m");
    LogAppender::ptr appender = std::make_shared<StdoutLogAppender>(LogLevel::DEBUG);
    logger->addAppender(appender);
    std::cout << "成功创建日志器: " << "global" << std::endl;
    m_logger_map.insert(std::make_pair("global",std::move(logger)));
    ensureGlobalLoggerExists();
}

// 确保只保留一个全局日志器
void __LoggerManager::ensureGlobalLoggerExists(){
    auto iter = m_logger_map.find("global");
    if(iter == m_logger_map.end()){
        // 如果不存在全局日志器，则重新构造一个默认的
        Logger::ptr logger = std::make_shared<Logger>("global",LogLevel::DEBUG,"%p%d%n%d%t%d%f%d%l%d%m");
        LogAppender::ptr appender = std::make_shared<StdoutLogAppender>(LogLevel::DEBUG);
        logger->addAppender(appender);
        m_logger_map.insert(std::make_pair("global",std::move(logger)));
    }else if(!iter->second){
        // 如果键存在，但日志器不存在，也重新构造一个
        iter->second = std::make_shared<Logger>("global",LogLevel::DEBUG,"%p%d%n%d%t%d%f%d%l%d%m");
        LogAppender::ptr appender = std::make_shared<StdoutLogAppender>(LogLevel::DEBUG);
        iter->second->addAppender(appender);
    }
}


}


