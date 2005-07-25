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
 * File:	MDSSignal.c
 *
 * Project:	Multi Signal Device Server
 *
 * Description:	Device Server Class which is a top Layer of DSSignal
 		Class to handle multiple signals.
 *
 * Author(s):	Ingrid Fladmark
 * 		$Author: jkrueger1 $
 *
 * Original:	24th of january 1997
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 12:59:10 $
 *
 *********************************************************************/

#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <Admin.h>

#include <DevServerP.h>
#include <MDSSignalP.h>
#include <MDSSignal.h>
#include <stdlib.h>
#include <stdio.h>
/*
 * public methods
 */

static long class_initialise(long *error);
static long object_create(char *name,DevServer *ds_ptr,long *error);
static long object_destroy      (MDSSignal ds, long *error);
static long object_initialise(MDSSignal ds,char *dev_class_name, long *error);
static long read_properties(MDSSignal ds,DevVarStringArray *properties,long *error);
static long check_alarms(MDSSignal ds,DevVarDoubleArray *values,DevVarLongArray *alarm_state, long *error);
static long read_alarms(MDSSignal ds,DevVarStringArray *alarm_mess,long *error);
static long signals_reset(MDSSignal ds, long *error);
static long get_signal_object (MDSSignal ds, char *signal_name, 
			       Dssignal *signal_object, long *error);

static	DevMethodListEntry methods_list[] = {
   	{DevMethodClassInitialise,	(DevMethodFunction)class_initialise},
   	{DevMethodInitialise,		(DevMethodFunction)object_initialise},
	{DevMethodCreate,		(DevMethodFunction)object_create},
        {DevMethodDestroy,              (DevMethodFunction)object_destroy},
   	{DevMethodReadProperties, 	(DevMethodFunction)read_properties},
   	{DevMethodCheckAlarms, 		(DevMethodFunction)check_alarms},
   	{DevMethodReadAlarms, 		(DevMethodFunction)read_alarms},
   	{DevMethodSignalsReset, 	(DevMethodFunction)signals_reset},
   	{DevMethodGetSigObject, 	(DevMethodFunction)get_signal_object},
   };


MDSSignalClassRec mDSSignalClassRec = {
   /* n_methods */        sizeof(methods_list)/sizeof(DevMethodListEntry),
   /* methods_list */     methods_list,
   };

MDSSignalClass mDSSignalClass = (MDSSignalClass)&mDSSignalClassRec;

/*
 * public commands
 */

static	DevCommandListEntry commands_list[] = {
   	{0, NULL, 0, 0, 0},
};

static long n_commands = sizeof(commands_list)/sizeof(DevCommandListEntry);

/*
 * reserve space for a default copy of the mDSSignal object
 */

static MDSSignalRec mDSSignalRec;
static MDSSignal mDSSignal = (MDSSignal)&mDSSignalRec;

/*
 * MDSSignal resource tables used to access the static database
 *
 */

static db_resource res_object[] = {
   {"signal_names",	D_VAR_STRINGARR, NULL},
   	};
static int res_object_size = sizeof(res_object)/sizeof(db_resource);

/*
 * Global variables to be freed in object_destroy()

static DevVarStringArray	*ds->mDSSignal.signals_properties = NULL;
static char			**ds->mDSSignal.property_ptr = NULL;
static long			*ds->mDSSignal.alarm_result = NULL;
 */

/*======================================================================
 Function:      static long class_initialise()

 Description:	routine to be called the first time a device is 
 		created which belongs to this class (or is a subclass
		thereof. This routine will be called only once.

 Arg(s) In:	none

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
 =======================================================================*/

static long class_initialise(long *error)
{

/*
 * MDSSignalClass is a subclass of the DevServerClass
 */

   mDSSignalClass->devserver_class.superclass = devServerClass;
   mDSSignalClass->devserver_class.class_name = (char*)malloc(sizeof("MDSSignalClass")+1);
   sprintf(mDSSignalClass->devserver_class.class_name,"MDSSignalClass");
   mDSSignalClass->devserver_class.class_inited = 1;
   mDSSignalClass->devserver_class.n_commands = n_commands;
   mDSSignalClass->devserver_class.commands_list = commands_list;

/*
 * initialise mDSSignal with default values. these will be used
 * for every MDSSignal object created.
 */

   	mDSSignal->devserver.class_pointer = (DevServerClass)mDSSignalClass;
	mDSSignal->devserver.state                 = DEVON;
	mDSSignal->mDSSignal.signal_names.length   = 0;
	mDSSignal->mDSSignal.signal_names.sequence = NULL;
	mDSSignal->mDSSignal.signal_obj            = NULL;
	mDSSignal->mDSSignal.signals_properties    = NULL;
	mDSSignal->mDSSignal.property_ptr          = NULL;
	mDSSignal->mDSSignal.alarm_result          = NULL;

/*
 * Interrogate the static database for default values
 *
 */

   return(DS_OK);
}

