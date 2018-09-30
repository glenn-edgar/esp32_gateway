/*
**
** File cf_status_bit.c
**
**
**
*/
#include <esp_types.h>
#include "cf_status_bit.h"


static uint32_t cf_status_register;



void cf_initialize_status_manager( void )
{
   cf_status_register = 0;
}

void  cf_status_clear_bit( uint32_t status_bit)
{
 

    cf_status_register = cf_status_register &(~status_bit);
    
}

void cf_clear_status(void )
{
      
  

    cf_status_register = 0;
    
}

void cf_set_interrupt_status_bit( uint32_t status_bit )
{
      cf_status_register |= status_bit;

}

void cf_set_status_bit( uint32_t status_bit )
{
    
    
   

   cf_status_register |= status_bit;
   
}


uint32_t cf_get_status( )
{
   int return_value;

   return_value = cf_status_register;
   cf_status_register = 0;
   
   return return_value;
}
             
  



