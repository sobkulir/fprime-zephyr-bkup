
#include <Components/ZephyrTime/ZephyrTime.hpp>
#include <Fw/Time/Time.hpp>

#include <zephyr/posix/time.h>

namespace Components {

    ZephyrTime::ZephyrTime(const char* name) : ZephyrTimeComponentBase(name)
    {
    }

    ZephyrTime::~ZephyrTime() {
    }

    void ZephyrTime::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        struct timespec stime;
        (void)clock_gettime(CLOCK_REALTIME,&stime);
        time.set(TB_PROC_TIME,0, stime.tv_sec, stime.tv_nsec/1000);
    
    }

    void ZephyrTime::init(NATIVE_INT_TYPE instance) {
        ZephyrTimeComponentBase::init(instance);
    }

}
