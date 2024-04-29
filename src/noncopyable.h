/*
    @file noncopyable.h
    @brief 不可对象的封装
*/ 

#ifndef __NONCOPY_ABLE__
#define __NONCOPY_ABLE__

namespace caizi{

// @brief 通过继承这个类，实现不可复制对象
class Noncopyable{
public:
    Noncopyable(const Noncopyable&) = delete;
    // 禁用移动构造，即使用move的方法
    Noncopyable(const Noncopyable&&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
};
}


#endif