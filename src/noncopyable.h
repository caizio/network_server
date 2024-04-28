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
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};
}


#endif