/**
 * \addtogroup arm-cpu
 *
 * @{
 */

/*
 * Copyright (c) 2010, STMicroelectronics.
 * Copyright (c) 2021, alexrayne.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
* \file
*			Clock.
* \author
*			Salvatore Pitrulli <salvopitru@users.sourceforge.net>
*/

#include "contiki.h"
#include "sys/clock.h"
#include CMSIS_CONF_HEADER_PATH
#include "sys/critical.h"
#include "sys/rtimer.h"
#include <sys/critical.h>



/*--------------------------------------------------------------------------*/

// provide rtimer_now API
#ifdef SYSTICK_CONF_RTIMERNOW
#define SYSTICK_RTIMERNOW  SYSTICK_CONF_RTIMERNOW
#else
#define SYSTICK_RTIMERNOW  0
#endif


#ifdef SYSTICK_RTC_ADJUST
/// @brief clock to stable RTC adjuster.
///         call every 1sec, to estimate difference sclock to stable RTC
/// @return -
int SYSTICK_RTC_ADJUST(void);
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////
#if 1
#include <assert.h>
#define ASSERT(...) assert(__VA_ARGS__);
#else
#define ASSERT(...)
#endif



////////////////////////////////////////////////////////////////////////////////////
#include "sys/log.h"
LOG_MODULE_LOCAL("systick");
#ifdef LOG_LEVEL_SYSTICK
#define LOG_LEVEL LOG_LEVEL_SYSTICK
#else
#define LOG_LEVEL LOG_LEVEL_NONE
#endif


/*--------------------------------------------------------------------------*/
/* The value that will be load in the SysTick value register. */
#ifdef FSYSTICK_HZ
#define RELOAD_VALUE (FSYSTICK_HZ / CLOCK_SECOND)
#else
#define RELOAD_VALUE (SystemCoreClock / CLOCK_SECOND)
#endif

volatile clock_time_t systick_clock_count;
#define count   systick_clock_count

static volatile unsigned long current_seconds = 0;
static unsigned int second_countdown = CLOCK_SECOND;

#if SYSTICK_RTIMERNOW

#include <sys/rtimer.h>

//static
volatile rtimer64_clock_t rtcount;

#define RTIMER_CLOCK_TIME (RTIMER_ARCH_SECOND / CLOCK_SECOND)
#define RTIMER_CLOCK_IRR (RTIMER_ARCH_SECOND % CLOCK_SECOND)
#define RTIMER_SCALE      (RELOAD_VALUE/RTIMER_CLOCK_TIME )

#endif


#ifdef SYSTICK_RTC_ADJUST
unsigned rtc_adjust_least;
unsigned long systick_reload;
#else
#define rtc_adjust_least 0
#endif


#ifndef IS_POW2
// @brief is exact power2 number - only 1bit
#define IS_POW2(x) ( (((x)-1) & (x))== 0 )
#endif

/*---------------------------------------------------------------------------*/
void
clock_init(void)
{
    int_master_status_t cs = critical_enter();

#if __CM_CMSIS_VERSION_MAIN < 5
    /* Counts the number of ticks. */
    count = 0;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_SetReload(RELOAD_VALUE-1);
    SysTick_ITConfig(ENABLE);
    SysTick_CounterCmd(SysTick_Counter_Enable);
#else
    SysTick_Config(RELOAD_VALUE);
#endif

#if SYSTICK_RTIMERNOW

#if (RTIMER_CLOCK_IRR != 0)
#error "not implemented irrational RTIMER_ARCH_SECOND / CLOCK_SECOND "
#endif
    ASSERT( RELOAD_VALUE == RTIMER_CLOCK_TIME );
    ASSERT( IS_POW2(RTIMER_SCALE) );
#endif

#ifdef SYSTICK_RTC_ADJUST
    rtc_adjust_least = 0;
#endif

    critical_exit(cs);
}

