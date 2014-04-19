#
# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright(c) 1994-2014 by European Synchrotron Radiation Facility,
#                     Grenoble, France
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# File:         Server_startup.py
#
# Project:      Automatic Beamline Alignment
#
# Description:  Example script how to start a TACO device server written in Python
#
# Author(s):    J.Meyer
#               $Author: jkrueger1 $
#
# Original:     Mai 2001
#
# Version:      $Revision: 1.6 $
#
# Date:         $Date: 2008-04-06 09:06:31 $
#

from TacoServer import *
import MyServer
import  YourServer
import os

def start():
	process_name = 'Python'
	server_name  = 'test'
	
	full_name = process_name + "/" + server_name
	print full_name

	#devices = TacoServer.dev_getdevlist (full_name)
	devices = ("test/python/my", "test/python/your")
	print devices
	
	dev = []
	#
	# Create two device objects
	#
	x=MyServer.MyServer ("test/python/my")
	dev.append(x)
	y=YourServer.YourServer ("test/python/your")
	dev.append(y)
	#
	# Put the two objects to be exported 
	# on the network in a list (no longer a tuple!!!!!)
	#
	# Export to the network and start the 
	# device server thread
	#
	# With a device server definition in the resource 
	# database as:
	# Python/test/device:	id/python/test1 \
	#			id/python/test2
	#	
	# 
	server_startup (dev, process_name=process_name,  server_name=server_name)

if __name__ == "__main__":

    	print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
      	raw_input ('Press Enter to continue: ')

	start()
	import time
	time.sleep(1000)
