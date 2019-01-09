/*
 * Copyright (c) 2019 kmauch@     All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in the
 *          documentation and/or other materials provided with the distribution.
 *        * Neither the name of The Linux Foundation nor
 *          the names of its contributors may be used to endorse or promote
 *          products derived from this software without specific prior written
 *          permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Abstract: a driver to test input and output of SITARA platform
 * 
 * Note: includes <linux/init.h>, <linux/kernel.h> not needed on 4.15.0-43-generic
 *
 */

/* module_init(), module_exit() */
#include <linux/module.h>

/* The GPIO interface  */
#include <linux/gpio.h>

/* IRQ numbers */
#include <linux/interrupt.h>

/* GPIO 61 - output pin */
static unsigned int gpioOutput = 61;

/* GPIO36 - input pin */
static unsigned int gpioInput = 36;

/* share IRQ num within file */
static unsigned int irqNumber;

/* store number of presses */
static unsigned int numberPresses = 0;

/* used to invert state of pin */
static bool outputOn = 0;

/* Dedicated IRQ handler declaration */
static irq_handler_t  input_output_gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

/* The TRPZ initialization function */
static int __init input_output_gpio_init(void)
{
int result = 0;

        printk(KERN_INFO "Initialize the TRPZ\n");

        /* Check if GPIO is allright */
        if (!gpio_is_valid(gpioOutput))
        {
                printk(KERN_INFO "output pin is invalid\n");

                return -ENODEV;
        }

        outputOn = true;

        /* Request output pin GPIO */
        gpio_request(gpioOutput, "sysfs");

        /* Set the output mode to a pin */
        gpio_direction_output(gpioOutput, outputOn);

        /* Plant a file in /sys/class/gpio for output pin */
        gpio_export(gpioOutput, false);

        /* Request an input pin GPIO */
        gpio_request(gpioInput, "sysfs");

        /* Set the intput mode to an input pin */
        gpio_direction_input(gpioInput);

        /* Plant a file in /sys/class/gpio for input pin */
        gpio_export(gpioInput, false);

        /* Comment */
        printk(KERN_INFO "The input pin value is: %d\n", gpio_get_value(gpioInput));

        /* Map the input pin into IRQ number */
        irqNumber = gpio_to_irq(gpioInput);

        /* Comment */
        printk(KERN_INFO "The pin is mapped to IRQ: %d\n", irqNumber);

        /* This next call requests an interrupt line */
        result = request_irq(irqNumber /* interrupt number requested*/, (irq_handler_t) input_output_gpio_irq_handler /* handler function */, IRQF_TRIGGER_RISING /* on rising edge (press, not release) */, "ebb_gpio_handler" /* used in /proc/interrupts */, NULL /* *dev_id for shared interrupt lines */);

        /* Tell what's up*/
        printk(KERN_INFO "IRQ requested, result: %d\n", result);

        return result;

} /* input_output_gpio_init() */

static void __exit input_output_gpio_exit(void)
{
        printk(KERN_INFO "Leaving the driver \n");

        /* Set the GPIO pin off */
        gpio_set_value(gpioOutput, 0);

        /* Unexport the pin of GPIO */
        gpio_unexport(gpioOutput);

        /* Free the IRQ number */
        free_irq(irqNumber, NULL /* no *dev_id */);

        /* Unexport the input pin of GPIO */
        gpio_unexport(gpioInput);

        /* Free the output pin of GPIO */
        gpio_free(gpioOutput);

        /* Free the input pin of GPIO */
        gpio_free(gpioInput);

        /* Tell what's up*/
        printk(KERN_INFO "Done\n");

} /* input_output_gpio_exit() */

/* IRQ handler, cannot be invoked concurrently as the input pin is masked out until the handling is over */
static irq_handler_t input_output_gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
        /* Invert current state of an output pin */
        outputOn = !outputOn;

        /* Se the output pin state */
        gpio_set_value(gpioOutput, outputOn);

        /* Tell what's up */
        printk(KERN_INFO "in interrupt, pin is: %d\n", gpio_get_value(gpioInput) );

        /* An amount of times theinput pin has been pressed */
        numberPresses++;

        /* Define the IRQ has been processed */
        return (irq_handler_t) IRQ_HANDLED;

} /* input_output_gpio_irq_handler(. . .) */

module_init(input_output_gpio_init);

module_exit(input_output_gpio_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("kmauch@ ");

MODULE_DESCRIPTION("A module to process input/output pins on GPIO bus");

MODULE_VERSION("0.9");