/*---------------------------------------------------------------------------*/
void
SysTick_Handler(void)
{
  ++count;

#if SYSTICK_RTIMERNOW

  int_master_status_t is = critical_enter();
  if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
      rtcount += RTIMER_CLOCK_TIME;
  critical_exit(is);

#endif

#ifdef SYSTICK_RTC_ADJUST
  if (rtc_adjust_least > 0){
      --rtc_adjust_least;
      //recover basic clock rate after adjusted
      if (rtc_adjust_least == 0)
          SysTick->LOAD  = systick_reload;
  }
  else {
    int diff = SYSTICK_RTC_ADJUST();
    if (diff == 0) {
        // cancel correction
        if (rtc_adjust_least > 0)
            SysTick->LOAD  = systick_reload;
        rtc_adjust_least = 0;
    }
    else
    if (diff > 0){
        rtc_adjust_least = diff;
        //rtc_adjust_least *= RTIMER_CLOCK_TIME;
        systick_reload = SysTick->LOAD;
        if (rtc_adjust_least < CLOCK_SECOND) {
            SysTick->LOAD  = systick_reload+1;
            LOG_TRACE("tick %d -> 1*%u \n", diff,  rtc_adjust_least);
        }
        else {
            //too big correction
            SysTick->LOAD  = systick_reload + ((rtc_adjust_least + CLOCK_SECOND/2)/CLOCK_SECOND);
            rtc_adjust_least = CLOCK_SECOND-1;
            LOG_TRACE("tick %d -> %ld*%u \n", diff, (SysTick->LOAD - systick_reload) , (unsigned)rtc_adjust_least);
        }
    }
    else if (diff < 0){
        rtc_adjust_least = -diff;
        //rtc_adjust_least *= RTIMER_CLOCK_TIME;
        systick_reload = SysTick->LOAD;
        if (rtc_adjust_least < CLOCK_SECOND) {
            SysTick->LOAD  = systick_reload-1;
            LOG_TRACE("tick %d -> -1*%u \n", diff,  rtc_adjust_least);
        }
        else {
            //too big correction
            SysTick->LOAD  = systick_reload - ((rtc_adjust_least + CLOCK_SECOND/2)/CLOCK_SECOND);
            rtc_adjust_least = CLOCK_SECOND-1;
            LOG_TRACE("tick %d -> %ld*%u \n", diff, (SysTick->LOAD - systick_reload) , (unsigned)rtc_adjust_least);
        }
    }
  } // else if (rtc_adjust_least > 0)
#endif

  if(--second_countdown == 0) {
    ++current_seconds;
    second_countdown = CLOCK_SECOND;

  } // if(--second_countdown == 0)

#ifdef SYSTICK_CLOCK_HANDLE
  SYSTICK_CLOCK_HANDLE( count );
#else
  if(etimer_pending()) {
    etimer_request_poll();
  }
#endif

}
/*---------------------------------------------------------------------------*/

union u64rec {
    uint64_t value;
    uint32_t w[2];
};
typedef union u64rec u64rec;

clock_time_t clock_time(void)
{
    u64rec  ret;
    volatile u64rec* wcount = (volatile u64rec*)&count;

    ret.w[1] = wcount->w[1];
    ret.w[0] = wcount->w[0];
    while (ret.w[1] != wcount->w[1]){
        ret.w[1] = wcount->w[1];
        ret.w[0] = wcount->w[0];
    }
    return ret.value;
}
/*---------------------------------------------------------------------------*/
/**
 * Delay the CPU for a multiple of TODO
 */
void
clock_delay(unsigned int i)
{
  for(; i > 0; i--) {           /* Needs fixing XXX */
    unsigned j;
    for(j = 50; j > 0; j--) {
      __NOP();
    }
  }
}
/*---------------------------------------------------------------------------*/
/*
 * Wait for a multiple of 1 ms.
 */
void
clock_wait(clock_time_t i)
{
  clock_time_t start;

  start = clock_time();
  while(clock_time() - start < (clock_time_t) i);
}
/*---------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
  return current_seconds;
}
/*--------------------------------------------------------------------------*/
/** @} */

#if SYSTICK_RTIMERNOW

rtimer64_clock_t rtimer64_now(void){
    int_master_status_t is = critical_enter();

    uint64_t t0         = rtcount;
    uint32_t intick     = SysTick->VAL;
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        t0 += RTIMER_CLOCK_TIME;
        rtcount  = t0;
        intick = SysTick->VAL;
    }
    critical_exit(is);

#ifdef SYSTICK_RTC_ADJUST
    uint32_t top = SysTick->LOAD+1;
#else
    uint32_t top = RELOAD_VALUE;
#endif

    return t0 + (top - intick)/RTIMER_SCALE;
}

rtimer_clock_t rtimer_arch_now(void){
    int_master_status_t is = critical_enter();

    uint32_t t0         = rtcount;
    uint32_t intick     = SysTick->VAL;
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
        t0 += RTIMER_CLOCK_TIME;
        rtcount  = t0;
        intick = SysTick->VAL;
    }
    critical_exit(is);

#ifdef SYSTICK_RTC_ADJUST
    uint32_t top = SysTick->LOAD+1;
#else
    uint32_t top = RELOAD_VALUE;
#endif

    return t0 + (top - intick)/RTIMER_SCALE;
}

#endif
