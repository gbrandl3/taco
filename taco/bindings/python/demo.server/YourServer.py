
from TacoServer  import *


class YourServer (TacoServer):
	"This is another test class"
	
#	Common variables for a class
	
	cmd_list = { DevState :[D_VOID_TYPE,D_SHORT_TYPE ,'state','DevState'],
		     DevStatus:[D_VOID_TYPE,D_STRING_TYPE,'status','DevStatus'],
		     DevOpen:[D_VOID_TYPE, D_VOID_TYPE ,  'open', 'DevOpen'],
		     DevClose:[D_VOID_TYPE, D_VOID_TYPE ,  'close', 'DevClose'],
		     DevReadSigValues:[D_VOID_TYPE,D_VAR_FLOATARR,'read_signals', 'DevReadSigValues']}
	
	class_name = "YourTestClass"		
	value      = 123.4
		
	def __init__ (self, name):
		TacoServer.__init__ (self, name)		
		return
		
	def state (self):
		return self.dev_state
		
	def status (self):
		if self.dev_state == DEVUNKNOWN:
			self.dev_status = "The device is in an unknown state"
		elif self.dev_state == DEVOPEN:
			self.dev_status = "The device is Open"
		elif self.dev_state == DEVCLOSE:
			self.dev_status = "The device is Closed"		
		return self.dev_status
		
	def open (self):
		self.dev_state = DEVOPEN
		
	def close (self):
		self.dev_state = DEVCLOSE	
		
	def read_signals (self):
		signals = (self.dev_state, self.value)
		return signals		
		
				
