from helper_functions import  Helper_Functions
from assembler          import  CF_Assembler

cf = CF_Assembler()
hf = Helper_Functions( cf )


cf.define_chain("initialization",True)


hf.one_step("setup_gpio")
hf.one_step("add_watch_dog")
hf.wait( "wait_for_mqtt_connect"  )
hf.enable_chain( "sampling_task","output_task")
hf.terminate()  #initialization is done now disable the chain
cf.end_chain()






#These chains are for actions every second 
cf.define_chain("feed_watch_dog", True )
hf.wait_event("CF_SECOND_TICK")
hf.one_step("pat_watch_dog")
hf.reset()
cf.end_chain()

#These chains are for actions every minute
cf.define_chain("sampling_task", False )
hf.wait_event("CF_TIME_TICK_EVENT")
hf.one_step("sample_switches")
hf.reset()
cf.end_chain()

#These chains are for actions every minute
cf.define_chain("output_task", False )
hf.wait_event_count("CF_SECOND_TICK",10)
hf.one_step("output_values")
hf.reset()
cf.end_chain()




cf.generate_c_header()