/*======================================================================
 Function:	static long object_create()

 Description:	routine to be called on creation of a device object

 Arg(s) In:	char *name - name to be given to device

 Arg(s) Out:	DevServer *ds_ptr - pointer to created device
		long *error - pointer to error code, in case routine fails
 =======================================================================*/

static long object_create(char *name,DevServer *ds_ptr,long *error)
{
   MDSSignal ds;

   ds = (MDSSignal)malloc(sizeof(MDSSignalRec));

/*
 * initialise device with default object
 */

   *(MDSSignalRec*)ds = *(MDSSignalRec*)mDSSignal;

/*
 * finally initialise the non-default values
 */

   ds->devserver.name = (char*)malloc(strlen(name)+1);
   sprintf(ds->devserver.name,"%s",name);

   *ds_ptr = (DevServer)ds;

   return(DS_OK);
}



/*============================================================================

Function:       static long object_destroy()

Description:    routine to be called to destroy a device object

Arg(s) In:      Dssignal ds     - object to destroy

Arg(s) Out:     long *error     - pointer to error code, in case routine fails
=============================================================================*/

static long object_destroy (MDSSignal ds, long *error)

{
	u_long		no_of_signals;
	short		i;


        *error = 0;

#ifdef EBUG
        dev_printdebug (DBG_METHODS | DBG_TRACE,
                        "\nobject_destroy() : entering routine\n");
#endif /* EBUG */

	/*
	 * Free memory allocated in dev_read_properties()
	 */

        no_of_signals = ds->mDSSignal.signal_names.length;

   	if (ds->mDSSignal.signals_properties != NULL)
	   {
	   for (i = 0; i < no_of_signals; i++)
	   free_var_str_array(&ds->mDSSignal.signals_properties[i]);
	   free(ds->mDSSignal.signals_properties);
	   free(ds->mDSSignal.property_ptr);
	   }

	/*
	 * Free memory allocated in dev_check_alarms()
	 */

        if (ds->mDSSignal.alarm_result != NULL)
		free(ds->mDSSignal.alarm_result);

	/*
	 * Destroy all created signal objects.
	 */

        /* destroy an object for each defined signal */
        for (i = 0; i < no_of_signals; i++)
           {
	   if (ds__method_finder(ds->mDSSignal.signal_obj[i],
				 DevMethodDestroy)
				 (ds->mDSSignal.signal_obj[i], 
				  error) == DS_NOTOK) 
              {
              printf("Failed to destroy signal object %s\n", 
		      ds->mDSSignal.signal_names.sequence[i]);
              }
	   }

	/*
	 * Free the signal names list allocated by db_getresource()
	 */

        for (i = 0; i < no_of_signals; i++)
	   {
           free_var_str_array (&ds->mDSSignal.signal_names);
           }


        /*
         * Free fields of signal part
         */

        free (ds->mDSSignal.signal_obj);

        /*
         * Free fields of devserver part
         */

        free (ds->devserver.name);

        /*
         * free the object structure
         */

        free (ds);

#ifdef EBUG
        dev_printdebug (DBG_METHODS | DBG_TRACE,
                        "object_destroy() : leaving routine\n");
#endif /* EBUG */

   return(DS_OK);
}




