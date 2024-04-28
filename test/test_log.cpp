#include "log.h"

using namespace std;
using namespace caizi;

void test1(){
    Logger::ptr logger(new Logger());

    StdoutLogAppender::ptr stdappender(new StdoutLogAppender());

    logger->addAppender(stdappender);
    LOG_DEBUG(logger, "消息消息 " + std::to_string(time(nullptr)) + "\n");
    LOG_INFO(logger, "消息消息 " + std::to_string(time(nullptr)) + "\n");
    LOG_WARN(logger, "消息消息 " + std::to_string(time(nullptr)) + "\n");
    LOG_ERROR(logger, "消息消息 " + std::to_string(time(nullptr)) + "\n");
    LOG_FATAL(logger, "消息消息 " + std::to_string(time(nullptr)) + "\n");
}



int main(){

    test1();
    auto logger = GET_ROOT_LOGGER();
    LOG_INFO(logger, "test\n");
    return 0;
}