/******************************************************************************
 *
 * File:	lv_dsclass.c
 *
 * Project:	Generic LabView device server startup and class
 *
 * Description:	This file implements a generic LabView device class and startup
 *		and the related routines for retrieving control information about Labview VIs
 *
 *
 * Author(s):	Andy Gotz
 *
 * Original:	October 1999
 *
 * $Revision: 1.3 $
 *
 * $Date: 2004-01-26 09:43:01 $
 * 
 * Last Modified: 01.11.2003, Hartmut Gilde
 * 
 ******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <API.h>
#include <ApiP.h>
#include <Admin.h>
#include <BlcDsNumbers.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <DevErrors.h>
#include <maxe_xdr.h>
#include <LabViewGenericP.h>
#include <LabViewGeneric.h>

extern configuration_flags      config_flags;
 
static int      udp_socket;
static int      tcp_socket;

static void _WINAPI devserver_prog_4    PT_( (struct svc_req *rqstp,SVCXPRT *transp) );
static _client_data * _DLLFunc rpc_cmd_get (_server_data *server_data);
static long lv_startup(char*, long*);
static long lv_ds__sendreply();
static long read_device_id (long,long *,long *,long *);


/*+======================================================================        
 Function   :   long lv_ds__main ()

 Description:   initialise device server global fields, create a 
		database connection, and call the device server 
		startup() function, afterwards return to calling
		function. This routine has to be called to create
		and initialise the LabView device server.

		NOTE : this routine has been cloned from the device
		server main() routine

 Arg(s) In  :   char *server_name   - server executable name
		char *pers_name  - server personal name

 Arg(s) Out :   

 Return(s)  :   DS_OK / DS_NOTOK
=======================================================================-*/    

long lv_ds__main(char *server_name, char *pers_name)
{
        SVCXPRT *transp;
        SVCXPRT *transp_tcp;
        char    host_name [19];
        char    dsn_name [37];
        char    *proc_name;
        char    *display;

        char                    res_path [80];
        char                    res_name[80];
        DevVarStringArray       default_access;
        db_resource             res_tab;

        long    prog_number;
        long    status;
        long    error = 0;
        int     pid = 0;
        short   m_opt = False;
        short   s_opt = True;
        short   sig;
        short   i;

#ifdef LV_DEBUG
	printf("lv_ds__main(): server %s name %s\n",server_name,pers_name);
#endif /* LV_DEBUG */

        /*
         *  read device server's class name and personal name
         *  check for lenght of names : server process name <= 23 char
         *                              personal name       <= 11 char
         */

        proc_name = server_name;

        if ( strlen(proc_name) > 23 )
           {
           printf ("Filename to long : server_name <= 23 char\n");
           return (-1);
           }

        if ( strlen(pers_name) > 11 )
           {
           printf ("Personal DS_name to long : personal_dsname <= 11 char\n");
           return (-1);
           }

	memset  (dsn_name,0,sizeof(dsn_name));
	strncat (dsn_name , proc_name, 23);
	strncat (dsn_name , "/", 1);
	strncat (dsn_name , pers_name, 11);

	/*
	 *  get process ID, host_name 
	 *  and create device server network name
	 */

        pid = getpid ();


	gethostname (host_name, 19);

   	TOLOWER(dsn_name);
   	TOLOWER(host_name);

	sprintf (config_flags.server_name,"%s", dsn_name); 
	sprintf (config_flags.server_host,"%s", host_name); 


	/*
	 * install signal handling for HPUX, SUN, OS9
	 */

/*
	(void) signal(SIGINT,  main_signal_handler);
	(void) signal(SIGTERM, main_signal_handler);
        (void) signal(SIGABRT, main_signal_handler);
	(void) signal(SIGQUIT, main_signal_handler);
	(void) signal(SIGHUP,  main_signal_handler);
	(void) signal(SIGPIPE, main_signal_handler);

	printf("device_server(): installed signal handling\n");
 */
	

	/*
	 *  import database server  
	 */

	if ( db_import(&error) < 0 )
	   {
	   printf ("db_import failed",error);
	   return(-1);
	   }		

	/*
	 *  check wether an old server with the same name
	 *  is mapped to portmap or still running
	 */

/*
	if ( svc_check(&error) < 0 )
	   {
	   printf ("svc_check()",error);
	   return(-1);
	   }		
 */


	/*
	 * If the security system is switched on, read the minimal
	 * access right for version 3 clients from the 
	 * security database.
	 */

/*
       if ( config_flags.security == True )
	   {
	   printf("lv_ds__main(): yes, check it ...\n");
	   default_access.length   = 0;
	   default_access.sequence = NULL;

       	   sprintf (res_name, "default");
	   res_tab.resource_name = res_name;
	   res_tab.resource_type = D_VAR_STRINGARR;
	   res_tab.resource_adr  = &default_access;

	   sprintf (res_path, "SEC/MINIMAL/ACC_RIGHT");

	   if (db_getresource (res_path, &res_tab, 1, &error) == DS_NOTOK)
	      {
	      dev_printerror_no (SEND,
	      "db_getresource() get default security access right\n",error);
	      return (-1);
	      }

       *
	    * Transforme the string array into an access right value.
	    *

           if ( default_access.length > 0 )
	      {
              for (i=0; i<SEC_LIST_LENGTH; i++)
                 {
                 if (strcmp (default_access.sequence[0], 
			     DevSec_List[i].access_name) == 0)
                    {
                    minimal_access = DevSec_List[i].access_right;
                    break;
                    }
                 }

              if ( i == SEC_LIST_LENGTH )
                 {
                 minimal_access = NO_ACCESS;
                 }
	      }	 
	   else 
	      {
              minimal_access = NO_ACCESS;
	      }

	   free_var_str_array (&default_access);
	   }
 */

	/*
	 *  get transient progamm number
	 */


        udp_socket = RPC_ANYSOCK;
        tcp_socket = RPC_ANYSOCK;

	prog_number = gettransient_ut(dsn_name, &udp_socket, &tcp_socket);
#ifdef LV_DEBUG
	printf("lv_ds__main(): transient program number = %d\n",prog_number);
#endif /* LV_DEBUG */

	/*
	 * Write the device server identification to the global
	 * configuration structure.
	 */

	config_flags.prog_number = prog_number;
	config_flags.vers_number = API_VERSION;


/*
 * DON'T import message server ...

	if (m_opt == True)
	   {
	    *
 	    *  read environmental variable DISPLAY
  	    * 

		if ( (display = (char *)getenv ("DISPLAY")) == NULL ) 
		{
			char msg[]= "Environment variable DISPLAY not defined, exiting...\n";

			fprintf (stderr,msg);
    		return (-1);
		}

	    *
	    *  import message server
	    * 


	   if (msg_import (dsn_name,host_name,prog_number,display,&error) < 0)
	      {
	      dev_printerror_no (SEND,"msg_import failed",error);
              kill (pid,SIGQUIT);
	      }		
	   }
 */

/*
 * let portmapper choose port numbers for services 
 */
        udp_socket = RPC_ANYSOCK;
        tcp_socket = RPC_ANYSOCK;

    /*
	 *  create server handle and register to portmap
	 */

	/*
	 *  register udp port
	 */
	transp = svcudp_create (udp_socket);
	if (transp == NULL) 
		{
		char msg[]="Cannot create udp service, exiting...\n";
			fprintf (stderr, msg);
            /*kill (pid,SIGQUIT);*/
            return(-1);
		}
	/*
	 * keep the socket, we need it later!
	 */
	/*sock_udp= transp->xp_sock;*/

	if (!svc_register(transp, prog_number, API_VERSION, 
			  devserver_prog_4, IPPROTO_UDP)) 
		{
		char msg[]="Unable to register server (UDP,4), exiting...\n"; 
			fprintf (stderr, msg); 
			/*kill (pid,SIGQUIT);*/
			return(-1);
		}
	
	
	udp_socket = transp->xp_sock;
	/*
	 *  register tcp port
	 */
	transp_tcp = svctcp_create(tcp_socket,0,0);
	if (transp_tcp == NULL) 
		{
		char msg[]= "Cannot create tcp service, exiting...\n";
			fprintf (stderr, msg); 
			/*kill (pid,SIGQUIT);*/
			return(-1);
		}
	/*
	 * keep the socket, we need it later
	 */
	/*sock_tcp= transp_tcp->xp_sock;*/

        if (!svc_register(transp_tcp, prog_number, API_VERSION,
			  devserver_prog_4, IPPROTO_TCP))
		{
	    char msg[]= "Unable to register server (TCP,4), exiting...\n";
			fprintf (stderr, msg); 
			/*kill (pid,SIGQUIT);*/
			return(-1);
		}

	tcp_socket = transp_tcp->xp_sock;
/*
 * if the process has got this far then it is a bona-fida device server 
 * set the appropiate flag
 */
	config_flags.device_server = True;

/*
 * Register the asynchronous rpc service so that the device server
 * can receive asynchronous calls from clients. The asynchronous
 * calls are sent as batched tcp requests without wait. The server
 * will return the results to the client asynchronously using batched
 * tcp.
 */
/*
	status = asynch_rpc_register(&error);
	if (status != DS_OK)
	{
		dev_printerror_no (SEND,"failed to register asynchronus rpc",error);
	}
 */

        /*
	 *  startup device server
	 */

	if (s_opt == True)
	   {
           /*
            * Set the startup configuration flag to SERVER_STARTUP
            * during the startup phase.
            */

           config_flags.startup = SERVER_STARTUP;

	   status = lv_startup(config_flags.server_name,&error);
	   if ( status < 0 )
	      {
	      printf ("lv_startup failed (error=%d)",error);
              /*kill (pid,SIGQUIT);*/
	      return(-1);
	      }		

	   /*
	    *  if ds__svcrun() is used, the server can return from
	    *  the startup function with status=1 to avoid svc_run()
	    *  and to do a proper exit.
	    */
	   if ( status == 1 )
	      {
                /*kill (pid,SIGQUIT);*/
                return(-1);
	      }

	   config_flags.startup = True;
	   }

	/*
	 *  set server into wait status
	 */

#ifdef LV_DEBUG
	printf("lv_ds__main(): return immediately, call lv_ds__poll() to execute client request ...\n");
#endif /* LV_DEBUG */
	return(DS_OK);

	svc_run();
	{
		char msg[]= "svc_run returned\n";
		fprintf (stderr, msg); 
		kill (pid,SIGQUIT);
	}
}