/*============================================================================

Function:	static long object_initialise()

Description:	routine to be called on initialisation of a device object

Arg(s) In:	MDSSignal ds	- object to initialise

Arg(s) Out:

		long *error     - pointer to error code, in case routine fails
=============================================================================*/
static long object_initialise(MDSSignal ds,char *dev_class_name, long *error)
{
	char		class_res_name[80];
	char		signal_name[80];
	u_long		no_of_signals;
	short		i;
	char            nethost[128];

	/*
	 * Read the list of signals for the specified device class to 
	 */

   res_object[0].resource_adr = &(ds->mDSSignal.signal_names);
	
   if( strncmp( ds->devserver.name , "//" , 2 )==0 ) {
     strcpy(nethost,&(ds->devserver.name[2]));
     *(strchr(nethost,'/'))='\0';
     sprintf (class_res_name, "//%s/CLASS/%s/DEFAULT",
     nethost,dev_class_name);
   } else {
     sprintf (class_res_name, "CLASS/%s/DEFAULT", dev_class_name);
   }

   if ( db_getresource (class_res_name, res_object, 
								res_object_size, error))
         {
		   printf("failed to read signal list for class , MDSSignal \n");
   	   return(DS_NOTOK);
         }
	
	/*
	 * Try to read a list of signals for the device only if no list
	 * was specified for the class.
	 * to keep it compatible with the old version.
	 */

	if ( ds->mDSSignal.signal_names.length == 0 )
	   {
      if ( db_getresource (ds->devserver.name, res_object, 
								   res_object_size, error))
         {
		   printf("failed to get resources , MDSSignal \n");
   	   return(DS_NOTOK);
         }
	   }

	no_of_signals = ds->mDSSignal.signal_names.length;

	   /*
    	 * Stop the initialisation of the multi signal object 
    	 * if no signal names are defined. A list of signal names
    	 * must be defined for the device class or the object.
    	 */

   	if ( no_of_signals == 0 )
      	   {
	   fprintf(stderr,"No list of signal names is specified for the class");
           *error = DevErr_SignalListNotFound;
           return (DS_NOTOK);
           }

	ds->mDSSignal.signal_obj = (Dssignal *)malloc
										(no_of_signals*sizeof(Dssignal*)+1);
	if (ds->mDSSignal.signal_obj == 0)
	{
		printf("can't allocate memory for signal structures -- exit \n");
		*error = DevErr_InsufficientMemory;
	   return (DS_NOTOK);
	}

	/* create an object for each defined signal */
	for (i = 0; i < no_of_signals; i++)
	{
		/* create signal name and store it */
		sprintf(signal_name,"%s/%s",
				  ds->devserver.name,ds->mDSSignal.signal_names.sequence[i]);
#ifdef PRINT
		printf("signal_name MDSSignal: %s \n",signal_name);
#endif /*PRINT*/
		
		if (ds__create(signal_name, dssignalClass,
				&ds->mDSSignal.signal_obj[i], error) == DS_NOTOK)
		{
			printf("failed on ds_create  , MDSSignal \n");
			return(DS_NOTOK);
		}

		if (ds__method_finder(ds->mDSSignal.signal_obj[i],
					DevMethodInitialise)
				(ds->mDSSignal.signal_obj[i], 
				dev_class_name, 
				error) == DS_NOTOK) 
		{
			printf("failed to DevMethodInitalise , MDSSignal \n");
			return(DS_NOTOK);
		}
	}
   return(DS_OK);
}


/*============================================================================
 Function:      static long read_properties()

 Description:	 Method to read an array of signal properties for each
 		defined signal.
   	
 Arg(s) In:	 MDSSignal 	ds 	- 
		 DevVoid  	*argin  - None
   				  
 Arg(s) Out:	 DevVarStringArray	*argout - A list of properties for all
					defined signals.
		 long		*error	- pointer to error code, in case
		 			routine fails. Error code(s):

 ============================================================================*/

