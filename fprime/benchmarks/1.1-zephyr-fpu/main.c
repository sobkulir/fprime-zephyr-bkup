#include <zephyr/kernel.h>

void main(void)
{
    volatile double a = 1.5;
    printk("Hello World: %lf\n", a / 4.2);
}