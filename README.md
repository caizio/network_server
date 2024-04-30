## 基于Sylar实现的一个高性能网络服务器框架，包括日志模块、配置模块、线程模块、协程模块、协程IO、Hook模块、网络模块、HTTP模块等。

代码正在不断实现中……

## 日志模块
主要包括以下类 

Logger：日志器

LogLevel：日志级别，包括DEBUG、INFO、WARN、ERROR、FATAL

LogEvent：日志事件，存储需要输出日志的信息，包括日志等级、文件名、行号、程序启动流逝时间、线程号、协程号、时间戳、线程名、日志内容等。

LogFormatter：日志格式

LogAppender（StdoutLogAppender、FileLogAppender）：日志输出地

## 配置模块

## 线程模块