static long cmd_in;
static char **lv_ds__string_in=NULL;
static DevVarStringArray lv_ds__strarr_in;
static double *lv_ds__double_in=NULL;
static DevVarDoubleArray lv_ds__dblarr_in;
void *lv_ds__argin, *lv_ds__argout;

/*static char 	**lv_ds_ctrl_name_in = NULL;
static double 	*lv_ds_ctrl_value_in = NULL;
static double 	*lv_ds_ctrl_value_out = NULL;*/

static DevString lv_ds__ctrl_name_in;
static DevDouble lv_ds__ctrl_value_in;
static DevDouble lv_ds__ctrl_value_out; 
static DevDouble vi_double;
/*=======================================================================

Function   :	long lv_ds__cmd_get()

Description:	look if there are any RPC client requests and service them
		If the rpc request was a dev_putget() the return the 
		command requested and a pointer to the input (if any)

Arg(s) In  :	none

Arg(s) Out :	void **lv_argin - double/string array input

Return(s)  :	minus one on failure, zero otherwise

========================================================================*/
long lv_ds__cmd_get(void **lv_argin)
{
	long error;

	cmd_in = 0;
	ds__svcrun(&error);

		
	if (((cmd_in == DevGetControlInfo) || (cmd_in == DevSetControlValue) || (cmd_in == DevGetIndicatorInfo)) && (lv_argin != NULL))
	{
		*lv_argin = lv_ds__argin;
	}	


/*
 * map TACO commands to LabView enumerated types {1, 2, 3, 4}
 */

	switch (cmd_in)
	{	

		case DevGetViInfo :
			cmd_in = LVGetViInfo;
			     	break;
		case DevGetControlList :
			cmd_in = LVGetControlList;
		       	break;

		case DevGetControlInfo : 
			cmd_in = LVGetControlInfo;
		       	break;

		case DevSetControlValue : 
			cmd_in = LVSetControlValue;
		       	break;

		case DevGetIndicatorList : 
			cmd_in = LVGetIndicatorList;
		       	break;

		case DevGetIndicatorInfo : 
			cmd_in = LVGetIndicatorInfo;
		       	break;

		case DevState : 
			cmd_in = LVState;
		       	break;

		case DevStatus : 
			cmd_in = LVStatus;
		       	break;

		default : 
			cmd_in = 0;
			break;
	}

	return(cmd_in);
}
	
