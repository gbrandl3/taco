import DEVCMDS
import Server

from DEVCMDS   import *
from DEV_XDR   import *
from DEVSTATES import *
from DEVERRORS import *

class TacoServer:
	"""
	This class is the Taco Python super class. All classes used in Python as TACO devices
	should be derived from this class.
	"""
	
#	Common variables for a class
	cmd_list = { DevState :[D_VOID_TYPE,D_SHORT_TYPE , 'DevState', 'DevState'],
		     DevStatus:[D_VOID_TYPE,D_STRING_TYPE, 'DevStatus','DevStatus']}

	class_name = "PythonClass"
	
#	Valriables for an object
	dev_name    = "NONE"
	dev_state   = DEVUNKNOWN
	dev_status  = "The device is in an unknown state"
	
	def __init__ (self, device_name, device_class = None, command_list = None):
		"""
		The constructor 
	
		@param device_name
		"""
		self.dev_name = device_name
		if device_class != None :
			self.class_name = device_class
		if command_list != None :
			self.cmd_list = command_list			
		return

	def DevState (self):
		"""
		Returns the device status as a number

		This method may be overwritten in subclasses
		
		@see status
		@return device status as a number
		"""
		return self.dev_state
		
	def DevStatus (self):
		"""
		Gets the device status as a string

		This method may be overwritten in subclasses

		@see state
		@return device status as a human readable string
		"""
		return self.dev_status

	def CommandList (self):
		"""
	 	Prints the command list of the object 
		"""
		
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
				
			# Get the command string from the resource datbase
			cmd_name = Server.cmd_string (cmd)
			if (cmd_name == None):
			   	cmd_name = Tab_dev_type_unk
					     
	    		print "%s %s %s" % (myin, myout, cmd_name)
		return None

  	def __getattr__ (self, cmd_name):
		"""
 		Catches all not defined attributes and call cmd_io()
 		to check whether the called symbol is a command name
		"""
      		self.command_name = cmd_name
      		return self.cmd_io

   	def cmd_io (self, *par, **kw):
		"""
 		Checks in the module DEVCMD for a valid entry a command name.
 		If an entry was found, get its integer value.
 		Searches with the integer value in the command list to get
 		the object methode to execute.
 		Execute the correct methode for the requested command name.
		@param par
		@param kw

		@return 
		"""
		cmd = DEVCMDS.__dict__[self.command_name]
		cmd_list_values = self.cmd_list[cmd]
		method_name = cmd_list_values[2]
		
      		ret = apply (getattr (self, method_name), par, kw)
      		return ret		

#
# Methods for server class
#
	def get_cmd_list (self) :
		"""
		Returns the informations of the provided commands

		@return informations of the provided commands
		"""
		return self.cmd_list
		
	def get_dev_name (self) :
		"""
		Returns the name of the device
		
		@return name of the device
		"""
		return self.dev_name
		
	def get_class_name (self) :
		"""
		Returns the name of the class

		@return name of the class
		"""
		return self.class_name		

def server_startup (devices, server_name = 'test', process_name = 'Python', nodb = 0, pn = 0):
	"""
	This is the main function for server startup
	
	@param devices tuple of the created devices should be served by the server
	@param server_name the personal name of the server. It should be different from 
		others
	@param process_name the name of the process itself.
	@param nodb if this parameter is set to 1 the server will work without the TACO database
	@param pn if this parameter is differen from 0 the server will try to use this number 
			as RPC program number. If this number is in use please try another. Normally
			there is no need to create your own number.
	"""
	if nodb == 0:
		Server.startup(process_name, server_name, devices)
	else:
		Server.startup_nodb(process_name, server_name, devices, pn)

