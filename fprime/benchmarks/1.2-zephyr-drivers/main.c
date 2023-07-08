#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>

#define UART_DEVICE_NODE DT_NODELABEL(usart2)
#define RING_BUF_SIZE 1024

const struct device *const uart = DEVICE_DT_GET(UART_DEVICE_NODE);
static struct ring_buf ring_buf;
static uint8_t ring_buf_data[RING_BUF_SIZE];

void serial_cb(const struct device *dev, void *user_data)
{
    struct ring_buf *ring_buf = user_data;

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

void main(void)
{
    ring_buf_init(&ring_buf, RING_BUF_SIZE, ring_buf_data);
    
    if (!device_is_ready(uart)) {
		printk("UART device not ready.\n");
        return;
	}

    int ret = uart_irq_callback_user_data_set(uart, serial_cb, &ring_buf);
    if (ret) {
        printk("Failed to set IRQ callback.\n");
        return;
    }

	uart_irq_rx_enable(uart);
	uart_irq_tx_disable(uart);

    while (true) {
        uint8_t c;
        uint32_t recv_size = ring_buf_get(&ring_buf, &c, 1);
        printk("%c", c);
    }

}