/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File : 	db_io_get.c
 *
 * Project:     TACO TCL bindings
 *
 * Description:	This file contains the source code for the  new command
 * 		db_io_get in a tcl interpreter
 * 		This new command is for getting information from the data base
 * 		of the esrf.
 * 		the syntax is :
 * 	    		db_io_get status error device_name [list res_name res_name ... res_name]
 *
 * Author(s):   Gilbert Pepellin
 *              J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:	Nov 1997
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-06 09:06:33 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/*SUPPRESS569*/
/*SUPPRESS610*/
/*SUPPRESS65*/
#include <db_io.h>
#include <debug.h>

#define MAXRES 250
/* Global variables */
/* ---------------- */
static int DB_DEBUG_MODE = DB_DEBUG_0;
 

/* ------------------------------------------ */
/* Function GetDBDebugMode                    */
/* Argin	: none                              */
/* Argout	: int (DB_DEBUG_0 DB_DEBUG_1        */
/* Description	:                             */
/*        Return the flag determining the     */
/*        level of debugging                  */
/* ------------------------------------------ */
int GetDBDebugMode ()
{
 return DB_DEBUG_MODE;
}

/* ------------------------------------------ */
/* Function SetDBDebugMode                    */
/* Argin	: int                         */
/* Argout	: int (False or True)         */
/* Description	:                             */
/*        Set the flag determining the        */
/*        level of debugging                  */
/* ------------------------------------------ */
int SetDBDebugMode (Argin)
    int Argin;
{
 switch (Argin)

        {
         case DB_DEBUG_0 :
                          DB_DEBUG_MODE = DB_DEBUG_0;
                          return True;
                          break;
         case DB_DEBUG_1 :
                          DB_DEBUG_MODE = DB_DEBUG_1;
                          return True;
                          break;
         default         :
                          return False;
                          break;
        }
        
}

/* ----------------------------------- */
/* Function GetType                    */
/* argin  : char *                     */
/* argout : DevType                    */
/* Description :                       */
/*       Convert a string to a devType */
/*       If an error occurs, -1 is     */
/*       returned                      */
/* ----------------------------------- */
DevType GetType (Name)
        char *Name;
{
 return 1;

}
                          

/* =========================================== */
/* ++++ ENTRY POINT: Function db_io_get        */
/* =========================================== */

