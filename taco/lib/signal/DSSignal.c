
/*********************************************************************
 *
 *File:		DSSignal.c
 *
 *Project:	SRRF3 control system
 *
 *Description:	A class to specify signal properties and signal treatment.
 *
 *Author(s):	J.Meyer
 *
 *Original:	June 1996
 *
 *Copyright(c) 1996 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#include <API.h>
#include <DevErrors.h>
#include <Admin.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <DSSignalP.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

/*
 * public methods
 */

static long class_initialise    (long *error);
static long object_create       (char *name, DevServer *ds_ptr, long *error);
static long object_destroy      (Dssignal ds, long *error);
static long object_initialise   (Dssignal ds, char *dev_class_name,
		                 long *error);
static long read_properties     (Dssignal ds, DevVarStringArray *properties,
		                 long *error);
static long check_alarm         (Dssignal ds, double read_value,
		                 long *alarm_state, long *error);
static long read_alarm          (Dssignal ds,  char **alarm_msg, long *error);
static long check_limits        (Dssignal ds, double set_value,
		                 long *limit_state, long *error);
static long reset               (Dssignal ds, long *error);
static long get_description     (Dssignal ds,
		                 DevVarStringArray *description_list,
				 long *error);

static	DevMethodListEntry methods_list[] = {
   	{DevMethodClassInitialise,	(DevMethodFunction)class_initialise},
   	{DevMethodInitialise,		(DevMethodFunction)object_initialise},
  	{DevMethodCreate,		(DevMethodFunction)object_create},
  	{DevMethodDestroy,		(DevMethodFunction)object_destroy},
  	{DevMethodReadProperties,	(DevMethodFunction)read_properties},
  	{DevMethodCheckAlarm,		(DevMethodFunction)check_alarm},
  	{DevMethodReadAlarm,		(DevMethodFunction)read_alarm},
  	{DevMethodCheckLimits,		(DevMethodFunction)check_limits},
  	{DevMethodSignalReset,		(DevMethodFunction)reset},
};


DssignalClassRec dssignalClassRec = {
   /* n_methods */        sizeof(methods_list)/sizeof(DevMethodListEntry),
   /* methods_list */     methods_list,
   };

DssignalClass dssignalClass = (DssignalClass)&dssignalClassRec;

/*
 * public commands
 */

static	DevCommandListEntry commands_list[] = {
	{0, NULL, 0, 0, 0},
};
static long n_commands = sizeof(commands_list)/sizeof(DevCommandListEntry);


/*
 * reserve space for a default copy of the signal object
 */

static DssignalRec dssignalRec;
static Dssignal dssignal = (Dssignal)&dssignalRec;


/*
 * Array of signal resource name
 * This array is used by the multi-signal class
 * in order to imporove the dev_get_sig_config speed.
 * This array must be maintain.
 */

 char *SIG_RES_NAME[ NUMBER_OF_RES ] =
 { "Label",
   "Unit",  
   "Format",
   "Descr",  
   "Max",     
   "Min",    
   "AlHigh",  
   "AlLow",    
   "Delta",   
   "Dta_t",
   "StdU" };

/*
 * Class resouce table, used to read default values from the static database
 */

static db_resource res_class[] = {
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
};
static long res_class_size = sizeof(res_class)/sizeof(db_resource);

/*
 * Allocated resouce table, used to read signal properties
 * static database. Used in object_initialise().
 */

