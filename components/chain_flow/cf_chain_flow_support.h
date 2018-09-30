/*

File: cf_chain_flow_support.h


*/

#ifndef _CF_CHAIN_FLOW_SUPPORT_H_
#define _CF_CHAIN_FLOW_SUPPORT_H_

//typedef unsigned char uint8;
//typedef unsigned  unsigned;

// Chain flow return codes

#define CF_CONTINUE   0
#define CF_HALT       1
#define CF_DISABLE    2  // disable node
#define CF_RESET      3
#define CF_TERMINATE  4  // disable chain



    
typedef int (*CF_FUN_AUX)(unsigned link_id, unsigned param_1, unsigned param_2, unsigned param_3, unsigned event, unsigned data);

    
typedef int (*CF_FUN)( unsigned link_id, CF_FUN_AUX param_1, unsigned param_2,
                      unsigned param_3, unsigned param_4,  unsigned event, unsigned event_data);





typedef struct
{
   CF_FUN    func;  
   CF_FUN_AUX  param_1; 
   unsigned  param_2; 
   unsigned  param_3; 
   unsigned  param_4;  
}LINK_CELL;           



typedef struct
{ 
   unsigned     link_index; 
   unsigned     chain_index;
   unsigned     link_number; 
   const LINK_CELL  *link_cells; 
}CHAIN_LINK; 

#define cf_reset_chain  cf_enable_chain

void initialize_cf_system( void );
void cf_process_cf_event( unsigned event, unsigned event_data );
unsigned cf_get_cell_value( unsigned link_id );
void  cf_store_cell_value( unsigned link_id , unsigned value );
void cf_change_state( unsigned chain_id, unsigned link_id);
void cf_enable_chain(  unsigned  chain_id );
void cf_disable_chain( unsigned  chain_id );
void cf_resume_chain(  unsigned  chain_id );
void cf_suspend_chain( unsigned  chain_id );

#endif
