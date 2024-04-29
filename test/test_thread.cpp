#include "caizi.h"
#include "util.h"

caizi::Logger::ptr g_logger = GET_ROOT_LOGGER();

static int count = 0;
caizi::Mutex s_mutex;
caizi::RWLock s_rwlock;
void fun1(){
    LOG_FMT_DEBUG(g_logger, 
        "当前线程 ID：%ld/%d, 当前线程名 = %s\n",
        caizi::GetThreadId(),
        caizi::Thread::GetThisId(),
        caizi::Thread::GetThisThreadName().c_str()
    );
}

void fun2(){
    // caizi::ScopeLock mutex(&s_mutex);
    caizi::WriteScopeLock rsl(&s_rwlock);
    for(int i = 0; i < 1000000; ++i){
        ++count;
    }
};

int main(int arg, char** args){
    LOG_INFO(g_logger, "thread test begin\n");

    std::vector<caizi::Thread::ptr> thrs;
    for(int i = 0; i < 5; i++){
        caizi::Thread::ptr thr(new caizi::Thread(&fun2, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for(int i = 0; i < 5; i++){
        thrs[i]->join();
    }

    LOG_INFO(g_logger, "thread test end\n");

    std::cout << count << std::endl;
    return 0;
}