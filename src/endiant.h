#ifndef __CAIZI__ENDIAN_H__
#define __CAIZI__ENDIAN_H__

// 小端和大端的宏
#define CAIZI_LITTLE_ENDIAN 1
#define CAIZI_BIG_ENDIAN 2

#include <cstdint>    // 用于 uint16_t 类型的定义
#include <byteswap.h> // 用于 bswap_16 函数的定义
#include <type_traits>
/*
    字节序处理
*/
namespace caizi{

// 2字节的字节序转化
template<class T> 
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
}

// 4字节的字节序转化
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value) {
    return (T)bswap_32((uint32_t)value);
}

// 8字节的字节序转化
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value){
    return  (T)bswap_64((uint64_t)value);
};

#if BYTE_ORDER == BIG_ENDIAN
#define CAIZI_BYTE_ORDER CAIZI_BIG_ENDIAN
#else
#define CAIZI_BYTE_ORDER CAIZI_LITTLE_ENDIAN
#endif


#if CAIZI_BYTE_ORDER == CAIZI_BIG_ENDIAN

// 转小端字节序
template <class T>
T byteswapOnLitterEndian(T value){
    return byteswap(value);
}

// 转大端字节序
template <class T>
T byteswapOnBigEndian(T value){
    return value;
};
#else

// 转小端字节序
template <class T>
T byteswapOnLitterEndian(T value){
    return value;
}

// 转大端字节序
template <class T>
T byteswapOnBigEndian(T value){
    return byteswap(value);
}

#endif
}

#endif