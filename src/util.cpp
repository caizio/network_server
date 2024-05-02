#include "util.h"
#include "log.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <execinfo.h>
#include <sstream>

static caizi::Logger::ptr g_logger = CAIZI_GET_LOGGER("system");
namespace caizi{
    
long GetThreadId(){
    return ::syscall(SYS_gettid);
}

// 获取栈信息
void __GetBacktrace(std::vector<std::string>&bt, int size, int skip){
    void** buf = (void**) malloc(sizeof(void*) * size);
    size_t s = ::backtrace(buf, size);

    char** stack_info = backtrace_symbols(buf, s);
    if(stack_info == NULL){
        LOG_ERROR(g_logger, "获取栈信息错误");
        free(buf);
        free(stack_info);
        return;
    }

    for(size_t i = skip; i < s; i++){
        bt.push_back(stack_info[i]);
    }

    free(buf);
    free(stack_info);
}

// 获取栈信息并返回字符串
std::string BacktraceToString(int size, int skip, const std::string& prefix){
    std::vector<std::string> bt;
    __GetBacktrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); i++){
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

}