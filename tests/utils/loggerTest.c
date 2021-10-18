#include "../../src/utils/logger.h"
#include<time.h>

int main()
{
    LOG_DEBUG("debug");
    LOG_INFO("info");
    LOG_WARNING("warning");
    sleep(2);
    LOG_ERROR("error %d", 1);
    LOG_CRITICAL("critical");    
    return 0;
}