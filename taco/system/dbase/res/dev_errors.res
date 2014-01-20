#static char RcsId[]      =
#"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/system/dbase/res/dev_errors.res,v 1.1 2003-04-25 12:54:17 jkrueger1 Exp $";
#
#/*+*******************************************************************
#
# File:		dev_errors.res
#
# Project:	Device Server
#
# Description:	rsource file for general device server error messages.
#
# Author(s):	Jens Meyer
#
# Original:	July 1993
#
# Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
#                       Grenoble, France
#
#********************************************************************-*/
#
#
# Number 250-299 security system error messages.
#
ERROR/0/0/250:	"The user ID is not correct for the user name"
ERROR/0/0/251:	"Access to the control system network denied"
ERROR/0/0/252:	"The group ID is not correct for the group name"
ERROR/0/0/253:	"Device access denied for the requested access"
ERROR/0/0/254:	"Undefined access right in the security database"
ERROR/0/0/255:	"Undefined access right"
ERROR/0/0/256:	"No valid password entry for the user"
ERROR/0/0/257:	"No valid group entry for the user"
ERROR/0/0/258:	"No valid host entry"
#
ERROR/0/0/259:	"The device is locked in adminstration mode"
ERROR/0/0/260:	"The device is locked in single user mode"
ERROR/0/0/261:	"Device access denied, no valid security key"
ERROR/0/0/262:	"Access to command denied, access right to low"
ERROR/0/0/263:	"Single user session was canceled"
ERROR/0/0/264:  "The administrator access was canceled"
ERROR/0/0/265:  "Cannot store the security key"
ERROR/0/0/266:  "Cannot change the protocol with an open single user access"
ERROR/0/0/267:  "No single user mode possible for version 3 libraries"
#
#/*
# * general error definitions
# */
ERROR/0/0/50:	"syntax error detected by device"
ERROR/0/0/51:	"no hardware installed for this device"
ERROR/0/0/52:	"device hardware error"
ERROR/0/0/53:	"device type not recognised"
ERROR/0/0/54:	"wrong parameter for device"
ERROR/0/0/55:	"tried to set a value out of the devices range"
ERROR/0/0/56:	"device timed out"
ERROR/0/0/57:	"semaphore timed out - dead process?"
ERROR/0/0/58:	"command not allowed in this device state"
ERROR/0/0/59:	"state not recognised by state machine"
ERROR/0/0/60:	"command was not executed, ignored"
ERROR/0/0/61:	"command failed in its execution"
ERROR/0/0/62:	"error in writing or reading to field bus"
ERROR/0/0/63:	"Cannot create link to sub-process"
ERROR/0/0/64:	"attempt to start new process failed"
ERROR/0/0/65:	"bad checksum on serial communication"
ERROR/0/0/66:	"Command ignored, device is in fault state "

ERROR/0/0/71:	"no input available"
ERROR/0/0/72:	"device not in remote mode"
ERROR/0/0/73:	"couldn't open connection to device"
ERROR/0/0/74:	"error detected writing to device"
ERROR/0/0/75:	"error detected reading from device"

