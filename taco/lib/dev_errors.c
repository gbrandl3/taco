static char RcsId[]      =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/dev_errors.c,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";

/*+*******************************************************************

 File:		dev_errors.c

 Project:	Device Server

 Description:	list of device server error messages

 Author(s):	Andy Goetz
		Jens Meyer
		$Author: jkrueger1 $

 Original:	9 March 1992

 Version:       $Revision: 1.1 $

 Date:          $Date: 2003-03-14 12:22:07 $

 Copyright (c) 1990-1997 by European Synchrotron Radiation Facility, 
                            Grenoble, France

********************************************************************-*/

#include <DevErrors.h>

/*
 **********************************
 *   List of device error strings *
 **********************************
 */


/*
 * device server error message strings
 */

DevErrListEntry DevErr_List[] = {
/*
 *  Number 0-9 RPC error message
 */
{ DevErr_CannotCreateClientHandle, "Cannot create RPC client handle"},
{ DevErr_RPCTimedOut, "RPC client call timed out"},
{ DevErr_RPCFailed, "RPC client call failed"},
{ DevErr_MsgImportFailed, "Import of message server failed"},
{ DevErr_BadXdrDataTypeDefinition,"The XDR data type is not correctly defined"},
{ DevErr_XdrTypeListOverflow, "The list of possible XDR data types is full"},
{ DevErr_XdrDataTypeNotLoaded, "The requested XDR data type is not loaded"},
{ DevErr_DeviceNotImportedYet, "Device has not been fully imported yet, (hint: start the device server)"},
/*
 *  Number 10-29 API-LIB client part
 */
{ DevErr_DbImportFailed, "Import of database server failed"},
{ DevErr_ExceededMaximumNoOfServers, "exceeded maximum number of servers"},
{ DevErr_BadServerConnection,"Lost connection to the device server"},
{ DevErr_DevArgTypeNotRecognised, "unrecognised device argument type"},
{ DevErr_InsufficientMemory, "device server ran out of memory"},
{ DevErr_NethostNotDefined, "environmental variable NETHOST not defined"},
{ DevErr_NoNetworkManagerAvailable, "no network manager available"},
{ DevErr_NetworkManagerNotResponding, "network manager is not responding"},
{ DevErr_SignalOutOfRange, "not a valid signal value"},
{ DevErr_CannotSetSignalHandler, "cannot setup signal handler function"},
{ DevErr_DevArgTypeNotCorrectImplemented, "device argument type is not correctly implemented"},
{ DevErr_UnknownInputParameter, "Input parameter is unknown"},
{ DevErr_CannotConnectToHost, "Cannot establish connection to host"},
{ DevErr_CannotDecodeArguments, "XDR: Cannot decode arguments"},
/*
 *  Number 30-49 API-LIB server part
 */
{ DevErr_DISPLAYNotSet, "environment variable DISPLAY has not been setup"},
{ DevErr_DeviceOfThisNameNotServed,"device of this name unknown to the server"},
{ DevErr_ExceededMaximumNoOfDevices,
		    "the maximum of devices which can be served has been exceeded"},
{ DevErr_IncompatibleCmdArgumentTypes, 
		    "incompatible arguments types for command function"},
{ DevErr_CommandNotImplemented, 
		    "tried to execute a command which is not implemented"},
{ DevErr_ServerAlreadyExists, "The same Device Server is already running"},
{ DevErr_ServerRegisteredOnDifferentHost, 
		    "The Device Server is still registered on a different host"},
{ DevErr_XDRLengthCalculationFailed, 
		    "Calculation of the XDR length of the data type failed"},
{ DevErr_NameStringToLong, "String exceeded length of character field"},
{ DevErr_DeviceNoLongerExported, "The device is no longer exported"},
{ DevErr_ExceededMaximumNoOfClientConn, 
		    "Exceeded the maximum number of client connections"},

/*
 *  Network manager error messages
 */
{ DevErr_DSHOMENotSet, "environment variable DSHOME has not been setup"},

/*
 *  message server error messages
 */
{ DevErr_CannotOpenErrorFile,"message server cannot open error file"},
{ DevErr_CannotOpenPipe, "message server cannot open named pipe"},
{ DevErr_CannotWriteToPipe, "message server cannot write to named pipe"},

/*
 *  Asynchronous call error messages
 */
{ DevErr_AsynchronousCallsNotSupported, "asynchronous calls not supported (hint: relink with V6 and/or use a remote device)"},
{ DevErr_AsynchronousServerNotImported, "asynchronous server not imported yet"},
{ DevErr_NoCallbackSpecified, "no callback specified for asynchronous reply"},
{ DevErr_ExceededMaxNoOfPendingCalls, "exceeded maximum no. of pending asynchronous calls (hint: try calling dev_synch() to clear pending calls)"},


/*
 *  Number 400-500 database server error messages
 */
{ DbErr_NoDatabase, "No database therefore the process cannot query it (hint: try running with database)"},
{ DbErr_CannotCreateClientHandle, 
		    "Cannot create RPC client handle to database server"},
{ DbErr_RPCreception, "Problem during data reception from server"},
{ DbErr_DatabaseAccess, "Something wrong during a database access function"},
{ DbErr_BooleanResource, "Bad definition for a boolean resource"},
{ DbErr_MemoryFree, "Problem occurs during XDR memory freeing"},
{ DbErr_ClientMemoryAllocation,
		    "Impossible to allocate memory in the function client part"},
{ DbErr_ServerMemoryAllocation,
		    "Impossible to allocate memory in the function server part"},
{ DbErr_DomainDefinition,"Bad domain definition in a resource definition"},
{ DbErr_ResourceNotDefined,"Resource not defined in the database"},
{ DbErr_DeviceServerNotDefined,"Device server not defined in the database"},
{ DbErr_MaxNumberOfDevice,"Too much devices for this device server"},
{ DbErr_DeviceNotDefined,"Device not defined in the database"},
{ DbErr_HostName,
		    "The host name used in the db_dev_export function is not the same than the host name defined in the database"},
{ DbErr_DeviceNotExported,
		    "Try to import a device which has not been previously exported"},
{ DbErr_FamilyDefinition,"Bad family definition in the data_key definition"},
{ DbErr_TimeCriterion,"Wrong time criterion for retrieval function"},
{ DbErr_BooleanDefinition,
		    "Bad boolean definition in the research criterions structure"},
{ DbErr_BadValueCriterion,"Wrong criterion used for numrical data"},
{ DbErr_BadBooleanCriterion,"Bad boolean criterion for retrieval function"},
{ DbErr_BadStringCriterion,"Wrong string criterion for retrieval function"},
{ DbErr_MemberDefinition,"Bad member definition in the data_key definition"},
{ DbErr_DataDefinition,"Wrong data definition in the data_key definition"},
{ DbErr_BadStructureDefinition,"Bad structure definition for data"},
{ DbErr_BadParameters,"Bad parameters initialization"},
{ DbErr_OS9_FileAccess,"Problem during file access in OS-9 stand alone system"},
{ DbErr_NethostNotDefined,"Environment varaible NETHOST not defined"},
{ DbErr_NoManagerAvailable,"No network manager available"},
{ DbErr_ManagerNotResponding,"Network manager is not responding"},
{ DbErr_ManagerReturnError,"Network manager returned error"},
{ DbErr_DbServerNotExported,
			    "No data available for this database server in the network manager"},
{ DbErr_MaxDeviceForUDP, "Too many exported devices for a UDP packet"},
{ DbErr_OtherProcessOnDb,
		    "The database server is not the only process which has connection to the database"},
{ DbErr_TooManyInfoForUDP,
			"The size of information sent back to the client is too big for UDP packet"},
{ DbErr_BadResourceType,"The resource is not initialised as an array"},
{ DbErr_StringTooLong,"The resource is too long to be stored in the database"},
{ DbErr_NameAlreadyUsed,"The pseudo device name is already used for a device"},
{ DbErr_CannotEncodeArguments,"XDR : Cannot encode arguments"},
{ DbErr_CannotDecodeResult,"XDR : Cannot decode results from db server"},
	};


/*
 * number of errors in message list
 */

#define MAX_ERR  	(sizeof(DevErr_List)/sizeof(DevErrListEntry))

long max_deverr = MAX_ERR;
