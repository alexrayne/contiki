/*
 * Copyright (c) 2014, SICS Swedish ICT.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \addtogroup tsch
 * @{
 * \file
 *	TSCH per-slot logging
*/

#ifndef __TSCH_LOG_H__
#define __TSCH_LOG_H__

/********** Includes **********/

#include "contiki.h"
#include "sys/rtimer.h"
#include "net/mac/tsch/tsch-types.h"
// need uip_lladdr_t
#include "net/ip/uip.h"

/******** Configuration *******/

/* TSCH per-slot logging. Enabled by default if DBG is enabled */
#ifdef TSCH_LOG_CONF_PER_SLOT
#define TSCH_LOG_PER_SLOT TSCH_LOG_CONF_PER_SLOT
#else /* TSCH_LOG_CONF_PER_SLOT */
#include "sys/log.h"
#define TSCH_LOG_PER_SLOT (LOG_CONF_LEVEL_MAC >= LOG_LEVEL_DBG)
#endif /* TSCH_LOG_CONF_PER_SLOT */

/* TSCH log levels:
 * -1: no any print, even failure reports
 * 0: no log
 * 1: basic PRINTF enabled
 * 2: basic PRINTF enabled and tsch-log module enabled */
#ifdef TSCH_LOG_CONF_LEVEL
#define TSCH_LOG_LEVEL TSCH_LOG_CONF_LEVEL
#else /* TSCH_LOG_CONF_LEVEL */

#if TSCH_LOG_PER_SLOT
#define TSCH_LOG_LEVEL 2
#else
#define TSCH_LOG_LEVEL 0
#endif

#endif /* TSCH_LOG_CONF_LEVEL */

#ifndef DEBUG
// TSCH_PRINT use net-debug.h debug printer
// so need to:
//  #define DEBUG (DEBUG_PRINT)
//  #include "net/net-debug.h"
#endif



/* The length of the log queue, i.e. maximum number postponed log messages */
#ifdef TSCH_LOG_CONF_QUEUE_LEN
#define TSCH_LOG_QUEUE_LEN TSCH_LOG_CONF_QUEUE_LEN
#else /* TSCH_LOG_CONF_QUEUE_LEN */
#define TSCH_LOG_QUEUE_LEN 8
#endif /* TSCH_LOG_CONF_QUEUE_LEN */

/* Returns an integer ID from a link-layer address */
#ifdef TSCH_LOG_CONF_ID_FROM_LINKADDR
#define TSCH_LOG_ID_FROM_LINKADDR(addr) TSCH_LOG_CONF_ID_FROM_LINKADDR(addr)
#else /* TSCH_LOG_ID_FROM_LINKADDR */
#if (LINKADDR_SIZE == 8)
#define TSCH_LOG_ID_FROM_LINKADDR(addr) ((addr) ? (addr)->u32[0] : 0)
#else
#define TSCH_LOG_ID_FROM_LINKADDR(addr) ((addr) ? (addr)->u8[LINKADDR_SIZE - 1] : 0)
#endif
#endif /* TSCH_LOG_ID_FROM_LINKADDR */

#if TSCH_LOG_LEVEL < 2 /* For log level 0 or 1, the logging functions do nothing */

#define TSCH_IS_LOG  0
#define TSCH_IS_PRINT ((DEBUG) & DEBUG_PRINT)

#define tsch_log_init()
#define tsch_log_process_pending()  0
#define TSCH_LOG_ADD(log_type, init_code)
#define TSCH_LOG_FRAME(msg, frame, raw)

#define TSCH_LOGS(... )
#define TSCH_LOGF(... )
#define TSCH_LOGF8(... )
#define TSCH_DBG(... )

#define TSCH_PUTS(txt)      PRINTF(txt)
#define TSCH_PRINTF(... )  PRINTF(__VA_ARGS__)
#define TSCH_PRINTF8(... )  PRINTF(__VA_ARGS__)
#define TSCH_ANNOTATE(... )  ANNOTATE(__VA_ARGS__)

#else /* TSCH_LOG_LEVEL */

#define TSCH_IS_LOG         1
#define TSCH_IS_PRINT       1

/************ Types ***********/

