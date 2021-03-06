/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
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
 */

/**
 * \file
 *         Slip configuration
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <err.h>
#include "contiki.h"

#include "slip-config.h"

#ifndef BAUDRATE
#define BAUDRATE B115200
#endif

struct SlipConfig slip_config = {
        0, //verbose
        0, //flowcontrol
        0, //timestamp
        NULL, //ipaddr;
        NULL, //siodev = NULL;
        NULL, //host = NULL;
        NULL, //port = NULL;
        { "" }, //tundev
        0, //basedelay
        BAUDRATE, //b_rate
};

/*---------------------------------------------------------------------------*/
int
slip_config_handle_arguments(int argc, char **argv)
{
  const char *prog;
  int c;
  int baudrate = 115200;

  slip_config.verbose = 0;

  prog = argv[0];
  while((c = getopt(argc, argv, "B:H::D:Lhs:t:v::d::a:p:T")) != -1) {
    switch(c) {
    case 'B':
      baudrate = atoi(optarg);
      break;

    case 'H':
      slip_config.flowcontrol = 1;
      if(optarg) {
          slip_config.flowcontrol = atoi(optarg);
      }
      break;

    case 'L':
      slip_config.timestamp = 1;
      break;

    case 's':
      if(strncmp("/dev/", optarg, 5) == 0) {
        slip_config.siodev = optarg + 5;
      } else {
        slip_config.siodev = optarg;
      }
      break;

    case 't':
      if(strncmp("/dev/", optarg, 5) == 0) {
        strncpy(slip_config.tundev, optarg + 5, sizeof(slip_config.tundev));
      } else {
        strncpy(slip_config.tundev, optarg, sizeof(slip_config.tundev));
      }
      break;

    case 'a':
      slip_config.host = optarg;
      break;

    case 'p':
      slip_config.port = optarg;
      break;

    case 'd':
      slip_config.basedelay = 10;
      if(optarg) {
        slip_config.basedelay = atoi(optarg);
      }
      break;

    case 'v':
      slip_config.verbose = 2;
      if(optarg) {
        slip_config.verbose = atoi(optarg);
      }
      break;

    case '?':
    case 'h':
    default:
      fprintf(stderr, "usage:  %s [options] [ipaddress]\n", prog);
      fprintf(stderr, "example: border-router.native -L -v2 -s ttyUSB1 fd00::1/64\n");
      fprintf(stderr, "Options are:\n");
#ifdef linux
      fprintf(stderr, " -B baudrate    9600,19200,38400,57600,115200,921600 (default 115200)\n");
#else
      fprintf(stderr, " -B baudrate    9600,19200,38400,57600,115200 (default 115200)\n");
#endif
      fprintf(stderr, " -H[style]      Hardware CTS/RTS flow control (default disabled)\n");
      fprintf(stderr, " -L             Log output format (adds time stamps)\n");
      fprintf(stderr, " -s siodev      Serial device (default /dev/ttyUSB0)\n");
      fprintf(stderr, " -a host        Connect via TCP to server at <host>\n");
      fprintf(stderr, " -p port        Connect via TCP to server at <host>:<port>\n");
      fprintf(stderr, " -t tundev      Name of interface (default tun0)\n");
#ifdef __APPLE__
      fprintf(stderr, " -v level       Verbosity level\n");
#else
      fprintf(stderr, " -v[level]      Verbosity level\n");
#endif
      fprintf(stderr, "    -v0         No messages\n");
      fprintf(stderr, "    -v1         Encapsulated SLIP debug messages (default)\n");
      fprintf(stderr, "    -v2         Printable strings after they are received\n");
      fprintf(stderr, "    -v3         Printable strings and SLIP packet notifications\n");
      fprintf(stderr, "    -v4         All printable characters as they are received\n");
      fprintf(stderr, "    -v5         All SLIP packets in hex\n");
#ifndef __APPLE__
      fprintf(stderr, "    -v          Equivalent to -v3\n");
#endif
      fprintf(stderr, " -d[basedelay]  Minimum delay between outgoing SLIP packets.\n");
      fprintf(stderr, "                Actual delay is basedelay*(#6LowPAN fragments) milliseconds.\n");
      fprintf(stderr, "                -d is equivalent to -d10.\n");
      exit(1);
      break;
    }
  }
  argc -= optind - 1;
  argv += optind - 1;

  if(argc >= 2) {
      slip_config.ipaddr = argv[1];
  }
  else {
      slip_config.ipaddr = NULL;
  }

  switch(baudrate) {
  case -2:
    break;			/* Use default. */
  case 9600:
    slip_config.b_rate = B9600;
    break;
  case 19200:
    slip_config.b_rate = B19200;
    break;
  case 38400:
    slip_config.b_rate = B38400;
    break;
  case 57600:
    slip_config.b_rate = B57600;
    break;
  case 115200:
    slip_config.b_rate = B115200;
    break;
#ifdef linux
  case 921600:
    slip_config.b_rate = B921600;
    break;
#endif
  default:
    err(1, "unknown baudrate %d", baudrate);
    break;
  }

  if(*slip_config.tundev == '\0') {
    /* Use default. */
    strcpy(slip_config.tundev, "tun0");
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
