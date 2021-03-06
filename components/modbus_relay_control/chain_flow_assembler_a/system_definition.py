from helper_functions import  Helper_Functions
from assembler          import  CF_Assembler

cf = CF_Assembler()
hf = Helper_Functions( cf )


cf.define_chain("initialization",True)

hf.one_step("add_watch_dog")

hf.terminate()  #initialization is done now disable the chain
cf.end_chain()






#These chains are for actions every second 
cf.define_chain("feed_watch_dog", True )
hf.wait_event("CF_SECOND_TICK")
hf.one_step("pat_watch_dog")
hf.reset()
cf.end_chain()



cf.define_chain("contact_input", True )
hf.wait_event(  "CF_HOST_CONTACT" )

hf.disable_chain("contact_watchdog")
hf.enable_chain("contact_watchdog")
hf.reset()
cf.end_chain()


cf.define_chain("start_irrigation_timer",True)
hf.wait_event(  "CF_RESTART_TIMER" )
hf.disable_chain("irrigation_timer")
hf.enable_chain("irrigation_timer")
hf.reset()
cf.end_chain()

cf.define_chain("stop_irrigation_timer",True)
hf.wait_event(  "CF_TERMINATE_TIMER" )
hf.disable_chain("irrigation_timer")
hf.reset()
cf.end_chain()


cf.define_chain("contact_watchdog", True )

hf.wait_event_count( "CF_SECOND_TICK",120 )

hf.one_step("modbus_relay_disable_irrigation_cf")
hf.reset()
cf.end_chain()

cf.define_chain("irrigation_timer", False )
hf.wait("modbus_relay_check_irrigation_timer_cf")
hf.one_step("modbus_relay_disable_irrigation_cf")
hf.reset()
cf.end_chain()






cf.generate_c_header()