static db_resource res_object[] = {
   	{NULL,	D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_VAR_STRINGARR, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
   	{NULL,  D_STRING_TYPE, NULL},
};
static long res_object_size = sizeof(res_object)/sizeof(db_resource);



/*======================================================================
 Function:      static long class_initialise()

 Description:	routine to be called the first time a device is 
 		created which belongs to this class (or is a subclass
		thereof. This routine will be called only once.

 Arg(s) In:	none

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
 =======================================================================*/

static long class_initialise (long *error)

{
	static char	*init_value = "Not specified";
	int i;

	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nclass_initialise() : entering routine\n");
#endif /* EBUG */

/*
 * DssignalClass is a subclass of the DevServerClass
 */

   dssignalClass->devserver_class.superclass = devServerClass;
   dssignalClass->devserver_class.class_name = (char*)malloc(sizeof("SignalClass")+1);
   sprintf(dssignalClass->devserver_class.class_name,"SignalClass");
   dssignalClass->devserver_class.class_inited = 1;
   dssignalClass->devserver_class.n_commands = n_commands;
   dssignalClass->devserver_class.commands_list = commands_list;

/*
 * initialise dssignal with default values. These will be used
 * for every Dssignal object created.
 */

   dssignal->devserver.class_pointer = (DevServerClass)dssignalClass;
   dssignal->devserver.state 		= DEVON;
   dssignal->dssignal.label  		= NULL;
   dssignal->dssignal.unit  		= NULL;
   dssignal->dssignal.format  		= NULL;
   dssignal->dssignal.description  	= NULL;
   dssignal->dssignal.alarm_high  	= NULL;
   dssignal->dssignal.alarm_low  	= NULL;
   dssignal->dssignal.max_limit  	= NULL;
   dssignal->dssignal.min_limit  	= NULL;
   dssignal->dssignal.delta  		= NULL;
   dssignal->dssignal.delta_time  	= NULL;
   dssignal->dssignal.std_unit_multiplier= NULL;

/*
 * Interrogate the static database for default values
 */
   res_class[0].resource_adr	= &(dssignal->dssignal.label);
   res_class[1].resource_adr	= &(dssignal->dssignal.unit);
   res_class[2].resource_adr	= &(dssignal->dssignal.format);
   res_class[3].resource_adr	= &(dssignal->dssignal.description);
   res_class[4].resource_adr	= &(dssignal->dssignal.max_limit);
   res_class[5].resource_adr	= &(dssignal->dssignal.min_limit);
   res_class[6].resource_adr	= &(dssignal->dssignal.alarm_high);
   res_class[7].resource_adr	= &(dssignal->dssignal.alarm_low);
   res_class[8].resource_adr	= &(dssignal->dssignal.delta);
   res_class[9].resource_adr	= &(dssignal->dssignal.delta_time);
   res_class[10].resource_adr	= &(dssignal->dssignal.std_unit_multiplier);

   for( i=0 ; i<NUMBER_OF_RES ; i++)
     res_class[i].resource_name = SIG_RES_NAME[i];

   if ( db_getresource("CLASS/SIGNAL/DEFAULT", res_class, res_class_size, 
		       error) == DS_NOTOK)
      {
      return(DS_NOTOK);
      }

   /*
    * Verify that resources were read. If not, set the pointers to a
    * static variable to avoid in any case uninitialised pointers.
    */

   if ( dssignal->dssignal.label == NULL )
        dssignal->dssignal.label = init_value;
   if ( dssignal->dssignal.unit == NULL )
        dssignal->dssignal.unit = init_value;
   if ( dssignal->dssignal.format == NULL )
        dssignal->dssignal.format = init_value;
   if ( dssignal->dssignal.description == NULL )
        dssignal->dssignal.description = init_value;
   if ( dssignal->dssignal.alarm_high == NULL )
        dssignal->dssignal.alarm_high = init_value;
   if ( dssignal->dssignal.alarm_low == NULL )
        dssignal->dssignal.alarm_low = init_value;
   if ( dssignal->dssignal.max_limit == NULL )
        dssignal->dssignal.max_limit = init_value;
   if ( dssignal->dssignal.min_limit == NULL )
        dssignal->dssignal.min_limit = init_value;
   if ( dssignal->dssignal.delta == NULL )
        dssignal->dssignal.delta = init_value;
   if ( dssignal->dssignal.delta_time == NULL )
        dssignal->dssignal.delta_time = init_value;
   if ( dssignal->dssignal.std_unit_multiplier == NULL )
        dssignal->dssignal.std_unit_multiplier = init_value;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"class_initialise() : leaving routine\n");
#endif /* EBUG */

   return(DS_OK);
}




/*======================================================================
 Function:	static long object_create()

 Description:	routine to be called on creation of a device object

 Arg(s) In:	char *name - name to be given to device

 Arg(s) Out:	DevServer *ds_ptr - pointer to created device
		long      *error  - pointer to error code, in case 
		                    routine fails
 =======================================================================*/

static long object_create (char *name, DevServer *ds_ptr, long *error)

