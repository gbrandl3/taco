import TacoServer
from DEVCMDS import *
from DEV_XDR import *
from TacoDevice	import Dev_Exception, dev_getresource

class MyServer (TacoServer.TacoServer):
	"""
	This is a test class for a TACO device
	"""
	
#	Command list for the devices	
	my_cmd_list = { DevState :	[D_VOID_TYPE, D_SHORT_TYPE , 'state'],
		     DevStatus:		[D_VOID_TYPE, D_STRING_TYPE, 'status'],
		     DevOn:		[D_VOID_TYPE, D_VOID_TYPE ,  'on'],
		     DevOff:		[D_VOID_TYPE, D_VOID_TYPE ,  'off'],
		     DevSetValue:	[D_FLOAT_TYPE, D_VOID_TYPE , 'set'],
		     DevSetParam:	[D_VAR_FLOATARR, D_VOID_TYPE , 'set_array'],
		     DevReadValue:	[D_VOID_TYPE, D_FLOAT_TYPE,  'read'],
		     DevReadSigValues:	[D_VOID_TYPE, D_VAR_FLOATARR,'read_signals'],
		     DevGetDevs:	[D_VOID_TYPE, D_VAR_STRINGARR,'read_names'],
		     DevSetDevs:	[D_VAR_STRINGARR, D_VOID_TYPE,'set_names'] }

#	Common variables for a class
	class_name = "TestClass"
	value      = 123.4
	names      = ('no', 'input')
	array	   = (1,2,3)
	
	def __init__ (self, name):
		"""
		Constructor
		
		@param name name of the device to be created
		"""
		TacoServer.TacoServer.__init__ (self, name, command_list = self.my_cmd_list)
		
		try :
			res = dev_getresource (self.get_dev_name(), "value")
			if res != "" :
				self.value = float(res)
		except Dev_Exception :
			pass
		return
	
	def state (self):
		"""
		Returns the current device state as a number

		@see status
		@return the current device state as a number
		"""
		return self.dev_state
		
	def status (self):
		"""
		Returns the current device state as a human readable text
	
		@see state
		@return the current device state as a human readable text
		"""
		if self.dev_state == DEVUNKNOWN:
			self.dev_status = "The device is in an unknown state"
		elif self.dev_state == DEVON:
			self.dev_status = "The device is switched ON"
		elif self.dev_state == DEVOFF:
			self.dev_status = "The device is switched OFF"		
		return self.dev_status
		
	def on (self):
		"""
		Switches the device into the state 'ON'
		@see off
		@see status
		@see state
		"""
		self.dev_state = DEVON
		
	def off (self):
		"""
		Switches the device into the state 'OFF'
		@see on
		@see status
		@see state
		"""
		self.dev_state = DEVOFF
		
	def read (self):
		"""
		Reads the main value of the device
		
		@see set
		@return main value of the devices
		"""
		return self.value

	def set (self, x):
		"""
		Sets the main value of the device.
		@see read
		@param x new main value of the device
		"""
	
		if x > 100:
			Server.error.taco_error = DevErr_ValueOutOfBounds	
			raise Server.error
			
		elif x < 0:
			x / 0
						
		self.value = x
		return 
		
	def read_signals (self):
		"""
		Gives a tuple of some 'signal' values.
		It is only an example for the return of tuple values.

		@see set_array
		@return tuple of the 'signal' values
		"""
		return self.array
		
	def read_names (self):
		"""
		It is only an example for the return of tuple string values.
		
		@see set_names
		@return tuple of string values
		"""
		return self.names	
		
	def set_names (self, in_names):
	        """
		Sets a tuple of string values. 
		A copy to a new tuple is needed here since self.names = in_names
		will result in a memory fault when executing read_names!
		
		@see read_names
		@param in_names tuple of string values 
		"""
		self.names = ()
		for i in in_names:
			self.names = self.names + (i, )
		print self.names
		return

	def set_array (self, x):
	        """
		Sets a tuple of 'signal' values.
		A copy to a new tuple is needed here since self.array = x
		will result in a memory fault when executing read_signals!
		
		@see read_signals
		@param x tuple of 'signal' values
		"""	
		self.array = ()
		for i in x:
			self.array = self.array + (i, )
		print self.array
		return 
		
