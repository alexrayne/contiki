/*
 * Copyright (c) 2015, Texas Instruments Incorporated - http://www.ti.com/
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
 * \addtogroup cc26xx
 * @{
 *
 * \defgroup rf-core CC13xx/CC26xx RF core
 *
 * Different flavours of chips of the CC13xx/CC26xx family have different
 * radio capability. For example, the CC2650 can operate in IEEE 802.15.4 mode
 * at 2.4GHz, but it can also operate in BLE mode. The CC1310 only supports
 * sub-ghz mode.
 *
 * However, there are many radio functionalities that are identical across
 * all chips. The rf-core driver provides support for this common functionality
 *
 * @{
 *
 * \file
 * Header file for the CC13xx/CC26xx RF core driver
 */
/*---------------------------------------------------------------------------*/
#ifndef RF_CORE_H_
#define RF_CORE_H_
/*---------------------------------------------------------------------------*/
#include "contiki-conf.h"
#include "driverlib/rf_common_cmd.h"

#include <stdint.h>
#include <stdbool.h>
/*---------------------------------------------------------------------------*/
#define RF_CORE_FRONT_END_MODE_DIFFERENTIAL   0
#define RF_CORE_FRONT_END_MODE_SINGLE_RFP     1
#define RF_CORE_FRONT_END_MODE_SINGLE_RFN     2

#define RF_CORE_BIAS_MODE_INTERNAL            0
#define RF_CORE_BIAS_MODE_EXTERNAL            1
/*---------------------------------------------------------------------------*/
/*
 * RF Front-End Mode and Bias for CMD_RADIO_SETUP (IEEE and BLE)
 * Default: Differential mode, internal bias
 */
#ifdef RF_CORE_CONF_RADIO_SETUP_FRONT_END_MODE
#define RF_CORE_RADIO_SETUP_FRONT_END_MODE RF_CORE_CONF_RADIO_SETUP_FRONT_END_MODE
#else
#define RF_CORE_RADIO_SETUP_FRONT_END_MODE RF_CORE_FRONT_END_MODE_DIFFERENTIAL
#endif

#ifdef RF_CORE_CONF_RADIO_SETUP_BIAS_MODE
#define RF_CORE_RADIO_SETUP_BIAS_MODE RF_CORE_CONF_RADIO_SETUP_BIAS_MODE
#else
#define RF_CORE_RADIO_SETUP_BIAS_MODE RF_CORE_BIAS_MODE_INTERNAL
#endif
/*---------------------------------------------------------------------------*/
/*
 * RF Front-End Mode and Bias for CMD_PROP_DIV_RADIO_SETUP (PROP mode)
 * Default: Differential mode, external bias
 */
#ifdef RF_CORE_CONF_PROP_FRONT_END_MODE
#define RF_CORE_PROP_FRONT_END_MODE RF_CORE_CONF_PROP_FRONT_END_MODE
#else
#define RF_CORE_PROP_FRONT_END_MODE RF_CORE_FRONT_END_MODE_DIFFERENTIAL
#endif

#ifdef RF_CORE_CONF_PROP_BIAS_MODE
#define RF_CORE_PROP_BIAS_MODE RF_CORE_CONF_PROP_BIAS_MODE
#else
#define RF_CORE_PROP_BIAS_MODE RF_CORE_BIAS_MODE_EXTERNAL
#endif
/*---------------------------------------------------------------------------*/
/* RF-Front End RAT resyncing strategy provides mechanisms for RAT sync monitoring
 *  and resyncing
 *  0 :    resync only on rf-core propety setup
 *  1 :    validate RF-timestamp in window of current operation, and resync if violate
 * */
#ifdef RF_CORE_CONF_HFOSC_STARTUP_TOUS
#define RF_CORE_HFOSC_STARTUP_TOUS    RF_CORE_CONF_HFOSC_STARTUP_TOUS
#else
//#define RF_CORE_HFOSC_STARTUP_TOUS    1000
#endif
/*---------------------------------------------------------------------------*/
/*
 * RF Front-End IRQ polling mode setup
 * Not Defined : polling mode enabled by driver.set_value(RADIO_PARAM_RX_MODE)
 * 0 :           only ISR mode use.
 * 1 :           only Poling mode use.
 */
