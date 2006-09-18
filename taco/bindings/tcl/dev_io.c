/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File : 	dev_io.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: This file contains the c commands for talking to the esrf devices
 * 		from Tcl/Tk.
 *
 * 		A new Tcl/Tk interpreter is builded with three procedures more :
 * 		dev_io, dev_io_async and dev_io_debug.
 *
 * 		All the main device types are included.
 *
 * 		When the output type is a structure in C code, you get in tcl
 * 		a list where the arguments are in the same order as defined in
 * 		the C structure
 * 		example : for a DevStateFloatReadPoint argout 
 *           	struct DevFloatReadPoint {
 *                                     short state;
 *                                     float set;
 *                                     float read;
 *                                    }
 *           	you get in tcl a list containing :
 *           	short float float
 *
 *		In this version only two modes of debug are possible (0=no debug mode 1=debug mode)
 *
 * Author(s):   Denis Beauvois
 *              Gilbert Pepellin
 *              J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:    June, 1996
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-09-18 22:04:49 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/* Codecenter comments to supress some warnings at load-time */
/*SUPPRESS569*/ /*The statement cannot be reached in switch() */
/*SUPPRESS610*/ /*Switch() both return with a value and return from bottom */
/*SUPPRESS65*/
#include <dev_io.h>
#include <debug.h>
#include <maxe_xdr.h>

#define	STRING_TMP_LENGTH 250

/* Global variables */
/* ---------------- */

int	TCL_TEST;

static int DEBUG_MODE=DEBUG_LEVEL_0;         /* Flag for debug mode      */
                                             /* default = 0 : no debug   */
static int IO_MODE=SYNCHRONOUS_MODE;         /* Flag for asynchronous IO */

/* 
 To have a link between the number of the device type
 and a string
 */
static struct DEVICE_TYPE DeviceType [NUMBER_OF_TYPES]= {
                   {D_VOID_TYPE,"void",},
                   {D_BOOLEAN_TYPE,"boolean",},
                   {D_USHORT_TYPE,"unsigned_short",},
                   {D_SHORT_TYPE,"short",},
                   {D_ULONG_TYPE,"unsigned_long",},
                   {D_LONG_TYPE,"long",},
                   {D_FLOAT_TYPE,"float",},
                   {D_DOUBLE_TYPE,"double",},
                   {D_STRING_TYPE,"string",},
                   {D_INT_FLOAT_TYPE,"int_float",},
                   {D_FLOAT_READPOINT,"float_float",},
                   {D_STATE_FLOAT_READPOINT,"short_float_float",},
                   {D_LONG_READPOINT,"long_long",},
                   {D_DOUBLE_READPOINT,"double_double",},
                   {D_VAR_CHARARR,"array-of-char",},
                   {D_VAR_STRINGARR,"array-of-string",},
                   {D_VAR_USHORTARR,"array-of-unsigned_short",},
                   {D_VAR_SHORTARR,"array-of-short",},
                   {D_VAR_ULONGARR,"array-of-unsigned_long",},
                   {D_VAR_LONGARR,"array-of-long",},
                   {D_VAR_FLOATARR,"array-of-float",},
                   {D_VAR_DOUBLEARR,"array-of-double",},
                   {D_VAR_FRPARR,"array-of-(float_float)",},
                   {D_VAR_SFRPARR,"array-of-(short_float_float)",},
                   {D_VAR_LRPARR,"array-of-(long_long)",},
                   {D_OPAQUE_TYPE,"array-of-char",},
                   {D_MULMOVE_TYPE,"long[8]_long[8]_float[8]",},
                   {D_MOTOR_LONG,"long_long",},
                   {D_MOTOR_FLOAT,"long_float",},
                  };
                  
/*
 To keep in memory the list of the already
 imported devices
*/

IMPORTED_DEVICE *FirstDevice;  /* Head list */
IMPORTED_DEVICE *CurrentDevice; /* Structure for the current device */

                  
/* -------------------------------------------------------------- */
/* Function SetIoMode                                             */
/* Argin  : int  (SYNCHRONOUS_MODE or ASYNCHRONOUS_MODE)          */
/* Argout : int (False=Error True=Ok)                             */
/* Description :                                                  */
/*         Set the flag to determine the mode of dev_io           */
/*         (synchronous or asynchronous)                          */
/* -------------------------------------------------------------- */
int SetIoMode (int Argin)
{
	switch (Argin)
	{
		case SYNCHRONOUS_MODE  :
		IO_MODE=SYNCHRONOUS_MODE;
       			return True;
       			break;
		case ASYNCHRONOUS_MODE :
			IO_MODE=ASYNCHRONOUS_MODE;
			return True;
			break;
		default :        /* None change made */
			return False;
			break;
	}
}

