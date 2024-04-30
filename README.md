## 基于Sylar实现的一个高性能网络服务器框架，包括日志模块、配置模块、线程模块、协程模块、协程IO、Hook模块、网络模块、HTTP模块等。

代码正在不断实现中……

## 日志模块(log.h)
主要包括以下类 

Logger：日志器

LogLevel：日志级别，包括DEBUG、INFO、WARN、ERROR、FATAL

LogEvent：日志事件，存储需要输出日志的信息，包括日志等级、文件名、行号、程序启动流逝时间、线程号、协程号、时间戳、线程名、日志内容等。

LogFormatter：日志格式

LogAppender（StdoutLogAppender、FileLogAppender）：日志输出地

## 配置模块(config.h)
ConfigVarBase（ConfigVar）：配置项

Config：配置项的管理类，实现数据的读取、解析


## 线程模块(thread.h)
Thread: 线程模块

ThreadData: 线程数据及运行

Semaphore：信号量

ScopedLockImpl（Mutex）：互斥锁

ReadScopeLockImpl、WriteScopeLockImpl（RWLock）：读写锁

## 地址模块(address.h endiant.h)