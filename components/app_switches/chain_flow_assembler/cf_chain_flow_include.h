/*
** Build Time: 2018-09-29 20:51:46.687571  
*/
#ifndef _CF_ASSEMBLER_H_
#define _CF_ASSEMBLER_H_



#define CHAIN_NUM   3
#define LINK_NUM    10



#include "cf_chain_flow_support.h" 
#include "cf_runtime_functions.h" 
#include "cf_external_functions.h" 
char  chain_state[3];
char  link_state[10];
unsigned link_data[10];
const unsigned start_state[]={1,1,0};



const LINK_CELL CHAIN_initialization_LINKS[]= 
{
{ one_step_fn,(CF_FUN_AUX)setup_gpio,(unsigned)0,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)add_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ wait_condition_fn,(CF_FUN_AUX)wait_for_mqtt_connect,(unsigned)0,(unsigned)0,(unsigned)0},
{ enable_chain_fn,(CF_FUN_AUX)NULL,(unsigned)sampling_task,(unsigned)-1,(unsigned)-1},
{ terminate_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
const LINK_CELL CHAIN_feed_watch_dog_LINKS[]= 
{
{ wait_event_fn,(CF_FUN_AUX)NULL,(unsigned)CF_SECOND_TICK,(unsigned)0,(unsigned)0},
{ one_step_fn,(CF_FUN_AUX)pat_watch_dog,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};
const LINK_CELL CHAIN_sampling_task_LINKS[]= 
{
{ one_step_fn,(CF_FUN_AUX)sample_switches,(unsigned)0,(unsigned)0,(unsigned)0},
{ reset_fn,(CF_FUN_AUX)NULL,(unsigned)0,(unsigned)0,(unsigned)0},
};



const CHAIN_LINK chain_control[] =
{
{ 0,0,5,CHAIN_initialization_LINKS},
{ 5,1,3,CHAIN_feed_watch_dog_LINKS},
{ 8,2,2,CHAIN_sampling_task_LINKS},
};
#endif