{
   Dssignal ds;

   *error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nobject_create() : entering routine\n");
#endif /* EBUG */

   ds = (Dssignal)malloc(sizeof(DssignalRec));

/*
 * initialise device with default object
 */

   *(DssignalRec*)ds = *(DssignalRec*)dssignal;

/*
 * finally initialise the non-default values
 */

   ds->devserver.name = (char*)malloc(strlen(name)+1);
   if ( ds->devserver.name == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf(ds->devserver.name,"%s",name);

   ds->dssignal.label = (char *) malloc (strlen(dssignal->dssignal.label) +1 );
   if ( ds->dssignal.label == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.label, "%s", dssignal->dssignal.label);

   ds->dssignal.unit = (char *) malloc (strlen(dssignal->dssignal.unit) +1 );
   if ( ds->dssignal.unit == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.unit, "%s", dssignal->dssignal.unit);

   ds->dssignal.format = (char *) malloc (strlen(dssignal->dssignal.format) +1);
   if ( ds->dssignal.format == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.format, "%s", dssignal->dssignal.format);

   ds->dssignal.description = (char *) malloc 
				(strlen(dssignal->dssignal.description) +1);
   if ( ds->dssignal.description == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.description, "%s", dssignal->dssignal.description);

   ds->dssignal.alarm_high = (char *) malloc 
				(strlen(dssignal->dssignal.alarm_high) +1 );
   if ( ds->dssignal.alarm_high == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.alarm_high, "%s", dssignal->dssignal.alarm_high);

   ds->dssignal.alarm_low = (char *) malloc 
				(strlen(dssignal->dssignal.alarm_low) +1 );
   if ( ds->dssignal.alarm_low == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.alarm_low, "%s", dssignal->dssignal.alarm_low);

   ds->dssignal.max_limit = (char *) malloc 
				(strlen(dssignal->dssignal.max_limit) +1 );
   if ( ds->dssignal.max_limit == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.max_limit, "%s", dssignal->dssignal.max_limit);

   ds->dssignal.min_limit = (char *) malloc 
				(strlen(dssignal->dssignal.min_limit) +1 );
   if ( ds->dssignal.min_limit == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.min_limit, "%s", dssignal->dssignal.min_limit);

   ds->dssignal.delta = (char *) malloc (strlen(dssignal->dssignal.delta) +1 );
   if ( ds->dssignal.delta == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.delta, "%s", dssignal->dssignal.delta);

   ds->dssignal.delta_time = (char *) malloc 
				(strlen(dssignal->dssignal.delta_time) +1 );
   if ( ds->dssignal.delta_time == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.delta_time, "%s", dssignal->dssignal.delta_time);


   ds->dssignal.std_unit_multiplier = (char *) malloc 
			(strlen(dssignal->dssignal.std_unit_multiplier) +1 );
   if ( ds->dssignal.std_unit_multiplier == NULL )
	 {
	 *error = DevErr_InsufficientMemory;
	 return (DS_NOTOK);
	 }
   sprintf (ds->dssignal.std_unit_multiplier, "%s", 
				dssignal->dssignal.std_unit_multiplier);


   *ds_ptr = (DevServer)ds;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"object_create() : leaving routine\n");
#endif /* EBUG */

   return(DS_OK);
}



/*============================================================================

Function:	static long object_destroy()

Description:	routine to be called to destroy a device object

Arg(s) In:	Dssignal ds	- object to destroy

Arg(s) Out:	long *error     - pointer to error code, in case routine fails
=============================================================================*/

static long object_destroy (Dssignal ds, long *error)

{
	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nobject_destroy() : entering routine\n");
#endif /* EBUG */

	/*
	 * Free fields of signal part
	 */

   	free (ds->dssignal.label);
   	free (ds->dssignal.unit);
   	free (ds->dssignal.format);
   	free (ds->dssignal.description);
   	free (ds->dssignal.alarm_high);
   	free (ds->dssignal.alarm_low);
   	free (ds->dssignal.max_limit);
   	free (ds->dssignal.min_limit);
   	free (ds->dssignal.delta);
   	free (ds->dssignal.delta_time);
   	free (ds->dssignal.std_unit_multiplier);

	if ( ds->dssignal.last_alarm_msg != NULL )
	   {
	   free (ds->dssignal.last_alarm_msg);
	   }

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

Arg(s) In:	Dssignal ds	- object to initialise

Arg(s) Out:	long *error     - pointer to error code, in case routine fails
=============================================================================*/

static long object_initialise (Dssignal ds, char *dev_class_name, long *error)

{
	char			*label 			= NULL;
	char			*unit 			= NULL;
	char			*format 		= NULL;
	char			*alarm_high 		= NULL;
	char			*alarm_low 		= NULL;
	char			*max_limit 		= NULL;
	char			*min_limit 		= NULL;
	char			*delta 			= NULL;
	char			*delta_time 		= NULL;
	char			*std_unit_multiplier 	= NULL;
        DevVarStringArray       description_list 	= {0,NULL};

	char 			dev_name[80];
	char 			class_res_name[80];
	char			res_names[NUMBER_OF_RES][80];
	char			*signal_name = NULL;
	short			i;
	char                    nethost[128];

	double			DOUBLE_MAX =  9.9e99;
	double			DOUBLE_MIN = -9.9e99;


   	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nobject_initialise() : entering routine\n");
#endif /* EBUG */

	/*
	 * Reinitialise the alarm limits.
	 * Alarm levels might have been deleted from the resource database.
	 */

  	free (ds->dssignal.alarm_high);
   	ds->dssignal.alarm_high = (char *) malloc
                                  (strlen(dssignal->dssignal.alarm_high) +1 );
        if ( ds->dssignal.alarm_high == NULL )
            {
            *error = DevErr_InsufficientMemory;
            return (DS_NOTOK);
            }
   	sprintf (ds->dssignal.alarm_high, "%s", dssignal->dssignal.alarm_high);

	free (ds->dssignal.alarm_low);
        ds->dssignal.alarm_low = (char *) malloc
                                 (strlen(dssignal->dssignal.alarm_low) +1 );
        if ( ds->dssignal.alarm_low == NULL )
              {
              *error = DevErr_InsufficientMemory;
              return (DS_NOTOK);
              }
        sprintf (ds->dssignal.alarm_low, "%s", dssignal->dssignal.alarm_low);


	ds->dssignal.dev_class_name = dev_class_name;
	
	/*
	 * Split the signal name with four fields into a device name
         * and a resource name to read from the static database.
         */

	sprintf (dev_name, "%s", ds->devserver.name);
	signal_name = strrchr (dev_name, '/');
	if (signal_name == NULL)
	   {
	   *error = DevErr_DeviceOfThisNameNotServed;
	   return (DS_NOTOK);
	   }

	*signal_name = '\0';
	signal_name++;
	
	/* 
         * Add the properties extensions to the signal name to read
         * the signal resources.
	 */

	for ( i=0; i<NUMBER_OF_RES; i++ )
	   {
	   sprintf (res_names[i], "%s.", signal_name);
           strcat (res_names[i],SIG_RES_NAME[i]);
       	   } 

	
	/*
         * Prepare the structures for db_getresource().
	 */

	/*
	 * Initialise the resource names.
	 */

	for ( i=0; i<NUMBER_OF_RES; i++ )
	   {
	   res_object[i].resource_name = res_names[i];
       	   } 



	/*
	 * read the class signal resources first.
	 */

   	res_object[0].resource_adr	= &label;
   	res_object[1].resource_adr	= &unit;
   	res_object[2].resource_adr	= &format;
   	res_object[3].resource_adr	= &(description_list);
   	res_object[4].resource_adr	= &max_limit;
   	res_object[5].resource_adr	= &min_limit;
   	res_object[6].resource_adr	= &alarm_high;
   	res_object[7].resource_adr	= &alarm_low;
	res_object[8].resource_adr      = &delta;
	res_object[9].resource_adr      = &delta_time;
	res_object[10].resource_adr     = &std_unit_multiplier;

	/* sprintf (class_res_name, "CLASS/%s/DEFAULT", 
	         ds->dssignal.dev_class_name); */

         if( strncmp( ds->devserver.name , "//" , 2 )==0 ) {
           strcpy(nethost,&(ds->devserver.name[2]));
           *(strchr(nethost,'/'))='\0';
           sprintf (class_res_name, "//%s/CLASS/%s/DEFAULT",
           nethost,dev_class_name);
         } else {
           sprintf (class_res_name, "CLASS/%s/DEFAULT", dev_class_name);
         }

#ifdef EBUG
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : class_res_name = %s\n", 
			 class_res_name);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : dev_name = %s\n", dev_name);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : sig_res  = %s\n", 
			 res_object[0].resource_name);