#ifdef RF_CORE_CONF_POLL_MODE
#define RF_CORE_POLL_MODE RF_CORE_CONF_POLL_MODE
// TSCH stack now useonly POLL_MODE
#elif MAC_CONF_WITH_TSCH
#define RF_CORE_POLL_MODE 1
#endif
/*---------------------------------------------------------------------------*/
/**
 * RF Front-End IRQ handling with app handlers
 * @{
 * */

/**< a type of application handler used for RF operation IRQ invoke */
typedef void (*rfc_irq_handle)(uint32_t status);

/**< turn on app handlers support */
#ifdef RF_CORE_CONF_APP_HANDLING
#define RF_CORE_APP_HANDLING    RF_CORE_CONF_APP_HANDLING
#else
#define RF_CORE_APP_HANDLING    1
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/*
 * RF Front-End pending() api style
 * 0 :  old behaviour - pending() return count of received packets in que.
 * 1 :  pending() return count of packets ready for read.
 * 2 :  read_frame(..) - scan packets que for any finished packet. This helps when
 *          for some reason RFcore skip some slot, leave it in PENDING state.
 */
#define RF_CORE_PENDING_RECEIVED    0
#define RF_CORE_PENDING_READS       1
#define RF_CORE_PENDING_READSANY    2
#ifdef RF_CORE_CONF_PENDING
#define RF_CORE_PENDING RF_CORE_CONF_PENDING
#else
#define RF_CORE_PENDING     RF_CORE_PENDING_RECEIVED
#endif

/*---------------------------------------------------------------------------*/
/* RF-Front End receining() detection style:
 *  0 :    receive meets by chanel_clear
 *  1 :    receive starts by sync words detected, and finished when pending new packet.
 *         This is more robust, vs chanel_clear, BUT:
 *         !!! Under CSMA operation, there is no immediately straightforward logic as to
 *              when it's OK to clear the MDMSOFT interrupt flag:
 *   - We cannot re-use the same logic as above, since CSMA may bail out of
 *     frame TX immediately after a single call this function here. In this
 *     scenario, is_receiving_packet would remain equal to one and we would
 *     therefore erroneously signal ongoing RX in subsequent calls to this
 *     function here, even _after_ reception has completed.
 *   - We can neither clear inside read_frame() nor inside the RX frame
 *     interrupt handler (remember, we are not in poll mode under CSMA),
 *     since we risk clearing MDMSOFT after we have seen a sync word for the
 *     _next_ frame. If this happens, this function here would incorrectly
 *     return 0 during RX of this next frame.
 *  -  prefer use it with TSCH
 * */
#define RF_CORE_RECV_BY_CLEAR      0
#define RF_CORE_RECV_BY_SYNC       1
#ifdef RF_CORE_CONF_RECV_STYLE
#define RF_CORE_RECV_STYLE    RF_CORE_CONF_RECV_STYLE
#else
#if MAC_CONF_WITH_TSCH
#define RF_CORE_RECV_STYLE    RF_CORE_RECV_BY_SYNC
#else
#define RF_CORE_RECV_STYLE    RF_CORE_RECV_BY_CLEAR
#endif
#endif

/*---------------------------------------------------------------------------*/
/* RF-Front End RAT resyncing strategy provides mechanisms for RAT sync monitoring
 *  and resyncing
 *  0 :    resync only on rf-core propety setup
 *  1 :    validate RF-timestamp in window of current operation, and resync if violate
 * */
