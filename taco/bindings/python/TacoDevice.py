#!/usr/bin/env python
# $Revision: 1.2 $
# $Date: 2003-04-25 11:52:10 $
# $Author: jkrueger1 $
#

from Taco import *
from string import *
import sys

Dev_deb = [0]
Dev_Exception = "TacoException"

#--------------------------------------------------------------
# Tab_dev: 
# dictionnary indexed with devices names (in lower case)
#
# { device_name1: { 'cobj' : C object,
#		    'ref'  : number of references to that object 
#                   'cmd'  : { command_name : [cmd,in_type,out_type],
#		               command_name : [cmd,in_type,out_type],
#			       ...
#			     }
#		  },
#   device_name2: { 'cobj' : C object, 
#		    'ref'  : number of references to that object 
#                   'cmd'  : { command_name : [cmd,in_type,out_type],
#		               command_name : [cmd,in_type,out_type],
#			       ...
#			     }
#		  }
# .....
			   
Tab_dev = {}

#--------------------------------------------------------------
# Tab_devC: 
# dictionnary indexed with data collector devices names (in lower case)
#
# { device_name1: { 'cobj' : C object,
#		    'ref'  : number of references to that object 
#                   'cmd'  : { command_name : [cmd,out_type],
#		               command_name : [cmd,out_type],
#			       ...
#			     }
#		  },
#   device_name2: { 'cobj' : C object, 
#		    'ref'  : number of references to that object 
#                   'cmd'  : { command_name : [cmd,out_type],
#		               command_name : [cmd,out_type],
#			       ...
#			     }
#		  }
# .....
			   
Tab_devC = {}

# Dictionnary for the correspondance between type number (in ascii
# representation) and a string for user
Tab_dev_type= { '0' :'void                         ',
		'1' :'boolean                      ',
		'70':'unsigned short               ',
		'2' :'short                        ',
		'71':'unsigned long                ',
		'3' :'long                         ',
		'4' :'float                        ',
		'5' :'double                       ',
		'6' :'string                       ',
		'27':'(long,float)                 ',
		'7' :'(float,float)                ',
		'8' :'(short,float,float           ',
		'22':'(long,long)                  ',
		'23':'(double,double)              ',
		'9' :'list of char                 ',
		'24':'list of string               ',
		'72':'list of unsigned short       ',
		'10':'list of short                ',
		'69':'list of unsigned long        ',
		'11':'list of long                 ',
		'12':'list of float                ',
		'68':'list of double               ',
		'25':'list of (float,float)        ',
		'73':'list of (short,float,float)  ',
		'45':'list of ((long,long)         ',
		'47':'opaque                       ',
		'46':'((8 long),(8 float),(8 long))',
		'54':'(long,long)                  ',
		'55':'(long,float)                 '
}

Tab_dev_type_unk =   '<unknown>                    '
Tab_dev_head = 'INPUT:                        OUTPUT:                       COMMAND:'
Tab_devC_head = 'OUTPUT:                       COMMAND:'

#--------------------------------------------------------------
def dev_debug(flag):
#  input:
#     flag: debug flag
#	0: no trace
#	else: trace
#
#  returns:
#	- 0: error
#  	- 1: OK
#--------------------------------------------------------------
   if (type(flag) != type(1)) :
      print "dev_debug: parameter not a number"
   else :
      Dev_deb[0] = flag
      if (Dev_deb[0] != 0) :
         print " debug mode %d" % Dev_deb[0]

#--------------------------------------------------------------
def dev_init(mdevname):
#  input:
#     mdevname: device name
#
#  returns list:
#	- [] if error
#
#  	- [devname,cpt]
#     		devname: mdevname in lowercase
#     		cpt:  index in C device table
#--------------------------------------------------------------
   print 'dev_init: ' + mdevname
   print Dev_deb[0]
   locname=lower(mdevname)

#  try to find in Tab_dev list if name already
#  found
   
   if Tab_dev.has_key(locname):
      print "device already found in Tab_dev"
      Tab_dev[locname]['ref'] = Tab_dev[locname]['ref'] + 1

   else:
#     have to import the device and create place in dict   
      try :
         mpt = esrf_import(locname)
      except error:
#         print error
#         print "dev_init: error on importing device %s" % locname
         raise Dev_Exception,sys.exc_value
         return []

