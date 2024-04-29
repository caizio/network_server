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
            m_mutex->lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_locked = false;
            m_mutex->unlock();
        }
    }

private:
    T* m_mutex;
    bool m_locked;
};

/*
    互斥量的封装
*/
class Mutex{
public:
    Mutex(){
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex(){
        pthread_mutex_destroy(&m_mutex);
    }

    int lock() { return pthread_mutex_lock(&m_mutex);}
    int unlock() { return pthread_mutex_unlock(&m_mutex);}
private:
    pthread_mutex_t m_mutex{};
};

using ScopeLock = ScopedLockImpl<Mutex>;

// 读锁包装器，T需要实现Readlock()和unlock()方法
template<class T>
class ReadScopeLockImpl{
public:
    explicit ReadScopeLockImpl(T* mutex): m_mutex(mutex) {
        m_mutex->readlock();
        m_locked = true;
    }

    ~ReadScopeLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex->readlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_locked = false;
            m_mutex->unlock();
        }
    }
private:
    T* m_mutex;
    bool m_locked;
};

// 写锁包装器，T需要实现Writelock()和unlock()方法
template<class T>
class WriteScopeLockImpl{
public:
    explicit WriteScopeLockImpl(T* mutex): m_mutex(mutex) {
        m_mutex->writelock();
        m_locked = true;
    }

    ~WriteScopeLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex->writelock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_locked = false;
            m_mutex->unlock();
        }
    }
private:
    T* m_mutex;
    bool m_locked;
};


/*
    读写锁的封装
*/
class RWLock{
public:
    RWLock(){
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWLock(){
        pthread_rwlock_destroy(&m_lock);
    }

    int readlock(){
        return pthread_rwlock_rdlock(&m_lock);
    }

    int writelock(){
        return pthread_rwlock_wrlock(&m_lock);
    }

    int unlock(){
        return pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock{};
};

// 可直接使用读写锁加锁，无需考虑释放的问题
using ReadScopeLock = ReadScopeLockImpl<RWLock>;
using WriteScopeLock = WriteScopeLockImpl<RWLock>;

}






#endif