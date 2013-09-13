#!/usr/bin/env python
#
# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright(c) 1994-2013 by European Synchrotron Radiation Facility,
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
# File:         TacoDevice.py
#
# Project:      Automatic Beamline Alignment
#
# Description:  Generic interface class to the TACO database.
#
# Author(s):    J.Meyer
#               $Author: jkrueger1 $
#
# Original:     Mai 2001
#
# Version:      $Revision: 1.5 $
#
# Date:         $Date: 2008-04-06 09:06:26 $
#

""" Python interface for ... """

__revision__ = "$Revision: 1.5 $"
__date__ = "$Date: 2008-04-06 09:06:26 $"
__author__ = "$Author: jkrueger1 $"

import Taco 
import sys
import string

Dev_deb = [0]
Dev_Exception = "TacoException"

#--------------------------------------------------------------
# Tab_dev: 
# dictionary indexed with devices names (in lower case)
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
# dictionary indexed with data collector devices names (in lower case)
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
Tab_dev_type = {'0' :'void                         ',
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
Tab_dev_head  = 'INPUT:                        OUTPUT:                       COMMAND:'
Tab_devC_head = 'OUTPUT:                       COMMAND:'

def dev_debug(flag):
    """
    @param flag debug flag, 0 no trace, else trace
    @return 0 error, 1 OK
    """
    if type(flag) != type(1) :
        print "dev_debug: parameter not a number"
    else :
        Dev_deb[0] = flag
        if Dev_deb[0] != 0 :
            print " debug mode %d" % Dev_deb[0]

def dev_init(mdevname):
    """
    @param mdevname device name in lowercase
    @returns list [] if error, [devname, cpt] index in C device table
    """
    if Dev_deb[0] == 1:      
        'dev_init: ' + mdevname
    locname = string.lower(mdevname)

# try to find in Tab_dev list if name already found
    if Tab_dev.has_key(locname):
        print "device already found in Tab_dev"
        Tab_dev[locname]['ref'] = Tab_dev[locname]['ref'] + 1

    else:
# have to import the device and create place in dict   
        try :
            mpt = Taco.esrf_import(locname)
# create in Tab_dev 
            Tab_dev[locname] = {}
            Tab_dev[locname]['cobj'] = mpt
            Tab_dev[locname]['ref'] = 1
            Tab_dev[locname]['cmd'] = dev_query(locname)
        except Taco.error:
            raise Dev_Exception, sys.exc_value

    if Dev_deb[0] == 1:      
        print 'dev_init: leaving OK'
    return locname, Tab_dev[locname]['cobj']

def dev_initC(mdevname):
    """
   input:
      mdevname: device name
 
   returns list:
 	- [] if error
 
   	- [devname,cpt]
      		devname: mdevname in lowercase
      		cpt:  index in C device table
    """
    if Dev_deb[0] == 1:      
        print 'dev_initC: ' + mdevname
    locname = string.lower(mdevname)

#  try to find in Tab_devC list if name already found
    if Tab_devC.has_key(locname):
        print "device already found in Tab_devC"
        Tab_devC[locname]['ref'] = Tab_devC[locname]['ref'] + 1

    else:
# have to import the device and create place in dict   
        try :
            mpt = Taco.esrf_dc_import(locname)
# create in Tab_devC 
            Tab_devC[locname] = {}
            Tab_devC[locname]['cobj'] = mpt
            Tab_devC[locname]['ref'] = 1
            Tab_devC[locname]['cmd'] = dev_queryC(locname)
        except Taco.error:
            raise Dev_Exception, sys.exc_value

    if Dev_deb[0] == 1:      
        print 'dev_initC: leaving OK'
    return locname, Tab_devC[locname]['cobj']


def dev_unref(mdevname):
    """
    decrement reference to that object in Tab_dev table
    If reference becomes 0, calls the C dev_free routines:
 
   input:
      mdevname: device name
 
   returns:
      0 : error
      1 : OK
 
    """
    if Tab_dev[mdevname]['ref'] <= 0:
        print 'reference is <0 ???'
        return 0
    else:
        Tab_dev[mdevname]['ref'] = Tab_dev[mdevname]['ref'] - 1
        if Tab_dev[mdevname]['ref'] == 0 :
            print ' *** calling C dev_free routine'
            del Tab_dev[mdevname] 
    return 1

def dev_unrefC(mdevname):
    """
    decrement reference to that object in Tab_devC table
    If reference becomes 0, calls the C dev_free routines:
 
   input:
      mdevname: device name
 
   returns:
      0 : error
      1 : OK
    """
    if Tab_devC[mdevname]['ref'] <= 0:
        print 'reference is <0 ???'
        return 0
    else:
        Tab_devC[mdevname]['ref'] = Tab_devC[mdevname]['ref'] - 1
        if Tab_devC[mdevname]['ref'] == 0 :
            print ' *** calling C dc_free routine'
            del Tab_devC[mdevname] 
    return 1 
         
def dev_query(mdevname):
    """
    input:
       mdevname: device name in lower case
 
    returns dictionary:
 	- {} if error
 
   	-{cmd_name:[cmd,in_type,out_type], ...}
 	        cmd_name: command string
      		cmd: command numeric value
 		in_type: input type
 		out_type: output type
    """
    if Tab_dev.has_key(mdevname):
        loc_c_pt = Tab_dev[mdevname]['cobj']
        try:
            locdict = Taco.esrf_query(loc_c_pt)
            return locdict
        except Taco.error:
            raise Dev_Exception, sys.exc_value
    else:
        print "dev_query: no device %s defined" % mdevname
        return None
      

def dev_queryC(mdevname):
    """
    input:
       mdevname: device name in lower case
 
    returns dictionary:
 	- {} if error
 
   	-{cmd_name:[cmd,out_type], ...}
 	        cmd_name: command string
      		cmd: command numeric value
 		out_type: output type
    """
    try:
        locdict = Taco.esrf_dc_info(mdevname)
        return locdict
    except Taco.error:
        raise Dev_Exception, sys.exc_value
   
def dev_tcpudp(mdevname, mode):
    """
    input:
       	mdevname: device name in lower case
 	mode: "tcp" or "udp"
 
    returns value:
 	- 0 if error
   	- 1 if OK
    """
    if Tab_dev.has_key(mdevname):
        loc_c_pt = Tab_dev[mdevname]['cobj']
        if Dev_deb[0] == 1:
            print loc_c_pt
        if (mode != "tcp") and (mode != "udp"):
            print 'usage: dev_tcpudp(<device_name>,udp|tcp)'
            return 0
        try:
            return Taco.esrf_tcpudp(loc_c_pt, mode)
        except Taco.error:
            raise Dev_Exception, sys.exc_value
    else:
        print "dev_tcpudp: no device %s defined" % mdevname
    return 0
      

def dev_timeout(mdevname, *mtime):
    """
    input:
       	mdevname: device name in lower case
 	mtime: optional argument:
 		- if not existing: read timeout required
 		- if exists: time in  second for setting timeout
 
    returns value:
 	- 0 if error
 	- time in sec (read or set) if OK
    """
    if Tab_dev.has_key(mdevname):
        loc_c_pt = Tab_dev[mdevname]['cobj']
        if Dev_deb[0] == 1:
            print loc_c_pt
        if (mtime == ()):
            print 'dev_timeout readmode '
            try:
                return Taco.esrf_timeout(loc_c_pt)
            except Taco.error:
                raise Dev_Exception, sys.exc_value
        else:
            itime = mtime[0]
            if (type(itime) == type(0)) or (type(itime) == type(2.4)):
                print 'dev_timeout set mode %f' % itime
                try:
                    return Taco.esrf_timeout(loc_c_pt, itime)
                except:
                    raise Dev_Exception, sys.exc_value
    else:
        print "dev_timeout: no device %s defined" % mdevname
    return 0

def dev_getresource(mdevname, resname):
    """
    input:
       	mdevname: device name 
 	resname:  resource name
 
    returns value packed as a string:
 	- resource value if OK
 	- None if error
    """
    try:
        ret = Taco.esrf_getresource(mdevname, resname)
        if (Dev_deb[0] == 1):
            print "string length is %s" % len(ret)
        return ret
    except Taco.error:
        raise Dev_Exception, sys.exc_value

def dev_putresource(mdevname, resname, value):
    """
    Sets a device resource
    input:
       	mdevname: device name 
 	resname:  resource name
 	value: the resource value packed as a string
 
    returns value:
 	- 1: if OK
 	- 0: if error
    """
    if type(value) != type("a"):
        print "dev_putresource: resource value must be packed as string"
    try:
        return Taco.esrf_putresource(mdevname, resname, value)
    except Taco.error:
        raise Dev_Exception, sys.exc_value
   
def dev_delresource(mdevname, resname):
    """
    removes a device resource
    input:
       	mdevname: device name 
 	resname:  resource name
 
    returns value:
 	- 1: OK
 	- 0: error
    """
    try:
        return Taco.esrf_delresource(mdevname, resname)
    except Taco.error:
        raise Dev_Exception, sys.exc_value
   
      
def dev_io(mdevname, mdevcommand, *parin, **keyword):
    """
    input:
       	mdevname: device name in lower case
 	parin: list of optional INPUT parameters
 	keyword: dictionary of optional OUTPUT parameters
 
    returns value:
 	- 1 : if no device ARGOUT or OUTPUT param provided
 	- device ARGOUT : if device ARGOUT and no OUTPUT param
 
    in case of error, global variable DEV_ERR is set to 1
    """
    if Dev_deb[0] == 1:
        print 'in dev_io'
    if Tab_dev.has_key(mdevname):
        loc_c_pt = Tab_dev[mdevname]['cobj']
        if Dev_deb[0] == 1:
            print loc_c_pt
            print '  devname : ' + mdevname
            print '  command : ' + mdevcommand
            print '  parin : %s' % (parin,)
            print '  kw : %s' % (keyword,)
#  check now that parameters are correct
#  then run the esrf_io commnd
#  first check that command exists:
        if Tab_dev[mdevname]['cmd'].has_key(mdevcommand):
            io_cmd = Tab_dev[mdevname]['cmd'][mdevcommand][0]
            io_in  = Tab_dev[mdevname]['cmd'][mdevcommand][1]
            io_out = Tab_dev[mdevname]['cmd'][mdevcommand][2]
            parin3 = parin
            if (len(parin) == 1):
                if type(parin[0])== type((2, 3)):
                    parin3 = parin[0]
                if type(parin[0]) == type([2, 3]):
                    parin3 = tuple(parin[0])
            parin2 = (Tab_dev[mdevname]['cobj'], mdevcommand, io_cmd, io_in, io_out, 0) + (parin3,)
            if Dev_deb[0] == 1:
                print 'esrf_io arg:'
                print parin2
                print 'esrf_io dict:'
                print keyword
            try:
                return apply(Taco.esrf_io, parin2, keyword)
            except Taco.error:
                raise Dev_Exception, sys.exc_value
        else:
            print "dev_io: no command %s for device %s" % (mdevcommand, mdevname)
    else:
        print "dev_io: no device %s defined" % mdevname
   
def my_esrf_io(name, command, *argin, **keyword):
    """ just for test """
    print 'my_esrf_io: %s %s' % (name, command)   
    print '  input: %s' % (argin,)
    print '  output: %s' % (keyword,)

def dev_ioC(mdevname, mdevcommand, **keyword):
    """
    input:
       	mdevname: device name in lower case
 	keyword: dictionary of optional OUTPUT parameters
 
    returns value:
 	- 1 : if no device ARGOUT or OUTPUT param provided
 	- device ARGOUT : if device ARGOUT and no OUTPUT param
 
    in case of error, global variable DEV_ERR is set to 1
    """
    if Dev_deb[0] == 1:
        print 'in dev_ioC'
    if Tab_devC.has_key(mdevname):
        loc_c_pt = Tab_devC[mdevname]['cobj']
        if Dev_deb[0] == 1:
            print loc_c_pt
            print '  devname : ' + mdevname
            print '  command : ' + mdevcommand
            print '  kw : %s' % (keyword,)
#  check now that parameters are correct
#  then run the esrf_io commnd
#  first check that command exists:
        if Tab_devC[mdevname]['cmd'].has_key(mdevcommand):
            io_cmd = Tab_devC[mdevname]['cmd'][mdevcommand][0]
            io_in  = 0	# void
            io_out = Tab_devC[mdevname]['cmd'][mdevcommand][1]
            parin2 = (Tab_devC[mdevname]['cobj'], mdevcommand, io_cmd, io_in, io_out, 1) + ((),)
            if Dev_deb[0] == 1:
                print 'esrf_io arg:'
                print parin2
                print 'esrf_io dict:'
                print keyword
            try:
                ret = apply(Taco.esrf_io, parin2, keyword)
                if Dev_deb[0] == 1:
                    print 'returned from esrf_io: %s' % ret	    
                return ret
            except Taco.error:
                raise Dev_Exception, sys.exc_value
        else:
            print "dev_ioC: no command %s for device %s" % (mdevcommand, mdevname)
    else:
        print "dev_ioC: no device %s defined" % mdevname

   
class TacoDevice:
    """
    class attributes:
       devname                         # device name
       ds_object                       # C ds object pointer
	imported			# 1 if succeed, else 0
	command				# set when device command
    """
#    print 'creating class TacoDevice'    # executed when imported

    def __init__(self, name):             
        """ constructor """
        print 'Welcome ' + name
        self.devname = name
        self.imported = 0
        Listout = dev_init(name)
        if Listout != []:
            self.devname = Listout[0]
            self.ds_object = Listout[1] 
            if Dev_deb[0] == 1:
                print self.devname
            self.imported = 1         	 
        else:
            print 'error on init'
	 
    def __del__(self):                   
        """ destructor """
        print 'Goodbye ' + self.devname
        if self.imported == 1:
            if dev_unref(self.devname) == 0 :
                print 'error deleting object ' + self.devname
      
    def __str__(self):			
        """ for print """
        print 'ds device:         ' + self.devname
        print " imported: %d" % self.imported
        print " device object : " 
        print self.ds_object
        return "0"

    def CommandList(self):
        """ print out the command list and the parameter's of the commands """
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
                print "%s %s %s" % (myin, myout, mykey)
        return None

    def tcp(self):
        """ Sets the communication to TCP """
        if self.imported == 1:
            if dev_tcpudp(self.devname,"tcp") == 0:
                print 'error setting tcp on object' + self.devname
	    
    def udp(self):
        """ Sets the communication to TCP """
        if self.imported == 1:
            if dev_tcpudp(self.devname,"udp") == 0:
                print 'error setting udp on object' + self.devname
      
    def timeout(self, *mtime):
        """ Sets the communication time out value """
        if self.imported == 1:
            if mtime == ():
                ret = dev_timeout(self.devname)
            else:
                ret = dev_timeout(self.devname, mtime[0])
            if Dev_deb[0] == 1:
                print 'timeout: %f' % ret
            return ret
        else :
            return None

    def __getattr__(self, name):
        """ overloaded function """
        self.command = name
        if Dev_deb[0] == 1:
            print name
        return self.device_io
      
    def device_io(self, *par, **keyw):
        """ execution of a TACO command """
        if Dev_deb[0] == 1:
            print 'in device_io'
        parin = (self.devname, self.command) + par
        if Dev_deb[0] == 1:
            print parin
            print keyw
        return apply(dev_io, parin, keyw)


class TacoDeviceC:
    """
    class attributes:
        devname                         # device name
        dc_object                       # C dc object pointer
 	imported			# 1 if succeed, else 0
 	command				# set when device command
    """
#    print 'creating class TacoDeviceC'       # executed when imported

    def __init__(self, name):
        """ constructor    """
        print 'Welcome ' + name
        self.devname = name
        self.imported = 0

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
            if dev_unrefC(self.devname) == 0:
                print 'error deleting object ' + self.devname
      
    def __str__(self):			
        """ for print 
            (__repr__ does not work!!)
        """
        print 'dc device:         ' + self.devname
        print " imported: %d" % self.imported
        print " device object : " 
        print self.dc_object
        return "0"
      
    def CommandList(self):
        """ print outs the command list and the parameter for each command """
        locdict = dev_queryC(self.devname)
        if locdict != {}:
            print Tab_devC_head
            for mykey in locdict.keys():
                my_stringtype_out = "%d" % locdict[mykey][1]
                if Tab_dev_type.has_key(my_stringtype_out):
                    myout = Tab_dev_type[my_stringtype_out]
                else:
                    myout = Tab_dev_type_unk	     
                print "%s %s" % (myout, mykey)
        return None
      
    def __getattr__(self, name):
        """
        overloaded function
        """
        self.command = name
        if Dev_deb[0] == 1:
            print name
        return self.device_io
      
    def device_io(self, *par, **keyw):
        """ executes a TACO command """
        if Dev_deb[0] == 1:
            print 'in device_io'
        parin = (self.devname, self.command) + ()
        if Dev_deb[0] == 1:
            print parin
            print keyw
        return apply(dev_ioC, parin, keyw)
