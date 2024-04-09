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

PROCESS(node_process, "RPL Root");
AUTOSTART_PROCESSES(&node_process);
uint64_t by_drone=0;

/*---------------------------------------------------------------------------*/
static void
leaf_to_root(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  uint64_t local_time_clock_ticks = tsch_get_network_uptime_ticks();
  uint64_t remote_time_clock_ticks;
  int16_t RSSI=uipbuf_get_attr(UIPBUF_ATTR_RSSI);
  LOG_INFO_("Storing Leaf Address");




  if(datalen >= sizeof(remote_time_clock_ticks)) {
    memcpy(&remote_time_clock_ticks, data, sizeof(remote_time_clock_ticks));

    LOG_INFO("Routing entries: %u\n", uip_ds6_route_num_routes());
    LOG_INFO("Routing links: %u\n", uip_sr_num_nodes());
    LOG_INFO("Received ");
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_(", created at %lu, now %lu, latency %lu clock ticks, rssi %d, lqi %u\n",
              (unsigned long)remote_time_clock_ticks,
              (unsigned long)local_time_clock_ticks,
              (unsigned long)(local_time_clock_ticks - remote_time_clock_ticks),
              RSSI,
              uipbuf_get_attr(UIPBUF_ATTR_LINK_QUALITY));

    if(RSSI<-30){
        by_drone=1;
        simple_udp_sendto(&udp_conn, &by_drone, sizeof(by_drone), sender_addr);
    }
    else{
        simple_udp_sendto(&udp_conn, &by_drone, sizeof(by_drone), sender_addr);


    }

  }
}

/*static void
node_to_root(struct simple_udp_connection *c,
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
}*/
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer periodic_timer;
  int is_coordinator;
//  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  is_coordinator = 1;





if(is_coordinator) {
  PROCESS_BEGIN();
  /* Set the transmission power level to -12 dBm */
  radio_value_t power_level;
  NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &power_level);
  radio_value_t new_power_level=power_level +(radio_value_t) -17;
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, new_power_level);
  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();
}


  /* Initialize UDP connections */
  simple_udp_register(&udp_conn, UDP_ROOT_PORT, NULL,
                      UDP_LEAF_PORT, leaf_to_root);

  NETSTACK_MAC.on();

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);


  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

  if(tsch_is_coordinator) {
    break;
  }



  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
