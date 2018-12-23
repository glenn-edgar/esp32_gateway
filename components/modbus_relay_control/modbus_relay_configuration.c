
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "msgpack_rx_handler.h"
#include "modbus_relay_configuration.h"

static bool modbus_relay_find_baud_rate( cmp_ctx_t *ctx, uint32_t *baud_rate);
static bool  modbus_relay_find_modbus_address( cmp_ctx_t *ctx, uint32_t *modbus_address);
static bool modbus_relay_find_baud_rtu_flag( cmp_ctx_t *ctx,bool *rtu_flag);
static bool modbus_relay_find_baud_parity( cmp_ctx_t *ctx, uint32_t *parity);

                                  
bool modbus_relay_read_file_configuration( uint32_t *baud_rate,
                                     uint32_t *modbus_address, 
                                     bool  *rtu_flag,
                                     uint32_t  *parity)
                                     
{
    
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/MD_RELAY.MPK",  &buffer,&buffer_size ) 
                                 != true)
    {
        
         return false;
    }
    msgpack_rx_handler_init(&ctx, buffer, buffer_size);
    if( modbus_relay_find_baud_rate(&ctx,baud_rate) == false)
    {goto error;}
    if( modbus_relay_find_modbus_address(&ctx,modbus_address) == false)
    {goto error;}
    if( modbus_relay_find_baud_rtu_flag(&ctx,rtu_flag) == false)
    {goto error;}
    if( modbus_relay_find_baud_parity(&ctx,parity) == false)
    {goto error;}


    
    free(buffer);
    return true;    
error:
    free(buffer);
    return false;    
}

static bool modbus_relay_find_baud_rate( cmp_ctx_t *ctx, uint32_t *baud_rate)
{
    
    return msgpack_rx_handler_find_unsigned( ctx,"BAUD_RATE", baud_rate );
    
}
static bool  modbus_relay_find_modbus_address( cmp_ctx_t *ctx, uint32_t *modbus_address)
{
    
    return msgpack_rx_handler_find_unsigned( ctx,"ADDRESS", modbus_address );
    
}
static bool modbus_relay_find_baud_rtu_flag( cmp_ctx_t *ctx, bool *rtu_flag)
{
    
    return msgpack_rx_handler_find_boolean( ctx,"RTU_FLAG", rtu_flag );
    
}

/*
parity values esp32 expects
UART_PARITY_DISABLE = 0x0,  
UART_PARITY_EVEN = 0x2,     
UART_PARITY_ODD  = 0x3      
*/                                             
                                           
                                             
static bool modbus_relay_find_baud_parity( cmp_ctx_t *ctx, uint32_t *parity)
{
    
    return msgpack_rx_handler_find_unsigned( ctx,"PARITY", parity );
    
}
                                         
                               
                                       
