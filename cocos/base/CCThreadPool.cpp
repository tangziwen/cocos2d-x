/****************************************************************************
origin from https://github.com/progschj/ThreadPool
 ****************************************************************************/
#include "base/CCThreadPool.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include <sys/sysctl.h>
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <cpu-features.h>
#endif

NS_CC_BEGIN

ThreadPool* ThreadPool::getInstance()
{
//    static int threadcnt = 0;
//    if (threadcnt == 0)
//    {
//#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
//        size_t size = sizeof(int);
//        int mib[2] = {CTL_HW, HW_NCPU};
//        sysctl(mib, 2, &threadcnt, &size, NULL, 0);
//#endif
//        
//#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
//        threadcnt = android_getCpuCount();
//#endif
//        threadcnt = MAX(threadcnt, 1);
//    }

    static ThreadPool s_ThreadPool(1);
    return &s_ThreadPool;
}

NS_CC_END
