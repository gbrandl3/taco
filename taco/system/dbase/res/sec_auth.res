#/*+*******************************************************************
#
# File:          sec_auth.res
#
# Project:       ESRF Security Authentication Resources
#
# Description:   Resource file for the security system.
#
# Autor(s):      Jens Meyer
#
# Original:      September 1993
#
# Copyright (c) 1990 by European Synchrotron Radiation Facility,
#		Grenoble, France
#
#********************************************************************-*/
# default access right, if no user or group entry can be
# found.
#
SEC/MINIMAL/ACC_RIGHT/default:	WRITE_ACCESS,	129.187.183,192.168.1	

#
#####################################################################
#
# user identification information
#               taco-name     uid,     erlaubte IP(subnetz oder einzelrechener)
#SEC/USER/IDENT/pedersen:        500, 129.187.183
#SEC/USER/IDENT/dserver:		1000,		192.168.1

#
# group identification information
#
#SEC/GROUP/IDENT/dserver:	200,		192.168.1,129.187.183
#
#
#                     x add your own domains hers
#SEC/GROUP/ACC_RIGHT/resi: dserver,WRITE_ACCESS
#SEC/USER/ACC_RIGHT/resi: pedersen,ADMIN_ACCESS 
