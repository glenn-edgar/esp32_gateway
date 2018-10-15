#ifndef _APP_ANALOG_EVENT_QUEUE_H_
#define _APP_ANALOG_EVENT_QUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>

#define ANALOG_SAMPLE_NUMBER 256
#define ANALOG_CHANNEL_NUMBER 6

typedef struct
{
  uint32_t gpio_pin;
  uint32_t raw_samples[ANALOG_SAMPLE_NUMBER];
}APP_ANALOG_EVENT_CHANNEL;

typedef struct
{
    uint32_t sample_index;
    uint32_t channel_number;
    uint32_t sample_number;
    APP_ANALOG_EVENT_CHANNEL analog_event_channel[ANALOG_CHANNEL_NUMBER];
}APP_ANALOG_EVENT_STRUCTURE;


typedef struct
{
    uint32_t gpio_pin;
    float dc_average;
    float ac_average;
}APP_ANALOG_MQTT_CHANNEL;

typedef struct
{
    
    uint32_t                 channel_number;
    APP_ANALOG_MQTT_CHANNEL  app_analog_mqtt_channel[ANALOG_CHANNEL_NUMBER];
}APP_MQTT_STRUCTURE;




#endif