#include "caizi.h"
#include "thread.h"
#include "address.h"
#include <iostream>

static caizi::Logger::ptr g_logger = CAIZI_GET_ROOT_LOGGER();
int main(){
    auto addr = caizi::IPAddress::create("192.168.1.1", 80);
    if(addr){
        LOG_INFO(g_logger, addr->toString() + "\n");
    }
    return 0;
}