import TacoServer
from DEVCMDS import *
from DEV_XDR import *

from TacoDevice import Dev_Exception, dev_getresource

class YourServer (TacoServer.TacoServer) :
	"""
	This is another test class for a TACO device
	"""
	
#	Command list
	cmd_list = { DevState :[D_VOID_TYPE,D_SHORT_TYPE ,'state','DevState'],
		     DevStatus:[D_VOID_TYPE,D_STRING_TYPE,'status','DevStatus'],
		     DevOpen:[D_VOID_TYPE, D_VOID_TYPE ,  'open', 'DevOpen'],
		     DevClose:[D_VOID_TYPE, D_VOID_TYPE ,  'close', 'DevClose'],
		     DevReadSigValues:[D_VOID_TYPE,D_VAR_FLOATARR,'read_signals', 'DevReadSigValues']}
	
#	Common variables for a class
	class_name = "YourTestClass"		
	value      = 123.4
		
	def __init__ (self, name):
		"""
		Constructor
		@param name name of the TACO device to be created
		"""
		TacoServer.TacoServer.__init__ (self, name,  command_list = self.cmd_list)		
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
		elif self.dev_state == DEVOPEN:
			self.dev_status = "The device is Open"
		elif self.dev_state == DEVCLOSE:
			self.dev_status = "The device is Closed"		
		return self.dev_status
		
	def open (self):
		"""
		Switches the device into the state 'OPEN'
		@see close
		@see status
		@see state
		"""
		self.dev_state = DEVOPEN
		
	def close (self):
		"""
		Switches the device into the state 'CLOSE'
		@see open
		@see status
		@see state
		"""
		self.dev_state = DEVOPEN
		self.dev_state = DEVCLOSE	
		
	def read_signals (self):
		"""
		Gives a tuple of some 'signal' values.
		It is only an example for the return of tuple values.

		@return tuple of the 'signal' values
		"""
		signals = (self.dev_state, self.value)
		return signals		
		
