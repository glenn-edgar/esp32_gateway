
#include <stdio.h>
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "gpio.h"
     
     
void gpio_config_output_pin( int pin )
{
     gpio_pad_select_gpio(pin);
     gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void gpio_config_input_pin( int pin )
{
   gpio_pad_select_gpio(pin); 
   gpio_set_direction(pin, GPIO_MODE_INPUT); 
}

void gpio_set_value(int pin, int value )
{
    
   gpio_set_level(pin,value);
    
}

int gpio_read_value(int pin)
{
    
   return gpio_get_level(pin); 
}

#if 0

for reference can set interrupt as well as pullup and pulldown
esp_err_t gpio_pulldown_en(gpio_num_t gpio_num);
esp_err_t gpio_pullup_en(gpio_num_t gpio_num);
typedef enum {
    GPIO_PULLDOWN_DISABLE = 0x0,     /*!< Disable GPIO pull-up resistor */
    GPIO_PULLDOWN_ENABLE = 0x1,      /*!< Enable GPIO pull-up resistor */
} gpio_pullup_t;
typedef enum {
    GPIO_PULLUP_DISABLE = 0x0,     /*!< Disable GPIO pull-up resistor */
    GPIO_PULLUP_ENABLE = 0x1,      /*!< Enable GPIO pull-up resistor */
} gpio_pullup_t;
typedef enum {
    GPIO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    GPIO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    GPIO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    GPIO_INTR_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
    GPIO_INTR_LOW_LEVEL = 4,   /*!< GPIO interrupt type : input low level trigger      */
    GPIO_INTR_HIGH_LEVEL = 5,  /*!< GPIO interrupt type : input high level trigger     */
    GPIO_INTR_MAX,
} gpio_int_type_t

    /* Set Button as input */
    gpio_set_direction(OLIMEX_BUT_PIN, GPIO_MODE_INPUT);
    /* Enable interrupt on both edges */
    gpio_set_intr_type(OLIMEX_BUT_PIN, GPIO_INTR_ANYEDGE);

    /* Install ISR routine */
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(OLIMEX_BUT_PIN, gpio_isr_handler, 0);

 static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    gpio_set_level(RELAY_GPIO, !gpio_get_level(OLIMEX_BUT_PIN));
}   
#endif