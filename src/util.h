#ifndef __CAIZI_UTIL_H__
#define __CAIZI_UTIL_H__

#include <vector>
#include <string>

namespace caizi{

long GetThreadId();

void __GetBacktrace(std::vector<std::string>&bt, int size, int skip = 0);
std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "  ");


}

#endif