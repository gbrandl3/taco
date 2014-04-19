#/*+*******************************************************************
#
# File:          sec_auth.res
#
# Project:       ESRF Security Authentication Resources
#
# Description:   Resource file for the security system.
#
# Autor(s):      Jens Meyer
#		 $Author: jkrueger1 $
#
# Original:      September 1993
#
# Version:	 $Revision: 1.4 $
#
# Date:		 $Date: 2008-04-06 09:07:39 $
#
# Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
#		Grenoble, France
#
#********************************************************************-*/
#
# default access right, if no user or group entry can be found.
#
SEC/MINIMAL/ACC_RIGHT/default:		READ_ACCESS,	192.168.1,172.25.18

#
# user identification information
#               taco-name     		uid,    	IP(subnet or single computer)
# SEC/USER/IDENT/pedersen:     		500, 		172.25.18
# SEC/USER/IDENT/dserver:		1000,		192.168.1

#
# group identification information
#					gid		IP(subnet or single computer)
# SEC/GROUP/IDENT/dserver:		200,		192.168.1,172.25.18

#
# group/user resources to access for a domain 
#
# SEC/GROUP/ACC_RIGHT/test: 		dserver,	WRITE_ACCESS
# SEC/USER/ACC_RIGHT/test: 		pedersen,	ADMIN_ACCESS 

#
# group/user resources to access for a family in a domain
#
# SEC/GROUP/ACC_RIGHT/test|test: 	dserver,	WRITE_ACCESS
# SEC/USER/ACC_RIGHT/test|test:  	pedersen,	ADMIN_ACCESS

#
# group/user resources to access to a device
#
# SEC/GROUP/ACC_RIGHT/test|test|dev: 	dserver,	WRITE_ACCESS
# SEC/USER/ACC_RIGHT/test|test|dev:  	pedersen,	WRITE_ACCESS