/*=======================================================================

Function   :    long lv_ds__cmd_put()

Description:    return output arguments of interrupted dev_putget() request

Arg(s) In  :    DevVarDoubleArray dblarr_out - double array output

Arg(s) Out :    none

Return(s)  :    minus one on failure, zero otherwise

========================================================================*/
long lv_ds__cmd_put(void *lv_argout)
{
   long error;
   int i;

	
	/*for (i = 0; i < ((DevVarStringArray *)lv_argout)->length; ++i) {
		printf("string out a : %s\n", ((DevVarStringArray *)lv_argout)->sequence[i]);
	}*/
	if (((cmd_in == LVGetViInfo) || (cmd_in == LVGetControlList) || (cmd_in == LVGetControlInfo) || (cmd_in == LVGetIndicatorList) || (cmd_in == LVGetIndicatorInfo)) && (lv_ds__argout != NULL))
	{
		*(DevVarStringArray*)lv_ds__argout = *(DevVarStringArray*)lv_argout;
	}
	if (cmd_in == LVSetControlValue && lv_ds__argout != NULL)
	{
		//*(DevVoid*)lv_ds__argout = *(DevVoid*)lv_argout;
	}
        lv_ds__sendreply();

        return(DS_OK);
}

/*=======================================================================

Function   :	long lv_startup()

Description:	create, initialise and export all of the objects
		for the LabView device server

Arg(s) In  :	svc_name - full server name

Arg(s) Out :	error - error return code

Return(s)  :	minus one on failure, zero otherwise

========================================================================*/

static long lv_startup(svr_name, error)
char *svr_name;
long *error;
{
   long		i,	status, n_exported;
   short		iret;
   static char 		**dev_list;
   static unsigned int	n_devices;
   static LabViewGeneric	*ds_list;

   if (db_getdevlist(svr_name,&dev_list,&n_devices,error))
   {
      printf("startup(): db_getdevlist() failed\n");
      return(-1);
   }
#ifdef LV_DEBUG
   printf("following devices found in static database: \n\n");
   for (i=0;i<n_devices;i++)
   {
      printf("\t%s\n",dev_list[i]);
   }
#endif /* LV_DEBUG */

/*
 * create, initialise and export all devices served by this server
 */
   ds_list=(LabViewGeneric *)malloc(n_devices*sizeof(LabViewGeneric*));
   if(ds_list==0)
   {
   	printf("can't allocate memory for object structures --> exit\n");
   	return(DS_NOTOK);
   }
   
   for (i=0, n_exported = 0; i < n_devices; i++, n_exported++)
   {
#ifdef LV_DEBUG
      printf("\t\tObject %s is\n",dev_list[i]);
#endif /* LV_DEBUG */
      if (ds__create(dev_list[i], (void*)labViewGenericClass, (void*)&(ds_list[i]),error) != 0)
      {
   	  printf("create failed\n");
   	  continue;
      }
      else 
      {
#ifdef LV_DEBUG
          printf("\t\t- Created\n");
#endif /* LV_DEBUG */
      }

/*
 * initialise the newly created LabViewGeneric
 */
  
      if((ds__method_finder((void*)ds_list[i],DevMethodInitialise))
			  (ds_list[i],error)!= 0)
      {
      	   printf("initialise failed\n");
   	   continue;
      }
      else 
      {
#ifdef LV_DEBUG 
           printf("\t\t- Initialised\n");
#endif /* LV_DEBUG */
      }

/*
 *  now export it to the outside world 
 */
 
      if ((ds__method_finder((void*)ds_list[i],DevMethodDevExport))
			   (dev_list[i],ds_list[i],error) != 0)
      {
   	   printf("export failed\n");
   	   continue;
      }
      else 
      {
#ifdef LV_DEBUG
           printf("\t\t- Exported\n\n");
#endif /* LV_DEBUG */
      }
   }

   switch(n_exported)
   {
      case 0:	
   	   printf("No devices exported - LabViewGeneric server exiting\n");
   	   *error = 0;
   	   iret = -1;
   	   break;

      case 1:	
#ifdef LV_DEBUG
   	   printf("LabViewGeneric server running with 1 device exported\n");
#endif /* LV_DEBUG */
   	   *error = 0;
   	   iret = 0;
   	   break;

      default:
#ifdef LV_DEBUG
   	   printf("LabViewGeneric server running %d devices exported\n",n_exported);
#endif /* LV_DEBUG */
   	   *error = 0;
   	   iret = 0;
   }
   return(iret);
}

















/* 
 * LabViewGeneric device class (normally in LabViewGeneric.c)
 */

static long dev_getviinfo();
static long dev_getcontrollist();
static long dev_getcontrolinfo();
static long dev_setcontrolvalue();
static long dev_getindicatorlist();
static long dev_getindicatorinfo();
static long dev_state();
static long dev_status();

static	DevCommandListEntry commands_list[] = {
   	{DevGetViInfo, dev_getviinfo, D_VOID_TYPE, D_VAR_STRINGARR},
   	{DevGetControlList, dev_getcontrollist, D_VOID_TYPE, D_VAR_STRINGARR},
   	{DevGetControlInfo, dev_getcontrolinfo, D_VAR_STRINGARR, D_VAR_STRINGARR},
   	{DevSetControlValue, dev_setcontrolvalue, D_VAR_STRINGARR, D_VOID_TYPE},
   	{DevGetIndicatorList, dev_getindicatorlist, D_VOID_TYPE, D_VAR_STRINGARR},
   	{DevGetIndicatorInfo, dev_getindicatorinfo, D_VAR_STRINGARR, D_VAR_STRINGARR},
   	{DevState, dev_state, D_VOID_TYPE, D_SHORT_TYPE},
   	{DevStatus, dev_status, D_VOID_TYPE, D_STRING_TYPE}
};

static long n_commands = sizeof(commands_list)/sizeof(DevCommandListEntry);

static long class_initialise();
static long object_initialise();
static long object_create();
static long state_handler();

static	DevMethodListEntry methods_list[] = {
   	{DevMethodClassInitialise,	class_initialise},
   	{DevMethodInitialise,		object_initialise},
  		{DevMethodCreate,				object_create},
   	{DevMethodStateHandler,		state_handler},
   };

LabViewGenericClassRec labViewGenericClassRec = {
   /* n_methods */        4,
   /* methods_list */     methods_list,
   };

LabViewGenericClass labViewGenericClass = (LabViewGenericClass)&labViewGenericClassRec;
/*
 * reserve space for a default copy of the LabViewGeneric object
 */

static LabViewGenericRec labViewGenericRec;
static LabViewGeneric labViewGeneric = (LabViewGeneric)&labViewGenericRec;

/*
 * LabViewGeneric resource tables used to access the static database
 *
 */

