#
# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright(c) 1994-2005 by European Synchrotron Radiation Facility,
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
# File:         ResDatabase.py
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
# Version:      $Revision: 1.4 $
#
# Date:         $Date: 2005-07-25 13:43:42 $
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
