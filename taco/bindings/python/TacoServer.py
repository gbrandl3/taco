import thread
import DEVCMDS
import Server

from TacoDevice import *

from DEVCMDS   import *
from DEV_XDR   import *
from DEVSTATES import *
from DEVERRORS import *

class TacoServer:
	"Taco Python super class"
	
#	Common variables for a class
	cmd_list = { DevState :[D_VOID_TYPE,D_SHORT_TYPE , 'state', 'DevState'],
		     DevStatus:[D_VOID_TYPE,D_STRING_TYPE,'status','DevStatus']}
	class_name = "PythonClass"
	
#	Valriables for an object
	dev_name    = "NONE"
	dev_state   = DEVUNKNOWN
	dev_status  = "The device is in an unknown state"
	
	def __init__ (self, device_name, device_class=0, command_list=0):
		self.dev_name = device_name
		if device_class != 0:
			self.class_name = device_class
		if command_list != 0:
			self.cmd_list = command_list			
		return

	def state (self):
		print 'executing python state method'
		return self.dev_state
		
	def status (self):
		print 'executing python status method'
		return self.dev_status

#
# 	Print the command list of the object 
#
	def CommandList (self):
		print Tab_dev_head
		
		cmd_list_keys = self.cmd_list.keys()
		for cmd in cmd_list_keys:
			cmd_list_values = self.cmd_list[cmd]
			type_in  = str (cmd_list_values[0])
			type_out = str (cmd_list_values[1])
			
			# Get the type description texts from
			# a global list in module TacoDevice.py
					
	 		if Tab_dev_type.has_key (type_in):
	       	   		myin = Tab_dev_type [type_in]
	    		else:
	       			myin = Tab_dev_type_unk
	    		if Tab_dev_type.has_key (type_out):
	       			myout = Tab_dev_type [type_out]
	    		else:
	       			myout = Tab_dev_type_unk
				
			# Get the command string from the resource
			# datbase
			
			cmd_name = Server.cmd_string (cmd)
			if (cmd_name == None):
			   	cmd_name = Tab_dev_type_unk
					     
	    		print "%s %s %s" % (myin, myout, cmd_name)
		return None
#
# 	Catch all not defined attributes and call cmd_io()
# 	to check whether the called symbol is a command name
#
  	def __getattr__ (self, cmd_name):
      		self.command_name = cmd_name
        	print cmd_name
      		return self.cmd_io
#
# 	Check in the module DEVCMD for a valid entry a command name.
# 	If an entry was found, get its integer value.
# 	Search with the integer value in the command list to get
# 	the object methode to execute.
# 	Execute the correct methode for the requested command neme.
#
   	def cmd_io (self, *par, **kw):
		cmd = DEVCMDS.__dict__[self.command_name]
		cmd_list_values = self.cmd_list[cmd]
		method_name = cmd_list_values[2]
		
      		ret = apply (getattr (self, method_name), par, kw)
      		return ret		

#
# Methods for server class
#
	def get_cmd_list (self):
		return self.cmd_list
		
	def get_dev_name (self):
		return self.dev_name
		
	def get_class_name (self):
		return self.class_name		

#
# function for server startup
#

def server_startup (devices, server_name='test', process_name='Python', nodb=0, pn=0):
	if nodb == 0:
		thread.start_new_thread (Server.startup, (process_name, server_name, devices))
	else:
		thread.start_new_thread (Server.startup_nodb, (process_name, server_name, devices, pn))

