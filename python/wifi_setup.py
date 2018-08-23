
import sys
import msgpack

prefix = "D:\\"
postfix = ".mpk"
#cmd args script    ssid password hostname

assert len(sys.argv) == 5,"script requires 4 args filename ssid password hostname "

filename = "WIFI.MPK"
filename = prefix + filename+postfix
ssid = sys.argv[2]
password = sys.argv[3]
hostname = sys.argv[4]

access_dict = {}
access_dict["ssid"] = ssid
access_dict["password"] = password
access_dict["hostname"] = hostname

packed_object = msgpack.packb(access_dict, use_bin_type=True)

f= open(filename,"wb")
f.write(packed_object)
f.close() 