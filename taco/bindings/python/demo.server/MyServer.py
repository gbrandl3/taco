from TacoServer  import *


class MyServer (TacoServer):
	"This is a test class"
	
#	Common variables for a class
	
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

	class_name = "TestClass"
		
	value      = 123.4
	names      = ('no', 'input')
	array	   = (1,2,3)
	
#	rem_device = Dev('id/python/test4')
	
	def __init__ (self, name):
		TacoServer.__init__ (self, name, command_list=self.my_cmd_list)
		
#		res = dev_getresource (self.dev_name, "value")
#		if res != None:
#			self.value = float(res)
		self.value = 23.
		return
	
	def state (self):
#		print 'remote device status:'
#		print self.rem_device.DevStatus()		
		return self.dev_state
		
	def status (self):
		if self.dev_state == DEVUNKNOWN:
			self.dev_status = "The device is in an unknown state"
		elif self.dev_state == DEVON:
			self.dev_status = "The device is switched ON"
		elif self.dev_state == DEVOFF:
			self.dev_status = "The device is switched OFF"		
		return self.dev_status
		
	def on (self):
		self.dev_state = DEVON
		
	def off (self):
		self.dev_state = DEVOFF
		
	def read (self):
		return self.value

	def set (self, x):
		print x
		
		if x > 100:
			Server.error.taco_error = DevErr_ValueOutOfBounds	
			raise Server.error
			
		elif x < 0:
			x / 0
						
		self.value = x
		return 
		
	def read_signals (self):
		return self.array
		
	def read_names (self):
		return self.names	
		
	def set_names (self, in_names):
	        #
		# A copy to a new tuple is needed here!!!
		# self.names = in_names
		# will result in a memory fault when
		# executing read_names!
		#
		self.names = ()
		for i in in_names:
			self.names = self.names + (i, )
		print self.names
		return

	def set_array (self, x):
	        #
		# A copy to a new tuple is needed here!!!
		# self.array = x
		# will result in a memory fault when
		# executing read_signals!
		#	
		self.array = ()
		for i in x:
			self.array = self.array + (i, )
		print self.array
		return 
		
