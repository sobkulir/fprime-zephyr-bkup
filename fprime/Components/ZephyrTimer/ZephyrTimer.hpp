// ======================================================================
// \title  ZephyrTimer.hpp
// \author user
// \brief  hpp file for ZephyrTimer component implementation class
// ======================================================================

#ifndef ZephyrTimer_HPP
#define ZephyrTimer_HPP

#include "Components/ZephyrTimer/ZephyrTimerComponentAc.hpp"
#include <Fw/Com/ComBuffer.hpp>

namespace Components {

  class ZephyrTimer :
    public ZephyrTimerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object ZephyrTimer
      //!
      ZephyrTimer(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object ZephyrTimer
      //!
      ~ZephyrTimer();

      void tick();


    };

} // end namespace Components

#endif
