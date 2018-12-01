/*
** Build Time: 2018-12-01 14:15:58.780000  
*/
#ifndef _CF_ASSEMBLER_H_
#define _CF_ASSEMBLER_H_



#define CHAIN_NUM   5
#define LINK_NUM    14



#include "chain_flow_support.h" 
#include "cf_runtime_functions.h" 
#include "cf_external_functions.h" 
static char  chain_state[5];
static char  link_state[14];
static unsigned link_data[14];
static unsigned start_state[]={1,1,1,1,1};



static LINK_CELL CHAIN_initialization_LINKS[]= 
{
{ one_step_fn,(CF_FUN_AUX)add_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ terminate_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_feed_watch_dog_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_SECOND_TICK,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)pat_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_contact_input_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_HOST_CONTACT,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_contact_watchdog_LINKS[]= 
{
{ wait_event_count_fn,(CF_FUN_AUX)NULL,(unsigned)CF_MINUTE_TICK,(unsigned)3,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)modbus_relay_disable_irrigation_cf,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_irrigation_timer_LINKS[]= 
{
{ wait_condition_fn,(CF_FUN_AUX)modbus_relay_check_irrigation_timer_cf,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)modbus_relay_disable_irrigation_cf,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};



static CHAIN_LINK chain_control[] =
{
{ 0,0,2,CHAIN_initialization_LINKS},
{ 2,1,3,CHAIN_feed_watch_dog_LINKS},
{ 5,2,3,CHAIN_contact_input_LINKS},
{ 8,3,3,CHAIN_contact_watchdog_LINKS},
{ 11,4,3,CHAIN_irrigation_timer_LINKS},
};
#endif
