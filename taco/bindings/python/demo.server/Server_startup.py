#
# Example script how to start a TACO device server written in Python
#

import TacoServer
import MyServer

def start():
	#
	# Create two device objects
	#
	x = MyServer.MyServer('test/python/test1')
	y = MyServer.MyServer('test/python/test2')

	#
	# Export to the network and start the device server thread
	#
	# With a device server definition in the resource database as:
	# process_name/server_name/device:
	#
	# e.g.:
	# Python/test/device:	test/python/test1 \
	#			test/python/test2
	#	
	# 
	TacoServer.server_startup((x, y), process_name = 'Python', server_name = 'test')

if __name__ == "__main__":
	start()
