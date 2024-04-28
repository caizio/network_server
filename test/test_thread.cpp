#include "caizi.h"
#include "util.h"

caizi::Logger::ptr g_logger = GET_ROOT_LOGGER();


void fun1(){
    LOG_FMT_DEBUG(g_logger, 
        "当前线程 ID：%ld/%d, 当前线程名 = %s",
        caizi::GetThreadId(),
        caizi::Thread::GetThisId(),
        caizi::Thread::GetThisThreadName().c_str()
    );
}

void fun2(){

};


int main(int arg, char** args){
    LOG_INFO(g_logger, "thread test begin\n");

    std::vector<caizi::Thread::ptr> thrs;
    for(int i = 0; i < 5; i++){
        caizi::Thread::ptr thr(new caizi::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for(int i = 0; i < 5; i++){
        thrs[i]->join();
    }

    LOG_INFO(g_logger, "thread test end\n");
    return 0;
}