
#ifndef ZephyrUartDriver_HPP
#define ZephyrUartDriver_HPP

#include "Drv/ByteStreamDriverModel/ByteStreamDriverModelComponentAc.hpp"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/ring_buffer.h>

#define RING_BUF_SIZE 1024

namespace Components {

class ZephyrUartDriver : public Drv::ByteStreamDriverModelComponentBase {
  public:
    enum SetupStatus {
        SETUP_OK, //!<  Setup was successful
        SETUP_DEVICE_NOT_READY, //!<  Device is not ready
    };

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    /**
     * \brief construct the ZephyrUartDriver component.
     * \param compName: name of this component
     */
    ZephyrUartDriver(const char* const compName);


    /**
     * \brief Initialize this component
     * \param instance: instance number of this component
     */
    void init(const NATIVE_INT_TYPE instance = 0);

    SetupStatus setup(const struct device *uart, const NATIVE_INT_TYPE readTaskPriority, const NATIVE_INT_TYPE readTaskStackSize, void *readTaskStack);

    /**
     * \brief Destroy the component
     */
    ~ZephyrUartDriver();

  PRIVATE:

    /**
     * @brief Starts the task that reads from the ring buffer and dispatches it to the deframer.
     * 
     * Must be called from `setup()`, after the ring buffer has been initialized and ideally before
     * the UART starts sending data to the ring_buffer.
     * 
     * @param name 
     * @param priority 
     * @param stackSize 
     * @param stack 
     */
    void startReadTask(const NATIVE_INT_TYPE priority, const NATIVE_INT_TYPE stackSize, void *stack);

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    /**
     * \brief Send data out of the ZephyrUart
     *
     * Passing data to this handler sends them out over UART.
     * 
     * \param portNum: fprime port number of the incoming port call
     * \param fwBuffer: buffer containing data to be sent
     * \return SEND_OK on success, SEND_RETRY when critical data should be retried and SEND_ERROR upon error
     */
    Drv::SendStatus send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer);

    /**
     * \brief Polls out the data from the driver.
     */
    Drv::PollStatus poll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer);

    static void readTask(void *ptr);

    Os::Task m_read_task;  // Task for reading ring_buf and dispatching it to deframer.
    const struct device *m_dev;
    uint8_t m_ring_buf_data[RING_BUF_SIZE];
    struct ring_buf m_ring_buf;
};

}  // end namespace Drv

#endif // end ZephyrUartDriver