#endif /* EBUG */
   
   	if ( db_getresource (class_res_name, res_object, res_object_size, 
			     error) == DS_NOTOK)
   	   {
   	   return(DS_NOTOK);
   	   }

	/*
	 * copy all read resources to the object structure.
	 * free the used pointers before copying
	 */

	if ( label != NULL )
	   {
	   free (ds->dssignal.label);
	   ds->dssignal.label = label;
	   }

	if ( unit != NULL )
	   {
	   free (ds->dssignal.unit);
	   ds->dssignal.unit = unit;
	   }

	if ( format != NULL )
	   {
	   free (ds->dssignal.format);
	   ds->dssignal.format = format;
	   }

	if ( alarm_high != NULL )
	   {
	   free (ds->dssignal.alarm_high);
	   ds->dssignal.alarm_high = alarm_high;
	   }

	if ( alarm_low != NULL )
	   {
	   free (ds->dssignal.alarm_low);
	   ds->dssignal.alarm_low = alarm_low;
	   }

	if ( max_limit != NULL )
	   {
	   free (ds->dssignal.max_limit);
	   ds->dssignal.max_limit = max_limit;
	   }

	if ( min_limit != NULL )
	   {
	   free (ds->dssignal.min_limit);
	   ds->dssignal.min_limit = min_limit;
	   }

	if ( delta != NULL )
	   {
	   free (ds->dssignal.delta);
	   ds->dssignal.delta = delta;
	   }

	if ( delta_time != NULL )
	   {
	   free (ds->dssignal.delta_time);
	   ds->dssignal.delta_time = delta_time;
	   }

	if ( std_unit_multiplier != NULL )
	   {
	   free (ds->dssignal.std_unit_multiplier);
	   ds->dssignal.std_unit_multiplier = std_unit_multiplier;
	   }

	/*
	 * Translate the description string array read as 
	 *  defualt signal description into a long string.
	 */

        if ( get_description (ds, &description_list, error) == DS_NOTOK )
	   {
	   return (DS_NOTOK);
	   }

	/*
	 * initialise all pionters before calling db_getresource()
	 */

	label 			= NULL;
	unit			= NULL;
	format			= NULL;
	alarm_high		= NULL;
	alarm_low		= NULL;
	max_limit		= NULL;
	min_limit		= NULL;
	delta			= NULL;
	delta_time		= NULL;
	std_unit_multiplier	= NULL;

	/*
	 * Read now the individual signal resources.
	 */

   	res_object[0].resource_adr	= &label;
   	res_object[1].resource_adr	= &unit;
   	res_object[2].resource_adr	= &format;
   	res_object[3].resource_adr	= &(description_list);
   	res_object[4].resource_adr	= &max_limit;
   	res_object[5].resource_adr	= &min_limit;
   	res_object[6].resource_adr	= &alarm_high;
   	res_object[7].resource_adr	= &alarm_low;
	res_object[8].resource_adr      = &delta;
	res_object[9].resource_adr      = &delta_time;
	res_object[10].resource_adr     = &std_unit_multiplier;

   	if ( db_getresource (dev_name, res_object, res_object_size, 
			     error) == DS_NOTOK)
   	   {
   	   return(DS_NOTOK);
   	   }

	/*
	 * copy all read resources to the object structure.
	 * free the used pointers before copying
	 */

	if ( label != NULL )
	   {
	   free (ds->dssignal.label);
	   ds->dssignal.label = label;
	   }

	if ( unit != NULL )
	   {
	   free (ds->dssignal.unit);
	   ds->dssignal.unit = unit;
	   }

	if ( format != NULL )
	   {
	   free (ds->dssignal.format);
	   ds->dssignal.format = format;
	   }

	if ( alarm_high != NULL )
	   {
	   free (ds->dssignal.alarm_high);
	   ds->dssignal.alarm_high = alarm_high;
	   }

	if ( alarm_low != NULL )
	   {
	   free (ds->dssignal.alarm_low);
	   ds->dssignal.alarm_low = alarm_low;
	   }

	if ( max_limit != NULL )
	   {
	   free (ds->dssignal.max_limit);
	   ds->dssignal.max_limit = max_limit;
	   }

	if ( min_limit != NULL )
	   {
	   free (ds->dssignal.min_limit);
	   ds->dssignal.min_limit = min_limit;
	   }

	if ( delta != NULL )
	   {
	   free (ds->dssignal.delta);
	   ds->dssignal.delta = delta;
	   }

	if ( delta_time != NULL )
	   {
	   free (ds->dssignal.delta_time);
	   ds->dssignal.delta_time = delta_time;
	   }

	if ( std_unit_multiplier != NULL )
	   {
	   free (ds->dssignal.std_unit_multiplier);
	   ds->dssignal.std_unit_multiplier = std_unit_multiplier;
	   }

	/*
	 * Translate the string array read as signal description
	 * into a long string.
	 */

        if ( get_description (ds, &description_list, error) == DS_NOTOK )
	   {
	   return (DS_NOTOK);
	   }

