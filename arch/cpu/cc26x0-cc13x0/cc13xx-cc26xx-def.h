/*
 * Copyright (c) 2017, George Oikonomou - http://www.spd.gr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
#ifndef CC13XX_CC26XX_DEF_H_
#define CC13XX_CC26XX_DEF_H_
/*---------------------------------------------------------------------------*/
//#include <ti/devices/DeviceFamily.h>
#if defined(CPU_FAMILY_CC13X0) || defined(CPU_FAMILY_CC26X0) //(DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)
#include <cm3/cm3-def.h>
#define CMSIS_CONF_HEADER_PATH              "cc13x0-cc26x0-cm3.h"
#elif defined(CPU_FAMILY_CC13X2) || defined(CPU_FAMILY_CC26X2) //(DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2)
#include <cm4/cm4-def.h>
#define CMSIS_CONF_HEADER_PATH              "cc13x2-cc26x2-cm4.h"
#else// legacy default behaviour
#include "cm3/cm3-def.h"
#define CMSIS_CONF_HEADER_PATH               "cc13x0-cc26x0-cm3.h"
#endif
/*---------------------------------------------------------------------------*/
#define RTIMER_CONF_ARCH_SECOND 65536u
/*---------------------------------------------------------------------------*/
#include "rf-core/radio-def.h"
/*---------------------------------------------------------------------------*/
/* Path to headers with implementation of mutexes, atomic and memory barriers */
#define MUTEX_CONF_ARCH_HEADER_PATH          "mutex-cortex.h"
#define ATOMIC_CONF_ARCH_HEADER_PATH         "atomic-cortex.h"
#define INT_MASTER_CONF_ARCH_HDR_PATH        "int-master-arch.h"
#define MEMORY_BARRIER_CONF_ARCH_HEADER_PATH "memory-barrier-cortex.h"
/*---------------------------------------------------------------------------*/
#define GPIO_HAL_CONF_ARCH_HDR_PATH          "dev/gpio-hal-arch.h"
#define GPIO_HAL_CONF_ARCH_SW_TOGGLE         0
#define GPIO_HAL_CONF_PORT_PIN_NUMBERING     0
/*---------------------------------------------------------------------------*/
#define SPI_CONF_CONTROLLER_COUNT    2
/*---------------------------------------------------------------------------*/
#endif /* CC13XX_CC26XX_DEF_H_ */
/*---------------------------------------------------------------------------*/
