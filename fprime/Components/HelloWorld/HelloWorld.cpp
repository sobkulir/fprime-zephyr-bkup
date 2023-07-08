// ======================================================================
// \title  HelloWorld.cpp
// \author user
// \brief  cpp file for HelloWorld component implementation class
// ======================================================================


#include <Components/HelloWorld/HelloWorld.hpp>
#include <FpConfig.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include <cstdio>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

namespace Components {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  HelloWorld ::
    HelloWorld(
        const char *const compName
    ) : HelloWorldComponentBase(compName), m_greetingCount(0)
  {
    // Should be in init probs.
    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE)) {
        printk("GPIO not ready");
    }
    if (!device_is_ready(led.port)) {
        printk("LED not ready");
    }
  }

  HelloWorld ::
    ~HelloWorld()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void HelloWorld ::
    schedIn_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context)
  {
      this->tlmWrite_GreetingCount(++this->m_greetingCount);
  }

  void HelloWorld::TOGGLE_LED_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
      if (!gpio_pin_toggle_dt(&led)) {
          printk("LED Toggled!\n");
      }
    	// Log event for NO_OP here.
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }


} // end namespace Components