#define RF_CORE_RAT_SYNC_PROP      0
#define RF_CORE_RAT_SYNC_AGRESSIVE 1
#ifdef RF_CORE_CONF_RAT_SYNC_STYLE
#define PROP_MODE_RAT_SYNC_STYLE    RF_CORE_CONF_RAT_SYNC_STYLE
#else
#define PROP_MODE_RAT_SYNC_STYLE    RF_CORE_RAT_SYNC_PROP
#endif
/*---------------------------------------------------------------------------*/
#define RF_CORE_CMD_ERROR                     0
#define RF_CORE_CMD_OK                        1
/*---------------------------------------------------------------------------*/
/**
 * \brief A data strcuture representing the radio's primary mode of operation
 *
 * The CC13xx / CC26xx radio supports up to potentially 3 modes: IEEE, Prop and
 * BLE. Within Contiki, we assume that the radio is by default in one of IEEE
 * or Prop in order to support standard 6LoWPAN / .15.4 operation. The BLE
 * mode interrupts this so called "primary" mode in order to send BLE adv
 * messages. Once BLE is done advertising, we need to be able to restore the
 * previous .15.4 mode. Unfortunately, the only way this can be done with
 * NETSTACK_RADIO API is by fully power-cycling the radio, which is something
 * we do not want to do.
 *
 * Thus, we declare a secondary data structure for primary mode drivers (IEEE
 * or Prop). We use this data structure to issue "soft off" and "back on"
 * commands. Soft off in this context means stopping RX (e.g. the respective
 * IEEE RX operation), but without shutting down the RF core (which is what
 * NETSTACK_RADIO.off() would have done). We then remember what mode we were
 * using in order to be able to re-enter RX mode for this mode.
 *
 * A NETSTACK_RADIO driver will declare those two functions somewhere within
 * its module of implementation. During its init() routine, it will notify
 * the RF core module so that the latter can abort and restore operations.
 */
typedef struct rf_core_primary_mode_s {
  /**
   * \brief A pointer to a function used to abort the current radio op
   */
  void (*abort)(void);

  /**
   * \brief A pointer to a function that will restore the previous radio op
   * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
   */
  uint8_t (*restore)(void);

  /**
   * \brief A pointer to a function that checks if the radio is on
   * \return 1 or 0
   */
  uint8_t (*is_on)(void);

  /**
   * \brief Offset of the end of SFD when compared to the radio HW-generated timestamp
   */
   int16_t sfd_timestamp_offset;
} rf_core_primary_mode_t;
/*---------------------------------------------------------------------------*/
/* RF Command status constants - Correspond to values in the CMDSTA register */
#define RF_CORE_CMDSTA_PENDING         0x00
#define RF_CORE_CMDSTA_DONE            0x01
#define RF_CORE_CMDSTA_ILLEGAL_PTR     0x81
#define RF_CORE_CMDSTA_UNKNOWN_CMD     0x82
#define RF_CORE_CMDSTA_UNKNOWN_DIR_CMD 0x83
#define RF_CORE_CMDSTA_CONTEXT_ERR     0x85
#define RF_CORE_CMDSTA_SCHEDULING_ERR  0x86
#define RF_CORE_CMDSTA_PAR_ERR         0x87
#define RF_CORE_CMDSTA_QUEUE_ERR       0x88
#define RF_CORE_CMDSTA_QUEUE_BUSY      0x89

/* Status values starting with 0x8 correspond to errors */
#define RF_CORE_CMDSTA_ERR_MASK        0x80

/* CMDSTA is 32-bits. Return value in bits 7:0 */
#define RF_CORE_CMDSTA_RESULT_MASK     0xFF

