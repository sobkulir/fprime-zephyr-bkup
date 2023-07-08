// ======================================================================
// \title  TcpClientComponentImpl.cpp
// \author mstarch
// \brief  cpp file for TcpClientComponentImpl component implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Components/ZephyrUartDriver/ZephyrUartDriver.hpp>
#include <FpConfig.hpp>
#include "Fw/Types/Assert.hpp"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>

#include <cassert>

namespace Components {

namespace {

void serial_cb(const struct device *dev, void *user_data)
{
    struct ring_buf *ring_buf = reinterpret_cast<struct ring_buf *>(user_data);

	if (!uart_irq_update(dev)) {
		return;
	}

	if (!uart_irq_rx_ready(dev)) {
		return;
	}

    uint8_t c;
    // TODO: Get rid of the endless loop (in an IRQ handler!).
    while (uart_fifo_read(dev, &c, 1) == 1) {
		if (ring_buf_put(ring_buf, &c, 1) != 1) {
            // TODO: Handle properly.
            printk("UART buffer overrun\n");
        }
	}
}

} // namespace

void ZephyrUartDriver::readTask(void *ptr) {
    ZephyrUartDriver *self = reinterpret_cast<ZephyrUartDriver *>(ptr);
    FW_ASSERT(self != nullptr);

    while (true) {
        Fw::Buffer buffer = self->allocate_out(0, 1024);
        U8 *data = buffer.getData();
        U32 size = buffer.getSize();
        FW_ASSERT(data && size > 0);

        uint32_t recv_size = ring_buf_get(&self->m_ring_buf, data, size);
        buffer.setSize(recv_size);
        Drv::RecvStatus recvStatus = (recv_size > 0) ? Drv::RecvStatus::RECV_OK : Drv::RecvStatus::RECV_ERROR;
        
        if (self->isConnected_recv_OutputPort(0)) {
            self->recv_out(0, buffer, recvStatus);
        }

        // This is a hack, we should use a semaphore instead.
        k_sleep(K_MSEC(2));
    }
}

void ZephyrUartDriver::startReadTask(const NATIVE_INT_TYPE priority, const NATIVE_INT_TYPE stackSize, void *stack) {
    FW_ASSERT(not this->m_read_task.isStarted()); 

    Os::TaskString name("ZephyrUartDriverReadTask");

    Os::Task::TaskStatus stat = this->m_read_task.start(name, readTask, /*arg=*/this, priority, stackSize, /*cpuAffinity=*/Os::Task::TASK_DEFAULT, /*identifier=*/Os::Task::TASK_DEFAULT, stack);
    FW_ASSERT(Os::Task::TASK_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));
}

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ZephyrUartDriver::ZephyrUartDriver(const char* const compName)
    : ByteStreamDriverModelComponentBase(compName) {}

void ZephyrUartDriver::init(const NATIVE_INT_TYPE instance) {
    ByteStreamDriverModelComponentBase::init(instance);
}

ZephyrUartDriver::SetupStatus ZephyrUartDriver::setup(const struct device *uart, const NATIVE_INT_TYPE readTaskPriority, const NATIVE_INT_TYPE readTaskStackSize, void *readTaskStack) {
    this->m_dev = uart;
    ring_buf_init(&this->m_ring_buf, RING_BUF_SIZE, this->m_ring_buf_data);
    
    if (!device_is_ready(this->m_dev)) {
		return SETUP_DEVICE_NOT_READY;
	}

    this->startReadTask(readTaskPriority, readTaskStackSize, readTaskStack);

    int ret = uart_irq_callback_user_data_set(this->m_dev, serial_cb, &this->m_ring_buf);
    FW_ASSERT(ret == 0, ret);

	uart_irq_rx_enable(this->m_dev);
	uart_irq_tx_disable(this->m_dev);

    return SETUP_OK;
}


ZephyrUartDriver::~ZephyrUartDriver() {}


// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus ZephyrUartDriver::send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    U8 *data = fwBuffer.getData();
    for (U32 i = 0; i < fwBuffer.getSize(); i++) {
        uart_poll_out(this->m_dev, data[i]);
    }

    if (this->isConnected_deallocate_OutputPort(0)) {
        deallocate_out(0, fwBuffer);
    }
    return Drv::SendStatus::SEND_OK;
}

Drv::PollStatus ZephyrUartDriver::poll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {

    U8 *data = fwBuffer.getData();

    uint32_t recv_size = ring_buf_get(&this->m_ring_buf, data, fwBuffer.getSize());
    fwBuffer.setSize(recv_size);
    return (recv_size > 0) ? Drv::PollStatus::POLL_OK : Drv::PollStatus::POLL_RETRY;
}

}  // end namespace Components