static long  read_properties(MDSSignal ds,DevVarStringArray *properties,long *error)
{
#ifdef PRINT
   printf("MDSSignal, read_properties(), entered\n");
#endif /*PRINT*/

	static char			nu_of_properties[10];
        DevVarStringArray               *help_ptr;
	u_long				no_of_signals = 0;
	short				length;
	short				str_number;
	short				counter;
	short				i, j;

	*error = 0;
	no_of_signals = ds->mDSSignal.signal_names.length;

	/* Free static string arrays */

   	if (ds->mDSSignal.property_ptr != NULL)
	      free(ds->mDSSignal.property_ptr);

	if (ds->mDSSignal.signals_properties != NULL)
	{
		for (i = 0; i < no_of_signals; i++)
 		   {
		   free_var_str_array(&ds->mDSSignal.signals_properties[i]);
		   }
		free(ds->mDSSignal.signals_properties);

	}

	/* allocate new memory for signal properties */

	ds->mDSSignal.signals_properties = (DevVarStringArray *)malloc
			     (no_of_signals*sizeof(DevVarStringArray));
	if ( ds->mDSSignal.signals_properties == 0)
	{
		printf("can't alocate memory for signal structures -- exit \n");
		*error = DevErr_InsufficientMemory;
		return(DS_NOTOK);
	}

	for (i = 0 ; i < no_of_signals; i++)
	{
		ds->mDSSignal.signals_properties[i].length = 0;
		ds->mDSSignal.signals_properties[i].sequence = NULL;
	}

	/*
	 * Copy the pointer to the string array before calling the
	 * methode. This avoids a strange error with the ultra C compiler!
	 */

	help_ptr = ds->mDSSignal.signals_properties;

	/* Read properties for each signal */
	for (i = 0; i < no_of_signals ; i++)
	{
		if (ds__method_finder(ds->mDSSignal.signal_obj[i],
				      DevMethodReadProperties)
				      (ds->mDSSignal.signal_obj[i],
				      &(help_ptr[i]), 
				      error) == DS_NOTOK)
		{
			return(DS_NOTOK);
		}
	}

	/* allocate pointers for the string array to return */ 

	length = ds->mDSSignal.signals_properties[0].length;

	str_number = (length * no_of_signals) + 1;
	ds->mDSSignal.property_ptr = 
				(char **) calloc(str_number, sizeof(char *));
	sprintf(nu_of_properties,"%d",length);

	ds->mDSSignal.property_ptr[0] = nu_of_properties;

	counter = 1;

	for (i = 0; i < no_of_signals; i++)
	{
		for (j = 0; j < length; j++)
		{
			ds->mDSSignal.property_ptr[counter] = 
				ds->mDSSignal.signals_properties[i].sequence[j];
			counter++;
		}
	}

	/* return result */
	properties->length   = counter;
	properties->sequence = ds->mDSSignal.property_ptr;

   return(DS_OK);
}

/*============================================================================
 Function:      static long check_alarms()

 Description:	 Check for each defined signal if it's value is in alarm
 		state.
   	
 Arg(s) In:	 MDSSignal 	ds 	- 
		   	*argin  - List of values to check, in same
					order as the signal list.
   				  
 Arg(s) Out:	 DevLong	*argout - Alarm state or OK.
		 long		*error	- pointer to error code, in case
		 			routine fails. Error code(s):

 ============================================================================*/

static long  check_alarms(MDSSignal ds, DevVarDoubleArray *values,DevVarLongArray *alarm_state,long *error)
{
	long no_of_values,i,alarm_read;
	long iret = False;

	*error = 0;

#ifdef PRINT
   printf("MDSSignal, check_alarms(), entered\n");
#endif /*PRINT*/

	no_of_values = values->length;
	if (ds->mDSSignal.alarm_result != NULL)
		free(ds->mDSSignal.alarm_result);

	ds->mDSSignal.alarm_result = (long *)malloc(no_of_values*sizeof(long));
	if (ds->mDSSignal.alarm_result == 0)
	{
		printf("can't allocate memory for ds->mDSSignal.alarm_result array --NOTOK \n");
		*error = DevErr_InsufficientMemory;
		return(DS_NOTOK);
	}

	for (i = 0; i < no_of_values ; i++)
	{
		if (ds__method_finder(ds->mDSSignal.signal_obj[i],
			DevMethodCheckAlarm)
			(ds->mDSSignal.signal_obj[i],
			values->sequence[i],&ds->mDSSignal.alarm_result[i], 
			error) == DS_NOTOK)
		{
			return(DS_NOTOK);
		}
		if (ds->mDSSignal.alarm_result[i] != DEVRUN)
			iret = True;
	}

	alarm_state->length = no_of_values;
	alarm_state->sequence = &ds->mDSSignal.alarm_result[0];

   return(iret);
}