#define RF_CORE_RADIO_OP_STATUS_IDLE   0x0000
/*---------------------------------------------------------------------------*/
#define RF_CORE_NOT_ACCESSIBLE 0x00
#define RF_CORE_ACCESSIBLE     0x01
/*---------------------------------------------------------------------------*/
/* RF Radio Op status constants. Field 'status' in Radio Op command struct */
#define RF_CORE_RADIO_OP_STATUS_IDLE                     0x0000
#define RF_CORE_RADIO_OP_STATUS_PENDING                  0x0001
#define RF_CORE_RADIO_OP_STATUS_ACTIVE                   0x0002
#define RF_CORE_RADIO_OP_STATUS_SKIPPED                  0x0003
#define RF_CORE_RADIO_OP_STATUS_DONE_OK                  0x0400
#define RF_CORE_RADIO_OP_STATUS_DONE_COUNTDOWN           0x0401
#define RF_CORE_RADIO_OP_STATUS_DONE_RXERR               0x0402
#define RF_CORE_RADIO_OP_STATUS_DONE_TIMEOUT             0x0403
#define RF_CORE_RADIO_OP_STATUS_DONE_STOPPED             0x0404
#define RF_CORE_RADIO_OP_STATUS_DONE_ABORT               0x0405
#define RF_CORE_RADIO_OP_STATUS_ERROR_PAST_START         0x0800
#define RF_CORE_RADIO_OP_STATUS_ERROR_START_TRIG         0x0801
#define RF_CORE_RADIO_OP_STATUS_ERROR_CONDITION          0x0802
#define RF_CORE_RADIO_OP_STATUS_ERROR_PAR                0x0803
#define RF_CORE_RADIO_OP_STATUS_ERROR_POINTER            0x0804
#define RF_CORE_RADIO_OP_STATUS_ERROR_CMDID              0x0805
#define RF_CORE_RADIO_OP_STATUS_ERROR_NO_SETUP           0x0807
#define RF_CORE_RADIO_OP_STATUS_ERROR_NO_FS              0x0808
#define RF_CORE_RADIO_OP_STATUS_ERROR_SYNTH_PROG         0x0809

/* Additional Op status values for IEEE mode */
#define RF_CORE_RADIO_OP_STATUS_IEEE_SUSPENDED           0x2001
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_OK             0x2400
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_BUSY           0x2401
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_STOPPED        0x2402
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_ACK            0x2403
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_ACKPEND        0x2404
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_TIMEOUT        0x2405
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_BGEND          0x2406
#define RF_CORE_RADIO_OP_STATUS_IEEE_DONE_ABORT          0x2407
#define RF_CORE_RADIO_OP_STATUS_ERROR_WRONG_BG           0x0806
#define RF_CORE_RADIO_OP_STATUS_IEEE_ERROR_PAR           0x2800
#define RF_CORE_RADIO_OP_STATUS_IEEE_ERROR_NO_SETUP      0x2801
#define RF_CORE_RADIO_OP_STATUS_IEEE_ERROR_NO_FS         0x2802
#define RF_CORE_RADIO_OP_STATUS_IEEE_ERROR_SYNTH_PROG    0x2803
#define RF_CORE_RADIO_OP_STATUS_IEEE_ERROR_RXOVF         0x2804
#define RF_CORE_RADIO_OP_STATUS_IEEE_ERROR_TXUNF         0x2805

/* Op status values for BLE mode */
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_OK              0x1400
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_RXTIMEOUT       0x1401
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_NOSYNC          0x1402
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_RXERR           0x1403
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_CONNECT         0x1404
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_MAXNACK         0x1405
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_ENDED           0x1406
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_ABORT           0x1407
#define RF_CORE_RADIO_OP_STATUS_BLE_DONE_STOPPED         0x1408
#define RF_CORE_RADIO_OP_STATUS_BLE_ERROR_PAR            0x1800
#define RF_CORE_RADIO_OP_STATUS_BLE_ERROR_RXBUF          0x1801
#define RF_CORE_RADIO_OP_STATUS_BLE_ERROR_NO_SETUP       0x1802
#define RF_CORE_RADIO_OP_STATUS_BLE_ERROR_NO_FS          0x1803
#define RF_CORE_RADIO_OP_STATUS_BLE_ERROR_SYNTH_PROG     0x1804
#define RF_CORE_RADIO_OP_STATUS_BLE_ERROR_RXOVF          0x1805
#define RF_CORE_RADIO_OP_STATUS_BLE_ERROR_TXUNF          0x1806