#     create in Tab_dev 
      Tab_dev[locname] = {}
      Tab_dev[locname]['cobj'] = mpt
      Tab_dev[locname]['ref'] = 1
      Tab_dev[locname]['cmd'] = dev_query(locname)

   if Dev_deb[0] == 1:      
      print 'dev_init: leaving OK'
   return locname,Tab_dev[locname]['cobj']

#--------------------------------------------------------------
def dev_initC(mdevname):
#  input:
#     mdevname: device name
#
#  returns list:
#	- [] if error
#
#  	- [devname,cpt]
#     		devname: mdevname in lowercase
#     		cpt:  index in C device table
#--------------------------------------------------------------
   print 'dev_initC: ' + mdevname
   print Dev_deb[0]
   locname=lower(mdevname)

#  try to find in Tab_devC list if name already
#  found
   
   if Tab_devC.has_key(locname):
      print "device already found in Tab_devC"
      Tab_devC[locname]['ref'] = Tab_devC[locname]['ref'] + 1

   else:
#     have to import the device and create place in dict   
      try :
         mpt = esrf_dc_import(locname)
      except error:
#         print error
#         print "dev_initC: error on importing device %s" % locname
         raise Dev_Exception,sys.exc_value
         return []

#     create in Tab_devC 
      Tab_devC[locname] = {}
      Tab_devC[locname]['cobj'] = mpt
      Tab_devC[locname]['ref'] = 1
      Tab_devC[locname]['cmd'] = dev_queryC(locname)

   if Dev_deb[0] == 1:      
      print 'dev_initC: leaving OK'
   return locname,Tab_devC[locname]['cobj']



#--------------------------------------------------------------
def dev_unref(mdevname):
#
# decrement reference to that object in Tab_dev table
# If reference becomes 0, calls the C dev_free routines:
#
#  input:
#     mdevname: device name
#
#  returns:
#     0 : error
#     1 : OK
#
#--------------------------------------------------------------
   if Tab_dev[mdevname]['ref'] <= 0:
      print 'reference is <0 ???'
      return 0
   else:
      Tab_dev[mdevname]['ref']=Tab_dev[mdevname]['ref']-1
      if Tab_dev[mdevname]['ref'] == 0 :
         print ' *** calling C dev_free routine'
         del Tab_dev[mdevname] 
   return 1

#--------------------------------------------------------------
def dev_unrefC(mdevname):
#
# decrement reference to that object in Tab_devC table
# If reference becomes 0, calls the C dev_free routines:
#
#  input:
#     mdevname: device name
#
#  returns:
#     0 : error
#     1 : OK
#
#--------------------------------------------------------------
   if Tab_devC[mdevname]['ref'] <= 0:
      print 'reference is <0 ???'
      return 0
   else:
      Tab_devC[mdevname]['ref']=Tab_devC[mdevname]['ref']-1
      if Tab_devC[mdevname]['ref'] == 0 :
         print ' *** calling C dc_free routine'
         del Tab_devC[mdevname] 
   return 1 
         
#--------------------------------------------------------------
def dev_query(mdevname):
#   input:
#      mdevname: device name in lower case
#
#   returns dictionnary:
#	- {} if error
#
#  	-{cmd_name:[cmd,in_type,out_type], ...}
#	        cmd_name: command string
#     		cmd: command numeric value
#		in_type: input type
#		out_type: output type
#--------------------------------------------------------------
   if Tab_dev.has_key(mdevname):
      loc_c_pt = Tab_dev[mdevname]['cobj']
#      print loc_c_pt
#      print Tab_dev[mdevname]['cobj']
      try:
         locdict = esrf_query(loc_c_pt)
      except error:
#         print 
#         print "dev_query: error on query for device %s" % mdevname
         raise Dev_Exception,sys.exc_value
	 return {}
   else:
      print "dev_query: no device %s defined" % mdevname
      return {}
      
   return locdict

#--------------------------------------------------------------
def dev_queryC(mdevname):
#   input:
#      mdevname: device name in lower case
#
#   returns dictionnary:
#	- {} if error
#
#  	-{cmd_name:[cmd,out_type], ...}
#	        cmd_name: command string
#     		cmd: command numeric value
#		out_type: output type
#--------------------------------------------------------------
   try:
      locdict = esrf_dc_info(mdevname)
   except error:
#      print 
#      print "dev_queryC: error on query for device %s" % mdevname
      raise Dev_Exception,sys.exc_value
      return {}
      
   return locdict
   
