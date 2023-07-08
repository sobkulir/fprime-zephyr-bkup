// ======================================================================
// \title  HelloWorld.hpp
// \author user
// \brief  hpp file for HelloWorld component implementation class
// ======================================================================

#ifndef HelloWorld_HPP
#define HelloWorld_HPP

#include "Components/HelloWorld/HelloWorldComponentAc.hpp"
#include <Fw/Com/ComBuffer.hpp>

namespace Components {

  class HelloWorld :
    public HelloWorldComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object HelloWorld
      //!
      HelloWorld(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object HelloWorld
      //!
      ~HelloWorld();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      void schedIn_handler(
          const NATIVE_INT_TYPE portNum,
          NATIVE_UINT_TYPE context
      );

      void TOGGLE_LED_cmdHandler(FwOpcodeType opCode, U32 cmdSeq);
      
      U32 m_greetingCount;

    };

} // end namespace Components

#endif