ERROR/0/0/81:	"device polarity is wrong"
ERROR/0/0/82:	"device flags error condition to be checked"
ERROR/0/0/83:	"device has no voltage at its input"
ERROR/0/0/84:	"wrong data type specified"
ERROR/0/0/85:	"failed to read/write access path"
ERROR/0/0/86:	"failed to open device access path"
#/*
# * errors for JCS 			
# */
ERROR/0/0/98:	"Jack upper limit switch activated"
ERROR/0/0/99:	"Jack lower limit switch activated"
ERROR/0/0/100:	"Jack is locked"
ERROR/0/0/101:	"JCS Motor driver card problem"
ERROR/0/0/102:	"JCS Motor driver power problem"
ERROR/0/0/103:	"JCS CPU faulty or loop connection problem"
ERROR/0/0/104:	"JCS communication buffer full"
ERROR/0/0/105:	"Girder limit switch upper right activated"
ERROR/0/0/106:	"Girder limit switch lower right activated"
ERROR/0/0/107:	"Girder limit switch upper left activated"
ERROR/0/0/108:	"Girder limit switch lower left activated"
#/*
# * errors for Ribber Ion Pump Controller
# */
ERROR/0/0/110:	"No high voltage available for distribution unit"
ERROR/0/0/111:	"Short circuit on distribution unit"
ERROR/0/0/112:	"High tension plug disconnected"
ERROR/0/0/113:	"Thermal cutout"
ERROR/0/0/114:	"Distribution unit not connected"
ERROR/0/0/115:	"High tension not connected"
ERROR/0/0/116:	"Pump volume not supported"
ERROR/0/0/117:	"Vacuum out of pump range"
ERROR/0/0/118:	"HT in use by another channel"
ERROR/0/0/119:	"Can't start pump - HT is already on"	
ERROR/0/0/120:	"Can't start pump - DU is already on"
ERROR/0/0/121:	"Pump startup failed - voltage is falling"	
ERROR/0/0/122:	"Pump startup failed - can't achieve power threshold"
#/*  
# *  errors for power supplies
# */
ERROR/0/0/140:	"Device not ready."
ERROR/0/0/141:	"Access to the data module failed"
ERROR/0/0/142:	"Response not recognised"
ERROR/0/0/143:	"You are not master"
#/*
# *  errors for GADC
# */
ERROR/0/0/150:	"device busy"
ERROR/0/0/151:	"data stale/data overflow"
ERROR/0/0/152:	"no data"
ERROR/0/0/153:	"erroneous data discarded"
ERROR/0/0/155:	"field-bus failed"
ERROR/0/0/156:	"field-bus buffer overflow"
ERROR/0/0/157:	"link to signal failed"
ERROR/0/0/158:	"internal server error detected"
ERROR/0/0/159:	"Device is not switched on"
#/*
# *  errors for pneum. Valves server
# */
ERROR/0/0/170:	"Cannot execute the specified command on a manual valve"
ERROR/0/0/171:	"Valve in fault state. Cannot execute command"
ERROR/0/0/172:	"Penning gauge interlock is activ"
ERROR/0/0/173:	"Penning gauge not in operating mode"
ERROR/0/0/174:	"No 24 Volt supply for PLC"
ERROR/0/0/175:	"detected PLC fault"
ERROR/0/0/176:	"A manual valve is still closed"
ERROR/0/0/177:	"Wrong u-switch indication"
ERROR/0/0/178:	"An error mode for the detected error is not implemented"
ERROR/0/0/179:	"The command was ignored by the valve"
ERROR/0/0/180:	"The specified internal PLC bit is not a member of the internal PLC word"
ERROR/0/0/181:	"A manual valve is still moving or blocked"
ERROR/0/0/182:	"Valve is blocked. Cannot execute command"
ERROR/0/0/183:	"Air pressure for pneumatic valve is to low"
ERROR/0/0/184:	"Valve is moving. Cannot execute command"
ERROR/0/0/185:	"The internal PLC program is stopped"
#/*
# *  errors for X-dev_menu
# */
ERROR/0/0/190:	"Output paramater type not implemented"
ERROR/0/0/191:	"Input paramater type not implemented"
#/*
# *  errors for VGC
# */
ERROR/0/0/500:	"Can't start penning, pressure too high"
ERROR/0/0/501:	"Can't start penning, associated pirani is off"
ERROR/0/0/502:	"Error in configuration of equipment"
ERROR/0/0/503:	"Can't start penning, gauge externally inhibited"
ERROR/0/0/504:	"Gauge specific error detected"
ERROR/0/0/505:	"Command ignored, VGC is in LOCAL mode"
ERROR/0/0/506:	"Command ignored, VGC is in FAULT state"
ERROR/0/0/507:	"Initialisation of serial line failed"
ERROR/0/0/508:	"VGC : Instrument battery is low"
ERROR/0/0/509:	"VGC : Internal settings lost"
ERROR/0/0/510:	"VGC : Command referenced to a non-existent gauge or relay"
ERROR/0/0/511:	"Pirani : gauge open circuit"
ERROR/0/0/512:	"Penning : Low pressure"
ERROR/0/0/513:	"VGC : Parameter was out of range"
ERROR/0/0/514:	"VGC : Command not accepted"
ERROR/0/0/515:	"Penning : HV supply is low"
ERROR/0/0/516:	"Penning : Pirani interlock prevents starting"
ERROR/0/0/517:	"Penning : Maximum pressure exceeded"
#/*
# *  errors for NEG pumps
# */
ERROR/0/0/530:	"Cannot open access to ICV712 card"
ERROR/0/0/531:	"Vacuum interlock is active, pressure to high"
ERROR/0/0/532:	"Neg pump interlock is active, powersupply failure"
ERROR/0/0/533:	"Device is still starting and not yet standby"
#/*
# * errors for LINAC
# */
ERROR/0/0/550:	"Command disallowed due to interlock conditions"
#
#/*
# *	Data collector server error messages
# */
ERROR/0/0/1001:	"Cannot create RPC client handle to data collector server"
ERROR/0/0/1002:	"Problem during data reception from server"
ERROR/0/0/1003:	"Bad parameters initialization"
ERROR/0/0/1004:	"Impossible to allocate memory in the function client part"
ERROR/0/0/1005:	"Can't build connection to static database"
ERROR/0/0/1006:	"Can't get Data Collector server host information from host file"
ERROR/0/0/1007:	"Can't retrieve Data Collector resources from static database"
ERROR/0/0/1008:	"Can't get Data Collector server network parameters (pn,vn,..)"
ERROR/0/0/1009:	"Too many commands for device"
ERROR/0/0/1010:	"No command for device"
ERROR/0/0/1011:	"Error during Data Collector database part function"
ERROR/0/0/1012:	"The device is not registered in the data collector"
ERROR/0/0/1013:	"The device has not been registered in the dc with a different cmd number"
ERROR/0/0/1014:	"The device has not been registered in the dc with this command code"
ERROR/0/0/1015:	"data buffer part of the data collector is full"
ERROR/0/0/1016:	"Error during freing an area of the data buffer"
ERROR/0/0/1017:	"The data in the data collector are not updated any more"
ERROR/0/0/1018:	"The cmd argument is not the sam than the one used during the dc_open call"
ERROR/0/0/1019:	"No data are yet available for the device"
ERROR/0/0/1020:	"This command is not used by the update daemon to poll the device"
ERROR/0/0/1021:	"Impossible to allocate memory in the data collector server"
ERROR/0/0/1022:	"Too many devices registered in the data collector"
ERROR/0/0/1023:	"Impossible to retrieve this command code in the data buffer"
ERROR/0/0/1024:	"No default data collector host has been defined for this host"
ERROR/0/0/1025:	"Can't get the data collector server host information from host file"
ERROR/0/0/1026:	"Try to free a device which has not been previously dc imported"
ERROR/0/0/1027:	"RPC client call timed out"
ERROR/0/0/1028:	"Can't convert data to XDR format before sending them to the data collector"
ERROR/0/0/1029:	"Can't set the time-out timer for semaphore management"
ERROR/0/0/1030:	"Time-out on the semaphore management"
ERROR/0/0/1031:	"The cellar table is full"
ERROR/0/0/1032:	"Device not defined in the device information part of the ptrs memory"
ERROR/0/0/1033:	"Device already defined in the device information part of the ptrs memory"
ERROR/0/0/1034:	"The indice does not correspond to the same device"
ERROR/0/0/1035:	"The command is not defined for this data collector record"
ERROR/0/0/1036:	"No response from data collector server"
ERROR/0/0/1037:	"Not able to determine if device is a pseudo device"
ERROR/0/0/1038:	"Cant register pseudo device(s) in database"
ERROR/0/0/1039:	"Cant unregister pseudo device(s) from database"
#
#/* Error for attenuators */
ERROR/0/0/1200:	"Fluorescent screen placed"
#
#/***** Error(s) for LECROY 1151 counter   -FEP-  *****/
ERROR/0/0/1120:	"Counter : Wrong channel number"
ERROR/0/0/1121:	"Counter : Device not ready"
ERROR/0/0/1122:	"Counter : Bad Fixed Identifier of the card"
ERROR/0/0/1123:	"Counter : Os9 error"
#
#/***** Error(s) for CAEN V462 gate generator  -FEP-  *****/
ERROR/0/0/1130:	"Gategen : Wrong channel number"
ERROR/0/0/1131:	"Gategen : Gate already active"
ERROR/0/0/1132:	"Gategen : Gate is in local mode"
ERROR/0/0/1133:	"Gategen : Os9 error"
ERROR/0/0/1134:	"Gategen : Value of time is out of range"
ERROR/0/0/1135:	"Gategen : Gate value has been rounded"
ERROR/0/0/1136:	"Gategen : Error at open, bad Ident Code"
ERROR/0/0/1137:	"Gategen : Error at open, bad Manufacturer Code"
ERROR/0/0/1138:	"Gategen : Function not supported by the hardware"
#
#/*********** Errors M4 - Hbm ***********/
ERROR/0/0/1211:	"HBM: Bad string read from the system"
ERROR/0/0/1212:	"HBM: Bad data read from the system"

ERROR/0/0/1220:	"M4: No device"
ERROR/0/0/1221:	"M4: incoherent data"
ERROR/0/0/1222:	"M4: Cannot disengage the clutch"
ERROR/0/0/1223:	"M4: Error on one of the axes"

#/*********** Some FRM-II generic errors ******/
ERROR/0/0/4091:	"Invalid device name"
ERROR/0/0/4092: "Invalid argument types"
ERROR/0/0/4093: "Cannot execute command"
ERROR/0/0/4094: "Command not found"
ERROR/0/0/4095: "Client not connected"
