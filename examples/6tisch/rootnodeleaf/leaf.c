/*
 * Copyright (c) 2015, SICS Swedish ICT.
 * Copyright (c) 2018, University of Bristol - http://www.bristol.ac.uk
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
 */
/**
 * \file
 *         A RPL+TSCH node demonstrating application-level time syncrhonization.
 *
 * \author Atis Elsts <atis.elsts@bristol.ac.uk>
 *         Simon Duquennoy <simonduq@sics.se>
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/mac/tsch/tsch.h"
#include "lib/random.h"
#include "sys/node-id.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_DBG

#define UDP_LEAF_PORT 3333
#define UDP_ROOT_PORT 1111

#define SEND_INTERVAL         (10 * CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
static struct simple_udp_connection udp_conn;

PROCESS(node_process, "RPL Leaf");
AUTOSTART_PROCESSES(&node_process);
uint64_t by_drone=0;

/*---------------------------------------------------------------------------*/

static void
root_to_leaf(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
 if(datalen >= sizeof(by_drone)) {


    memcpy(&by_drone, data, sizeof(by_drone));
    LOG_INFO_("RECIVED A PACKAGE FROM SERVER");
    LOG_INFO_("%lu ",(unsigned long)by_drone);

 }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer periodic_timer;
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  is_coordinator = 0;

#if CONTIKI_TARGET_COOJA
  is_coordinator = (node_id == 1);
#endif


radio_value_t power_level;
NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &power_level);
radio_value_t new_power_level = power_level + (radio_value_t)-17;
NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, new_power_level);
  /* Initialize UDP connections */
  simple_udp_register(&udp_conn, UDP_LEAF_PORT, NULL,
                      UDP_ROOT_PORT,root_to_leaf);

  NETSTACK_MAC.on();

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if(tsch_is_coordinator) {
      break;
    }
    if(by_drone){
        LOG_INFO_("GOING BY DRONE NOW");
    }

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

      /* Send network uptime timestamp to the DAG root */
      uint64_t network_uptime;
      network_uptime = tsch_get_network_uptime_ticks();
      simple_udp_sendto(&udp_conn, &network_uptime, sizeof(network_uptime), &dest_ipaddr);

      LOG_INFO("Sent network uptime timestamp %lu to ", (unsigned long)network_uptime);
      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO_("\n");
    } else {
      LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
               - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
