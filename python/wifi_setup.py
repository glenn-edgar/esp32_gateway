
import sys
import msgpack

prefix = "D:\\"
postfix = ".msg"
#get command  filename  ssid password hostname

assert len(sys.argv) == 5,"script requires 4 args filename ssid password hostname "

filename = sys.argv[1]
filename = prefix + filename+postfix
ssid = sys.argv[2]
password = sys.argv[3]
hostname = sys.argv[4]

access_dict = {}
access_dict["ssid"] = ssid
access_dict["password"] = password
access_dict["hostname"] = hostname

packed_object = msgpack.packb(access_dict, use_bin_type=True)

f= open(filename,"w+")
f.write(packed_object)
f.close() 