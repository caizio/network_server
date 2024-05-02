#include "caizi.h"
#include "util.h"
#include <assert.h>
#include <vector>
#include <string>
#include <execinfo.h>

void test_backtrace(){
    std::string bt = caizi::BacktraceToString(128, 0, "");
    std::cout << bt << std::endl;
}

int main(){
    test_backtrace();
    
    return 0;
}