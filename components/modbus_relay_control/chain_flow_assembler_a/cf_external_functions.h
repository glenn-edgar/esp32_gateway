/*
**
** File: cf_external_functions.h
** This file is automatically generated by the
** chain flow assembler
** Build Time: 2018-12-23 15:10:50.522000  
**
*/

#ifndef _EXTERNAL_FUNCTIONS_H_
#define _EXTERNAL_FUNCTIONS_H_ 

#include "chain_flow_support.h" 
#include "cf_events.h"
/*
This is the symbolic definitions of the chains
*/



#define  initialization   0
#define  feed_watch_dog   1
#define  contact_input   2
#define  start_irrigation_timer   3
#define  stop_irrigation_timer   4
#define  contact_watchdog   5
#define  irrigation_timer   6



/*
This is the symbolic definitions of the links for chain initialization
*/
#define  initialization_0   0
#define  initialization_1   1



/*
This is the symbolic definitions of the links for chain feed_watch_dog
*/
#define  feed_watch_dog_0   0
#define  feed_watch_dog_1   1
#define  feed_watch_dog_2   2



/*
This is the symbolic definitions of the links for chain contact_input
*/
#define  contact_input_0   0
#define  contact_input_contact_watchdog   1
#define  contact_input_2   2



/*
This is the symbolic definitions of the links for chain start_irrigation_timer
*/
#define  start_irrigation_timer_0   0
#define  start_irrigation_timer_irrigation_timer   1
#define  start_irrigation_timer_2   2



/*
This is the symbolic definitions of the links for chain stop_irrigation_timer
*/
#define  stop_irrigation_timer_0   0
#define  stop_irrigation_timer_1   1
#define  stop_irrigation_timer_2   2



/*
This is the symbolic definitions of the links for chain contact_watchdog
*/
#define  contact_watchdog_0   0
#define  contact_watchdog_1   1
#define  contact_watchdog_2   2



/*
This is the symbolic definitions of the links for chain irrigation_timer
*/
#define  irrigation_timer_0   0
#define  irrigation_timer_1   1
#define  irrigation_timer_2   2



int modbus_relay_check_irrigation_timer_cf(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data);


int pat_watch_dog(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data);


int add_watch_dog(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data);


int modbus_relay_disable_irrigation_cf(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data);






#endif