/* -------------------------------------------------------------- */
/* Function GetIoMode                                             */
/* Argin  : none                                                  */
/* Argout : int  (SYNCHRONOUS_MODE or ASYNCHRONOUS)MODE)          */
/* Description :                                                  */
/*         Get the flag determining the mode of dev_io            */
/*         (synchronous or asynchronous)                          */
/* -------------------------------------------------------------- */
int GetIoMode()
{
	return IO_MODE;
}

/* -------------------------------------------------------------- */
/* Function GetDebugMode                                          */
/* Argin  : none                                                  */
/* Argout : int (DEBUG_LEVEL_0 .... DEBUG_LEVEL_3)                */
/* Description :                                                  */
/*         Get the flag determining the level of debugging        */
/*         (there are 4 levels of debugging)                      */
/* -------------------------------------------------------------- */
int GetDebugMode()
{
	return DEBUG_MODE;
}

/* -------------------------------------------------------------- */
/* Function SetDebugMode                                          */
/* Argin  : int (DEBUG_LEVEL_0 .... DEBUG_LEVEL_3)                */
/* Argout : int (False=Error True=Ok)                             */
/* Description :                                                  */
/*         Set the flag to determine the level of debugging       */
/*         (there are 4 levels of debugging)                      */
/* -------------------------------------------------------------- */
int SetDebugMode(int Argin)
{
	switch (Argin)
	{
		case DEBUG_LEVEL_0 :
			DEBUG_MODE=DEBUG_LEVEL_0;
			return True;
			break;
		case DEBUG_LEVEL_1 :
			DEBUG_MODE=DEBUG_LEVEL_1;
			return True;
			break;
		case DEBUG_LEVEL_2 :
			DEBUG_MODE=DEBUG_LEVEL_2;
			return True;
			break;
		case DEBUG_LEVEL_3 :
			DEBUG_MODE=DEBUG_LEVEL_3;
			return True;
       			break; 
		default :
			return False;
			break;
	}
}

/* -------------------------------------------------------------- */
/* Function GetImportedDevice                                     */
/* Argin  : IMPORTED_DEVICE *                                     */
/* Argout : IMPORTED_DEVICE *                                     */
/* Description :                                                  */
/*         To get a IMPORTED_DEVICE structure                     */
/*         Especialy used with linked lists                       */
/* -------------------------------------------------------------- */
IMPORTED_DEVICE* GetImportedDevice(IMPORTED_DEVICE *Device) 
{
	return (Device->Next);
}

/* -------------------------------------------------------------- */
/* Function ConvertType                                           */
/* Argin  : int                                                   */
/* Argout : char *                                                */
/* Description :                                                  */
/*         To get string representing a device type               */
/*         knowing its number                                     */
/*         Return NULL if an error occures                        */
/* -------------------------------------------------------------- */ 
char * ConvertType (int Number)
{
	int i;
 
	for (i=0;i<NUMBER_OF_TYPES;i++)
		if (DeviceType[i].Type==(DevType)Number)
        		return DeviceType[i].Name;
	return NULL;
}

/* =============================================================== */
/* --------------------------------------------------------------- */
/*                       Function dev_io_asyn                      */
/* Description :                                                   */
/*         This is the function called by the tcl interpreter      */
/*         It requires at least 4 arguments :                      */
/*            status, error, device_name, command_name, {argin}    */
/* --------------------------------------------------------------- */
/* =============================================================== */
int dev_io_asyn (ClientData clientdata, Tcl_Interp *interp,
		 int argc, char **argv)
{
	SetIoMode(ASYNCHRONOUS_MODE);
	dev_io (clientdata,interp,argc,argv);
} 


/* =============================================================== */
/* --------------------------------------------------------------- */
/*                       Function dev_io                           */
/* Description :                                                   */
/*         This is the function called by the tcl interpreter      */
/*         It requires at least 4 arguments :                      */
/*            status, error, device_name, command_name, {argin}    */
/* --------------------------------------------------------------- */
/* =============================================================== */
int dev_io (ClientData clientdata, Tcl_Interp *interp,
	    int argc, char **argv)