#--------------------------------------------------------------
def dev_tcpudp(mdevname,mode):
#   input:
#      	mdevname: device name in lower case
#	mode: "tcp" or "udp"
#
#   returns value:
#	- 0 if error
#  	- 1 if OK
#--------------------------------------------------------------
   if Tab_dev.has_key(mdevname):
      loc_c_pt = Tab_dev[mdevname]['cobj']
      if Dev_deb[0] == 1:
         print loc_c_pt
      if (mode != "tcp") and (mode != "udp"):
         print 'usage: dev_tcpudp(<device_name>,udp|tcp)'
	 return 0
      try:
         ret = esrf_tcpudp(loc_c_pt,mode)
      except error:
#         print "dev_tcpudp: error on esrf_tcpudp for device %s" % mdevname
         raise Dev_Exception,sys.exc_value
	 return 0
   else:
      print "dev_tcpudp: no device %s defined" % mdevname
      return 0
      
   return 1

#--------------------------------------------------------------
def dev_timeout(mdevname,*mtime):
#   input:
#      	mdevname: device name in lower case
#	mtime: optional argument:
#		- if not existing: read timeout required
#		- if exists: time in  second for setting timeout
#
#   returns value:
#	- 0 if error
#	- time in sec (read or set) if OK
#--------------------------------------------------------------
   if Tab_dev.has_key(mdevname):
      loc_c_pt = Tab_dev[mdevname]['cobj']
      if Dev_deb[0] == 1:
         print loc_c_pt
      if (mtime == ()):
         print 'dev_timeout readmode '
	 try:
	    ret = esrf_timeout(loc_c_pt)
	 except error:
#	    print 'error on esrf_timeout for device ' + mdevname
            raise Dev_Exception,sys.exc_value
	    return 0
	 return ret
      else:
         itime = mtime[0]
	 if (type(itime) == type(0)) or (type(itime) == type(2.4)):
            print 'dev_timeout set mode %f' % itime
	    try:
	       ret = esrf_timeout(loc_c_pt,itime)
	    except:
#	       print 'error on esrf_timeout for device ' + mdevname
               raise Dev_Exception,sys.exc_value
	       return 0	    
	    return ret
   else:
      print "dev_timeout: no device %s defined" % mdevname
      return 0
#--------------------------------------------------------------
def dev_getresource(mdevname,resname):
#   input:
#      	mdevname: device name 
#	resname:  resource name
#
#   returns value packed as a string:
#	- resource value if OK
#	- None if error
#--------------------------------------------------------------
   try:
      ret = esrf_getresource(mdevname,resname)
   except error:
#      print "dev_getresource: error for device %s on resource %s" % (mdevname,resname)
      raise Dev_Exception,sys.exc_value
      return None
   if (Dev_deb[0] == 1):
     print "string length is %s" % len(ret)
   return ret

#--------------------------------------------------------------
def dev_putresource(mdevname,resname,value):
#   Sets a device resource
#   input:
#      	mdevname: device name 
#	resname:  resource name
#	value: the resource value packed as a string
#
#   returns value:
#	- 1: if OK
#	- 0: if error
#--------------------------------------------------------------
   if (type(value) != type("a")):
      print "dev_putresource: resource value must be packed as string"
   try:
      ret = esrf_putresource(mdevname,resname,value)
   except error:
#      print "dev_putresource: error for device %s on resource %s" % (mdevname,resname)
      raise Dev_Exception,sys.exc_value
      return 0
   return 1
   
#--------------------------------------------------------------
def dev_delresource(mdevname,resname):
#   removes a device resource
#   input:
#      	mdevname: device name 
#	resname:  resource name
#
#   returns value:
#	- 1: OK
#	- 0: error
#--------------------------------------------------------------
   try:
      ret = esrf_delresource(mdevname,resname)
   except error:
#      print "dev_delresource: error for device %s on resource %s" % (mdevname,resname)
      raise Dev_Exception,sys.exc_value
      return 0
   return 1
   
      
#--------------------------------------------------------------
def dev_io(mdevname,mdevcommand,*parin,**kw):
#   input:
#      	mdevname: device name in lower case
#	parin: list of optional INPUT parameters
#	kw: dictionnary of optional OUTPUT parameters
#
#   returns value:
#	- 1 : if no device ARGOUT or OUTPUT param provided
#	- device ARGOUT : if device ARGOUT and no OUTPUT param
#
#   in case of error, global variable DEV_ERR is set to 1
#--------------------------------------------------------------
   if Dev_deb[0] == 1:
      print 'in dev_io'
   if Tab_dev.has_key(mdevname):
      loc_c_pt = Tab_dev[mdevname]['cobj']
      if Dev_deb[0] == 1:
         print loc_c_pt
         print '  devname : ' + mdevname
         print '  command : ' + mdevcommand
         print '  parin : %s' % (parin,)
         print '  kw : %s' % (kw,)
