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
# Description:  Generic interface class to the TACO database.
#
# Author(s):    J.Meyer
#               $Author: jkrueger1 $
#
# Original:     Mai 2001
#
# Version:      $Revision: 1.3 $
#
# Date:         $Date: 2005-07-25 13:43:42 $
#

""" TACO demo server class """

__author__ = "$Author: jkrueger1 $"
__date__ = "$Date: 2005-07-25 13:43:42 $"
__revision__ = "$Revision: 1.3 $"

import TacoServer
from DEVCMDS import *
from DEV_XDR import *

from TacoDevice import Dev_Exception, dev_getresource

class YourServer (TacoServer.TacoServer) :
    """
    This is another test class for a TACO device
    """
# Command list
    cmd_list = { DevState : [D_VOID_TYPE, D_SHORT_TYPE, 'state', 'DevState'],
                 DevStatus: [D_VOID_TYPE, D_STRING_TYPE, 'status', 'DevStatus'],
                 DevOpen  : [D_VOID_TYPE, D_VOID_TYPE, 'open', 'DevOpen'],
                 DevClose : [D_VOID_TYPE, D_VOID_TYPE, 'close', 'DevClose'],
                 DevReadSigValues : [D_VOID_TYPE, D_VAR_FLOATARR, 'read_signals', 'DevReadSigValues']}
    
# Common variables for a class
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
        
