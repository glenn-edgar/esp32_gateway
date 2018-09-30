/*
**
** File cf_events.h
**
*/

#ifndef _CF_EVENTS_H_
#define _CF_EVENTS_H_

#define TICK_INTERVAL          10

#define CF_SYSTEM_INIT         0
#define CF_INIT_EVENT          1
#define CF_TIME_TICK_EVENT     2
#define CF_SECOND_TICK         5
#define CF_MINUTE_TICK         6
#define CF_APP_EVENT_START    10



void cf_initialize_event_manager( void );
int cf_event_number(void);
void cf_send_interrupt_event( unsigned event, unsigned data);
void cf_send_event( unsigned event, unsigned data );
int cf_rx_event( unsigned *event, unsigned *event_data );

#endif