int db_io_get (ClientData clientdata, Tcl_Interp *interp, int argc, char **argv)
/*SUPPRESS761*/
{
/* pt_dbresource is a pointer to a structure of type _db_resource*/
	db_resource 	tabres[MAXRES]; 
/* local variables that hold the arguments of db_io_get*/
	char 		*State,
			*Error,
			*DeviceName;
	int 		resnumber;
	DevVarStringArray tabres_str[MAXRES];
	char 		**LRes;  /* will be allocated by Tcl_SplitList. Need to be freeed */
 
/* status and error returned by db_getresource */
	int 		db_status;
	DevLong 	db_error;
	char 		*db_serr;
/* some working variables*/
	int 		i,
			j,
			ret;
	char 		TmpString[250];

/* to return values to Tcl env */
	char 		*sub_list;
	int 		nb_elemt;
	
/* --------------------------- */
/* Setting debug mode          */
/* --------------------------- */
	if (GetDBDebugMode() != DB_DEBUG_0) 
		TCL_TEST=True; 
  
/* ------------------------------*/
/* Check the number of arguments */
/* ------------------------------*/ 
	if (TCL_TEST) 
	{
		printf ("db_io_get. entering...\n");
		printf ("db_io_get. Start argument line parsing\n");
		printf ("db_io_get. Check number of arguments\n"); 
	}
/* Usage: db_io_get status error dev_name [list res1 res2 ... resn] */
	if (argc != 5)
	{
		if (TCL_TEST) 
			printf("Bad argument number. should be 5\n");
		Tcl_AppendResult(interp,"Bad argument number. Usage:\n","db_io_get 'status' 'error' 'dev_name' '[list r1 r2 ... rn]' ",NULL);
		return TCL_ERROR;
	}

	State        = argv[1];
	Error        = argv[2];
	DeviceName   = argv[3];

/* get resource names from the list argv[4] */
/* set the array LRes (List Of Requested resources) */
/* set resnumber: number of requested resources */
	ret = Tcl_SplitList(interp,argv[4],&resnumber,&LRes);
	if(ret != TCL_OK) 
		printf("db_io_get. +++++++ Syntax error in res list\n");
	if(TCL_TEST) 
	{
		printf("db_io_get. number of requested resources= %d\n",resnumber);
		for(i=0; i<resnumber; i++) 
			printf("db_io_get. res#%d= mem_allocated= %p, name= %s\n",i+1,(char*)LRes[i],(char*)LRes[i]);
		printf ("db_io_get. Parsing argument line...DONE\n");
	}
 
/* ----------------------------------- */
/* Initialization of the tcl variables */
/* and dynamical variables             */
/* ----------------------------------- */ 

	if (TCL_TEST) 
		printf ("db_io_get. Initialization of dynamical variables...\n");

	Tcl_ResetResult(interp);
	Tcl_SetVar(interp,State,"0",0);
	Tcl_SetVar(interp,Error," ",0);
 
/* Init structure tabres */
	for(i=0; i<resnumber; i++) 
	{
		tabres[i].resource_name = (char*)(LRes[i]);
		tabres[i].resource_type = D_VAR_STRINGARR;
		tabres[i].resource_adr = &tabres_str[i];
		tabres_str[i].length = 0;
		tabres_str[i].sequence = NULL;
	}
 
/* call db_getresource */
	if(TCL_TEST) 
		printf("db_io_get. Call db_getresource\n");
	db_status = db_getresource(DeviceName, &tabres[0], resnumber, &db_error); 
	if(db_status != DS_OK) 
	{
		snprintf (TmpString, sizeof(TmpString), "%d",db_status);
		Tcl_SetVar(interp,State,TmpString,0);
		snprintf( TmpString ,  sizeof(TmpString), "%s [%ld]" , db_serr= dev_error_str(db_error) , (long)db_error); 
		Tcl_SetVar (interp,Error,TmpString,0);
		if (TCL_TEST) 
			printf("db_io_get: db_getresource failed. error:\n %s",db_serr);
		free(db_serr);
		return TCL_OK;   
	}
 
/* db_getresource succeeded */ 
/* Pass value to Tcl environment */ 
	if(TCL_TEST) 
	{
		printf("db_io_get. db_getresource succeeded\n");
	 	printf("db_io_get. transfert results into Tcl environment...\n");
	}
	for(i= 0; i<resnumber; i++) 
	{
/* Check if this resource value is either a scalar or a variable string array */
		nb_elemt=tabres_str[i].length;
		if(TCL_TEST) 
			printf("db_io_get.... resource#%d, nb_elemt= %d\n",i,nb_elemt);
		if(nb_elemt == 1) 
		{
/* it is a scalar value */
			snprintf( TmpString, sizeof(TmpString), "%s",(char*)(tabres_str[i].sequence[0]) );
			if(TCL_TEST) 
				printf("............. name: %s value: %s\n",tabres[i].resource_name,TmpString);
			Tcl_AppendElement( interp,(char*)(tabres_str[i].sequence[0]) );
		}
		else 
		{
/* We need to build a list before appending it to the result */
			if(TCL_TEST) 
				printf("............. name: %s \n",tabres[i].resource_name);
			sub_list = Tcl_Merge(nb_elemt,(char**)tabres_str[i].sequence);
			Tcl_AppendElement( interp,(char*)sub_list);
			Tcl_Free((char*)sub_list);
		}
	} /* ends  for(i= 0; i<resnumber; i++) */  
	
/* Free dynamic allocation done by Tcl_SplitList */
	if(TCL_TEST) 
		printf("db_io_get. Free memory allocated by tcl_SplitList\n");
	Tcl_Free( (char*)LRes);
	 	 
/* Free dynamic allocation done by db_getresource */
	if(TCL_TEST) 
		printf("db_io_get. Free memory allocated by db_getresource\n");
	for(i= 0; i<resnumber; i++) 
	{
		for(j=0; j<tabres_str[i].length; j++) 
			free( (char*)(tabres_str[i].sequence[j]) );		 
		free( (char*)(tabres_str[i].sequence) );
	}  
 
	return TCL_OK;
} /* ends function db_io_get */       
