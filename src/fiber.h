#ifndef __CAIZI__FIBER_H__
#define __CAIZI__FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>

namespace caizi{

class Fiber: public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State{
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

    Fiber(std::function<void()> callback,  size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    void reset(std::function<void()> callback);
    void swapIn();
    void swapOut();
    void call();
    void back();
    
    uint64_t getId() const{ return m_id; };
    State getcurrentState() const{ return m_state; };

    static void SetThis(Fiber* fiber);
    static Fiber::ptr GetThis();
    static void YieldToReady();
    static void YieldToHole();
    static uint64_t GetTotalFiberCount();

    static void MainFunction();
    static void CallMainFunction();

    static uint64_t GetFiberID();

private:
    Fiber();

private:
    uint64_t m_id = 0;                  // 协程id
    uint32_t m_stacksize = 0;           // 协程运行栈大小
    State m_state = INIT;               // 协程运行状态
    ucontext_t m_context;               // 协程上下文
    void* m_stack = nullptr;            // 协程运行栈指针
    std::function<void()> m_callback;   // 协程运行函数
};

}

#endif