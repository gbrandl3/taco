#!/usr/bin/env python
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
# File:         TacoServer.py
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
# Version:      $Revision: 1.6 $
#
# Date:         $Date: 2008-04-06 09:06:31 $
#

""" TACO device server """

__author__ = "Jens Meyer, $Author: jkrueger1 $"
__date__ = "$Date: 2008-04-06 09:06:31 $"
__revision__ = "$Revision: 1.6 $"

import thread
import Server
import DEVCMDS
import DEVSTATES 

from TacoDevice import *

from DEVCMDS   import *
from DEV_XDR   import *
from DEVSTATES import *
from DEVERRORS import *
from BLCDSNUMBERS import *


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

class TacoServer:
    """
    This class is the Taco Python super class. All classes used in Python as TACO devices
    should be derived from this class.
    """
	
# Common variables for a class
    cmd_list = { 
#       DEVCMDS.DevState : [DEVCMDS.D_VOID_TYPE, DEVCMDS.D_SHORT_TYPE , 'DevState', 'DevState'],
#       DEVCMDS.DevStatus:[DEVCMDS.D_VOID_TYPE, DEVCMDS.D_STRING_TYPE, 'DevStatus','DevStatus'],
       DevState : [D_VOID_TYPE, D_SHORT_TYPE , 'DevState', 'DevState'],
       DevStatus:[D_VOID_TYPE, D_STRING_TYPE, 'DevStatus','DevStatus'],
    }

    class_name = "PythonClass"
	
# Variables for an object
    dev_name    = "NONE"
    dev_state   = DEVSTATES.DEVUNKNOWN
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

    def cmd_io (self, *par, **keyw):
        """
        Checks in the module DEVCMD for a valid entry a command name.
        If an entry was found, get its integer value.
        Searches with the integer value in the command list to get
        the object methode to execute.
        Execute the correct methode for the requested command name.
        @param par
        @param keyw

        @return 
        """
        cmd = DEVCMDS.__dict__[self.command_name]
        cmd_list_values = self.cmd_list[cmd]
        method_name = cmd_list_values[2]
		
        return apply (getattr (self, method_name), par, keyw)

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

def server_startup (devices, server_name = 'test', process_name = 'Python', nodb = 0, procnum = 0):
    """
    This is the main function for server startup
	
    @param devices tuple of the created devices should be served by the server
    @param server_name the personal name of the server. It should be different 
                  from others
    @param process_name the name of the process itself.
    @param nodb if this parameter is set to 1 the server will work without the 
                  TACO database
    @param pn if this parameter is differen from 0 the server will try to use 
                  this number as RPC program number. If this number is in use 
                  please try another. Normally there is no need to create your 
                  own number.
    """

    if nodb == 0:
        thread.start_new_thread (Server.startup, (process_name, server_name, devices))
    else:
        thread.start_new_thread (Server.startup_nodb, (process_name, server_name, devices, pn))

