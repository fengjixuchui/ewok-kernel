/* \file usart.c
 *
 * Copyright 2018 The wookey project team <wookey@ssi.gouv.fr>
 *   - Ryad     Benadjila
 *   - Arnauld  Michelizza
 *   - Mathieu  Renard
 *   - Philippe Thierry
 *   - Philippe Trebuchet
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *
 */
#include "autoconf.h"
#include "types.h"
#include "devices.h"
#include "gpio.h"
#include "libc.h"
#include "soc-usart.h"
#include "syscalls.h"

device_t kusart_dev;

void usart_init(void)
{
#ifndef CONFIG_KERNEL_NOSERIAL
    uint32_t    args[4] = { 0 };
    char       *name;

    memset(&kusart_dev, 0, sizeof(device_t));
    /*
     * First registering device and gpio (this will only register
     * a kernel device and lock the associated IP as the kernel
     * as no INIT_DONE principle. IRQ handlers are executed
     * in handler mode.
     */
    kusart_dev.irq_num = 1;
    kusart_dev.size = 0x400;
    kusart_dev.irqs[0].irq = soc_usarts[CONFIG_KERNEL_USART].irq;

    switch (CONFIG_KERNEL_USART) {
        case 1:
            name = "kusart1";
            kusart_dev.address = 0x40011000;
            kusart_dev.irqs[0].handler = (void*)USART1_IRQ_Handler;
            break;
        case 6:
            name = "kusart6";
            kusart_dev.address = 0x40011400;
            kusart_dev.irqs[0].handler = (void*)USART6_IRQ_Handler;
            break;
        case 4:
            name = "kusart4";
            kusart_dev.address = 0x40004C00;
            kusart_dev.irqs[0].handler = (void*)UART4_IRQ_Handler;
            break;

        default:
            break;
    }

    memcpy(kusart_dev.name, name, strlen(name));
    kusart_dev.gpio_num = 2;
    kusart_dev.gpios[0].mask = GPIO_MASK_SET_ALL;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    /* No possible typecasting from uint8_t to :4 */
    kusart_dev.gpios[0].kref.port = soc_usarts[CONFIG_KERNEL_USART].port;
    kusart_dev.gpios[0].kref.pin = soc_usarts[CONFIG_KERNEL_USART].tx_pin;
#pragma GCC diagnostic pop
    kusart_dev.gpios[0].afr = soc_usarts[CONFIG_KERNEL_USART].af;
    kusart_dev.gpios[0].mode = GPIO_PIN_ALTERNATE_MODE;
    kusart_dev.gpios[0].speed = GPIO_PIN_VERY_HIGH_SPEED;
    kusart_dev.gpios[0].lck = 0;

    kusart_dev.gpios[1].mask = GPIO_MASK_SET_ALL;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    /* No possible typecasting from uint8_t to :4 */
    kusart_dev.gpios[1].kref.port = soc_usarts[CONFIG_KERNEL_USART].port;
    kusart_dev.gpios[1].kref.pin = soc_usarts[CONFIG_KERNEL_USART].rx_pin;
#pragma GCC diagnostic pop
    kusart_dev.gpios[1].afr = soc_usarts[CONFIG_KERNEL_USART].af;
    kusart_dev.gpios[1].mode = GPIO_PIN_ALTERNATE_MODE;
    kusart_dev.gpios[1].speed = GPIO_PIN_VERY_HIGH_SPEED;
    kusart_dev.gpios[1].lck = 0;

    int descriptor = 0;
    /* Kernel task is hosting the kernel devices list */
    args[1] = (uint32_t)&kusart_dev;
    args[2] = (uint32_t)&descriptor;
    init_do_reg_devaccess(ID_KERNEL, args, TASK_MODE_MAINTHREAD);
#endif
}
