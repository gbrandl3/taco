#------------------------------------------------------------------------
#
# Resource file for Serial-Server
#
#------------------------------------------------------------------------
 
#------------------------------------------------------------------------
#                               Devices list
#------------------------------------------------------------------------
# >>>>>>>>>>> domain: Test - use <hostname> as personal name to identify
#                            the host which runs the corresponding server!
# *** devices 1 on host "slot"

Serialds/test/device:           Test/Serialds/0
 
Test/Serialds/0/serialline:     /dev/ttyS0
Test/Serialds/0/baudrate:       9600
Test/Serialds/0/charlength:    	8
Test/Serialds/0/parity:		none
Test/Serialds/0/stopbits:	1
Test/Serialds/0/timeout:	1024
Test/Serialds/0/newline:	13

#-------------------------------------------------------------------------
#				CLASS Resources
#-------------------------------------------------------------------------
CLASS/Serialds/DEFAULT/logpath:		/tmp/log

