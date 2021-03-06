/*
 * Copyright (c) 2015, SICS Swedish ICT.
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
 *         TSCH constants
 * \author
 *         Simon Duquennoy <simonduq@sics.se>
 */

#ifndef __TSCH_CONST_H__
#define __TSCH_CONST_H__

/********** Includes **********/
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/tsch/tsch-conf.h" /* Required for timestlot timing consts */

/********** Constants **********/

/* Link options */
#define LINK_OPTION_TX              1
#define LINK_OPTION_RX              2
#define LINK_OPTION_SHARED          4
#define LINK_OPTION_TIME_KEEPING    8
//< scheduler skip this slot
#define LINK_OPTION_DISABLE         0x10
//< scheduler, when TXslot have no any data to send, turn on option flag LINK_OPTION_DISABLE
//  this intends to help reduce slot activity when no data sends
#define LINK_OPTION_IDLED_AUTOOFF   0x20
//< this feature escapes to activate timesource EB slot to latest possible time, enough
//      to keep net in timesync
//  scheduler plan slot with this flag in future to:
//      N*FrameLen+SlotTime < (TSCH_DESYNC_THRESHOLD_SLOTS - Tryes*FrameLen)
//  where:
//      TSCH_DESYNC_THRESHOLD_SLOTS - timeout for timesource EB
//      Tryes - expected amount of packets loose \sa TSCH_CONF_TIMESYNC_EB_LOOSES
//  this flag works when active TSCH_SCHEDULE_OMMIT_NOXFER
#define LINK_OPTION_TIME_EB_ESCAPE  0x40
//< scheduler forced to stops on this slot. this is like phantom LINK_OPTION_TX
//      but actualy not affects transmit.
//      it helpful for TSCH_SCHEDULE_OMMIT_NOXFER policy style, to stop and replan
//      at desired slot, even if nothing transfers
#define LINK_OPTION_PLANPOINT       0x80

//> signals TSCH_CALLBACK_LINK_SIGNAL on slot time
//   this signal is not overlaps by more prioritysed links
#define LINK_OPTION_SIGNAL           0x100
//> same as _SIGNAL, but automatic clears after signaling
//  but this option clears only on active link
//  this use by scheduler as temporary flag, for selected active link
#define LINK_OPTION_SIGNAL_ONCE      0x200




/* Default IEEE 802.15.4e hopping sequences, obtained from https://gist.github.com/twatteyne/2e22ee3c1a802b685695 */
/* 16 channels, sequence length 16 */
#define TSCH_HOPPING_SEQUENCE_16_16 (uint8_t[]){ 16, 17, 23, 18, 26, 15, 25, 22, 19, 11, 12, 13, 24, 14, 20, 21 }
/* 4 channels, sequence length 16 */
#define TSCH_HOPPING_SEQUENCE_4_16 (uint8_t[]){ 20, 26, 25, 26, 15, 15, 25, 20, 26, 15, 26, 25, 20, 15, 20, 25 }
/* 4 channels, sequence length 4 */
#define TSCH_HOPPING_SEQUENCE_4_4 (uint8_t[]){ 15, 25, 26, 20 }
/* 2 channels, sequence length 2 */
#define TSCH_HOPPING_SEQUENCE_2_2 (uint8_t[]){ 20, 25 }
/* 1 channel, sequence length 1 */
#define TSCH_HOPPING_SEQUENCE_1_1 (uint8_t[]){ 20 }

/* Max TSCH packet length equal to the length of the packet buffer */
#define TSCH_PACKET_MAX_LEN PACKETBUF_SIZE
//#define TSCH_PACKET_MAX_LEN MIN(127,PACKETBUF_SIZE)

/* The jitter to remove in ticks.
 * This should be the sum of measurement errors on Tx and Rx nodes.
 * */
#define TSCH_TIMESYNC_MEASUREMENT_ERROR US_TO_RTIMERTICKS(32)

//  this value allows faster learn times for statistics accumulation on new timesource
#define TSCH_DRIFT_SYNC_ESTIMATE_FASTER_INIT   2

/* The approximate number of slots per second */
#define TSCH_SLOTS_PER_SECOND (1000000 / tsch_timing_us[tsch_ts_timeslot_length])

/* Calculate packet tx/rx duration in rtimer ticks based on packet length in bytes. */
#define TSCH_PACKET_DURATION(len) US_TO_RTIMERTICKS(RADIO_BYTE_AIR_TIME * ((len) + RADIO_PHY_OVERHEAD))

/* Convert rtimer ticks to clock and vice versa */
#define TSCH_CLOCK_TO_TICKS(c) (((c) * RTIMER_SECOND) / CLOCK_SECOND)
#define TSCH_CLOCK_TO_SLOTS(c, timeslot_length) ((TSCH_CLOCK_TO_TICKS(c) + timeslot_length - 1) / timeslot_length)

#endif /* __TSCH_CONST_H__ */
/** @} */
