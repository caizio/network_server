#include "thread.h"
#include "log.h"
#include <cassert>
#include "util.h"

namespace caizi{

static thread_local Thread* t_thread = nullptr;
static thread_local pid_t t_tid = 0;
static thread_local std::string t_thread_name = "UNKNOW";

static Logger::ptr system_logger = CAIZI_GET_LOGGER("system");

/*
    Semaphore 类的实现
    对信号量进行封装
*/
Semaphore::Semaphore(uint32_t count){
    if(sem_init(&m_semaphore, 0, count)){
        LOG_FATAL(system_logger, "sem_init() 初始化信号量失败！");
        throw std::system_error();
    }
}

Semaphore::~Semaphore(){
    sem_destroy(&m_semaphore);
}

void Semaphore::wait(){
    if(sem_wait(&m_semaphore)){
        LOG_FATAL(system_logger,"Semaphore::wait() 等待信号量失败！");
        throw std::system_error();
    }
}

// 增加信号量，并通知线程启用
void Semaphore::notify(){
    if(sem_post(&m_semaphore)){
        LOG_FATAL(system_logger,"Semaphore::notify() 增加信号量资源失败！");
        throw std::system_error();
    }   
}

/*
    Thread类的实现
*/

Thread* Thread::GetThis(){
    return t_thread;
}

pid_t Thread::GetThisId(){
    return t_tid;
}

const std::string& Thread::GetThisThreadName(){
    return t_thread_name;
}

void Thread::SetThisThreadName(const std::string& name){
    t_thread_name = name;
}

Thread::Thread(Threadfunc callback, const std::string& name):
    m_id(-1), m_name(name), m_thread(0), m_callback(callback), m_semaphore(0), m_started(true), m_joined(false){
        ThreadData* data = new ThreadData(m_callback, m_name, &m_id, &m_semaphore);
        int result = pthread_create(&m_thread, nullptr, &Thread::Run, data);
        if(result){
            m_started = false;
            delete data;
            LOG_FMT_FATAL(
                system_logger,
                "pthread_create() 线程创建失败, 线程名 = %s, 错误码 = %d",
                name.c_str(), result);
            throw std::system_error();
        }else{
            m_semaphore.wait();
            assert(m_id > 0);
        }
}
Thread::~Thread(){
    if(m_started && !m_joined){
        pthread_detach(m_thread);
    }
}

int Thread::join(){
    assert(m_started);  
    assert(!m_joined);
    m_joined = true;
    return pthread_join(m_thread, nullptr);
}

void* Thread::Run(void* arg){
    std::unique_ptr<ThreadData> data((ThreadData*) arg);
    data->runInThread();
    return 0;
}

/*
    ThreadData结构体
*/
ThreadData::ThreadData(ThreadFunc func, const std::string& name, pid_t* pid, Semaphore* sem):
    m_callback(func), 
    m_name(name),
    m_id(pid),
    m_semaphore(sem) {};

void ThreadData::runInThread(){
    *m_id = GetThreadId();
    m_id = nullptr;
    m_semaphore->notify();
    m_semaphore = nullptr;
    t_tid = GetThreadId();
    t_thread_name = m_name.empty() ? "UNKNOW" : m_name;
    pthread_setname_np(pthread_self(), m_name.substr(0, 15).c_str());
    try{
        m_callback();
    }catch(const std::exception& e){
        LOG_FMT_FATAL(
            system_logger,
            "ThreadData::runInThread() 线程执行函数抛出异常：%s",
            e.what()
        );
        throw std::system_error();
    }
}

}