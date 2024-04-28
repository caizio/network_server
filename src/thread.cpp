#include "thread.h"
#include "log.h"
#include <cassert>

namespace caizi{

static thread_local Thread* t_thread = nullptr;
static thread_local pid_t t_tid = 0;
static thread_local std::string t_thread_name = "UNKNOW";

static Logger::ptr system_logger = GET_LOGGER("system");

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

}
Thread::~Thread(){

}

int Thread::join(){
    if(m_thread){
        int rt = pthread_join(m_thread, nullptr);
        if(rt){
            LOG_ERROR(system_logger,"Thread::join() 添加线程失败！");
            throw std::system_error();
        }
        m_thread = 0;
    }
    return 0;
}

void* Thread::Run(void* arg){
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    thread->m_id = GetThisId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());
    Threadfunc cb;
    cb.swap(thread->m_callback);
    cb();
    return 0;
}

/*
    ThreadData结构体
*/
ThreadData::ThreadData(){};
void ThreadData::runInThread(void* arg){

}

}