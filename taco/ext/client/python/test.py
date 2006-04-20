# startup script for python to enable saving of interpreter history and
# enabling name completion

# import needed modules
import atexit
import os
import readline
import rlcompleter

# where is history saved
historyPath = os.path.expanduser("~/.pyhistory")

# handler for saving history
def save_history(historyPath=historyPath):
    import readline
    readline.write_history_file(historyPath)

# read history, if it exists
if os.path.exists(historyPath):
    readline.read_history_file(historyPath)

# register saving handler
atexit.register(save_history)

# enable completion
readline.parse_and_bind('tab: complete')

# cleanup
del os, atexit, readline, rlcompleter, save_history, historyPath

from TACOClient import *

a=Client('test/rs485/motor')
print "The network timeout is : %f seconds" % a.clientNetworkTimeout()
print "Set network timeout to 2 s"
a.setClientNetworkTimeout(2.0)
print "The network timeout is : %f seconds" % a.clientNetworkTimeout()

print "The network protocol is (888 - TCP, 999 - UDP) : %d " % a.clientNetworkProtocol()
print "Device has version : %s" % a.deviceVersion()
if a.isDeviceOff() :
	a.deviceOn()
print "Device is in status : %d(%s)" % (a.deviceState(), a.deviceStatus())
a.deviceReset()
print "Device is in status : %d(%s)" % (a.deviceState(), a.deviceStatus())
a.deviceOff()
print "Device is in status : %d(%s)" % (a.deviceState(), a.deviceStatus())
a.deviceOn()
print "Device is in status : %d(%s)" % (a.deviceState(), a.deviceStatus())
res=a.deviceQueryResourceInfo()
print "Device has %d resources : " % len(res)
for i in res :
	print "   %s : %s" % (i, res[i]["info"])
	print "       Value : %s" % a.deviceQueryResource(i)

a.deviceOff()
print "Set baudrate to 9600"
a.deviceUpdateResource("baudrate", "9600")
print "Baudrate is : %s" % a.deviceQueryResource("baudrate")
print "Set baudrate to 19200"
a.deviceUpdateResource("baudrate", "19200")
print "Baudrate is : %s" % a.deviceQueryResource("baudrate")
a.deviceUpdate()
a.deviceOn()

print "Is device off? ", a.isDeviceOff()

types = a.deviceTypes()
print "Device has the following types"
for i in types :
	print i, 
print
cmd=a.deviceQueryCommandInfo()
print "Device has %d commands : " % len(cmd)
for i in cmd :
	print "   %s : " % i

print "Execute commands with the 'execute' method"
print "DEVICE_STATUS : %s" % a.execute(DEVICE_STATUS)
print "DEVICE_STATE : %d" % a.execute(DEVICE_STATE)
print "DEVICE_VERSION : %s" % a.execute(DEVICE_VERSION)
