// 实现单例

#ifndef SINGLETON
#define SINGLETON

namespace caizi{

// 通过私有化构造函数，通过静态方法调用，每次获取一样的对象

template <class T>
class Singleton final{
public:
    static T* getInstance(){
        static T ins;
        return &ins;
    }

private:
    Singleton() = default;
};

template <class T>
class SingletonPtr final{
public:
    static std::shared_ptr<T> getInstance(){
        static auto ins = std::make_shared<T>();
        return ins;
    };
private:
    SingletonPtr() = default;
};

}

#endif