/* Op status values for proprietary mode */
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_OK             0x3400
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_RXTIMEOUT      0x3401
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_BREAK          0x3402
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_ENDED          0x3403
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_STOPPED        0x3404
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_ABORT          0x3405
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_RXERR          0x3406
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_IDLE           0x3407
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_BUSY           0x3408
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_IDLETIMEOUT    0x3409
#define RF_CORE_RADIO_OP_STATUS_PROP_DONE_BUSYTIMEOUT    0x340A
#define RF_CORE_RADIO_OP_STATUS_PROP_ERROR_PAR           0x3800
#define RF_CORE_RADIO_OP_STATUS_PROP_ERROR_RXBUF         0x3801
#define RF_CORE_RADIO_OP_STATUS_PROP_ERROR_RXFULL        0x3802
#define RF_CORE_RADIO_OP_STATUS_PROP_ERROR_NO_SETUP      0x3803
#define RF_CORE_RADIO_OP_STATUS_PROP_ERROR_NO_FS         0x3804
#define RF_CORE_RADIO_OP_STATUS_PROP_ERROR_RXOVF         0x3805
#define RF_CORE_RADIO_OP_STATUS_PROP_ERROR_TXUNF         0x3806

/* Bits 15:12 signify the protocol */
#define RF_CORE_RADIO_OP_STATUS_PROTO_MASK               0xF000
#define RF_CORE_RADIO_OP_STATUS_PROTO_GENERIC            0x0000
#define RF_CORE_RADIO_OP_STATUS_PROTO_BLE                0x1000
#define RF_CORE_RADIO_OP_STATUS_PROTO_IEEE               0x2000
#define RF_CORE_RADIO_OP_STATUS_PROTO_PROP               0x3000

/* Bits 11:10 signify Running / Done OK / Done with error */
#define RF_CORE_RADIO_OP_MASKED_STATUS                   0x0C00
#define RF_CORE_RADIO_OP_MASKED_STATUS_RUNNING           0x0000
#define RF_CORE_RADIO_OP_MASKED_STATUS_DONE              0x0400
#define RF_CORE_RADIO_OP_MASKED_STATUS_ERROR             0x0800
/*---------------------------------------------------------------------------*/
/* Command Types */
#define RF_CORE_COMMAND_TYPE_MASK                        0x0C00
#define RF_CORE_COMMAND_TYPE_RADIO_OP                    0x0800
#define RF_CORE_COMMAND_TYPE_IEEE_BG_RADIO_OP            0x0800
#define RF_CORE_COMMAND_TYPE_IEEE_FG_RADIO_OP            0x0C00

#define RF_CORE_COMMAND_PROTOCOL_MASK                    0x3000
#define RF_CORE_COMMAND_PROTOCOL_COMMON                  0x0000
#define RF_CORE_COMMAND_PROTOCOL_BLE                     0x1000
#define RF_CORE_COMMAND_PROTOCOL_IEEE                    0x2000
#define RF_CORE_COMMAND_PROTOCOL_PROP                    0x3000
/*---------------------------------------------------------------------------*/
/* Radio timer register */
#define RATCNT  0x00000004
/*---------------------------------------------------------------------------*/
/* Special value returned by CMD_IEEE_CCA_REQ when an RSSI is not available */
#define RF_CORE_CMD_CCA_REQ_RSSI_UNKNOWN     -128

/* Used for the return value of channel_clear */
#define RF_CORE_CCA_CLEAR                       1
#define RF_CORE_CCA_BUSY                        0

/* Used as an error return value for get_cca_info */
#define RF_CORE_GET_CCA_INFO_ERROR           0xFF

/*
 * Values of the individual bits of the ccaInfo field in CMD_IEEE_CCA_REQ's
 * status struct
 */
#define RF_CORE_CMD_CCA_REQ_CCA_STATE_IDLE      0 /* 00 */
#define RF_CORE_CMD_CCA_REQ_CCA_STATE_BUSY      1 /* 01 */
#define RF_CORE_CMD_CCA_REQ_CCA_STATE_INVALID   2 /* 10 */

