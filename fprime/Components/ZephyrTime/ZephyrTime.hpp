// ======================================================================
// ZephyrTime.hpp
// Standardization header for ZephyrTime
// ======================================================================

#ifndef Components_ZephyrTime_HPP
#define Components_ZephyrTime_HPP

#include <Components/ZephyrTime/ZephyrTimeComponentAc.hpp>

namespace Components {

class ZephyrTime: public ZephyrTimeComponentBase {
    public:
        ZephyrTime(const char* compName);
        virtual ~ZephyrTime();
        void init(NATIVE_INT_TYPE instance);
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
};

}

#endif
