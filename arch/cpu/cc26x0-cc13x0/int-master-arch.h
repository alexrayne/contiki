/*
 * Copyright (c) 2020, alexrayne <alexraynepe196@gmail.com>
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
/**
 * \addtogroup cc26xx
 * @{
 *
 * \defgroup cc26xx-interrupts CC13xx-CC26xx master interrupt manipulation
 *
 * Master interrupt manipulation routines for the CC13xx and CC26xx CPUs
 *
 * @{
 *
 * \file
 * Master interrupt manipulation implementation for the TI CC13xx/CC26xx
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "sys/int-master.h"

#ifdef CMSIS_CONF_HEADER_PATH
#include CMSIS_CONF_HEADER_PATH
#else
#include "cc13x0-cc26x0-cm3.h"
#endif

#include "ti-lib.h"

#include <stdbool.h>
/*---------------------------------------------------------------------------*/

#define int_master_arch_enable()            CPUcpsie()
#define int_master_arch_read_and_disable()  CPUcpsid()
#define int_master_arch_status_set(status)  __set_PRIMASK(status)
#define int_master_arch_is_enabled()        (ti_lib_cpu_primask() ? false : true)

/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