#define RF_CORE_CMD_CCA_REQ_CCA_CORR_IDLE       (0 << 4)
#define RF_CORE_CMD_CCA_REQ_CCA_CORR_BUSY       (1 << 4)
#define RF_CORE_CMD_CCA_REQ_CCA_CORR_INVALID    (3 << 4)
#define RF_CORE_CMD_CCA_REQ_CCA_CORR_MASK       (3 << 4)

#define RF_CORE_CMD_CCA_REQ_CCA_SYNC_BUSY       (1 << 6)
/*---------------------------------------------------------------------------*/
#define RF_CORE_RX_BUF_INCLUDE_CRC 0
#define RF_CORE_RX_BUF_INCLUDE_RSSI 1
#define RF_CORE_RX_BUF_INCLUDE_CORR 1
#define RF_CORE_RX_BUF_INCLUDE_TIMESTAMP 1
/*---------------------------------------------------------------------------*/
/* How long to wait for an ongoing ACK TX to finish before starting frame TX */
#define RF_CORE_TX_TIMEOUT       (RTIMER_SECOND >> 11)

/* How long to wait for the RF to enter RX in rf_cmd_ieee_rx */
#define RF_CORE_ENTER_RX_TIMEOUT (RTIMER_SECOND >> 10)

/* How long to wait for the RF to react on CMD_ABORT: around 1 msec */
#define RF_CORE_TURN_OFF_TIMEOUT (RTIMER_SECOND >> 10)

/* How long to wait for the RF to finish TX of a packet or an ACK */
#define RF_CORE_TX_FINISH_TIMEOUT (RTIMER_SECOND >> 7)

/*---------------------------------------------------------------------------*/
/* Make the main driver process visible to mode drivers */
PROCESS_NAME(rf_core_process);
/*---------------------------------------------------------------------------*/
/* Buffer full flag */
extern volatile bool rf_core_rx_is_full;
/*---------------------------------------------------------------------------*/
/* RSSI of the last read frame */
extern volatile int8_t rf_core_last_rssi;
/* Correlation/LQI of the last read frame */
extern volatile uint8_t rf_core_last_corr_lqi;
/* SFD timestamp of the last read frame, in rtimer ticks */
extern volatile uint32_t rf_core_last_packet_timestamp;
/*---------------------------------------------------------------------------*/
/* Are we currently in poll mode? */
#ifndef RF_CORE_POLL_MODE
extern uint8_t rf_core_poll_mode;
#else
#define rf_core_poll_mode RF_CORE_POLL_MODE
#endif
/*---------------------------------------------------------------------------*/
/**
 * \brief Check whether the RF core is accessible
 * \retval RF_CORE_ACCESSIBLE The core is powered and ready for access
 * \retval RF_CORE_NOT_ACCESSIBLE The core is not ready
 *
 * If this function returns RF_CORE_NOT_ACCESSIBLE, rf_core_power_up() must be
 * called before any attempt to access the core.
 */
uint8_t rf_core_is_accessible(void);


/**
 * \brief Get the status of the last issued radio command
 * @return status of a last cmd started by rf_core_start_cmd(...) or
 *      @sa RF_CORE_CMDSTA_xxx codes
 */
static inline
uint32_t rf_core_cmd_status(void){
    extern uint32_t rf_core_last_cmd_status;
    return rf_core_last_cmd_status;
}

/**
 * \brief Sends a command to the RF core.
 *
 * \param cmd The command value or a pointer to a command buffer
 * \param status A pointer to a variable which will hold the status
 * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
 *
 * This function supports all three types of command (Radio OP, immediate and
 * direct)
 *
 * For immediate and Radio OPs, cmd is a pointer to the data structure
 * containing the command and its parameters. This data structure must be
 * 4-byte aligned.
 *
 * For direct commands, cmd contains the value of the command alongside its
 * parameters. This value will be written to CMDSTA verbatim, so the command
 * ID must be in the 16 high bits, and the 2 LS bits must be set to 01 by the
 * caller.
 *
 * The caller is responsible of allocating and populating cmd for Radio OP and
 * immediate commands
 *
 * The caller is responsible for allocating status
 *
 * For immediate commands and radio Ops, this function will set the command's
 * status field to RF_CORE_RADIO_OP_STATUS_IDLE before sending it to the RF
 */
