#
# Example script how to start a 
# Taco device server written in Python
#

import TacoServer
import MyServer

def start():
	#
	# Create two device objects
	#
	x=MyServer.MyServer ('test/python/test1')
	y=MyServer.MyServer ('test/python/test2')

	#
	# Put the two objects to be exported 
	# on the network in a tuple
	#
	dev=(x,y)

	#
	# Export to the network and start the 
	# device server thread
	#
	# With a device server definition in the resource 
	# database as:
	# Python/test/device:	test/python/test1 \
	#			test/python/test2
	#	
	# 
	TacoServer.server_startup(dev, process_name='Python', server_name='test')

if __name__ == "__main__":
	start()
	import time
	time.sleep(1000)
		
				
