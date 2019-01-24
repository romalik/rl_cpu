/*
  SerialIP.h - Arduino implementation of a uIP wrapper class.
  Copyright (c) 2010 Adam Nielsen <malvineous@shikadi.net>
  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SerialIP_h
#define SerialIP_h


#include "uip/timer.h"
#include "uip/uip.h"

typedef struct {
	int a, b, c, d;
} IP_ADDR;

/*
#define IP_INCOMING_CONNECTION  0
#define IP_CONNECTION_CLOSED    1
#define IP_PACKET_ACKED         2
#define IP_INCOMING_DATA        3
#define IP_SEND_PACKET          4
*/
//typedef struct psock ip_connection_t;

typedef void (*fn_uip_cb_t)(uip_tcp_appstate_t *conn);

typedef void (*fn_my_cb_t)(unsigned long a);
extern fn_my_cb_t x;



void serialip_use_device(int fd);
void serialip_begin(IP_ADDR myIP, IP_ADDR subnet);
void serialip_set_gateway(IP_ADDR myIP);
void serialip_listen(uint16_t port);

		// tick() must be called at regular intervals to process the incoming serial
		// data and issue IP events to the sketch.  It does not return until all IP
		// events have been processed.
void serialip_tick();

		// Set a user function to handle raw uIP events as they happen.  The
		// callback function can only use uIP functions, but it can also use uIP's
		// protosockets.
void serialip_set_uip_callback(fn_uip_cb_t fn);

struct timer periodic_timer;
		//, arp_timer;
struct serialip_state *serialip_cur_conn; // current connection (for print etc.)
extern fn_uip_cb_t fn_uip_cb;
void uip_callback();

//???void serialip_appcall(void);


//void handle_ip_event(uint8_t type, ip_connection_t *conn, void **user);


#endif