db_resource res_object[] = {
   {"read",	D_FLOAT_TYPE, NULL},
   {"set",	D_FLOAT_TYPE, NULL},
   	};
int res_object_size = sizeof(res_object)/sizeof(db_resource);


db_resource res_class[] = {
   {"state",       D_LONG_TYPE, NULL},
   {"config",   D_LONG_TYPE, NULL},
   	};
int res_class_size = sizeof(res_class)/sizeof(db_resource);


/*======================================================================
 Function:      static long class_initialise()

 Description:	routine to be called the first time a device is 
 		created which belongs to this class (or is a subclass
		thereof. This routine will be called only once.

 Arg(s) In:	none

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
 =======================================================================*/

static long class_initialise(error)
long *error;
{

/*
 * LabViewGenericClass is a subclass of the DevServerClass
 */

   labViewGenericClass->devserver_class.superclass = devServerClass;
   labViewGenericClass->devserver_class.class_name = (char*)malloc(sizeof("LabViewGenericClass")+1);
   sprintf(labViewGenericClass->devserver_class.class_name,"LabViewGenericClass");
   labViewGenericClass->devserver_class.class_inited = 1;
   labViewGenericClass->devserver_class.n_commands = n_commands;
   labViewGenericClass->devserver_class.commands_list = commands_list;

/*
 * initialise LabViewGeneric with default values. these will be used
 * for every LabViewGeneric object created.
 */

   labViewGeneric->devserver.class_pointer = (DevServerClass)labViewGenericClass;

   labViewGenericClass->LabViewGeneric_class.config_param	= 0;

   labViewGeneric->devserver.state = DEVON;

/*
 * Interrogate the static database for default values
 *
 */

   res_class[0].resource_adr	= &(labViewGeneric->devserver.n_state);
   res_class[1].resource_adr	= &(labViewGenericClass->LabViewGeneric_class.config_param);

/*
   if(db_getresource("CLASS/LabViewGeneric/DEFAULT", res_class, res_class_size, error))
   {
      return(DS_NOTOK);
   }
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

static long object_create(name, ds_ptr, error)
char *name;
DevServer *ds_ptr;
long *error;
{
   LabViewGeneric ds;

   ds = (LabViewGeneric)malloc(sizeof(LabViewGenericRec));

/*
 * initialise device with default object
 */

   *(LabViewGenericRec*)ds = *(LabViewGenericRec*)labViewGeneric;

/*
 * finally initialise the non-default values
 */

   ds->devserver.name = (char*)malloc(strlen(name)+1);
   sprintf(ds->devserver.name,"%s",name);

   *ds_ptr = (DevServer)ds;

   return(DS_OK);
}

/*============================================================================

Function:	static long object_initialise()

Description:	routine to be called on initialisation of a device object

Arg(s) In:	LabViewGeneric ds	- object to initialise

Arg(s) Out:

		long *error     - pointer to error code, in case routine fails
=============================================================================*/
static long object_initialise(ds, error)
LabViewGeneric ds;
long  *error;
{

   ds->LabViewGeneric.read_value 	= 0;
   ds->LabViewGeneric.set_value 	= 0;


   res_object[0].resource_adr        = &(ds->LabViewGeneric.read_value);
   res_object[1].resource_adr        = &(ds->LabViewGeneric.set_value);
   
/*
   if(db_getresource(ds->devserver.name, res_object, res_object_size, error))
   {
   	return(DS_NOTOK);
   }
 */
   return(DS_OK);
}

/*======================================================================
 Function:      static long state_handler()

 Description:	this routine is reserved for checking wether the command
		requested can be executed in the present state.

 Arg(s) In:	LabViewGeneric ds - device on which command is to executed
		DevCommand cmd - command to be executed

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
 =======================================================================*/

static long state_handler( ds, cmd, error)
LabViewGeneric ds;
DevCommand cmd;
long *error;
{
   long int p_state, n_state;
   long iret = DS_OK;
/*
 * Get here the real state of the physical device
 *
 *  example:
 *  ds->devserver.state=get_state();
 */
   p_state = ds->devserver.state;

/*
 * Before checking out the state machine assume that the state doesn't
 * change i.e. new state == old state
 *
 */

   n_state = p_state;

   switch (p_state)
   {
    
   	case (DEVON) :	/* Device is on */
   	{
   		switch (cmd)
   		{
   			/* Allowed Command(s) */

   			case (DevGetViInfo):	n_state = DEVON;break;
   			case (DevGetControlList):	n_state = DEVON;break;
   			case (DevGetControlInfo):	n_state = DEVON;break;
   			case (DevSetControlValue):	n_state = DEVON;break;
				case (DevGetIndicatorList):	n_state = DEVON;break;
				case (DevGetIndicatorInfo):	n_state = DEVON;break;
   		}
   	}
   	break;
   	default:
   		*error = DevErr_UnrecognisedState;
   		iret = DS_NOTOK;
   		break;
   }
   ds->devserver.n_state = n_state;

   return(iret);
}

/*
 * public commands
 */


/*============================================================================
 Function:      static long dev_getviinfo ()

 Description:	Get an array of strings describing the VI
   	
 Arg(s) In:	LabViewGeneric 	ds 	- 
		DevVoid		*argin - none
   				  
 Arg(s) Out:	DevVarStringArray *argout - Get vi description  
		long		*error	- pointer to error code, in case
		 			  routine fails. Error code(s):
					  none
 ============================================================================*/

static long  dev_getviinfo(ds, argin, argout, error)
LabViewGeneric 	ds;
DevVoid 	*argin;
DevVarStringArray *argout;
long 		*error;
{

#ifdef LV_DEBUG
   printf("LabViewGeneric dev_getviinfo()\n");
#endif /*LV_DEBUG*/
	
   lv_ds__argout = (void*)argout;
	return(DS_OK);
}


/*============================================================================
 Function:      static long dev_getcontrollist ()

 Description:	Get an array of strings Describing the controls found in the attached VI
   	
 Arg(s) In:	LabViewGeneric 	ds 	- 
		DevVoid		*argin - none
   				  
 Arg(s) Out:	DevVarStringArray *argout - Get controls 
		long		*error	- pointer to error code, in case
		 			  routine fails. Error code(s):
					  none
 ============================================================================*/

static long  dev_getcontrollist(ds, argin, argout, error)
LabViewGeneric 	ds;
DevVoid 	*argin;
DevVarStringArray *argout;
long 		*error;
{

#ifdef LV_DEBUG
   printf("LabViewGeneric dev_getcontrollist()\n");
#endif /*LV_DEBUG*/

   lv_ds__argout = (void*)argout;

   return(DS_OK);
}



