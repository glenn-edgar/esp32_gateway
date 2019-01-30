/*
** Build Time: 2019-01-29 17:17:15.214411  
*/
#ifndef _CF_ASSEMBLER_H_
#define _CF_ASSEMBLER_H_



#define CHAIN_NUM   4
#define LINK_NUM    17



#include "chain_flow_support.h" 
#include "cf_runtime_functions.h" 
#include "cf_external_functions.h" 
static char  chain_state[4];
static char  link_state[17];
static unsigned link_data[17];
static unsigned start_state[]={1,1,0,0};



static LINK_CELL CHAIN_initialization_LINKS[]= 
{
{ one_step_fn,(CF_FUN_AUX)add_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)mqtt_current_initialize_ref_max_currents,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)mqtt_current_register_subscriptions,(unsigned)0,(unsigned)0,(unsigned)0},
{ enable_chain_fn,(CF_FUN_AUX)NULL,(unsigned)equipment_relay_monitor,(unsigned)-1,(unsigned)-1},
{ enable_chain_fn,(CF_FUN_AUX)NULL,(unsigned)irrigation_relay_monitor,(unsigned)-1,(unsigned)-1},
{ wait_condition_fn,(CF_FUN_AUX)wait_for_mqtt_connect,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)mqtt_current_set_subscription_flag,(unsigned)0,(unsigned)0,(unsigned)0},
{ terminate_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_feed_watch_dog_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_SECOND_TICK,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)pat_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_equipment_relay_monitor_LINKS[]= 
{
{ wait_event_count_fn,(CF_FUN_AUX)NULL,(unsigned)CF_SECOND_TICK,(unsigned)10,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)mqtt_current_monitor_equipment_relay,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
static LINK_CELL CHAIN_irrigation_relay_monitor_LINKS[]= 
{
{ wait_event_count_fn,(CF_FUN_AUX)NULL,(unsigned)CF_SECOND_TICK,(unsigned)10,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)mqtt_current_monitor_irrigation_relay,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};



static CHAIN_LINK chain_control[] =
{
{ 0,0,8,CHAIN_initialization_LINKS},
{ 8,1,3,CHAIN_feed_watch_dog_LINKS},
{ 11,2,3,CHAIN_equipment_relay_monitor_LINKS},
{ 14,3,3,CHAIN_irrigation_relay_monitor_LINKS},
};
#endif
