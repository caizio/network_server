#ifndef __CAIZI__THREAD__
#define __CAIZI__THREAD__

#include <thread>
#include <pthread.h>
#include <functional>
#include <memory>
#include <semaphore.h>
#include "noncopyable.h"

namespace caizi{

class Semaphore : public Noncopyable{
public:
    explicit Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void notify();
private:
    sem_t m_semaphore;
};

class Thread : public Noncopyable{
public:
    typedef std::shared_ptr<Thread> ptr;
    typedef std::unique_ptr<Thread> uptr;
    typedef std::function<void()> Threadfunc;

    Thread(Threadfunc cb, const std::string& name);
    ~Thread();

    pid_t getId() { return m_id; };
    const std::string& getName() const { return m_name; };
    void setName(const std::string &name) {m_name = name; };
    int join();

public:
    static Thread* GetThis();
    static pid_t GetThisId();
    static const std::string& GetThisThreadName();
    static void SetThisThreadName(const std::string& name);

private:
    Thread(const Thread& other) = delete;
    Thread(const Thread&& other) = delete;
    Thread& operator=(const Thread& other) = delete;
    static void* Run(void* arg);

private:
    pid_t m_id;
    pthread_t m_thread;
    std::string m_name;
    Threadfunc m_callback;
    Semaphore m_semaphore;
    bool m_started;
    bool m_joined;
};

struct ThreadData{
    typedef Thread::Threadfunc ThreadFunc;
    ThreadFunc m_callback;
    std::string m_name;
    pid_t* m_id;
    Semaphore* m_semaphore;

    ThreadData(ThreadFunc func, const std::string& name, pid_t* pid, Semaphore* sem);
    void runInThread();
};

// 锁的模板类
template<class T>
class ScopedLockImpl{
public:
    explicit ScopedLockImpl(T* mutex) : m_mutex(mutex){
            m_mutex->lock();
            m_locked = true;
        }

    ~ScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_locked = false;
            m_mutex.unlock();
        }
    }

private:
    T* m_mutex;
    bool m_locked;
};


}






#endif