/*============================================================================
 Function:      static long dev_getcontrolinfo ()

 Description:	Get some info yout VI in a string array
   	
 Arg(s) In:	LabViewGeneric 	ds 	- 
		DevString *argin  - Name of control
   				  
 Arg(s) Out:	DevVarDoubleArray *argout - Get Info values
		long		*error	- pointer to error code, in case
		 			  routine fails. Error code(s):
					  none
 ============================================================================*/

static long  dev_getcontrolinfo(ds, argin, argout, error)
LabViewGeneric 	ds;
DevVarStringArray *argin;
DevVarStringArray *argout;
long 		*error;
{

#ifdef LV_DEBUG
   printf("LabViewGeneric dev_getcontrolinfo()\n");
#endif /* LV_DEBUG */

   //lv_ds__argin = (void*)&lv_ds__ctrl_name_in;
   lv_ds__argin = (void*)argin;
   lv_ds__argout = (void*)argout;

   return(DS_OK);
}


/*============================================================================
 Function:      static long dev_getindicatorlist ()

 Description:	Get an array of strings Describing the indicators found in the attached VI
   	
 Arg(s) In:	LabViewGeneric 	ds 	- 
		DevVoid		*argin - none
   				  
 Arg(s) Out:	DevVarStringArray *argout - Get indicators 
		long		*error	- pointer to error code, in case
		 			  routine fails. Error code(s):
					  none
 ============================================================================*/

static long  dev_getindicatorlist(ds, argin, argout, error)
LabViewGeneric 	ds;
DevVoid 	*argin;
DevVarStringArray *argout;
long 		*error;
{

#ifdef LV_DEBUG
   printf("LabViewGeneric dev_getindicatorlist()\n");
#endif /*LV_DEBUG*/

   lv_ds__argout = (void*)argout;

   return(DS_OK);
}


/*============================================================================
 Function:      static long dev_getindicatorinfo ()

 Description:	Get some info about the indicator in a string array
   	
 Arg(s) In:	LabViewGeneric 	ds 	- 
		DevString *argin  - Name of control
   				  
 Arg(s) Out:	DevVarDoubleArray *argout - Get Info values
		long		*error	- pointer to error code, in case
		 			  routine fails. Error code(s):
					  none
 ============================================================================*/

static long  dev_getindicatorinfo(ds, argin, argout, error)
LabViewGeneric 	ds;
DevVarStringArray *argin;
DevVarStringArray *argout;
long 		*error;
{

#ifdef LV_DEBUG
   printf("LabViewGeneric dev_getindicatorinfo()\n");
#endif /* LV_DEBUG */

   lv_ds__argin = (void*)argin;
   lv_ds__argout = (void*)argout;

   return(DS_OK);
}



void lv_print_debug(char* text) {
#ifdef LV_DEBUG
   printf(text);
#endif /*PRINT*/
}


/*============================================================================
 Function:      static long dev_setcontrolvalue()

 Description:	Set an array of doubles to a LabView device
   	
 Arg(s) In:	LabViewGeneric 	ds 	- 
		DevDouble *argin  - Set control value (delivered as string, has to be casted afterwards)
   				  
 Arg(s) Out:	DevString		*argout - name of control
		long		*error	- pointer to error code, in case
		 			  routine fails. Error code(s):
					  none
 ============================================================================*/

static long  dev_setcontrolvalue(ds, argin, argout, error)
LabViewGeneric 	ds;
DevVarStringArray *argin;
DevVoid *argout;
long 		  *error;
{
   long i, string_len;

#ifdef LV_DEBUG
   printf("START LabView dev_setcontrolvalue() START\n");
#endif /*PRINT*/

   lv_ds__strarr_in.length = argin->length;
   lv_ds__strarr_in.sequence = argin->sequence;

   lv_ds__argin = (void*)&lv_ds__strarr_in;
   lv_ds__argout = (void*)argout;
#ifdef LV_DEBUG
   printf("END LabView dev_setcontrolvalue END()\n");
#endif /*PRINT*/
   return(DS_OK);   
}


/*============================================================================
 Function:      static long dev_state()

 Description:	return the state of the device

 Arg(s) In:	 LabViewGeneric 	ds 	- 
		 DevVoid  	*argin  - none
   				  
 Arg(s) Out:	 DevShort	*argout - returned state 
		 long *error - 	pointer to error code, in case routine fails

============================================================================*/

static long dev_state(ds, argin, argout, error)

LabViewGeneric	ds;
DevVoid		*argin;
DevShort	*argout;
long		*error;
{
#ifdef LV_DEBUG
   printf("LabViewGeneric dev_state()\n");
#endif /* LV_DEBUG */
   *argout	= ds->devserver.state;
   return(DS_OK);
}

/*============================================================================
 Function:      static long dev_status()

 Description:	return state of the device as an ASCII string

 Arg(s) In:	 LabViewGeneric 	ds 	- 
		 DevVoid  	*argin  - none
   				  
 Arg(s) Out:	 DevString	*argout - contains string 
============================================================================*/

static long dev_status(ds, argin, argout, error)

LabViewGeneric        ds;
DevVoid         *argin;
DevString	*argout;
long		*error;
{
   static	char	str[80];

#ifdef LV_DEBUG
   printf("LabViewGeneric dev_status()\n");
#endif /* LV_DEBUG */
   sprintf(str,"LabViewGeneric is %s\n", DEVSTATES[ds->devserver.state]);
   *argout = str;
   return (DS_OK);
}

static xdrproc_t xdr_argument,xdr_result;
static SVCXPRT *ds__transp;
static char *ds__result;
static union {
		_dev_import_in	rpc_dev_import_4_arg;
		_dev_free_in 	rpc_dev_free_4_arg;
		_server_data 	rpc_dev_putget_4_arg;
		_server_data 	rpc_dev_put_4_arg;
                _dev_query_in   rpc_dev_query_cmd_4_arg;
		_server_data 	rpc_dev_putget_raw_4_arg;
		_server_data 	rpc_dev_put_asyn_4_arg;
	} argument;


/*+======================================================================
 Function   :   static void devserver_prog_4()

 Description:   Entry point for received RPCs.
	    :   Switches to the requested remote procedure.

 Arg(s) In  :   struct svc_rep *rqstp - RPC request handle
	    :   SVCXPRT *transp       - Service transport handle

 Arg(s) Out :   none

 Return(s)  :   none
=======================================================================-*/

