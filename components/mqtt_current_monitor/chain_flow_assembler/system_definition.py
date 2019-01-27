from helper_functions import  Helper_Functions
from assembler          import  CF_Assembler

cf = CF_Assembler()
hf = Helper_Functions( cf )


cf.define_chain("initialization",True)
hf.one_step("add_watch_dog")
hf.one_step("mqtt_current_initialize_ref_max_currents")
hf.one_step("mqtt_current_register_subscriptions")
hf.enable_chain("equipment_relay_monitor")
hf.enable_chain("irrigation_relay_monitor")
hf.wait( "wait_for_mqtt_connect"  )
hf.one_step("mqtt_current_set_subscription_flag")
hf.terminate()  #initialization is done now disable the chain
cf.end_chain()




#These chains are for actions every second 
cf.define_chain("feed_watch_dog", True )
hf.wait_event("CF_SECOND_TICK")
hf.one_step("pat_watch_dog")
hf.reset()
cf.end_chain()






#These chains are for actions every second 
cf.define_chain("equipment_relay_monitor", False )
hf.wait_event("CF_SECOND_TICK")
hf.one_step("mqtt_current_monitor_equipment_relay")
hf.reset()
cf.end_chain()

#These chains are for actions every second 
cf.define_chain("irrigation_relay_monitor", False )
hf.wait_event("CF_SECOND_TICK")
hf.one_step("mqtt_current_monitor_irrigation_relay")
hf.reset()
cf.end_chain()







cf.generate_c_header()


