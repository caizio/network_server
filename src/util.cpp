#include "util.h"
#include <sys/syscall.h>
#include <unistd.h>

namespace caizi{

long GetThreadId(){
    return ::syscall(SYS_gettid);
}

}