uint_fast8_t rf_core_start_cmd(uint32_t cmd);

static inline
uint_fast8_t rf_core_send_cmd(uint32_t cmd, uint32_t *status){
    uint_fast8_t ok = rf_core_start_cmd(cmd);
    *status = rf_core_cmd_status();
    return ok;
}

/**
 * \brief Block and wait for a Radio op to complete
 * \param cmd A pointer to any command's structure
 * \retval RF_CORE_CMD_OK the command completed with status _DONE_OK
 * \retval RF_CORE_CMD_ERROR Timeout exceeded or the command completed with
 *         status _DONE_xxx (e.g. RF_CORE_RADIO_OP_STATUS_DONE_TIMEOUT)
 */
uint_fast8_t rf_core_wait_cmd_done(void *cmd);

/**
 * \brief Turn on power to the RFC and boot it.
 * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
 */
int rf_core_power_up(void);

/**
 * \brief Disable RFCORE clock domain in the MCU VD and turn off the RFCORE PD
 */
void rf_core_power_down(void);

/**
 * \brief Initialise RF APIs in the RF core
 * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
 *
 * Depending on chip family and capability, this function will set the correct
 * value to PRCM.RFCMODESEL
 */
uint8_t rf_core_set_modesel(void);

/**
 * \brief Start the CM0 RAT
 * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
 *
 * This function must be called each time the CM0 boots. The boot sequence
 * can be performed automatically by calling rf_core_boot() if patches are not
 * required. If patches are required then the patches must be applied after
 * power up and before calling this function.
 */
uint8_t rf_core_start_rat(void);

/**
 * \brief Stop the CM0 RAT synchronously
 * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
 *
 * This function is not strictly necessary, but through calling it it's possibly
 * to learn the RAT / RTC offset, which useful to get accurate radio timestamps.
 */
uint8_t rf_core_stop_rat(void);

/**
 * \brief Restart the CM0 RAT
 * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
 *
 * This function restarts the CM0 RAT and therefore resynchornizes it with RTC.
 * To achieve good timing accuracy, it should be called periodically.
 */
uint8_t rf_core_restart_rat(void);

/**
 * \brief Boot the RF Core
 * \return RF_CORE_CMD_OK or RF_CORE_CMD_ERROR
 *
 * This function will perform the CM0 boot sequence. It will first power it up
 * and then start the RAT. If a patch is required, then the mode driver must
 * not call this function and perform the sequence manually, applying patches
 * after boot and before calling rf_core_start_rat().
 *
 * The function will return RF_CORE_CMD_ERROR if any of those steps fails. If
 * the boot sequence fails to complete, the RF Core will be powered down.
 */
uint8_t rf_core_boot(void);

/**
 * \brief Setup RF core interrupts
 */
void rf_core_setup_interrupts(void);

/**
 * \brief Enable interrupt on command done.
 * \param fg set true to enable irq on foreground command done and false for
 * background commands or if not in ieee mode.
 * \param poll_mode true if the driver is in poll mode
 *
 * This is used within TX routines in order to be able to sleep the CM3 and
 * wake up after TX has finished
 *
 * \sa rf_core_cmd_done_dis()
 */
void rf_core_cmd_done_en(bool fg);

/**
 * \brief Disable the LAST_CMD_DONE and LAST_FG_CMD_DONE interrupts.
 *
 * This is used within TX routines after TX has completed
 *
 * \sa rf_core_cmd_done_en()
 */
void rf_core_cmd_done_dis(void);