/*SUPPRESS761*/
{
	static int 	FirstCall=True; /* To initialise dynamic variables */
	char 		*State,
			*Error,
			*DeviceName,
			*CommandName; /* matched with arguments of the command line */
	int 		i,
			j;
	int 		test1,
			test2;
	long 		DeviceStatus; /* To obtain the state of a device */
	devserver 	ds;
	long 		DeviceError; /* To have the error of a device */
	char 		*StringTemp = NULL,
			*StringAux = NULL;
	long 		readwrite = 0; /* To precise the divice access */
	DevCmdInfo 	*InfoCmd; /* To have information about each command */
	long 		Cmd; /* For identifying the command of the device; */
	DevVarCmdArray  Info; /* To have the information's structure about a command */
	char 		*InType ; /* String which contains the argin required by a device's command  */
	char 		*OutType; /* String which contains the argout required by a device's command */
	IMPORTED_DEVICE *LastDevice;
	DevType 	ArginRequired,
			ArgoutRequired; /* For knowing the types needed by the device's command */
	char 		*TclAux = NULL; /* For converting OutputArg into TCL variables */
	void 		*TclTemp = NULL; /* For converting OutputArg into TCL variables when var array is used */

	int 		R; /* For testing the returned code of the functions of conversion */
	int 		NEW_DEVICE = False;
	long 		errload;
	char 		*tcl_precision;
	long 		precision; /* to get the global Tcl variable tcl_precision */
	char 		format[32] ;   /* to build the format when call sprintf to convert */
					/* argout from dev_putget to Tcl string */

/* -------------------------------- */
/* Setting debug mode               */
/* -------------------------------- */
	TCL_TEST = (GetDebugMode() != DEBUG_LEVEL_0); 
		
/* get the value of tcl_precision*/
	tcl_precision = Tcl_GetVar(interp,"tcl_precision",0);
	if (tcl_precision != NULL) 
		R=Tcl_ExprLong (interp,tcl_precision,&precision);
  	else 
		precision = 6;
	if (TCL_TEST) 
	{
		printf("tcl_precision= %d\n",precision); 
		fflush (stdout); 
		printf("++++++++++++ Entering dev_io ++++++++++++++++\n");
	}

/* ------------------------------------------------- */
/* First initialisation of dynamical variables       */
/* Only during the first call                        */
/* ------------------------------------------------- */
	if (FirstCall==True)
	{
		if (TCL_TEST) 
			printf("First call of dev_io\n"); 
		FirstCall=False;
		FirstDevice = (IMPORTED_DEVICE *) malloc (sizeof(struct IMPORTED_DEVICE));
		FirstDevice->Next          = NULL;
		FirstDevice->Name          = " ";
		FirstDevice->ds            = NULL;
		FirstDevice->imported_flag = False;
	}
        
/* ------------------------------------------------- */
/* Put the arguments into new C variables            */
/* and verifying the number of the arguments         */
/* ------------------------------------------------- */
	if (TCL_TEST) 
		printf("Verifying the number of the input arguments\n"); 
	if (argc<(NUMBER_ARGUMENTS_MIN+1) || argc>NUMBER_ARGUMENTS_MAX+1)
	{
		Tcl_AppendResult(interp,"should be... dev_io 'status' 'error' 'device' 'command' '{argin}'"\
				,(char *)NULL);
		return TCL_ERROR;
	}   
	State       = argv[1];
	Error       = argv[2];
	DeviceName  = argv[3];
	CommandName = argv[4];
	if (TCL_TEST) 
		printf("Devicename=%s CommandName=%s\n",DeviceName , CommandName);
 
/* Transforming State and Error in Tcl variable */
	Tcl_SetVar (interp,State,"0",0); /* No error by default */
	Tcl_SetVar (interp,Error,"",0);
 
/* ------------------------------------- */
/* Initialize temporary variables */
/* ------------------------------------- */

	if (TCL_TEST) 
		printf("Initialization of dynamical temporary variables\n");
	StringTemp = (char*) calloc (STRING_TMP_LENGTH,sizeof(char)); /* Necessary for the sprintf function */
	SequencePointer = NULL;
	TclAux = NULL;
	InputArg = NULL; 
	OutputArg = NULL;
	StringType = NULL;
	if (TCL_TEST) 
		printf("StringTemp=%X SequencePointer=%X TclAux=%X\n InputArg=%X OutputArg=%X StringType =%X\n", 
                       StringTemp,SequencePointer,TclAux ,InputArg,OutputArg,StringType);
 
/* ----------------------------------*/
/* Import the device if mot done yet */
/* ----------------------------------*/
	if (TCL_TEST) 
		printf("check if the device: %s is already imported\n",DeviceName);
/* First we check if the device is already imported */
	CurrentDevice = FirstDevice;
	test1 = 0;
	do
	{
/* Check if this device is already marked as imported */
		test1 = strcmp(CurrentDevice->Name,DeviceName); 
		if (test1==0) 
			break;
		LastDevice = CurrentDevice;
		CurrentDevice = GetImportedDevice(CurrentDevice);
	}
	while (CurrentDevice != NULL);
 
	if (test1 != 0)  /* We are dealing with a new Device */
	{
		NEW_DEVICE=True;
		if (TCL_TEST) 
			printf("%s is not registered as imported. send dev_import\n",DeviceName);
		DeviceError = 0;
		DeviceStatus = dev_import (DeviceName,readwrite,&ds,&DeviceError);     
		if (TCL_TEST) 
			printf("error_import= %d\n",DeviceError);
		if (DeviceStatus != DS_OK) /* dev_import failed */
		{
			if (TCL_TEST) 
				printf("dev_import failed\n");
			snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
			Tcl_SetVar (interp,State,StringTemp,0);
			StringAux = dev_error_str(DeviceError);
			snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
			Tcl_SetVar (interp,Error,StringTemp,0);
			free(StringAux); 
			free (StringTemp);
			return TCL_OK;
		}     
/* dev_import always returns DS_OK if the device is defined in the data base
 * Call dev_cmd_query to check if the device if fully exported.
 *   If yes: Register this device as imported
 *           store its commands
 *   If no : Return the error generated by dev_cmd_query
 */
		if (TCL_TEST) 
			printf("Call dev_cmd_query to check if the device is successfully imported\n"); 
		DeviceStatus = dev_cmd_query (ds,&Info,&DeviceError);
		if (DeviceStatus != DS_OK) /* dev_cmd_query failed */
		{
			if (TCL_TEST) 
				printf("dev_cmd_query failed\n"); 

			snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
			Tcl_SetVar (interp,State,StringTemp,0);
			StringAux = dev_error_str(DeviceError);
			snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
			Tcl_SetVar (interp,Error,StringTemp,0);
			free(StringAux); 
			free (StringTemp);
			dev_free (ds, &DeviceError);
			return TCL_OK;
		}

		if (TCL_TEST) 
			printf("dev_cmd_query OK for %s\n",DeviceName); 
		CurrentDevice = (IMPORTED_DEVICE *)malloc(sizeof(struct IMPORTED_DEVICE));
		CurrentDevice->Next = NULL;
		CurrentDevice->Name = (char *) malloc (strlen(DeviceName)+1);
		strcpy (CurrentDevice->Name,DeviceName);
		CurrentDevice->imported_flag = True;
		CurrentDevice->ds = ds;
		LastDevice->Next = CurrentDevice;
		(CurrentDevice->InfoArray).length = Info.length;
		(CurrentDevice->InfoArray).sequence = Info.sequence;
	} /* ends if clause of if (test1 != 0) ie. This is a new device */
	else
	{ 
/* device is already imported */
/*
 * If the imported_flag is False, the connection was freed
 * and the device must be reimported.
 */
		if (CurrentDevice->imported_flag == False)
		{
			if (TCL_TEST) 
				printf("%s was freed, import it again.\n",DeviceName);
			DeviceError = 0;
			if ( dev_import (DeviceName,readwrite,&ds,&DeviceError) == DS_NOTOK )
			{
				if (TCL_TEST) 
					printf("dev_import failed\n");
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free(StringAux); 
				free (StringTemp);
				return TCL_OK;
			}
	
/* copy the initialised handle */
			CurrentDevice->ds = ds;
		}

/* The device is now imported or reimported */
		CurrentDevice->imported_flag = True;

		if (TCL_TEST) 
			printf("%s is already imported..\n",DeviceName);
		Info = (CurrentDevice->InfoArray);
 
	} /* ends else clause of if (test1 != 0) ie. device is already imported */				
      
/* ---------------------------------- */
/*  Now we test if the command is '?'  */
/* ---------------------------------- */
	if (TCL_TEST) 
		printf("Testing if the command is '?'....\n");
     
	test1 = False; /* True if the device's command is ? */
	test2 = False; /* True if the device's command is available */
	if (strcmp("?",CommandName)==0)  /* CommandName=? */
		test1 = test2 = True; 

	Tcl_ResetResult(interp);
	for (i=0;i<Info.length;i++)
	{
		InfoCmd = Info.sequence+i;
		InType  = ConvertType(InfoCmd->in_type);
		OutType = ConvertType(InfoCmd->out_type);
		if (InType==NULL)  
			InType = "unknown_type";
		if (OutType==NULL) 
			OutType = "unknown_type";
 
/* free the in_name and out_name variables */
		if (InfoCmd->in_name  != NULL) 
			free (InfoCmd->in_name);

		if (InfoCmd->out_name != NULL) 
			free (InfoCmd->out_name);
 
/* Get command number and arguments types */ 
		if (strcmp(CommandName,InfoCmd->cmd_name)==0)
		{
			ArginRequired  = InfoCmd->in_type;
			ArgoutRequired = InfoCmd->out_type;
			Cmd            = InfoCmd->cmd;
			test2 = True;

			if (TCL_TEST) 
				printf("%s is a valid command for %s\n",CommandName,DeviceName);
			if (TCL_TEST) 
				printf("ArginRequired=%d ArgoutRequired=%d\n",ArginRequired,ArgoutRequired);
		} /* ends if (strcmp(CommandName,InfoCmd->cmd_name)==0) */
 
/* Setting Tcl Variables with the infos as a list if the command is ?*/ 
/*SUPPRESS65*/
		if (test1)
			Tcl_AppendResult (interp,InfoCmd->cmd_name," ",InType," ",OutType," ",(char *)NULL);
         
	} /* ends for (i=0;i<Info.length;i++) */
        
	if (test1) 
	{
		free (StringTemp); 
		return TCL_OK;
	}
        
/* If the command is not valid generate error# 34 "CommandNotImplemented" */   
	if (TCL_TEST) 
		printf("Check if command %s is valid\n",CommandName);
	if (test2==False) 
	{
/* Not a valid command */
  		snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DS_NOTOK);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(34);
		snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,34);
		Tcl_SetVar (interp,Error,StringTemp,0);
		free(StringAux); 
		free (StringTemp);
		return TCL_OK;
	}   