/** \brief Structure for a log. Union of different types of logs */
struct tsch_log_t {
  enum { tsch_log_tx,
         tsch_log_rx,
         tsch_log_rx_drift,
         tsch_log_change_timesrc,
         tsch_log_frame,
         tsch_log_packet,       //< post packet[index] info
         tsch_log_packet_verbose,  //< post locked packet[index]->quebuf neibohour info
         tsch_log_text,         //< post static const string
         tsch_log_fmt,          //< post static const string
         tsch_log_fmt8,          //< post static const string
         tsch_log_message
  } type;
  struct tsch_asn_t asn;
  tsch_slot_t   timeslot;
  tsch_sf_h     slotframe_handle;
  uint8_t burst_count;
  uint8_t channel;
  uint8_t channel_offset;
  union {
    char message[48];
    const char* text;
    struct {
        const char* text;
        int         arg[8];
    } fmt;
    struct {
      int mac_tx_status;
      linkaddr_t dest;
      int drift;
      uint8_t num_tx;
      uint8_t datalen;
      uint8_t is_data;
      uint8_t sec_level;
      uint8_t drift_used;
      uint8_t seqno;
      uint8_t sec_key;
    } tx;
    struct {
      linkaddr_t  src;
      int drift;
      int estimated_drift;
      uint8_t datalen;
      uint8_t is_unicast;
      uint8_t is_data;
      uint8_t sec_level;
      uint8_t drift_used;
      uint8_t seqno;
      uint8_t sec_key;
    } rx;
    struct {
        int32_t     drift;
        uint32_t    start_us;
        uint32_t    expect_us;
    } rx_drift;
    struct {
        linkaddr_t  was;
        linkaddr_t  now;
    } timesrc_change;
    struct {
        const char* fmt;
        struct tsch_packet *p;
        int                 index;
        struct tsch_neighbor *n;
        struct queuebuf      *qb;
        int                 locked;
    } packet;
    struct {
        const char*         msg;
        uint8_t             raw[2];
        uint8_t             frame_version;
        uint8_t             frame_type;
        uint16_t            src_pid;
        uint16_t            dst_pid;
        uip_lladdr_t        src_addr;
        uip_lladdr_t        dst_addr;
        char                tmp[16];
    } frame;
  };
};

/********** Functions *********/

/**
 * \brief Prepare addition of a new log.
 * \return A pointer to log structure if success, NULL otherwise
 */
struct tsch_log_t *tsch_log_prepare_add(void);
/**
 * \brief Actually add the previously prepared log
 */
void tsch_log_commit(void);
/**
 * \brief Initialize log module
 */
void tsch_log_init(void);
/* Process pending log messages */
// \return - 0 if no messages printed
int tsch_log_process_pending(void);
/**
 * \brief Stop logging module
 */
void tsch_log_stop(void);

/************ Macros **********/

/** \brief Use this macro to add a log to the queue (will be printed out
 * later, after leaving interrupt context) */
#define TSCH_LOG_ADD(log_type, init_code) do { \
    struct tsch_log_t *log = tsch_log_prepare_add(); \
    if(log != NULL) { \
      log->type = (log_type); \
      init_code; \
      tsch_log_commit(); \
    } \
} while(0);



void tsch_log_puts(const char* txt);
void tsch_log_printf3(const char* fmt, int arg1, int arg2, int arg3);
void tsch_log_printf4(const char* fmt, int arg1, int arg2, int arg3, int arg4);
void tsch_log_printf8(const char* fmt
                      , int arg1, int arg2, int arg3, int arg4
                      , int arg5, int arg6, int arg7, int arg8);

#define _TSCH_LOGF3( fmt, arg1,  arg2, arg3, ...) \
        tsch_log_printf3(fmt, (int)(arg1), (int)(arg2), (int)(arg3))
#define _TSCH_LOGF4( fmt, arg1, arg2, arg3, arg4, ...) \
        tsch_log_printf4(fmt, (int)(arg1), (int)(arg2), (int)(arg3), (int)(arg4))
#define _TSCH_LOGF8( fmt, arg1,  arg2, arg3, arg4, arg5, arg6, arg7, arg8,...) \
        tsch_log_printf8(fmt, (int)(arg1), (int)(arg2), (int)(arg3), (int)(arg4)\
                        , (int)(arg5), (int)(arg6), (int)(arg7), (int)(arg8) )

#define TSCH_LOGS(msg)    tsch_log_puts(msg)
#define TSCH_LOGF(...)    _TSCH_LOGF4(__VA_ARGS__, 0,0,0,0)
#define TSCH_LOGF8( ... ) _TSCH_LOGF8(__VA_ARGS__, 0,0,0,0, 0,0,0,0)

#define TSCH_PUTS(txt)  tsch_log_puts(txt)
#define TSCH_PRINTF( ... ) _TSCH_LOGF4(__VA_ARGS__, 0,0,0,0)
#define TSCH_PRINTF8( ... ) _TSCH_LOGF8(__VA_ARGS__, 0,0,0,0, 0,0,0,0)

#define TSCH_DBG(... ) do { if(LOG_LEVEL_DBG <= (LOG_LEVEL)) TSCH_LOGF( __VA_ARGS__ ); } while(false)

#define TSCH_ANNOTATE( ... ) do { \
    if ((DEBUG) & DEBUG_ANNOTATE)\
        TSCH_LOGF(__VA_ARGS__, 0,0,0,0); \
    } while(false)


#include "net/mac/framer/frame802154.h"
void tsch_log_print_frame(const char* msg, frame802154_t *frame, const void* raw);
#define TSCH_LOG_FRAME(msg, frame, raw)  tsch_log_print_frame(msg, frame, raw)


#endif /* TSCH_LOG_LEVEL */

#endif /* __TSCH_LOG_H__ */
/** @} */
