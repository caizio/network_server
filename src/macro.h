#ifndef __CAIZI_MARCO_H__
#define __CAIZI_MARCO_H__

#include <string>
#include <assert.h>

#include "log.h"

static caizi::Logger::ptr logger = CAIZI_GET_LOGGER("system");

// 如果是GCC编译器，告诉编译器按照参数的预期结果进行优化
#if defined __GNUC__
#define CAIZI_LIKELY(x) __builtin_expect(!!(x), 1)
#define CAIZI_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define CAIZI_LIKELY(x) (x)
#define CAIZI_UNLIKELY(x) (x)
#endif

// 断言的封装，输出断言结果和调用函数的栈信息
// #define CAIZI_ASSERT(x) \
//     if(CAIZI_UNLIKELY(!(x))){ \
//         // CAIZI_Lo \
//                     \
//     }


#endif