#ifdef EBUG
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Label   = %s\n", 
		         ds->dssignal.label);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Unit    = %s\n", 
		         ds->dssignal.unit);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Format  = %s\n", 
		         ds->dssignal.format);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Descr   = %s\n", 
		         ds->dssignal.description);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : AlHigh  = %s\n", 
		         ds->dssignal.alarm_high);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : AlLow   = %s\n", 
		         ds->dssignal.alarm_low);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Max     = %s\n", 
		         ds->dssignal.max_limit);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Min     = %s\n", 
		         ds->dssignal.min_limit);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Delta   = %s\n", 
		         ds->dssignal.delta);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : Delta_t = %s\n", 
		         ds->dssignal.delta_time);
	dev_printdebug (DBG_METHODS, 
			"object_initialise() : StdUnit = %s\n", 
		         ds->dssignal.std_unit_multiplier);
#endif /* EBUG */

	/*
	 * convert text values to double numbers.
	 */

	if ( strcmp (ds->dssignal.alarm_high, "Not specified") == 0 )
	   {
	   /*
 	    * Value not specified, initialise to the maximum value.
	    */

	   ds->dssignal.alarm_high_value = DOUBLE_MAX;
	   }
	else
	   {
	   ds->dssignal.alarm_high_value = atof (ds->dssignal.alarm_high);
	   }

	if ( strcmp (ds->dssignal.alarm_low, "Not specified") == 0 )
	   {
	   /*
 	    * Value not specified, initialise to the minimum value.
	    */

	   ds->dssignal.alarm_low_value = DOUBLE_MIN;
	   }
	else
	   {
	   ds->dssignal.alarm_low_value = atof (ds->dssignal.alarm_low);
	   }

	if ( strcmp (ds->dssignal.max_limit, "Not specified") == 0 )
	   {
	   /*
 	    * Value not specified, initialise to the maximum value.
	    */

	   ds->dssignal.max_limit_value = DOUBLE_MAX;
	   }
	else
	   {
	   ds->dssignal.max_limit_value = atof (ds->dssignal.max_limit);
	   }

	if ( strcmp (ds->dssignal.min_limit, "Not specified") == 0 )
	   {
	   /*
 	    * Value not specified, initialise to the minimum value.
	    */

	   ds->dssignal.min_limit_value = DOUBLE_MIN;
	   }
	else
	   {
	   ds->dssignal.min_limit_value = atof (ds->dssignal.min_limit);
	   }

	if ( strcmp (ds->dssignal.delta, "Not specified") == 0 )
	   {
	   /*
 	    * Value not specified, initialise to the max value.
	    */

	   ds->dssignal.delta_value = DOUBLE_MAX;
	   }
	else
	   {
	   ds->dssignal.delta_value = atof (ds->dssignal.delta);
	   }

	if ( strcmp (ds->dssignal.delta_time, "Not specified") == 0 )
	   {
	   /*
 	    * Value not specified, initialise to zero value.
	    */

	   ds->dssignal.delta_time_value = 0;
	   }
	else
	   {
	   ds->dssignal.delta_time_value = atol (ds->dssignal.delta_time);
	   }

	if ( strcmp (ds->dssignal.std_unit_multiplier, "Not specified") == 0 )
	   {
	   /*
 	    * Value not specified, initialise to 1.
	    */

	   ds->dssignal.std_unit_multiplier_value = 1.0;
	   }
	else
	   {
	   ds->dssignal.std_unit_multiplier_value = 
					atof (ds->dssignal.std_unit_multiplier);
	   }


	/*
	 * Initialise the last alarm message pointer
	 */

	ds->dssignal.last_alarm_msg = NULL;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"object_initialise() : leaving routine\n");