#  check now that parameters are correct

#  then run the esrf_io commnd
#  first check that command exists:
      if Tab_dev[mdevname]['cmd'].has_key(mdevcommand):
         io_cmd = Tab_dev[mdevname]['cmd'][mdevcommand][0]
         io_in  = Tab_dev[mdevname]['cmd'][mdevcommand][1]
         io_out = Tab_dev[mdevname]['cmd'][mdevcommand][2]
         parin3 = parin
	 if (len(parin) == 1):
	    if type(parin[0])== type((2,3)):
	       parin3 = parin[0]
	    if type(parin[0]) == type([2,3]):
	       parin3 = tuple(parin[0])
         parin2 = (Tab_dev[mdevname]['cobj'],mdevcommand,io_cmd,io_in,io_out,0) + (parin3,)
         if Dev_deb[0] == 1:
	    print 'esrf_io arg:'
	    print parin2
	    print 'esrf_io dict:'
	    print kw
	 ret = None
	 try:
#	    kw={'out':2}
#	    kw = {}
            ret = apply(esrf_io,parin2,kw)
	 except error:
#	    print "esrf_io: error on device %s" % mdevname
            raise Dev_Exception,sys.exc_value
	 return ret
      else:
         print "dev_io: no command %s for device %s" % (mdevcommand,mdevname)
	 
   else:
      print "dev_io: no device %s defined" % mdevname
   
# just for test
def my_esrf_io(name,command,*argin,**kw):
   print 'my_esrf_io: %s %s' % (name,command)   
   print '  input: %s' % (argin,)
   print '  output: %s' % (kw,)

#--------------------------------------------------------------
def dev_ioC(mdevname,mdevcommand,**kw):
#   input:
#      	mdevname: device name in lower case
#	kw: dictionnary of optional OUTPUT parameters
#
#   returns value:
#	- 1 : if no device ARGOUT or OUTPUT param provided
#	- device ARGOUT : if device ARGOUT and no OUTPUT param
#
#   in case of error, global variable DEV_ERR is set to 1
#--------------------------------------------------------------
   if Dev_deb[0] == 1:
      print 'in dev_ioC'
   if Tab_devC.has_key(mdevname):
      loc_c_pt = Tab_devC[mdevname]['cobj']
      if Dev_deb[0] == 1:
         print loc_c_pt
         print '  devname : ' + mdevname
         print '  command : ' + mdevcommand
         print '  kw : %s' % (kw,)
#  check now that parameters are correct

#  then run the esrf_io commnd
#  first check that command exists:
      if Tab_devC[mdevname]['cmd'].has_key(mdevcommand):
         io_cmd = Tab_devC[mdevname]['cmd'][mdevcommand][0]
         io_in  = 0	# void
         io_out = Tab_devC[mdevname]['cmd'][mdevcommand][1]
         parin2 = (Tab_devC[mdevname]['cobj'],mdevcommand,io_cmd,io_in,io_out,1) + ((),)
         if Dev_deb[0] == 1:
	    print 'esrf_io arg:'
	    print parin2
	    print 'esrf_io dict:'
	    print kw
	 ret = None
	 try:
#	    kw={'out':2}
#	    kw = {}
            ret = apply(esrf_io,parin2,kw)
	    
            if Dev_deb[0] == 1:
	    	print 'returned from esrf_io: %s' % ret	    
	 except error:
#	    print "esrf_ioC: error on device %s" % mdevname
            raise Dev_Exception,sys.exc_value
	 return ret
      else:
         print "dev_ioC: no command %s for device %s" % (mdevcommand,mdevname)
	 
   else:
      print "dev_ioC: no device %s defined" % mdevname
   

   
#--------------------------------------------------------------
class TacoDevice:
#  class attributes:
#       devname                         # device name
#       ds_object                       # C ds object pointer
#	imported			# 1 if succeed, else 0
#	command				# set when device command

   print 'creating class TacoDevice'        # executed when imported

   def __init__(self,name):             # constructor     
      print 'Welcome ' + name
      self.devname = name
      self.imported = 0