static void _WINAPI devserver_prog_4 (struct svc_req *rqstp, SVCXPRT *transp) 
{
	char	*help_ptr;
	int	pid = 0;

	/*char *result;*/
#ifdef __cplusplus
	DevRpcLocalFunc local;
#else
	char *(*local)();
#endif

	/*
	 *  call the right server routine
	 */

	switch (rqstp->rq_proc) {
	case NULLPROC:
#ifdef _UCC
		svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
#else
		svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif /* _UCC */
		return;

        case RPC_QUIT_SERVER:
#ifdef _UCC
		svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
#else
		svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif /* _UCC */

#if defined (_NT)
                raise(SIGABRT);
#else  /* _NT */
#if !defined (vxworks)
                pid = getpid ();
#else  /* !vxworks */
                pid = taskIdSelf ();
#endif /* !vxworks */
                kill (pid,SIGQUIT);
#endif /* _NT */
		return;

	case RPC_CHECK:
		help_ptr = &(config_flags.server_name[0]);
		svc_sendreply (transp, (xdrproc_t)xdr_wrapstring, 
			       (caddr_t) &help_ptr);
		return;

	case RPC_DEV_IMPORT:
		xdr_argument = (xdrproc_t)xdr__dev_import_in;
		xdr_result = (xdrproc_t)xdr__dev_import_out;
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_dev_import_4;
#else
		local = (char *(*)()) rpc_dev_import_4;
#endif
		break;

	case RPC_DEV_FREE:
		xdr_argument = (xdrproc_t)xdr__dev_free_in;
		xdr_result = (xdrproc_t)xdr__dev_free_out;
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_dev_free_4;
#else
		local = (char *(*)()) rpc_dev_free_4;
#endif
		break;

	case RPC_DEV_PUTGET:
		xdr_argument = (xdrproc_t)xdr__server_data;
		xdr_result = (xdrproc_t)xdr__client_data;
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_cmd_get;
#else
		local = (char *(*)()) rpc_cmd_get;
#endif
		break;

	case RPC_DEV_PUT:
		xdr_argument = (xdrproc_t)xdr__server_data;
		xdr_result = (xdrproc_t)xdr__client_data;
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_dev_put_4;
#else
		local = (char *(*)()) rpc_dev_put_4;
#endif
		break;

        case RPC_DEV_CMD_QUERY:
		xdr_argument = (xdrproc_t)xdr__dev_query_in;
		xdr_result = (xdrproc_t)xdr__dev_query_out;
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_dev_cmd_query_4;
#else
		local = (char *(*)()) rpc_dev_cmd_query_4;
#endif
		break;

	case RPC_DEV_PUTGET_RAW:
		xdr_argument = (xdrproc_t)xdr__server_data;
		xdr_result = (xdrproc_t)xdr__client_raw_data;
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_dev_putget_raw_4;
#else
		local = (char *(*)()) rpc_dev_putget_raw_4;
#endif
		break;

	case RPC_DEV_PUT_ASYN:
		/*
		 * Read incoming arguments and send
                 * the reply immediately without waiting
		 * the execution of the function.
		 */
		xdr_argument = (xdrproc_t)xdr__server_data;
		xdr_result = (xdrproc_t)xdr__client_data;
	        /*
	         * Function only for the adminstration and security part
		 * of the asynchronous call.
		 */
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_dev_put_asyn_4;
#else
		local = (char *(*)()) rpc_dev_put_asyn_4;
#endif
	        break;

	case RPC_DEV_PING:
		xdr_argument = (xdrproc_t)xdr__dev_import_in;
		xdr_result = (xdrproc_t)xdr__dev_import_out;
#ifdef __cplusplus
		local = (DevRpcLocalFunc) rpc_dev_ping_4;
#else
		local = (char *(*)()) rpc_dev_ping_4;
#endif
		break;

	default:
		svcerr_noproc(transp);
		return;
	}


	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND,
		"svcerr_decode : server couldn't decode incoming arguments");
		svcerr_decode(transp);
		return;
	}


	ds__result = (*local)(&argument, rqstp);

	if  ( rqstp->rq_proc == RPC_DEV_PUTGET )
	{
		ds__transp = transp;

		return; /* LabViewGeneric will call sendreply() later */
	}

	if (ds__result != NULL && !svc_sendreply(transp, xdr_result, (caddr_t)ds__result)) 
	{
		dev_printerror (SEND,
		"svcerr_systemerr : server couldn't send repply arguments");
		svcerr_systemerr(transp);
	}


	/*
	 * If an asynchronous call was requested, execute now the
	 * command. After the answer was already send back to 
	 * the client.
	 */

	if ( rqstp->rq_proc == RPC_DEV_PUT_ASYN )
	   {
	   rpc_dev_put_asyn_cmd ((_server_data *)&argument);
	   }

	/*
	 * If dev_free() was called AND the server is on OS9 AND tcp
	 * then give OS9 a hand in closing this end of the tcp socket
	 * This fixes a bug in the OS9 (>3.x) implementation which led to
	 * the server blocking for a few seconds when closing a tcp 
	 * connection
	 */

	if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND,
		"svc_freeargs : server couldn't free arguments !!");
		return;
	}
}





/*+======================================================================
 Function   :   static long svc_check()

 Description:   Checks wether a device server with 
		the same name is already running.

 Arg(s) In  :   none

 Arg(s) Out :   long *error - Will contain an appropriate error
			      code if the corresponding call
			      returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
=======================================================================-*/

static long svc_check (long *error)
{
        CLIENT          *clnt;
	enum clnt_stat  clnt_stat;
	char		*host_name;
	char		*svc_name = NULL;
	unsigned int	prog_number;
	unsigned int	vers_number;

	*error = 0;

	if ( db_svc_check (config_flags.server_name,
			   &host_name, &prog_number, &vers_number, error) < 0 )
		return (-1);

   /*
	 * old server already unmapped ?
 	 */

	if ( prog_number == 0 )
	   {
	   return (0);
	   }

	/*
	 *  was the old server running on the same host ?
	 */

	if (strcmp (config_flags.server_host,host_name) != 0)
	   {
	   *error = DevErr_ServerRegisteredOnDifferentHost;
	   return (-1);
	   }

/*
 * force version number to latest API_VERSION [4] , new servers do not
 * support the old DSERVER_VERSION [1] anymore
 */
	vers_number = API_VERSION;

    /*
     *  old server still exists ?
	  */

	clnt = clnt_create (config_flags.server_host,
	  	  	    prog_number,vers_number,"udp");
	if (clnt != NULL)
	   {
           clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &msg_retry_timeout);
	        clnt_control (clnt, CLSET_TIMEOUT, (char *) &msg_timeout);

	   /*
	    *  call device server check function
	    */

           clnt_stat = clnt_call (clnt, RPC_CHECK, (xdrproc_t)xdr_void, NULL,
     				  (xdrproc_t)xdr_wrapstring, (caddr_t) &svc_name, 
			          TIMEVAL(msg_timeout));
 	   if (clnt_stat == RPC_SUCCESS)
	   {
	      if (strcmp (config_flags.server_name,
			  svc_name) == 0)
	         {
	         *error = DevErr_ServerAlreadyExists;
	         clnt_destroy (clnt);
	         return (-1);
		 }
	   }
	   else
	   {
	      if (clnt_stat != RPC_PROCUNAVAIL)
		 {
	         /*pmap_unset (prog_number, DEVSERVER_VERS);*/
	         pmap_unset (prog_number, API_VERSION);
	         pmap_unset (prog_number, ASYNCH_API_VERSION);
		 }
	   }

	   clnt_destroy (clnt);
	   }

	return (0);
}