#endif /* EBUG */

   	return(DS_OK);
}


/*============================================================================
 Function:       static long check_alarm()

 Description:    Method to check wether the read value exceeds
		 the specified alarm levels for the signal.

 Arg(s) In:      Dssignal	ds           - signal object
                 double         read_value   - value to be checked.

 Arg(s) Out:     long	   	*alarm_state - result of the check:
					       DEVHIGH    = exceeded high level.
					       DEVLOW     = exceeded low level.
					       DEVEXTRACTED = out of time interval
					       DEVRUN     = OK.
                 long           *error       - pointer to error code, in case
                                               routine fails.
 ============================================================================*/

static long check_alarm (Dssignal ds, double read_value, 
			  long *alarm_state, long *error)
{
	time_t 	now;
	char 	tmp[160];
	double 	ab;
	short	len;

	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\ncheck_alarm() : entering routine\n");
#endif /* EBUG */

	*alarm_state = DEVRUN;
	tmp[0] = '\0';

	/*
 	 * Free the message which was allocated during the las alarms check.
	 */

	if ( ds->dssignal.last_alarm_msg != NULL )
	   {
	   free (ds->dssignal.last_alarm_msg);
	   ds->dssignal.last_alarm_msg = NULL;
	   }


	/* Ckeck for Max Min */

	if( !Is_NaN( &read_value ) ) {
	
	  if ( read_value > ds->dssignal.alarm_high_value )
	     {
	     *alarm_state = DEVHIGH;
	     sprintf(tmp,"%s higher than alarm level\012", ds->dssignal.label);
	     }
	  else
	     {
	     if ( read_value < ds->dssignal.alarm_low_value )
	        {
	        *alarm_state = DEVLOW;
	        sprintf(tmp,"%s lower than alarm level\012",ds->dssignal.label);
	        }
	     }
	}

	/* Check for time interval */

	if(ds->dssignal.delta_time_value>0 && ds->dssignal.last_set_time>0 ) 
	{
	  now=time(NULL);
	  if((now - ds->dssignal.last_set_time) >=ds->dssignal.delta_time_value)
	  {
	      ab = (double)fabs(ds->dssignal.last_set_value-read_value);
	      if(ab >= ds->dssignal.delta_value) 
	      {
	        *alarm_state = DEVEXTRACTED;
	        sprintf (tmp,
   "%sDifference between read and set. Range = +/-%s %s, Delta_t = %s sec.\012",
			tmp,
			ds->dssignal.delta, ds->dssignal.unit, 
			ds->dssignal.delta_time);
	      }
	  }
	}

	/*
	 * allocate memory for the alarm message to be kept and
	 * copy the message.
	 */

	len = strlen (tmp);
	if ( len > 0 )
	   {
	   ds->dssignal.last_alarm_msg = (char *) malloc (len +1);
	   if ( ds->dssignal.last_alarm_msg == NULL )
	      {
	      *error = DevErr_InsufficientMemory;
	      return (DS_NOTOK);
	      }
	   sprintf (ds->dssignal.last_alarm_msg, "%s", tmp);
	   }

#ifdef EBUG
	dev_printdebug (DBG_METHODS, 
			"check_alarm() : alarm_high = %g, alarm_low = %g\n", 
			 ds->dssignal.alarm_high_value,
		         ds->dssignal.alarm_low_value);
	dev_printdebug (DBG_METHODS, 
			"check_alarm() : read_value = %g, alarm_state = %d\n", 
		         read_value, *alarm_state);
	dev_printdebug (DBG_METHODS, 
			"check_alarm() : last_value = %g \n", 
		         ds->dssignal.last_set_value);
	dev_printdebug (DBG_METHODS, 
			"check_alarm() : System Time = %d, Last set time= %d\n",
		         now,ds->dssignal.last_set_time);
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"check_alarm() : leaving routine\n");
#endif /* EBUG */

	return (DS_OK);
}



