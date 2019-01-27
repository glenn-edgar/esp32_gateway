#ifndef _APP_ANALOG_TASK_H_
#define _APP_ANALOG_TASK_H_

float *app_analog_get_analog_channels(void);
void app_analog_initialize_sampling_task(  xQueueHandle mqtt_que );

#endif