#define PMAP_GETPORT pmap_getport
#include <rpc/pmap_clnt.h>
#include <sys/socket.h>

int gettransient_ut (char *ds_name, int *udp_socket, int *tcp_socket)
{

	u_long	base=0x50000000, key=131, maxoffset=0x10000000;
	u_long  offset, prognum, i;
	struct  sockaddr_in addr_udp, addr_tcp;
	int 	s_udp=RPC_ANYSOCK, s_tcp=RPC_ANYSOCK;
#ifdef _XOPEN_SOURCE_EXTENDED
	unsigned int ulen;
#else
	int len;
#endif /* _XOPEN_SOURCE_EXTENDED */
	u_short port_udp;
	

/*
 * determine the offset from base transient program number using a hashing
 * function to calculate a (almost) unique code based on the device
 * server full name (server/personal name). Because the personal
 * name (server/personal name) is unique in a TACO control system
 * the offset will be unique. The hashing function used is very similar
 * to the so-called "coalesced hashing" function also used by E.Taurel
 * in the data collector. It has been simplified here to the following
 *
 * hashing code = sum ( ds_name[i] * 131^i ) modulo 0x10000000
 *
 * The modulo is necessary because transient program numbers must lie 
 * between 40000000 and 5fffffff and to avoid confusion with old servers
 * a base of 0x50000000 has been used. 
 *
 * andy 12jun97
 */

	offset = 0;
	for (i=0; i<strlen(ds_name); i++)
	{
		offset = (offset*key + (u_long)ds_name[i])%maxoffset;
	}

	prognum = base + offset;


/*
 * Because the hashing function is not unique a check must be made for
 * collisions. This is done by looping round testing to see if the program 
 * number is already bound until an undbound program number is found.
 *
 * (1) bind a socket for UDP (needed by pmap_set() function below)
 */

        if ((s_udp = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
        {
           perror ("socket");
           return (0);
        }
	*udp_socket = s_udp;
        addr_udp.sin_addr.s_addr = 0;
        addr_udp.sin_family = AF_INET;
        addr_udp.sin_port = 0;

        /*
         * may be already bound, so do not check for error
         */

#if defined (_XOPEN_SOURCE_EXTENDED)
        ulen = sizeof (addr_udp);
        bind (s_udp, (struct sockaddr *)&addr_udp, ulen);
        if (getsockname (s_udp, (struct sockaddr *)&addr_udp, &ulen) < 0)
#else
        len = sizeof (addr_udp);
        bind (s_udp, (struct sockaddr *)&addr_udp, len);
        if (getsockname (s_udp, (struct sockaddr *)&addr_udp, &len) < 0)
#endif /* XOPEN_SOURCE_EXTENDED */
        {
           return (0);
        }
	port_udp = addr_udp.sin_port;
/*
 *
 * (2) bind a socket for TCP (nedded by pmap_set() function below)
 */
        if ((s_tcp = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        {
           return (0);
        }
	*tcp_socket = s_tcp;
        addr_tcp.sin_addr.s_addr = 0;
        addr_tcp.sin_family = AF_INET;
        addr_tcp.sin_port = 0;

        /*
         * may be already bound, so do not check for error
         */

#ifdef _XOPEN_SOURCE_EXTENDED
        ulen = sizeof (addr_tcp);
        bind (s_tcp, (struct sockaddr *)&addr_tcp, ulen);
        if (getsockname (s_tcp, (struct sockaddr *)&addr_tcp, &ulen) < 0)
#else
        len = sizeof (addr_tcp);
        bind (s_tcp, (struct sockaddr *)&addr_tcp, len);
        if (getsockname (s_tcp, (struct sockaddr *)&addr_tcp, &len) < 0)
#endif /* _XOPEN_SOURCE_EXTENDED */
        {
           return (0);
        }
/*	port_tcp = addr_tcp.sin_port;*/

/*
 * following problems encountered on OS9  we (JM+AG) have changed
 * strategy for requesting program numbers so as to avoid collisions
 * as much as possible
 *
 * In the new approach (not adopted for NT yet) we declare a program
 * number free only if we can reserve it using pmap_set() for both
 * protocols (UDP + TCP)
 *
 * NOTE: this approach has one NASTY side effect for some platforms
 *       e.g. Linux. Doing a pmap_set() to reserve the program number
 *       and then doing an svc_register() afterwards (in DevServerMain.c)
 *       will cause that port number + version number to be blocked.
 *       therefore we use the version=1 to reserve the program number.
 *       after which this version is not available for general use anymore
 *	 this is not a problem because the version 1 was only there
 *       for backwards compatibility with VERY old clients.
 *
 * andy+jens 26oct98
 */
/*
	while (!pmap_set(prognum, DEVSERVER_VERS, IPPROTO_UDP, port_udp) ||
	       !pmap_set(prognum, DEVSERVER_VERS, IPPROTO_TCP, port_tcp))
 */
	while (!pmap_set(prognum, DEVSERVER_VERS, IPPROTO_UDP, port_udp))
	{
#ifdef LV_DEBUG
        printf ("gettransient_ut(%s) : program number collision detected (0x%08x) !\n",
             ds_name, prognum);
#endif /* LV_DEBUG */

/*
 * increment the program number by 1 mod maxoffset
 */
		if ( prognum >= (base+maxoffset-1))
		{
			prognum = base;
		}
		else
		{
			prognum++;
		}
	}

/*
 * unregister any old program numbers lying around
 */

      	pmap_unset(prognum,API_VERSION);
     	pmap_unset(prognum,ASYNCH_API_VERSION); 

      	return (prognum);
}

extern DevServerDevices *devices;

/*+======================================================================
 Function   :   extern _client_data *rpc_cmd_get()

 Description:   RPC procedure corresponding to LabViewGeneric lv_ds__cmd_get().
            :   Its only job is to unpack the input arguments and 
		make them available to LabView.

 Arg(s) In  :   _server_data *server_data - contains information
                about device identification, the command, and
                the input and output arguments for the command.

 Arg(s) Out :   none

 Return(s)  :   _client_data : contains the output arguments of
                the executed command,
                the status of the function (0 or -1) and
                an appropriate error number if the function fails.
=======================================================================-*/

static _client_data * _DLLFunc rpc_cmd_get (_server_data *server_data)
{
        static _client_data     client_data;
        DevServer               ds;
        long                    ds_id;
        long            connection_id;

        DevDataListEntry        data_type;


#ifdef LV_DEBUG
        dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS,
            "\nrpc_dev_putget_4() : entering routine\n");
        dev_printdebug ( DBG_DEV_SVR_CLASS,
            "\nrpc_dev_putget_4() : with ds_id = %d\n",
            server_data->ds_id);
#endif /* LV_DEBUG */

   /*
    * Free and preset the structure for outgoing arguments.
    */
        free (client_data.argout);
        memset ((char *)&client_data,0,sizeof(client_data));


   /*
    * Split up the device identification.
    */

        if (read_device_id (server_data->ds_id, &ds_id, &connection_id,
            &client_data.error) == DS_NOTOK)
        {
                client_data.status = DS_NOTOK;
                return (&client_data);
        }

        ds = devices[(_Int)ds_id].ds;

   /*
    * Do the security checks for the command access.
    */

        if ( config_flags.security == True )
        {
                if ( sec_svc_cmd (&devices[(_Int)ds_id], connection_id,
                    server_data->client_id, server_data->access_right,
                    server_data->cmd, &client_data.error)
                    == DS_NOTOK )
                {
                        client_data.status = DS_NOTOK;
                        return (&client_data);
                }
        }

/*
 *  allocate and initialise outgoing arguments
 */

        client_data.argout_type = server_data->argout_type;

        if (client_data.argout_type != D_VOID_TYPE)
        {                                              
     /*
      * Get the XDR data type from the loaded type list
      */

                if ( xdr_get_type( client_data.argout_type, &data_type,
                    &client_data.error) == DS_NOTOK)
                {
#ifdef LV_DEBUG
                        dev_printdebug (DBG_ERROR | DBG_DEV_SVR_CLASS,
                            "\nrpc_dev_putget_4() : xdr_get_type(%d) returned error %d\n",
                            client_data.argout_type, client_data.error);
#endif /* LV_DEBUG */

                        client_data.status = DS_NOTOK;
                        return (&client_data);
                }


                client_data.argout =
                    (char *) malloc ((unsigned int)data_type.size);
                if ( client_data.argout == NULL )
                {
                        client_data.status = DS_NOTOK;
                        client_data.error  = DevErr_InsufficientMemory;
                        return (&client_data);
                }
                memset (client_data.argout, 0, (size_t)data_type.size);
        }
        else
        {
                client_data.argout = NULL;
        }

	cmd_in = server_data->cmd;

/*
 * in the simple case the command is passed directly on to the command_handler
 * method
 */
        client_data.status = (ds__method_finder (ds, DevMethodCommandHandler))
            (   ds,
            server_data->cmd,
            server_data->argin,
            server_data->argin_type,
            client_data.argout,
            client_data.argout_type,
            &client_data.error);

        /*
 *  Because in case of error the status of the outgoing arguments
 *  is undefined, initialise argout to NULL before serialising.
 */

        if (client_data.status == -1)
        {
                free (client_data.argout);
                client_data.argout = NULL;
        }

        return (&client_data);
}

/*
 * lv_ds__sendreply()
 */

static long lv_ds__sendreply()
{
/*printf(__FUNCTION__);
printf("svc_freeargs\n");*/
	{
		DevVarStringArray *a = (DevVarStringArray *)lv_ds__argout;
		int i;
		printf("%p %p\n", a, &argument);
		/*for (i = 0; i < a->length; ++i)
			printf("string out a : %s\n", a->sequence[i]);*/
	}
        if (!svc_freeargs(ds__transp, xdr_argument, (caddr_t) &argument))
        {
        	dev_printerror (SEND,
                        "svc_freeargs : server couldn't free arguments !!");
                        return;
	}
//printf("svc_sendreply\n");
        if (!svc_sendreply(ds__transp, xdr_result, (caddr_t)ds__result))
        {
                dev_printerror (SEND,
                "svcerr_systemerr : server couldn't send reply arguments");
                /*svcerr_systemerr(transp);*/
        }
//printf("done\n");
}
/*+======================================================================
 Function   :	static long read_device_id ()

 Description:	Split up the device identification into its 
		information fields.
		ds_id -> Place in the array of exported devices.
		connection_id -> Place in the connections (or
		client access) array of an exported device.
		export_count-> A counter to avoid access from
		ancient clients of destroyed devices. 

 Arg(s) In  :	device_id   - client handle to access the device.

 Arg(s) Out :   long *ds_id         - access to exported device.
		long *connection_id - access to device connections.
		long *error - pointer to error code, in case routine fails.

 Return(s)  :	DS_OK / DS_NOTOK
=======================================================================-*/

static long read_device_id (long device_id, long *ds_id, long *connection_id, long *error)
{
	long 	export_counter;

#ifdef LV_DEBUG
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS,
	    "\nread_device_id() : entering routine\n");
#endif /* LV_DEBUG */

	*error = 0;

	/*
    	* Split up the device identification
    	*/

	*ds_id        = device_id & DEVICES_MASK;

	export_counter = device_id >> COUNT_SHIFT;
	export_counter = export_counter & COUNT_MASK;

	*connection_id = device_id >> CONNECTIONS_SHIFT;
	*connection_id = *connection_id & CONNECTIONS_MASK;

	/*
    	* Verify the count of exports.
    	*/

	if ( export_counter != devices[(_Int)*ds_id].export_counter )
	{
		*error = DevErr_DeviceNoLongerExported;
		return (DS_NOTOK);
	}

#ifdef LV_DEBUG
	dev_printdebug (DBG_DEV_SVR_CLASS,
	    "read_device_id() : ds_id = %d   conn_id = %d\n",
	    *ds_id, *connection_id);
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS,
	    "read_device_id() : leaving routine\n");
#endif /* LV_DEBUG */

	return (DS_OK);
}