/*============================================================================
 Function:       static long read_alarm()

 Description:    Method to read the last alarm message.
		 The message will be deleted by a reset.

 Arg(s) In:      Dssignal	ds           - signal object

 Arg(s) Out:     char	   	*alarm_msg   - message from last alarm detected.
                 long           *error       - pointer to error code, in case
                                               routine fails.
 ============================================================================*/
static long read_alarm (Dssignal ds,  char **alarm_msg, long *error)
{

	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nread_alarm() : entering routine\n");
#endif /* EBUG */

	if ( ds->dssignal.last_alarm_msg == NULL )
	   {
	   *alarm_msg = NULL;
	   }
	else
	   {
	   *alarm_msg = (char *) malloc 
				 (strlen(ds->dssignal.last_alarm_msg) + 1);
	   sprintf (*alarm_msg, ds->dssignal.last_alarm_msg);
	   }

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"read_alarm() : leaving routine\n");
#endif /* EBUG */

	return (DS_OK);
}



/*============================================================================
 Function:       static long check_limits()

 Description:    Method to check wether the set value exceeds
		 the specified range for the signal.

 Arg(s) In:      Dssignal	ds           - signal object
                 double         set_value    - value to be checked.

 Arg(s) Out:     long	   	*limit_state - result of the check:
					       DEVHIGH    = exceeded maximum.
					       DEVLOW     = exceeded minimum.
					       DEVRUN     = OK.
                 long           *error       - pointer to error code, in case
                                               routine fails.
 ============================================================================*/
static long check_limits 	(Dssignal ds, double set_value, 
				 long *limit_state, long *error)
{
	time_t now;
	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\ncheck_limits() : entering routine\n");
#endif /* EBUG */

	*limit_state = DEVRUN;
	if ( set_value > ds->dssignal.max_limit_value )
	   {
	   *limit_state = DEVHIGH;
	   }
	else
	   {
	   if ( set_value < ds->dssignal.min_limit_value )
	      {
	      *limit_state = DEVLOW;
	      }
	   }
	
	/* Time setting alarm management */

	now=time(NULL);
	ds->dssignal.last_set_value=set_value;
	ds->dssignal.last_set_time=(long)now;


#ifdef EBUG
	dev_printdebug (DBG_METHODS, 
			"check_limits() : max_limit = %g, min_limit = %g\n", 
			 ds->dssignal.max_limit_value,
		         ds->dssignal.min_limit_value);
	dev_printdebug (DBG_METHODS, 
			"check_limits() : set_value = %g, limit_state = %d\n", 
		         set_value, *limit_state);
	dev_printdebug (DBG_METHODS, 
			"check_limits() : System_Time = %d\n", 
		         now);
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"check_limits() : leaving routine\n");
#endif /* EBUG */
	
	return (DS_OK);
}



/*============================================================================
 Function:       static long reset()

 Description:    Method to reinitialise the signal properties.

 Arg(s) In:      Dssignal	ds           - signal object

 Arg(s) Out:     long           *error       - pointer to error code, in case
                                               routine fails.
 ============================================================================*/
static long reset 		(Dssignal ds, long *error)
{
	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nreset() : entering routine\n");
#endif /* EBUG */

	/*
	 * Call object_initialise to read the object resources again.
	 * Reinitialises the signal object.
	 */

	if ( object_initialise (ds, ds->dssignal.dev_class_name, error) 
	     == DS_NOTOK)
	   {
	   return (DS_NOTOK);
	   }
	
#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"reset() : entering routine\n");
#endif /* EBUG */

	return (DS_OK);
}


/*============================================================================
 Function:       static long read_properties()

 Description:    Method to read an array of signal properties.
	         The array is of fixed lenght with all possible
		 property field initialised.

 Arg(s) In:      Dssignal	ds	- signal object

 Arg(s) Out:     DevVarStringArray  *properties 
					- array of signal properies.
                 long           *error  - pointer to error code, in case
                                               routine fails.
 ============================================================================*/
