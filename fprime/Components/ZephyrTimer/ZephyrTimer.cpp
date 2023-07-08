// ======================================================================
// \title  HelloWorld.cpp
// \author user
// \brief  cpp file for HelloWorld component implementation class
// ======================================================================


#include <Components/ZephyrTimer/ZephyrTimer.hpp>
#include <FpConfig.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Svc/Cycle/TimerVal.hpp>

namespace Components {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ZephyrTimer ::
    ZephyrTimer(
        const char *const compName
    ) : ZephyrTimerComponentBase(compName)
  {

  }

  ZephyrTimer ::
    ~ZephyrTimer()
  {

  }

  void ZephyrTimer :: tick() {
    // get time
    Svc::TimerVal timer;
    timer.take();
    this->CycleOut_out(0, timer);
  }

} // end namespace Components
