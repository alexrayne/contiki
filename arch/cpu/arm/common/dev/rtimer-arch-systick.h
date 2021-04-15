/*
 * Copyright (c) 2021, alexraynepe196@gmail.com
 * Copyright (c) 2014, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
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
 * \addtogroup arm-clocks
 * @{
 *
 * \defgroup systick-rtimer SysTick rtimer
 *
 * Implementation of the rtimer module for the SysTick clocks
 * @{
 */
/**
 * \file
 * Header file for the CC13xx/CC26xx rtimer driver
 */
/*---------------------------------------------------------------------------*/
#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_
/*---------------------------------------------------------------------------*/
#include "contiki.h"
/*---------------------------------------------------------------------------*/
//* this is hardware specified default RTC sub-second resolution 32768Hz
#define RTIMER_ARCH_SECOND_NORM     (1000)

#ifdef RTIMER_CONF_ARCH_SECOND
#define RTIMER_ARCH_SECOND RTIMER_CONF_ARCH_SECOND
#else
#define RTIMER_ARCH_SECOND RTIMER_ARCH_SECOND_NORM
#endif
/*---------------------------------------------------------------------------*/
/* Need 4 ticks to syncronize with AON, 2 ticks to account for 2 tick RTC read
   precision and 2 more ticks to put the value in the future. */
#define RTIMER_MINIMAL_SAFE_SYSTICK_SCHEDULE 8u

/*---------------------------------------------------------------------------*/
rtimer_clock_t rtimer_arch_now(void);

typedef uint64_t rtimer64_clock_t;
rtimer64_clock_t rtimer64_now(void);

#define RTIMER_ARCH_TOL  1u

//* this scale used for achieve maximum arguments range, avoiding overload on int32 value
#define RTIMER_USCALC_SCALE     8u
#define RTIMER_US               1000000ul
#define RTIMER_SCALED_US        (RTIMER_US/RTIMER_USCALC_SCALE)
#define RTIMER_SCALED_SECOND    (RTIMER_ARCH_SECOND/RTIMER_USCALC_SCALE)
#if ( (RTIMER_SCALED_SECOND*RTIMER_USCALC_SCALE) != (RTIMER_ARCH_SECOND) )
#error please fix RTIMER_USCALC_SCALE selection for your RTIMER_ARCH_SECOND!!!
#endif

#define US_TO_RTIMERTICKS(US)  (RTIMER_ARCH_TOL * ( ((US) >= 0) ?                        \
                               (((uint32_t)(US) * (RTIMER_SCALED_SECOND / RTIMER_ARCH_TOL) + RTIMER_SCALED_US/2u) / RTIMER_SCALED_US) :      \
                                -(((uint32_t)(-US) * (RTIMER_SCALED_SECOND / RTIMER_ARCH_TOL) + RTIMER_SCALED_US/2u) / RTIMER_SCALED_US)\
                                ) )

#define RTIMERTICKS_TO_US(T)   (((T) >= 0) ?                     \
                               (((uint32_t)(T) * RTIMER_SCALED_US + (RTIMER_SCALED_SECOND/2u)) / RTIMER_SCALED_SECOND) : \
                               -(((uint32_t)(-T) * RTIMER_SCALED_US + (RTIMER_SCALED_SECOND/2u)) / RTIMER_SCALED_SECOND)\
                               )

/*
#if US_TO_RTIMERTICKS(RTIMERTICKS_TO_US(10000)) != 10000
#error fix US_TO_RTIMERTICKS, RTIMERTICKS_TO_US functions !!!
#endif
#if US_TO_RTIMERTICKS(RTIMERTICKS_TO_US(-10000)) != -10000
#error fix US_TO_RTIMERTICKS, RTIMERTICKS_TO_US functions !!!
#endif
*/
/* A 64-bit version because the 32-bit one cannot handle T >= 4295 ticks.
   Intended only for positive values of T. */
#define RTIMERTICKS_TO_US_64(T)  ((uint32_t)(((uint64_t)(T) * 1000000 + ((RTIMER_ARCH_SECOND) / 2)) / (RTIMER_ARCH_SECOND)))
/*---------------------------------------------------------------------------*/
#endif /* RTIMER_ARCH_H_ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