/*============================================================================
 Function:      static long read_alarms()

 Description:	 Method to read alarm messages found for defined
 		signals.
   	
 Arg(s) In:	 MDSSignal 	ds 	- 
		 DevVoid  	*argin  - None
   				  
 Arg(s) Out:	 DevVarStringArray	*argout - List of alarmmessages, if alarms
					for defined signals occured and
					not reset.
		 long		*error	- pointer to error code, in case
		 			routine fails. Error code(s):

 ============================================================================*/

static long  read_alarms(MDSSignal ds,DevVarStringArray *alarm_mess,long *error)
{
	static char			**alarm_tab   = NULL;
	static u_long 		no_of_signals = 0;
	char 					*alarm_msg    = NULL;
	short					i,j,length;

#ifdef PRINT
   printf("MDSSignal, read_alarms(), entered\n");
#endif /*PRINT*/

	/* free space if already allocated, before reallocating */
	if (alarm_tab != NULL)
	   {
	   for (i = 0; i < no_of_signals; i++)
	      {
		   if (alarm_tab[i] != NULL)
		      {
		      free(alarm_tab[i]);
		      alarm_tab[i] = NULL;
		      }
	      }
		free (alarm_tab);
	   }

	no_of_signals = ds->mDSSignal.signal_names.length;

	alarm_tab = (char **)calloc(no_of_signals,sizeof(char *));
	if (alarm_tab == 0)
	{
#ifdef PRINT
		printf("can't allocate memory for alarm_message array --NOTOK \n");
#endif /*PRINT*/
		*error = DevErr_InsufficientMemory;
		return(DS_NOTOK);
	}
		
	/* read alarm message if any, for each device */ 	
	j = 0;
	for (i = 0; i < no_of_signals ; i++)
	{
		alarm_msg = NULL;
		if (ds__method_finder(ds->mDSSignal.signal_obj[i],
							DevMethodReadAlarm)
							(ds->mDSSignal.signal_obj[i],
							&alarm_msg, error) == DS_NOTOK)
		{
			return(DS_NOTOK);
		}

		if (alarm_msg != NULL)
		{
			alarm_tab[j] = alarm_msg;
			j++;
		}
	}

	
	/* result */
	alarm_mess->length = j;
	alarm_mess->sequence = alarm_tab;

   return(DS_OK);
}

/*============================================================================
 Function:      static long signals_reset()

 Description:	 Method to reintialise the signal properties for each
 		defined signals.
   	
 Arg(s) In:	 MDSSignal 	ds 	- 
		 DevVoid  	*argin  - None
   				  
 Arg(s) Out:	 DevVoid	*argout - None
		 long		*error	- pointer to error code, in case
		 			routine fails. Error code(s):

 ============================================================================*/

static long  signals_reset(MDSSignal ds,long *error)
{
	short i, no_of_signals;
#ifdef PRINT
   printf("MDSSignal, signals_reset(), entered\n");
#endif /*PRINT*/

	no_of_signals = ds->mDSSignal.signal_names.length;
	
	for (i = 0; i < no_of_signals ; i++)
	{
		if (ds__method_finder(ds->mDSSignal.signal_obj[i],
							DevMethodSignalReset)
							(ds->mDSSignal.signal_obj[i], error) == DS_NOTOK)
		{
			return(DS_NOTOK);
		}
	}

   return(DS_OK);
}




/*============================================================================
 Function:      static long get_signal_object()

 Description:	Methode to extract the pointer to a signal object 
		from a multi signal object.
   	
 Arg(s) In:	MDSSignal 	    ds - multi signal object
		char      *signal_name - name of the signal to extract
   				  
 Arg(s) Out:	Dssignal *signal_object - extracted signal object
		long		*error - pointer to error code, in case
		 			 routine fails.
 ============================================================================*/

static long get_signal_object (MDSSignal ds, char *signal_name, 
			       Dssignal *signal_object, long *error)
{
	u_long	i;

	*error        = 0;
	*signal_object = NULL;

	/*
	 * Loop over all signals known in the multi signal object and compare
	 * the name with the searched signal name.
	 */

	for (i=0; i<ds->mDSSignal.signal_names.length; i++)
	   {
	   if ( strcasecmp (ds->mDSSignal.signal_obj[i]->devserver.name, 
		        signal_name) == 0 )
	      {
	      *signal_object = ds->mDSSignal.signal_obj[i];
	      return (DS_OK);
	      }
	   }

	*error = DevErr_SignalNameNotFound;
	return(DS_NOTOK);
}