#     import device
      Listout = dev_init(name)
      if Listout != []:
         self.devname = Listout[0]
         self.ds_object = Listout[1] 
         if Dev_deb[0] == 1:
            print self.devname
	 self.imported = 1         	 
      else:
         print 'error on init'
          
	 
   def __del__(self):                   # destructor
      print 'Goodbye ' + self.devname
      if self.imported == 1:
         ret = dev_unref(self.devname) 
	 if ret == 0:
	    print 'error deleting object ' + self.devname
      
   def __str__(self):			# for print
      print 'ds device:         ' + self.devname
      print " imported: %d" % self.imported
      print " device object : " 
      print self.ds_object
      return "0"

   def CommandList(self):
      locdict = dev_query(self.devname)
      if locdict != {}:
         print Tab_dev_head
         for mykey in locdict.keys():
	    my_stringtype_in = "%d" % locdict[mykey][1]
	    my_stringtype_out = "%d" % locdict[mykey][2]
	    if Tab_dev_type.has_key(my_stringtype_in):
	       myin = Tab_dev_type[my_stringtype_in]
	    else:
	       myin = Tab_dev_type_unk
	    if Tab_dev_type.has_key(my_stringtype_out):
	       myout = Tab_dev_type[my_stringtype_out]
	    else:
	       myout = Tab_dev_type_unk	     
	    print "%s %s %s" % (myin,myout,mykey)
      return None

   def tcp(self):
      if self.imported == 1:
         ret = dev_tcpudp(self.devname,"tcp")
	 if ret ==0:
	    print 'error setting tcp on object' + self.devname
	    
   def udp(self):
      if self.imported == 1:
         ret = dev_tcpudp(self.devname,"udp")
	 if ret ==0:
	    print 'error setting udp on object' + self.devname
      
   def timeout(self,*mtime):
      if self.imported == 1:
         if mtime == ():
	    ret = dev_timeout(self.devname)
	    if Dev_deb[0] == 1:
	       print 'timeout: %f' % ret
	    return ret
	 else:
	    ret = dev_timeout(self.devname,mtime[0])
	    return ret

   def __getattr__(self,name):
      self.command = name
      if Dev_deb[0] == 1:
         print name
      return self.device_io
      
   def device_io(self,*par,**kw):
      if Dev_deb[0] == 1:
         print 'in device_io'
      parin = (self.devname,self.command) + par
      if Dev_deb[0] == 1:
         print parin
         print kw
      ret = apply(dev_io,parin,kw)
      return ret


#--------------------------------------------------------------
class TacoDeviceC:
#  class attributes:
#       devname                         # device name
#       dc_object                       # C dc object pointer
#	imported			# 1 if succeed, else 0
#	command				# set when device command

   print 'creating class TacoDeviceC'       # executed when imported

   def __init__(self,name):             # constructor     
      print 'Welcome ' + name
      self.devname = name
      self.imported = 0
#     import device
      Listout = dev_initC(name)
      if Listout != []:
         self.devname = Listout[0]
         self.dc_object = Listout[1] 
         if Dev_deb[0] == 1:
            print self.devname
	 self.imported = 1         	 
      else:
         print 'error on init'
          
	 
   def __del__(self):                   # destructor
      print 'Goodbye ' + self.devname
      if self.imported == 1:
         ret = dev_unrefC(self.devname) 
	 if ret == 0:
	    print 'error deleting object ' + self.devname
      
   def __str__(self):			# for print 
   					# (__repr__ does not work!!)
      print 'dc device:         ' + self.devname
      print " imported: %d" % self.imported
      print " device object : " 
      print self.dc_object
      return "0"
      
   def CommandList(self):
      locdict = dev_queryC(self.devname)
      if locdict != {}:
         print Tab_devC_head
         for mykey in locdict.keys():
	    my_stringtype_out = "%d" % locdict[mykey][1]
	    if Tab_dev_type.has_key(my_stringtype_out):
	       myout = Tab_dev_type[my_stringtype_out]
	    else:
	       myout = Tab_dev_type_unk	     
	    print "%s %s" % (myout,mykey)
      return None
      
   def __getattr__(self,name):
      self.command = name
      if Dev_deb[0] == 1:
         print name
      return self.device_io
      
   def device_io(self,*par,**kw):
      if Dev_deb[0] == 1:
         print 'in device_io'
      parin = (self.devname,self.command) +()
      if Dev_deb[0] == 1:
         print parin
         print kw
      ret = apply(dev_ioC,parin,kw)
      return ret      
