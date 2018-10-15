
#include "app_analog_dac.h"


static bool read_dac_file_configuration(void);
static void add_read_dac_configuration(void);
static void add_dac_update_subscription(void);

void app_analog_setup_dacs(void ) // these are only subscription 
{
    
    if( read_dac_file_configuration() == true)
    {
        add_dac_update_subscription();
        add_read_dac_configuration();
    }
    
    
}



static void add_dac_update_subscription(void)
{
    
    ;
}

static void add_read_dac_configuration(void)
{
    
    ;
    
    
}

static bool read_dac_file_configuration(void)
{
    return false;
    
}