/**
 * \brief Returns a pointer to the most recent proto-dependent Radio Op
 * \return The pointer
 *
 * The RF Core driver will remember the most recent proto-dependent Radio OP
 * issued, so that other modules can inspect its type and state at a subsequent
 * stage. The assumption is that those commands will be issued by a function
 * that will then return. The following commands will be "remembered"
 *
 * - All BLE Radio Ops (0x18nn)
 * - All Prop Radio Ops (0x38nn)
 * - IEEE BG Radio Ops (0x28nn)
 *
 * The following commands are assumed to be executed synchronously and will
 * thus not be remembered by the core and not returned by this function:
 *
 * - Direct commands
 * - Proto-independent commands (including Radio Ops and Immediate ones)
 * - IEEE FG Radio Ops (0x2Cxx)
 *
 * This assumes that all commands will be sent to the radio using
 * rf_core_send_cmd()
 */
static inline
rfc_radioOp_t *rf_core_get_last_radio_op(void){
    extern rfc_radioOp_t* rf_core_last_radio_op;
    return rf_core_last_radio_op;
}

/**
 * \brief Prepare a buffer to host a Radio Op
 * \param buf A pointer to the buffer that will host the Radio Op
 * \param len The buffer's length
 * \param command The command ID
 *
 * The caller is responsible to allocate the buffer
 *
 * This function will not check whether the buffer is large enough to hold the
 * command. This is the caller's responsibility
 *
 * This function will wipe out the buffer's contents.
 */
void rf_core_init_radio_op(rfc_radioOp_t *buf, uint16_t len, uint16_t command);

/**
 * \brief Register a primary mode for radio operation
 * \param mode A pointer to the struct representing the mode
 *
 * A normal NESTACK_RADIO driver will normally register itself by calling
 * this function during its own init().
 *
 * \sa rf_core_primary_mode_t
 */
void rf_core_primary_mode_register(const rf_core_primary_mode_t *mode);

/**
 * \brief Abort the currently running primary radio op
 */
void rf_core_primary_mode_abort(void);

/**
 * \brief Abort the currently running primary radio op
 */
uint8_t rf_core_primary_mode_restore(void);

/**
 * \brief Abort the currently running primary radio op
 */
uint8_t rf_core_primary_mode_is_on(void);

/**
 * \brief Initialize the RAT to RTC conversion machinery
 */
uint8_t rf_core_rat_init(void);

/**
 * \brief Check if RAT overflow has occured and increment the overflow counter if so
 */
uint8_t rf_core_check_rat_overflow(void);

/**
 * \brief Convert from RAT timestamp to rtimer ticks
 */
uint32_t rf_core_convert_rat_to_rtimer(uint32_t rat_timestamp);

#if RF_CORE_APP_HANDLING
/**
 * \brief install AppHandle for new. RF operation, and arm it's IRQ.
 *        this handle invokes single time, and after exec curretn poll resotres.
 * \param op != NULL provides app handler for next IRQ invokation, and enables
 *              IRQ for IRQ_RX_ENTRY_DONE, errors and IRQ_LAST_COMMAND_DONE
 *      - op == NULL - disables ap handler, and restore current poll mode
 */
void rf_core_arm_app_handle( rfc_irq_handle op, uint32_t enabled_irqs);
#endif


//==============================================================================
/* RF core and RF HAL API */
#include "ti-lib.h"
#include "inc/hw_rfc_dbell.h"
#include "inc/hw_rfc_pwr.h"

/*
 *  Poll the RFACKIFG and clear the flag afterwards. This is used during the power up sequence
 *  of the RF core where interlaying processing is implemented.
 *
 *  Input:  none
 *  Return: none
 */
static inline
void rfc_dbell_sync_on_ack(void)
{
    while (!HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG));
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;
}

/*
 *  Submit a command to the doorbell without blocking command execution. This is used during the
 *  power up sequence where the system CPU can continue with processing data while the RF core
 *  executes the submitted command.
 *
 *  Input:  rawCmd - The raw command to be written to the doorbell. This can be a pointer or a
 *                   a direct/immediate command.
 *  Return: none
 */
static inline
void rfc_dbell_submit_cmd_async(uint32_t rawCmd)
{
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_CMDR)     = rawCmd;
}

/*---------------------------------------------------------------------------*/
#endif /* RF_CORE_H_ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