static long read_properties 	(Dssignal ds, DevVarStringArray *properties,
				 long *error)
{
	*error = 0;

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nread_properties() : entering routine\n");
#endif /* EBUG */

	/*
	 * Allocate memory for a set of character pointers
	 */

	properties->sequence = (char **)calloc ((NUMBER_OF_RES+1), 
					        sizeof (char *));

	/*
	 * Allocate the strings in the array.
	 */

	properties->sequence[0] = (char *)malloc(strlen(ds->devserver.name)+1); 
	sprintf (properties->sequence[0], "%s", ds->devserver.name);

	properties->sequence[1] = (char *)malloc(strlen(ds->dssignal.label)+1); 
	sprintf (properties->sequence[1], "%s", ds->dssignal.label);

	properties->sequence[2] = (char *)malloc(strlen(ds->dssignal.unit)+1); 
	sprintf (properties->sequence[2], "%s", ds->dssignal.unit);

	properties->sequence[3] = (char *)malloc(strlen(ds->dssignal.format)+1); 
	sprintf (properties->sequence[3], "%s", ds->dssignal.format);

	properties->sequence[4] = (char *)malloc(strlen
					         (ds->dssignal.description)+1); 
	sprintf (properties->sequence[4], "%s", ds->dssignal.description);

	properties->sequence[5] = (char *)malloc(strlen
					         (ds->dssignal.max_limit)+1);
	sprintf (properties->sequence[5], "%s", ds->dssignal.max_limit);

	properties->sequence[6] = (char *)malloc(strlen
						 (ds->dssignal.min_limit)+1);
	sprintf (properties->sequence[6], "%s", ds->dssignal.min_limit);

	properties->sequence[7] =(char *)malloc(strlen
						(ds->dssignal.alarm_high)+1);
	sprintf (properties->sequence[7], "%s", ds->dssignal.alarm_high);

	properties->sequence[8] = (char *)malloc(strlen
						 (ds->dssignal.alarm_low)+1);
	sprintf (properties->sequence[8], "%s", ds->dssignal.alarm_low);

	properties->sequence[9] = (char *)malloc(strlen
						 (ds->dssignal.delta)+1);
	sprintf (properties->sequence[9], "%s", ds->dssignal.delta);

	properties->sequence[10] = (char *)malloc(strlen
					 (ds->dssignal.delta_time)+1);
	sprintf (properties->sequence[10], "%s", ds->dssignal.delta_time);

	properties->sequence[11] = (char *)malloc(strlen
					 (ds->dssignal.std_unit_multiplier)+1);
	sprintf (properties->sequence[11], "%s", ds->dssignal.std_unit_multiplier);

	properties->length = NUMBER_OF_RES+1;

#ifdef EBUG
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[0]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[1]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[2]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[3]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[4]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[5]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[6]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[7]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[8]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[9]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[10]);
	dev_printdebug (DBG_METHODS, 
			"read_properties() : %s\n", properties->sequence[11]);

	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"read_properties() : leaving routine\n");
#endif /* EBUG */
	
	return (DS_OK);
}



/*============================================================================
 Function:       static long get_description()

 Description:    Is an internal function to transfer the description
		 message read from the resource database as string
		 array into a long string.

 Arg(s) In:      Dssignal	     ds	             - signal object
		 DevVarStringArray *description_list - description string array

 Arg(s) Out:    long           *error  - pointer to error code, in case
                                         routine fails.
 ============================================================================*/
static long get_description (Dssignal ds, DevVarStringArray *description_list, 
			     long *error)
{
	char	tmp_description[256];
	short	i;

	*error = 0;
	tmp_description[0] = '\0';

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"\nget_description() : entering routine\n");
#endif /* EBUG */

	if ( description_list->length > 0 )
	   {

	   /*
	    * Translate the description string array 
	    * into a long temporary description string.
	    */

	   for (i=0; i<description_list->length; i++)
	      {
	      if (i == 0)
	         {
	         sprintf (tmp_description, "%s", 
		          description_list->sequence[i]);
	         }
	      else
	         {
	         strcat (tmp_description, " ");
	         strcat (tmp_description, description_list->sequence[i]);
	         }
	      }

	   /*
	    * free the string array memory allocated by db_getresource.
	    */

	   for (i=0; i<description_list->length; i++)
	       {
	       free (description_list->sequence[i]);
	       }
   	   free (description_list->sequence);

    	   description_list->length   = 0;
    	   description_list->sequence = NULL;

	   /*
	    * Allocat the memory for the description in the signal
	    * structure and copy the temporary description string.
	    */

	   free (ds->dssignal.description);
	   ds->dssignal.description = (char *) malloc 
					(strlen (tmp_description) + 1);
	   if ( ds->dssignal.description == NULL )
	      {
	      *error = DevErr_InsufficientMemory;
	      return (DS_NOTOK);
	      }
	   sprintf (ds->dssignal.description, "%s", tmp_description);
	   }

#ifdef EBUG
	dev_printdebug (DBG_METHODS | DBG_TRACE, 
			"get_description() : leaving routine\n");
#endif /* EBUG */
	
	return (DS_OK);
}

/*============================================================================
 Function:      void get_sig_prop_suffix()

 Description:   Returns the signal propertie suffixes. Used by the
		MDSignal_config.c

 Arg(s) Out:     DevVarStringArray *suf_list - suffixes string array
 ============================================================================*/
void get_sig_prop_suffix (DevVarStringArray *suf_list )
{
  suf_list->length   = NUMBER_OF_RES;
  suf_list->sequence = SIG_RES_NAME;
}

/*============================================================================
 Function:      void load_NaN(void *v)

 Description:   Load the Not A Number value into a float or a double

 Arg(s) In:     void *v - address of a float or a double
 ============================================================================*/
void load_NaN(void *v)
{
  *(unsigned long *)v=(unsigned long)0x7FF40000;
}

/*============================================================================
 Function:      int Is_NaN(void *v)

 Description:   Return true is *v is NaN

 Arg(s) In:     void *v - address of a float or a double
 ============================================================================*/
int Is_NaN(void *v)
{
  return *(unsigned long *)v==0x7FF40000;
}