/* ------------------------------------------ */
/* Check if an argin argument is required */
/* ------------------------------------------ */
	if (TCL_TEST) 
		printf("Check if an input argument is required..\n");

	if ( (ArginRequired != False && argc != NUMBER_ARGUMENTS_MAX+1)  || 
	     (ArginRequired==False && argc==NUMBER_ARGUMENTS_MAX+1) )
	{
/* An argin is provided but not requested
     generate IncompatibleCmdArgumentTypes (33)
*/
		snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DS_NOTOK);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DevErr_IncompatibleCmdArgumentTypes);
		snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DevErr_IncompatibleCmdArgumentTypes);
		Tcl_SetVar (interp,Error,StringTemp,0);
		free(StringAux); free (StringTemp);
		return TCL_OK;
	}

    
/* ----------------------------------------------------- */
/* Now converting the argin for dev_putget               */
/* InputArg will contain a pointer on the specified type */
/* ----------------------------------------------------- */
	if (TCL_TEST) 
		printf("Converting the input argument for dev_putget..\n");

	InputArg=NULL;

	if(TCL_TEST) 
		printf("ArginRequired= %d\n",ArginRequired);
	switch (ArginRequired)
	{
		case D_VOID_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VOID_TYPE\n");
			R = True;
			break;
		case D_BOOLEAN_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_BOOLEAN_TYPE\n");
			R=F_D_BOOLEAN_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_BOOLEAN_TYPE");
			break;
		case D_USHORT_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_USHORT_TYPE\n");
			R=F_D_USHORT_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_USHORT_TYPE");
			break;
		case D_SHORT_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_SHORT_TYPE\n");
			R=F_D_SHORT_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_SHORT_TYPE");
			break;
		case D_ULONG_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_ULONG_TYPE\n");
			R=F_D_ULONG_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_ULONG_TYPE");
			break;
		case D_LONG_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_LONG_TYPE\n");
			R=F_D_LONG_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_LONG_TYPE");
			break;
		case D_FLOAT_TYPE: 
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_FLOAT_TYPE\n");
			R=F_D_FLOAT_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_FLOAT_TYPE");
			break;
		case D_DOUBLE_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_DOUBLE_TYPE\n");
			R=F_D_DOUBLE_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_DOUBLE_TYPE");
			break;
		case D_STRING_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_STRING_TYPE\n");
			R=F_D_STRING_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_STRING_TYPE");
			break;
		case D_INT_FLOAT_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_INT_FLOAT_TYPE\n");
			R=F_D_INT_FLOAT_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_INT_FLOAT_TYPE");
			break;
		case D_FLOAT_READPOINT:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_FLOAT_READPOINT\n");
			R=F_D_FLOAT_READPOINT(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_FLOAT_READPOINT");
			break;
		case D_STATE_FLOAT_READPOINT:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_STATE_FLOAT_READPOINT\n");
			R=F_D_STATE_FLOAT_READPOINT(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_SATE_FLOAT_READPOINT");
			break;
		case D_LONG_READPOINT:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_LONG_READPOINT\n");
			R=F_D_LONG_READPOINT(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_LONG_READPOINT");
			break;
		case D_DOUBLE_READPOINT:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_DOUBLE_READPOINT\n");
			R=F_D_DOUBLE_READPOINT(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_DOUBLE_READPOINT");
			break;
		case D_VAR_CHARARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_CHARARR\n");
			R=F_D_VAR_CHARARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_CHARARR");
			break;
		case D_VAR_STRINGARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_STRINGARR\n");
			R=F_D_VAR_STRINGARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_STRINGARR");
			break;
		case D_VAR_USHORTARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_USHORTARR\n");
			R=F_D_VAR_USHORTARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_USHORTARR");
			break;
		case D_VAR_SHORTARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_SHORTARR\n");
			R=F_D_VAR_SHORTARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_SHORTARR");
			break;
		case D_VAR_ULONGARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_ULONGARR\n");
			R=F_D_VAR_ULONGARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_ULONGARR");
			break;
		case D_VAR_LONGARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_LONGARR\n");
			R=F_D_VAR_LONGARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_LONGARR");
			break;
		case D_VAR_FLOATARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_FLOATARR\n");
			R=F_D_VAR_FLOATARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_FLOATARR");
			break;
		case D_VAR_DOUBLEARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_DOUBLEARR\n");
			R=F_D_VAR_DOUBLEARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_DOUBLEARR");
			break;
		case D_VAR_FRPARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_FRPARR\n");
			R=F_D_VAR_FRPARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_FRPARR");
			break;
		case D_VAR_SFRPARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_SFRPARR\n");
			R=F_D_VAR_SFRPARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_SFRPARR");
			break;
		case D_VAR_LRPARR:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_VAR_LRPARR\n");
			R=F_D_VAR_LRPARR(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_VAR_LRPARR");
			break;
		case D_OPAQUE_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_OPAQUE_TYPE\n");
			R=F_D_OPAQUE_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_OPAQUE_TYPE");
			break;
		case D_MULMOVE_TYPE:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_MULMOVE_TYPE\n");
			R=F_D_MULMOVE_TYPE(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_MULMOVE_TYPE");
			break;
		case D_MOTOR_LONG:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_MOTOR_LONG\n");
			R=F_D_MOTOR_LONG(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_MOTOR_LONG");
			break;
		case D_MOTOR_FLOAT:
			if(TCL_TEST) 
				printf("dev_io: ArginRequired= D_MOTOR_FLOAT\n");
			R=F_D_MOTOR_FLOAT(interp,argv[5],INPUT);
			strcpy(StringTemp,"F_D_MOTOR_FLOAT");
			break;
		default:
			printf ("ERROR during the conversion of the argin..type unknown \n");
			free (StringTemp);
			return TCL_ERROR;
			break;
	}
 
	if (R==False)
	{
		printf("ERROR during the conversion of the input argument in the function %s\n",StringTemp);
		free (StringTemp);
		return TCL_ERROR;
	}
       
/* ------------------------------------------------------ */
/* Now setting the argout for dev_putget                  */
/* OutputArg will contain a pointer on the specified type */
/* ------------------------------------------------------ */
	if (TCL_TEST) 
		printf("Setting the ouput argument in a good format..\n");
	OutputArg = NULL;
	switch (ArgoutRequired)
	{
		case D_VOID_TYPE         	:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VOID_TYPE\n");
			break;
		case D_BOOLEAN_TYPE      	:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_BOOLEAN_TYPE\n");
			F_D_BOOLEAN_TYPE(interp,"",OUTPUT);
			break;
		case D_USHORT_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_USHORT_TYPE\n");
			F_D_USHORT_TYPE(interp,"",OUTPUT);
			break;
		case D_SHORT_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_SHORT_TYPE\n");
			F_D_SHORT_TYPE(interp,"",OUTPUT);
			break;
		case D_ULONG_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_ULONG_TYPE\n");
			F_D_ULONG_TYPE(interp,"",OUTPUT);
			break;
		case D_LONG_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_LONG_TYPE\n");
			F_D_LONG_TYPE(interp,"",OUTPUT);
			break;
		case D_FLOAT_TYPE		: 
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_FLOAT_TYPE\n");
			F_D_FLOAT_TYPE(interp,"",OUTPUT);
			break;
		case D_DOUBLE_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_DOUBLE_TYPE\n");
			F_D_DOUBLE_TYPE(interp,"",OUTPUT);
			break;
		case D_STRING_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_STRING_TYPE\n");
			F_D_STRING_TYPE(interp,"",OUTPUT);
			break;
		case D_INT_FLOAT_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_INT_FLOAT_TYPE\n");
			F_D_INT_FLOAT_TYPE(interp,"",OUTPUT);
			break;
		case D_FLOAT_READPOINT		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_FLOAT_READPOINT\n");
			F_D_FLOAT_READPOINT(interp,"",OUTPUT);
			break;
		case D_STATE_FLOAT_READPOINT	:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_STATE_FLOAT_READPOINT\n");
			F_D_STATE_FLOAT_READPOINT(interp,"",OUTPUT);
			break;
		case D_LONG_READPOINT		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_LONG_READPOINT\n");
			F_D_LONG_READPOINT(interp,"",OUTPUT);
			break;
		case D_DOUBLE_READPOINT		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_DOUBLE_READPOINT\n");
			F_D_DOUBLE_READPOINT(interp,"",OUTPUT);
			break;
		case D_VAR_CHARARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_CHARARR\n");
			F_D_VAR_CHARARR(interp,"",OUTPUT);
			break;
		case D_VAR_STRINGARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_STRINGARR\n");
			F_D_VAR_STRINGARR(interp,"",OUTPUT);
			break;
		case D_VAR_USHORTARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_USHORTARR\n");
			F_D_VAR_USHORTARR(interp,"",OUTPUT);
			break;
		case D_VAR_SHORTARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_SHORTARR	\n");
			F_D_VAR_SHORTARR(interp,"",OUTPUT);
			break;
		case D_VAR_ULONGARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_ULONGARR\n");
			F_D_VAR_ULONGARR(interp,"",OUTPUT);
			break;
		case D_VAR_LONGARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_LONGARR\n");
			F_D_VAR_LONGARR(interp,"",OUTPUT);
			break;
		case D_VAR_FLOATARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_FLOATARR\n");
			F_D_VAR_FLOATARR(interp,"",OUTPUT);
			break;
		case D_VAR_DOUBLEARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_DOUBLEARR\n");
			F_D_VAR_DOUBLEARR(interp,"",OUTPUT);
			break;
		case D_VAR_FRPARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_FRPARR\n");
			F_D_VAR_FRPARR(interp,"",OUTPUT);
			break;
		case D_VAR_SFRPARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_SFRPARR\n");
			F_D_VAR_SFRPARR(interp,"",OUTPUT);
			break;
		case D_VAR_LRPARR		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_VAR_LRPARR\n");
			F_D_VAR_LRPARR(interp,"",OUTPUT);
			break;
		case D_OPAQUE_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_OPAQUE_TYPE\n");
			F_D_OPAQUE_TYPE(interp,"",OUTPUT);
			break;
		case D_MULMOVE_TYPE		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_MULMOVE_TYPE	\n");
			F_D_MULMOVE_TYPE(interp,"",OUTPUT);
			break;
		case D_MOTOR_LONG		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_MOTOR_LONG\n");
			F_D_MOTOR_LONG(interp,"",OUTPUT);
			break;
		case D_MOTOR_FLOAT		:
			if(TCL_TEST) 
				printf("dev_io: ArgoutRequired= D_MOTOR_FLOAT\n");
			F_D_MOTOR_FLOAT(interp,"",OUTPUT);
			break;
		default				:
			printf ("ERROR during the output argument setting.. \n");
			free (StringTemp); 
			return TCL_ERROR;
			break;
	}
       
/* ------------------------------ */
/* We call dev_putget  !!!        */
/* ------------------------------ */

	if (GetIoMode()==SYNCHRONOUS_MODE)
	{
		if (TCL_TEST) 
			printf("Call dev_putget...%d\n",Cmd);
			DeviceStatus = dev_putget(CurrentDevice->ds,Cmd, InputArg,ArginRequired,
                              OutputArg,ArgoutRequired, &DeviceError);
/* GP TESTS */
		if (TCL_TEST) 
		{
			printf("dev_putget returns: error=%d :%s\n",DeviceError,StringTemp);
                	fflush(stdout);
		}
	}
	else	
	{
		SetIoMode(SYNCHRONOUS_MODE);
		DeviceStatus = dev_put_asyn(CurrentDevice->ds,Cmd, InputArg,ArginRequired, &DeviceError);
	}
   
	snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
	Tcl_SetVar (interp,State,StringTemp,0);
	if(DeviceError != 0) 
	{
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
	}
	if (TCL_TEST) 
		printf("dev_putget returns: error=%d :%s\n",DeviceError,StringTemp);

	if (DeviceStatus != DS_OK) /* Error during dev_putget */
	{
		free (StringTemp); 
		free (StringAux);
		if (OutputArg != NULL) 
		{
			free (OutputArg); 
			OutputArg=NULL;
		} 
		return TCL_OK;
	}

	if(StringAux != NULL) 
		free (StringAux); 
 
/* ------------------------------------------- */
/* Now converting C variables in TCL variables */
/* ------------------------------------------- */
	if (TCL_TEST) 
		printf("Converting the output argument in the tcl format...\n");
     				
	Tcl_ResetResult (interp);
	switch (ArgoutRequired)
	{
		case D_VOID_TYPE:
			break;
		case D_BOOLEAN_TYPE      	:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((char*)OutputArg));        
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_USHORT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((unsigned short*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_SHORT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((short*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_ULONG_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((unsigned long*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_LONG_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((long*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_FLOAT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%f",*((float*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL); 
			break;
		case D_DOUBLE_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
 /*************************************************************************/                
			snprintf(TclAux, TCLAUX_LENGTH, "%17.8e",*((double*)OutputArg));
/*			printf("TclAux=%17.8e\n",*((double*)OutputArg));       */
/*			fflush stdout;                                         */
/**************************************************************************/                    
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_STRING_TYPE:
			Tcl_AppendResult(interp,StringTypeOut,NULL);
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}
			break;
		case D_INT_FLOAT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",(((DevIntFloat*)OutputArg)->state));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevIntFloat*)OutputArg)->value));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_FLOAT_READPOINT:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevFloatReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevFloatReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_STATE_FLOAT_READPOINT:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",(((DevStateFloatReadPoint*)OutputArg)->state));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%g",(((DevStateFloatReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%g",(((DevStateFloatReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_LONG_READPOINT:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",(((DevLongReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%d",(((DevLongReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL); 
			break;
		case D_DOUBLE_READPOINT	:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%g",(((DevDoubleReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevDoubleReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL); 
			break;
		case D_VAR_CHARARR:
			j = ((DevVarCharArray*)OutputArg)->length;      
			TclAux = (char *) malloc (j * sizeof(char) + 1 );
			strncpy ((char*)TclAux,(char *)((DevVarCharArray*)OutputArg)->sequence,j);
			*(TclAux+j) = '\0';
			Tcl_AppendResult(interp,TclAux,NULL);
		
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
       			}
       			break;
		case D_VAR_STRINGARR:
			j = ((DevVarStringArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				StringArray = (DevString*)( (((DevVarStringArray*)OutputArg)->sequence+i) );
				Tcl_AppendElement(interp,*StringArray);
			}                
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}
			break;
		case D_VAR_USHORTARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarUShortArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (u_short*)( ((DevVarUShortArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",*((unsigned short*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_SHORTARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarShortArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (short*)( ((DevVarShortArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",*((short*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_ULONGARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarULongArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (unsigned long*)( ((DevVarULongArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",*((unsigned long*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_LONGARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarLongArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (long*)( ((DevVarLongArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",*((long*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_FLOATARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarShortArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (float*)( ((DevVarFloatArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",*((float*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_DOUBLEARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarDoubleArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (double*)( ((DevVarDoubleArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%17.8e",*((double*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_FRPARR:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarFloatReadPointArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (DevFloatReadPoint*)(((DevVarFloatReadPointArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevFloatReadPoint*)TclTemp)->set);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevFloatReadPoint*)TclTemp)->read);
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_SFRPARR:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarStateFloatReadPointArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (DevFloatReadPoint*)(((DevVarStateFloatReadPointArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",((DevStateFloatReadPoint*)TclTemp)->state);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevStateFloatReadPoint*)TclTemp)->set);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevStateFloatReadPoint*)TclTemp)->read);
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_LRPARR:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarLongReadPointArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (DevLongReadPoint*)(((DevVarLongReadPointArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",((DevLongReadPoint*)TclTemp)->set);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",((DevLongReadPoint*)TclTemp)->read);         
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_OPAQUE_TYPE:
			j = ((DevOpaque*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclAux = (char*)(((DevOpaque*)OutputArg)->sequence+i);
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_LENGTH, "%d",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_LENGTH, "%s [%d]",StringAux,DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}
			break;
		case D_MULMOVE_TYPE:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			for (i=0;i<8;i++)
			{
				snprintf(TclAux, TCLAUX_LENGTH, "%d",&( ((DevMulMove*)OutputArg)->action[i]));
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
			for (i=0;i<8;i++)
			{
				snprintf(TclAux, TCLAUX_LENGTH, "%d",&( ((DevMulMove*)OutputArg)->delay[i]));
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
			for (i=0;i<8;i++)
			{
				snprintf(TclAux, TCLAUX_LENGTH, "%f",&( ((DevMulMove*)OutputArg)->position[i]));
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
       			break;
		case D_MOTOR_LONG: 
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",&( ((DevMotorLong*)OutputArg)->axisnum));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%d",&( ((DevMotorLong*)OutputArg)->value));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_MOTOR_FLOAT:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",&( ((DevMotorFloat*)OutputArg)->axisnum));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",&( ((DevMotorFloat*)OutputArg)->value));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		default:
			printf ("ERROR, output argument type is unknown.. \n");
			free (StringTemp); 
			return TCL_ERROR;
			break;
       }

 
	if (TCL_TEST) 
	{
		printf("free the memory....\n");
		printf("...................TclAux %X\n",TclAux);
	}
	if (TclAux != NULL) 
		free (TclAux);
	if (TCL_TEST) 
		printf("...................SequencePointer %X\n",SequencePointer);
	if (SequencePointer != NULL) 
		free (SequencePointer);
	if (TCL_TEST) 
		printf("...................StringTemp %X\n",StringTemp);
	if (StringTemp != NULL) 
		free (StringTemp);
	if (TCL_TEST) 
		printf("...................StringAux %X\n",StringAux);
	if (StringAux != NULL) 
		free (StringAux);
	if (TCL_TEST) 
		printf("...................StringType %X\n",StringType);
	if (StringType != NULL) 
		free (StringType); 

	if (ArginRequired == D_VAR_STRINGARR) 
	{
		if (TCL_TEST) 
			printf("...................ArginRequired == D_VAR_STRINGARR\n");
		j = ((DevVarStringArray*)InputArg)->length;
		for (i=0;i<j;i++)
		{
			if (TCL_TEST) 
      				printf("...................Free %X\n",((DevVarStringArray*)InputArg)->sequence+i);
			free(((DevVarStringArray*)InputArg)->sequence+i);
		}  
	}

/*GP: Linux pbs*/
	if (ArginRequired != D_STRING_TYPE) 
	{
		if (TCL_TEST) 
			printf("...................InputArg %X\n",InputArg);
 
		if (InputArg != NULL) 
		{
			free (InputArg); 
			InputArg=NULL;
		}
	}

	if (ArgoutRequired != D_STRING_TYPE) 
	{
		if (TCL_TEST) 
			printf("...................OutputArg %X\n",OutputArg); 
		if (OutputArg != NULL) 
		{
			free (OutputArg); 
			OutputArg=NULL;
		}
	}

	if (TCL_TEST) 
		printf("End of free\n");

	if (TCL_TEST) 
		printf("leaving dev_io function...\n");

	return TCL_OK; 
}  /* ends function dev_io */ 



              
                   


                   
             
            
