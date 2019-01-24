/*
  SerialIP.cpp - Arduino implementation of a uIP wrapper class.
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

#include "serialip.h"
#include "uip-conf.h"

#include "uip.h"
#include "uip_arp.h"
#include "slipdev.h"
#include "timer.h"

#include <unistd.h>

// Because uIP isn't encapsulated within a class we have to use global
// variables, so we can only have one TCP/IP stack per program.  But at least
// we can set which serial port to use, for those boards with more than one.

fn_uip_cb_t fn_uip_cb = 0;

static int serial_device_fd = 0;


void serialip_use_device(int fd)
{
  serial_device_fd = fd;
}

void serialip_begin(IP_ADDR myIP, IP_ADDR subnet)
{
	uip_ipaddr_t ipaddr;

	//??????timer_set(&this->periodic_timer, CLOCK_SECOND / 4);

	slipdev_init();
	uip_init();

	uip_ipaddr(ipaddr, myIP.a, myIP.b, myIP.c, myIP.d);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, subnet.a, subnet.b, subnet.c, subnet.d);
	uip_setnetmask(ipaddr);

}

void serialip_set_gateway(IP_ADDR myIP)
{
  uip_ipaddr_t ipaddr;
  uip_ipaddr(ipaddr, myIP.a, myIP.b, myIP.c, myIP.d);
  uip_setdraddr(ipaddr);
}

void serialip_listen(uint16_t port)
{
  uip_listen(HTONS(port));
}

void serialip_tick()
{
  int i;
	uip_len = slipdev_poll();
	if(uip_len > 0) {
		uip_input();
		// If the above function invocation resulted in data that
		// should be sent out on the network, the global variable
		// uip_len is set to a value > 0.
		if (uip_len > 0) slipdev_send();

	} else if (timer_expired(&periodic_timer)) {
		timer_reset(&periodic_timer);
        for (i = 0; i < UIP_CONNS; i++) {
			uip_periodic(i);
			// If the above function invocation resulted in data that
			// should be sent out on the network, the global variable
			// uip_len is set to a value > 0.
			if (uip_len > 0) slipdev_send();
		}

#if UIP_UDP
		for (int i = 0; i < UIP_UDP_CONNS; i++) {
			uip_udp_periodic(i);
			// If the above function invocation resulted in data that
			// should be sent out on the network, the global variable
			// uip_len is set to a value > 0. */
			if (uip_len > 0) slipdev_send();
		}
#endif /* UIP_UDP */
	}
}

void serialip_set_uip_callback(fn_uip_cb_t fn)
{
	fn_uip_cb = fn;
}

void uip_callback()
{
	struct serialip_state *s = &(uip_conn->appstate);
	//SerialIP.cur_conn = s;
	if (fn_uip_cb) {
		// The sketch wants to handle all uIP events itself, using uIP functions.
		fn_uip_cb(s);//->p, &s->user);
	} else {
	}
}


// uIP callback function
void serialip_appcall(void)
{
	uip_callback();
}



/*
 * Code to interface the serial port with the SLIP handler.
 *
 * See slipdev.h for further explanation.
 */


// Put a character on the serial device.
void slipdev_char_put(u8_t c)
{
  write(serial_device_fd, &c, 1);
}

// Poll the serial device for a character.
u8_t slipdev_char_poll(u8_t *c)
{
  u8_t retval;
  retval =  read(serial_device_fd, c, 1);
  //printf("read(%d)! 0x%02X\n", retval, *c);
  return retval;

}
