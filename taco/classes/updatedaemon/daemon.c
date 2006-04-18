static char RcsID[]="$Header: /home/jkrueger1/sources/taco/backup/taco/classes/updatedaemon/daemon.c,v 1.3 2006-04-18 06:25:59 jkrueger1 Exp $";

/*********************************************************************

 File:		daemon.c

 Project:	Device Servers

 Description:	source code to implement DaemonClass

 Author(s);	Michael Schofield

 Original:	April 1992

 Copyleft (c) 1992 by European Synchrotron Radiation Facility, 
                      Grenoble, France


 *********************************************************************/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <Admin.h>
#include <DevErrors.h>
#include <DevServer.h>
#include <DevSignal.h>

#include <dataport.h>

#include <ud_server.h>
#include <DevServerP.h>
#include <ud_serverP.h>

#include <daemon_xdr.h>
#include <private/ApiP.h>
#include <daemon.h>

/* long minimal_access = WRITE_ACCESS; */

/*
 * public methods
 */

static long class_initialise();
static long object_create();
static long state_handler();
static long object_initialise();

static DevMethodListEntry methods_list[] = {
 {DevMethodClassInitialise, class_initialise},
 {DevMethodCreate, object_create},
 {DevMethodInitialise, object_initialise},
 {DevMethodStateHandler, state_handler},
};

DaemonClassRec daemonClassRec = {
   /* n_methods */        sizeof(methods_list)/sizeof(DevMethodListEntry),
   /* methods_list */     methods_list,
};

DaemonClass daemonClass = (DaemonClass)&daemonClassRec;

#if defined (TEST)
db_resource class_tab [] = {
   {"ud_test_path", D_STRING_TYPE},
   {"ud_test_name", D_STRING_TYPE},
   };
#else
db_resource class_tab [] = {
   {"ud_poller_path", D_STRING_TYPE},
   {"ud_poller_name", D_STRING_TYPE},
   };
#endif

db_resource pl_tab [] = {
   {"ud_poll_list",   D_VAR_STRINGARR},
   };
db_resource res3[] = {
		      {"dir_path", D_STRING_TYPE},
	             }; 
db_resource resource1[] = {
		           {"ud_command_list", D_VAR_STRINGARR},
			  };  

db_resource resource2[] = {
		           {"ud_poll_interval", D_LONG_TYPE},
			  };  

extern configuration_flags config_flags;

/**************************************************************/
/** daemon resource table used to access the static database **/
/**************************************************************/

static db_resource res_class[] = {
			          {"state",	D_LONG_TYPE,	NULL},
                                 };
static  int res_class_size = sizeof(res_class)/sizeof(db_resource);

static char* polres_name[] = {
			        "ud_poll_list",
                              };

static char* devres[] = {
			   "ud_command_list",
			   "ud_poll_interval",
                         };

/*
 * public commands
 */

static long dev_off();  
static long dev_on();   
static long dev_state();
static long dev_getdeviceident();
static long dev_getdevicename();
static long dev_initialisecmd();
static long dev_initialisedev();
static long dev_startpolling();
static long dev_stoppolling();
static long dev_changeinterval();
static long dev_definemode();
static long dev_pstatus();
static long dev_dastatus();
static long dev_longstatus();
static long dev_haltsystem();
static long dev_saveconfi();
static long dev_removedevice();
static long dev_poll_list_uptodate();
static long dev_status();

void check_timestamp();

static DevCommandListEntry commands_list[] = {
/* {DevOff,                dev_off,            D_VOID_TYPE,   D_VOID_TYPE}, */
/* {DevOn,                 dev_on,             D_VOID_TYPE,   D_VOID_TYPE}, */
 {DevState,              dev_state,          D_VOID_TYPE,   D_SHORT_TYPE},
 {DevGetDeviceIdent,     dev_getdeviceident, D_STRING_TYPE, D_LONG_TYPE},
 {DevGetDeviceName,      dev_getdevicename,  D_LONG_TYPE,   D_STRING_TYPE},
 {DevInitialiseDevice,   dev_initialisecmd,  D_DAEMON_STRUCT, D_LONG_TYPE},
 {DevStartPolling,       dev_startpolling,   D_LONG_TYPE,   D_VOID_TYPE},
 {DevStopPolling,        dev_stoppolling,    D_LONG_TYPE,   D_VOID_TYPE},
 {DevChangeInterval,     dev_changeinterval, D_DAEMON_DATA, D_VOID_TYPE},
 {DevLongStatus,         dev_longstatus,     D_VOID_TYPE,   D_STRING_TYPE},
 {DevDefineMode,         dev_definemode,     D_DAEMON_DATA, D_VOID_TYPE},
 {DevPollStatus,         dev_pstatus,        D_LONG_TYPE,   D_DAEMON_STATUS},
 {DevAccessStatus,       dev_dastatus,       D_LONG_TYPE,   D_DAEMON_STATUS},
 {DevSaveConfi,      	 dev_saveconfi,      D_VOID_TYPE,   D_VOID_TYPE},
 {DevRemoveDevice,	 dev_removedevice,   D_LONG_TYPE,   D_VOID_TYPE},
 {DevStatus,             dev_status,         D_VOID_TYPE,   D_STRING_TYPE}, 
};

static long n_commands = sizeof(commands_list)/sizeof(DevCommandListEntry);


/*
 * reserve space for a default copy of the daemon object
 */

static DaemonRec daemonRec;
Daemon ud_daemon = (Daemon)&daemonRec;
static Dataport *dp;
static dataport_struct_type *dipc;

static char dataport_name[64];
static char poller_path[128];
static char poller_name[128];
struct timeval wait_time = {0,GET_DP_WAIT};
struct timeval wait_time_2 = {SEC_GET_DP_WAIT,0};

DevVarStringArray stringarray;


/************************************************************************

 Function   :  void signal_handler()

 Description:  function which is executed on reception of certain
               signals, allowing graceful shutdown of daemon and poller

 Arg(s) In  :  none

 Arg(s) Out :  none

 Return(s)  :  nothing - should return to API signal handler

*************************************************************************/
void signal_handler(signal)
int signal;
{

int retkill;

#if defined (EBUG)
   fprintf(stderr,"THIS IS THE SIGNAL HANDLER\n");
   fprintf(stderr,"Sending SIGTERM to process %ld\n",(long)(dipc->poller_id));
   fprintf(stderr,"Closing %s\n",DAEMON_DATAPORT);
#endif

   printf ("UD_DAEMON: SIGNAL HANDLER WITH SIGNAL=%d\n",signal);
   fflush(stdout);

   if (dipc->poller_id != 0)
   {
#if defined (EBUG)
     fprintf(stderr,"SEND A SIGINT SIGNAL : %d\n",SIGINT);
#endif

printf ("UD_DAEMON is going to kill UD_POLLER (signal handler)\n");fflush(stdout);
/* OSD */
#if (OSK || _OSK)
     retkill = kill ((int)(dipc->poller_id),SIGINT);    /* Stop poller */
#else
     retkill = kill ((pid_t)(dipc->poller_id),SIGINT);    /* Stop poller */
#endif

#if defined (EBUG)
     fprintf (stderr, "retkill = %d\n",retkill);
#endif

     if (retkill == -1)
     {
       perror("kill error : ");
     }
   }
   sleep (2);
   CloseDataport (dp,dataport_name);

/* removed exit because the API signal handler will do this */

}


/************************************************************************

 Function   :  long initialise_ipc(pid)

 Description:  initialises the dataport required for the inter-process
               communication (with the poller). Failure of this procedure
               will cause failure of the update daemon to correctly startup.

 Arg(s) In  :  pid - the process id of the daemon.

 Arg(s) Out :  none

 Return(s)  :  DS_OK    - if dataport correctly initialised
               DS_NOTOK - otherwise

*************************************************************************/
long initialise_ipc(pid)

#if (OSK || _OSK)
int pid;
#else
pid_t pid;
#endif

{
#if defined(EBUG)
  fprintf(stderr,"initialise_ipc()\n");
#endif

  Make_Dataport_Name (dataport_name,DAEMON_DATAPORT,pid);

  dp=OpenDataport (dataport_name,sizeof(dataport_struct_type));
  if (dp==NULL)
  {
    dp=CreateDataport (dataport_name,sizeof(dataport_struct_type));
    if (dp==NULL)
    {
#if defined(EBUG)
      fprintf(stderr,"CreateDataport failed.\n");
      fprintf(stderr,"endf initialise_ipc()\n");
#endif

      return(DS_NOTOK);
   } 
  } 
  dipc = (dataport_struct_type *)&(dp->body);
  AccessDataport(dp);
    dipc->status=D_INIT;
/* OSD */
    dipc->timestamp=(long)time((time_t *)0);
    dipc->poller_id=0;
    dipc->signal_valid=FALSE;
  ReleaseDataport(dp);

#if defined(EBUG)
  fprintf(stderr,"end initialise_ipc()\n");
#endif

  return(DS_OK);
}


/************************************************************************

 Function   :  long get_ready_dp()

 Description:  ensures the user has access to a dataport which is in
               a D_READY state.

 Arg(s) In  :  none

 Arg(s) Out :  none

 Return(s)  :  DS_OK
               Note that returning from this function leaves the user with
               access rights to the dataport.

*************************************************************************/
long get_ready_dp()
{
   int ready=FALSE;
   int first=FALSE;

#if defined(EBUG)
   fprintf(stderr,"get_ready_dp()\n");
#endif

   while (ready==FALSE)
   {
     check_timestamp();
     AccessDataport(dp);
     if (dipc->status==D_READY)
     {
       ready=TRUE;
     }
     ReleaseDataport(dp);
 /*    else
     {
       ReleaseDataport(dp);
     }*/
/* to avoid a TOO busy wait, sleep for wait_time */
/* OSD */
     if (ready == FALSE)
     {
     	if (first==FALSE)
     	{
		wait_time.tv_sec = 0;
		wait_time.tv_usec = GET_DP_WAIT;
		select (0,0,0,0,&wait_time);
		first=TRUE;
     	}
     	else
     	{
		wait_time_2.tv_sec = SEC_GET_DP_WAIT;
		wait_time_2.tv_usec = 0;
     		select (0,0,0,0,&wait_time_2);
     	}
      }
   }

#if defined(EBUG)
  fprintf(stderr,"end get_ready_dp()\n");
#endif

   return(DS_OK);
}



/************************************************************************

 Function   :  long let_go_dp()

 Description:  releases the dataport and informs the poller process that
               the communication state has changed by sending a SIGUSR1
               (if this is permitted). dipc->signal_valid is only set to
               TRUE by the poller process in the case that it requires to
               wait before a poll.

 Arg(s) In  :  none

 Arg(s) Out :  none

 Return(s)  :  DS_OK
               Note that returning from this function leaves the user with
               access rights to the dataport.

*************************************************************************/
long let_go_dp()
{

#if defined(EBUG)
  	fprintf(stderr,"let_go_dp()\n");
#endif

  	if (dipc->signal_valid==TRUE)
  	{
/* OSD */
#if (OSK || _OSK)
    		kill ((int)(dipc->poller_id), SIGUSR1); 
#else
    		kill ((pid_t)(dipc->poller_id), SIGUSR1);
#endif
/*   		dipc->signal_valid=FALSE;*/
  	}

  ReleaseDataport(dp);

#if defined(EBUG)
  	fprintf(stderr,"end let_go_dp()\n");
#endif

  	return (DS_OK);

}


/************************************************************************

 Function   :  void check_timestamp()

 Description:  checks that the poller process is running by examining the
               timestamp of the dataport. If the timestamp is too old, the
               signal_handler is called by raising a SIGINT signal.

 Arg(s) In  :  none

 Arg(s) Out :  none

 Return(s)  :  nothing

*************************************************************************/
void check_timestamp()
{
   long now_time;

/* OSD */
   now_time = time((time_t *)0);
   if (now_time > ((dipc->timestamp)+TIMEOUT))
   {

#if defined(EBUG)
     fprintf(stderr,"Timestamp has expired .. exiting\n");
#endif

printf ("UD_DAEMON will halt because of (Timestamp)\n");fflush(stdout);
printf ("  It will raise a SIGINT signal itself\n");fflush(stdout);

#ifdef OSK
     kill (getpid(), SIGINT);
#else
#ifdef sun
     kill (getpid(),SIGINT);
#else
     raise (SIGINT);
#endif /* sun */
#endif /* OSK */
   }
}


/************************************************************************

 Function   :  long get_response_dp(message,data,error)

 Description:  This function waits for a response from a previously
               despatched command via the dataport.

 Arg(s) In  :  none

 Arg(s) Out :  message  - the string output from the poller
               data     - the data array returned from the poller
               error    - command error 

 Return(s)  :  DS_OK    -  if the previously sent command to the poller was OK
               DS_NOTOK - otherwise

*************************************************************************/
long get_response_dp(message,data,error)
char *message;
long *data;
long *error;
{
   int finished=FALSE;
   register int i;
   int good_stat = FALSE;
   int first=FALSE;

#if defined(EBUG)
  fprintf(stderr,"get_response_dp ()\n");
#endif

   check_timestamp();
   while (good_stat == FALSE)
   {
     AccessDataport(dp);
     if (dipc->status==D_PROCESSED || dipc->status==D_ERROR)
     {
       good_stat = TRUE;
     }
     ReleaseDataport(dp);
     check_timestamp();
/* to avoid a TOO busy wait, sleep for wait_time */
/* OSD */

     if (good_stat == FALSE)
     {
     	if (first == FALSE)
     	{
		wait_time.tv_sec = 0;
		wait_time.tv_usec = GET_DP_WAIT;
     		select (0,0,0,0,&wait_time);
		first = TRUE;
     	}
     	else
     	{
		wait_time_2.tv_sec = SEC_GET_DP_WAIT;
		wait_time_2.tv_usec = 0;
     		select (0,0,0,0,&wait_time_2);
     	}
      }
   }
   AccessDataport(dp);

#if defined(EBUG)
   fprintf(stderr,"status=%d\n",dipc->status);
   for (i=0;i<5;i++)
     fprintf(stderr,"data[%d]=%d\n",i,dipc->data[i]);
   fprintf(stderr,"string=%s\n",dipc->string);
#endif

   if (dipc->status==D_ERROR)
   {
     *error=dipc->error;
     strcpy(message,dipc->string);
     dipc->status=D_CLEARED;
     let_go_dp(); /* error obtained so release dp */

#if defined(EBUG)
  fprintf(stderr,"endf get_response_dp ()\n");
#endif

     return(DS_NOTOK);
   }
   else
   {
     for (i=0;i<5;i++)
     {
       data[i]=dipc->data[i];
     }
     strcpy(message,dipc->string);
     dipc->status=D_CLEARED;
     let_go_dp(); /* data obtained so release dp */
#if defined(EBUG)
  fprintf(stderr,"end get_response_dp ()\n");
#endif

     return(DS_OK);
   }
}


/************************************************************************

 Function:      static long class_initialise()

 Description:	routine to be called the first time a device is 
		created which belongs to this class (or is a subclass
		thereof). This routine will be called only once.

 Arg(s) In:	none

 Arg(s) Out:	long *error - pointer to error code, in case routine fails

 Function   :  get_response_dp(message,data,error)

*************************************************************************/
static long class_initialise(error)
long *error;
{
   int iret = DS_OK;
   long status;
   long class_tab_size=sizeof(class_tab) / sizeof(db_resource);


#if (OSK || _OSK)
   unsigned int i,j,l;
   char pn[60],sn[60],s[60];
   char *pers_name,*svr_name,*svr;
   char *pol_name[3]; 
   int proc_pid, fork_pid;
   extern int os9forkc();
   extern char **_environ;
#else
   pid_t proc_pid, fork_pid;
#endif

   static char pid_string[10];
   char *path_res,*name_res;
   char *t_path;


/* setup signal handling, using API calls to ensure correct termination */

/* OSD ? */
   status = ds__signal (SIGINT, signal_handler, error);
   status = ds__signal (SIGTERM, signal_handler, error);
   status = ds__signal (SIGALRM, signal_handler, error);
   status = ds__signal (SIGQUIT, signal_handler, error);

#if defined(TRACE)
   printf("class_initialise \n");
#endif

   if (status != DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"signal handler setup failed\n");
#endif

   }
 


/*
 * DaemonClass is a subclass of the DevServerClass
 */


   daemonClass->devserver_class.superclass = devServerClass;
   daemonClass->devserver_class.class_name = (char*)malloc(sizeof("DaemonClass"));
   if (daemonClass->devserver_class.class_name==0)
   {
     *error=DevErr_InsufficientMemory;
     return (DS_NOTOK);  /* No memory allocated */
   }
   sprintf(daemonClass->devserver_class.class_name,"DaemonClass");
   daemonClass->devserver_class.class_inited = 1;
   daemonClass->devserver_class.n_commands = n_commands;
   daemonClass->devserver_class.commands_list = commands_list;

/*********************************************/
/**  initialise daemon with default values  **/
/*********************************************/

   ud_daemon->devserver.class_pointer = (DevServerClass)daemonClass;
   ud_daemon->devserver.state = DEVON;
  
/********************************************************/
/** Interrogate the static database for default values **/
/********************************************************/

   res_class[0].resource_adr = &(ud_daemon->devserver.n_state);

   if (db_getresource("CLASS/Daemon/DEFAULT",res_class,res_class_size,error))
   {
     return(DS_NOTOK);
   } 

/**************************************************************/
/** three private structure definitions for daemon commandis **/
/**************************************************************/
   
   if ( LOAD_DAEMON_DATA(error) == DS_NOTOK ) { return(DS_NOTOK); }
   if ( LOAD_DAEMON_NEWD(error) == DS_NOTOK ) { return(DS_NOTOK); }
   if ( LOAD_DAEMON_STATUS(error) == DS_NOTOK ) { return(DS_NOTOK); }
   

/*******************************************************/
/* Get the TACO_PATH env. variable                     */
/*******************************************************/

   t_path = getenv("TACO_PATH");
   if (t_path == NULL)
   {
   	strcpy (poller_path, POLLER_PATH);
   	strcpy (poller_name, "");
   }
   else
   {
   	strcpy(poller_path, t_path);
	strcpy(poller_name, "/ud_poller");
   }
   path_res = NULL;
   name_res = NULL;
   class_tab[0].resource_adr = &path_res;
   class_tab[1].resource_adr = &name_res;
   fork_pid = 0;
   
   status = db_getresource("SYS/DaemonClass/Default",class_tab,class_tab_size,error);
   if (status != DS_OK)
   {
      status = db_getresource("SYS/DaemonClass/Default",class_tab,class_tab_size,error);
      if (status != DS_OK)
      {
#if defined(EBUG)
        fprintf(stderr,"endf class_initialise()\n");
#endif
        return (DS_NOTOK);
      }
   }
   

   if (path_res != NULL)
   {
     strcpy (poller_path,path_res);
   }

   if (name_res != NULL)
   {
     strcpy (poller_name,name_res);
   }
   
   if (poller_path[0]=='?')
   {
     poller_path[0]='/';  
     /* Static DB does not accept / as first character */
     /* of a resource, so ? used instead               */
   }
   
   strcat (poller_path, poller_name);
   fprintf (stderr, "UD_POLLER TO EXECUTE = %s\n",poller_path);
   proc_pid = getpid (); /* get this process' id */
#if defined (EBUG)
   fprintf(stderr,"daemon pid = %ld\n",(long)proc_pid);
#endif
   status=initialise_ipc(proc_pid); /* initialise dataport */
   if (status!=DS_OK)
   {
     fprintf(stderr,"initialise_ipc() failed !\n");
     return (DS_NOTOK);
   }

/*#if defined(FORK)*/

#if (OSK || _OSK)
 pol_name[0] = "ud_poller";
 sprintf (pid_string, "%ld", proc_pid);
 pol_name[1] = (char *) pid_string;
 pol_name[2] = NULL;
 fork_pid = os9exec (os9forkc, pol_name[0], pol_name, _environ, 0, 0, 3);
#else
 fork_pid = fork();    /* Forks this process in two */
#endif


   if (fork_pid == -1)   /* fork failed for some reason */
   {
     fprintf (stderr,"Failed to fork poller process. Exiting\n");
     perror ("");
     exit (-1);
   }
  

#if defined (unix)

   if (fork_pid == 0)   /* now the child process */
   {
    /* setup the poller argument, the daemon's process id */
    sprintf (pid_string, "%ld", proc_pid);
#if defined (FORKDEBUG)
    fprintf (stderr,"execl (%s,%s)\n",poller_path,pid_string);
#endif
   /* startup the poller process with pid_string being the argument */
   printf("execl() poller %s\n",poller_path);

#ifdef unix
   execl (poller_path, poller_path, pid_string, NULL);
#endif

    printf ("Failed to exec poller process %s. Exiting\n",poller_path);


/* very serious error, so do a full stop */
     perror ("");
#ifdef OSK
     kill (proc_pid,SIGINT);   /* kill off father */
     kill (getpid(), SIGINT);  /* kill itself     */
#else
     kill (proc_pid,SIGINT);   /* kill off father */
#ifdef sun
     kill (getpid(),SIGINT);
#else
     raise (SIGINT);         /* kill itself */
#endif /* sun */
#endif
     exit (-1);         /* just in case, shouldn't get here */

   } 
   else
   {
			/* must still be the parent */
     ;                  /* so continue as normal */

#if defined (EBUG)
     fprintf (stderr,"Poller process forked and execed.\n");
#endif

   }

#endif /* unix */

/*#endif*/

/* end FORK */
  
#if defined(TRACE)
   printf("end class_initialise \n");
#endif

   return(iret);
}


/************************************************************************

 Function    : static long object_create()

 Description : routine to be called on creation of a device object

 Arg(s) In   : char *name - name to be given to device

 Arg(s) Out  : DevServer *ds_ptr - pointer to created device
	       long      *error - pointer to error code, in case routine fails

 Return(s)   : DS_OK    
               DS_NOTOK - otherwise

*************************************************************************/
static long object_create(name, ds_ptr, error)
char *name;
DevServer *ds_ptr;
long *error;
{
   int iret = DS_OK;
   Daemon dae;

#if defined(TRACE)
   printf("object_create \n");
#endif

   dae = (Daemon)malloc(sizeof(DaemonRec));
   if (dae==0)
   {
     *error=DevErr_InsufficientMemory;
     return(DS_NOTOK);
   }

/*
 * initialise device with default object
 */

   *(DaemonRec*)dae = *(DaemonRec*)ud_daemon;

/*
 * finally initialise the non-default values
 */

   dae->devserver.name = (char*)malloc(strlen(name)+1);
   if (dae->devserver.name==0)
   {
     *error=DevErr_InsufficientMemory;
     return(DS_NOTOK);
   }
   sprintf(dae->devserver.name,"%s",name);

   *ds_ptr = (DevServer)dae;

#if defined(TRACE)
   printf("end object_create \n");
#endif

   return(iret);
}


/************************************************************************

 Function    : static long object_initialise()

 Description : routine to be called on creation of a device object

 Arg(s) In   : char *name - name to be given to device

 Arg(s) Out  : DevServer *ds_ptr - pointer to created device
	       long      *error - pointer to error code, in case routine fails

 Return(s)   : DS_OK    
               DS_NOTOK - otherwise

*************************************************************************/
static long object_initialise(ds,error)
DevServer ds;
long *error;
{
   long status; 
   int i;
   Daemon dae;
   long pl_tab_size=sizeof(pl_tab) / sizeof(db_resource);
   struct DevDaemonStruct argin;
   DevLong argout;
   char d_n[60];
   db_resource resource4;
   unsigned int num_resource,new_memory; 
   DevBoolean db_update;

#if defined(EBUG)
  fprintf(stderr,"object_initialise()\n");
#endif

   db_update = '0';;
   argin.dev_n = d_n;
   argin.cmd_list.length = 0;
   argin.cmd_list.sequence= NULL;
   argin.poller_frequency = 0;

   dae = (Daemon)ds;

   /* ds->devserver.state = DevOn; */

   pl_tab[0].resource_adr = &stringarray;

   status = db_getresource(dae->devserver.name,pl_tab,pl_tab_size,error);
   if (status != DS_OK)
   {
      status = db_getresource(dae->devserver.name,pl_tab,pl_tab_size,error);
      if (status != DS_OK)
      {
#if defined(EBUG)
        fprintf(stderr,"endf object_initialise()\n");
#endif
	return (DS_NOTOK);
      }
   }
   printf("Number of devices for this daemon : %d\n",stringarray.length);
#if defined(EBUG)
   fprintf(stderr,"stringarray.length=%d\n",stringarray.length);
#endif
   for (i=0;i<stringarray.length;i++)
   {
      argin.dev_n = (char *) stringarray.sequence[i];
#if defined(EBUG)
     fprintf(stderr,"call dev_initialisedev() with argin = %s\n",argin.dev_n);
#endif
     status = dev_initialisedev (ds, &(argin.dev_n), &argout, db_update, error);
   }
  
#if defined(EBUG)
   fprintf(stderr,"end object_initialise()\n");
#endif
 

   for (i=0;i<(argin.cmd_list.length);i++)
   {
    free((argin.cmd_list.sequence)[i]);   
   }
   free(argin.cmd_list.sequence);   
   return (DS_OK);
}


/************************************************************************

 Function    :  static long state_handler()

 Description :	this routine is reserved for checking whether the command
		requested can be executed in the present state.

 Arg(s) In   :	Daemon ds - device on which command is to executed
		DevCommand cmd - command to be executed

 Arg(s) Out  :	long *error - pointer to error code, in case routine fails

 Return(s)   :  DS_OK    
                DS_NOTOK - otherwise

*************************************************************************/

static long state_handler( ds, cmd, error)
Daemon ds;
DevCommand cmd;
long *error;
{
    
  long int p_state, n_state;
  long iret = DS_OK;

  
  p_state = ds->devserver.state;

  n_state = p_state;

  switch (p_state)
  {

   case (DEVON) :
   {

     switch (cmd)
     {
	/* Allowed commands */

	case (DevGetDeviceIdent): 	n_state = DEVON;break;
	case (DevGetDeviceName): 	n_state = DEVON;break;
	case (DevInitialiseDevice): 	n_state = DEVON;break;
	case (DevStartPolling): 	n_state = DEVON;break;
	case (DevStopPolling): 		n_state = DEVON;break;
	case (DevChangeInterval): 	n_state = DEVON;break;
	case (DevLongStatus): 		n_state = DEVON;break;
	case (DevDefineMode): 		n_state = DEVON;break;
	case (DevPollStatus): 		n_state = DEVON;break;
	case (DevSaveConfi): 		n_state = DEVON;break;
	case (DevRemoveDevice): 	n_state = DEVON;break;
	case (DevStatus): 		break;
	case (DevState): 		break;

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



/************************************************************************
 Function:      static long dev_off()

 Description:	routine to switch daemon off

 Arg(s) In:	Daemon ds - daemon to be switched off
		void *argin - none

 Arg(s) Out:	void *argout - none
		long *error - pointer to error code, in case routine fails
*************************************************************************/
static long dev_off (ds, argin, argout, error)
Daemon ds;
void *argin;
void *argout;
long *error;
{
   long status;
   char mess[255];
   long data[5];

#if defined(EBUG)
 fprintf(stderr,"dev_off()\n");
#endif

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevOff;
   dipc->status=D_NEW;
   status=let_go_dp();    /* release dataport */
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
 fprintf(stderr,"endf dev_off()\n");
#endif
     return(DS_NOTOK);
   }

#if defined(EBUG)
 fprintf(stderr,"end dev_off()\n");
#endif

   return(DS_OK);
}  

/************************************************************************
 Function:      static long dev_on()

 Description:	routine to switch daemon on

 Arg(s) In:	Daemon ds - daemon to be switched on
		void *argin - none

 Arg(s) Out:	void *argout - none
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_on (ds, argin, argout, error)
Daemon ds;
DevVoid *argin;
DevVoid *argout;
long *error;
{
   long status;
   char mess[255];
   long data[5];

#if defined(EBUG)
  fprintf(stderr,"dev_on()\n");
#endif

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevOn;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_on()\n");
#endif

     return(DS_NOTOK);
   }

#if defined(EBUG)
  fprintf(stderr,"end dev_on()\n");
#endif

   return(DS_OK);
}

/************************************************************************
 Function:      static long dev_state()

 Description:	routine to read state of daemon

 Arg(s) In:	Daemon ds - update daemon to work with
		void *argin - none

 Arg(s) Out:	short *argout - returned state
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_state (ds, argin, argout, error)
Daemon ds;
DevVoid *argin;
DevShort *argout;
long *error;
{
   long status;
   char mess[255];
   long data[5];

#if defined(EBUG)
  fprintf(stderr,"dev_state()\n");
#endif


   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevState;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);

#if defined(EBUG)
   fprintf(stderr,"state message = %s\n",mess);
#endif

   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_state()\n");
#endif

     return(DS_NOTOK);
   }

   *argout = data[0];

#if defined(EBUG)
   fprintf(stderr,"end dev_state()\n");
#endif

   return(DS_OK);
}


/************************************************************************
 Function:      static long dev_status()

 Description:	dummy routine to return state as an ASCII string 

 Arg(s) In:	Daemon ds - update daemon to work with
		void *argin - none

 Arg(s) Out:	DevString *argout - contains string 
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_status (ds, argin, argout, error)
Daemon ds;
DevVoid *argin;
DevString *argout;
long *error;
{
  long status;
  char mess[255];
  long data[5];
  static char str[80];

#ifdef PRINT
 printf("Daemon, DevStatus(), entered\n");
#endif /*PRINT*/

   sprintf(str,"the device is :%s\n",DEVSTATES[ds->devserver.state]);
   *argout = str;

#ifdef PRINT
 printf("Daemon, DevStatus(), exit\n");
#endif /*PRINT*/

  return (DS_OK);

}


/************************************************************************
 Function:      static long dev_getdeviceident()

 Description:	routine to read ddid for device 

 Arg(s) In:	Daemon ds - update daemon to work with
		DevString *argin - none

 Arg(s) Out:	DevLong *argout - returned ddid
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_getdeviceident (ds, argin, argout, error)
Daemon ds;
DevString *argin;
DevLong *argout;
long *error;
{
   long status;
   char mess[255];
   long data[5];
   int i,l;
   char device_name[60];


#if defined(EBUG)
   fprintf(stderr,"dev_getdeviceident()\n");
   fprintf(stderr,"dev_getdeviceident argin = %s\n",*argin);
#endif


   strcpy(device_name,*argin);
   l = strlen(device_name);
   for (i = 0;i < l;i++)
	device_name[i] = tolower(device_name[i]);
   

   status=get_ready_dp();
   strcpy(dipc->string,device_name);
   dipc->command=DevGetDeviceIdent;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_getdeviceident()\n");
#endif

     return(DS_NOTOK);
   }
  
  
  *argout=data[0];



#if defined(EBUG)
   fprintf(stderr,"end dev_getdeviceident()\n");
#endif


   return(DS_OK);
}


/************************************************************************
 Function:      static long dev_getdevicename()

 Description:	routine to read name for device given ddid

 Arg(s) In:	Daemon ds - update daemon to work with
		DevLong *argin - ddid

 Arg(s) Out:	DevString *argout - returned name
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_getdevicename (ds, argin, argout, error)
Daemon ds;
DevLong *argin;
DevString *argout;
long *error;
{
   long status;
   static char mess[255];
   long data[5];

#if defined(EBUG)
   fprintf(stderr,"dev_getdevicename ()\n");
   fprintf(stderr,"dev_getdevicename argin = %l\n",*argin);
#endif


   status=get_ready_dp();
   dipc->data[0]=*argin;
   dipc->command=DevGetDeviceName;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_getdeviceident()\n");
#endif
     
     return(DS_NOTOK);
   }

   *argout=mess;

#if defined(EBUG)
   fprintf(stderr,"end dev_getdeviceident()\n");
#endif

   return(DS_OK);
}



/************************************************************************
 Function:      static long dev_initialisecmd()

 Description:	routine to inform poller about new device to poll

 Arg(s) In:	Daemon ds - update daemon to work with
		DevDaemonStruct *argin - this structure contains a 
		device name (char *), a list of commands (DevVarStringArray)
		and a frequency (long) 

 Arg(s) Out:	DevLong *argout - the daemon data id
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_initialisecmd (ds, argin, argout, error)
Daemon ds;
struct DevDaemonStruct *argin;
DevLong *argout;
long *error;
{

  long status; 
  int i,l,j;
  long pl_tab_size = sizeof(pl_tab) / sizeof(db_resource);
  char device_name[60];
  unsigned int new_memory; 
  DevVarStringArray poll_list;   
  DevBoolean db_update;
  char **res4;

  /** test correct keyboarding **/ 

  if ((argin->cmd_list.length != 0) & (argin->poller_frequency != 0) & (argin->cmd_list.sequence != NULL) & (argin->dev_n != NULL))
  {

  db_update = '1';
  j = 0;
  res4 = (char **) devres; 

  /*************************************************************/
  /**  memory reallocation for stringarray DevVarStringArray  **/
  /**  pointed by pl_tab.resource_adr .                       **/
  /*************************************************************/

  strcpy(device_name,argin->dev_n);
  l = strlen(device_name);
  for (i = 0;i < l;i++)
     device_name[i] = toupper(device_name[i]);
   
  /** test if device has already been polled **/ 
  
  for (i=0 ; i<(stringarray.length) ; i++)
  {
    if (strcmp(stringarray.sequence[i],device_name) == 0) j = 1;
    if (j == 1) break;
  }
  
 if ( j == 0)  /** ok : device has not already been polled **/
 {
    stringarray.length += 1; 
    new_memory = sizeof(stringarray) + sizeof(stringarray.sequence) + 4; 
     
    for (i=0 ; i<(stringarray.length)-1 ; i++)
                new_memory += strlen(stringarray.sequence[i] + 1);
   
    new_memory += strlen(device_name) + 1;
  
    (stringarray.sequence) = (char **) realloc((stringarray.sequence),new_memory);
    stringarray.sequence[i] = (char *) malloc(strlen(device_name) + 1); 
    strcpy((stringarray.sequence[i]),device_name);
 
   /************************************************************/
   /**  ud_poll_list up to date with db_putresource .         **/
   /************************************************************/

    pl_tab[0].resource_adr = &stringarray ;   
  
    if (db_putresource((ds->devserver.name),pl_tab,pl_tab_size,error)!=0)
    {
#ifdef PRINT
     printf("db_putresource failed : %s %s \n",(ds->devserver.name),dev_error_str(error));
     fflush(stdout);
#endif
     *error = DbErr_BadResourceType;
     return(DS_NOTOK);
    }  
 
    status = dev_initialisedev (ds, argin, argout, db_update, error);
    if (status != DS_OK) 
    {
      status = dev_poll_list_uptodate(ds,argin->dev_n,error);
      if (status!=DS_OK)
      {
#ifdef PRINT
       printf("dev_poll_list_uptodate failed : %s %s \n",argin->dev_n,dev_error_str(error));
       fflush(stdout);
#endif
       return(DS_NOTOK);
      }
       if (db_delresource(argin->dev_n,res4,2,error) != 0)
       {
#ifdef PRINT
        printf("db_delresource failed : %s %s \n",device_name,dev_error_str(error));
        fflush(stdout);
#endif
        *error = DbErr_ResourceNotDefined;
        return(DS_NOTOK);
       }

#ifdef PRINT
     printf("ud_poll_list reseting \n");
     fflush(stdout);
#endif
     AccessDataport(dp);
      *error = dipc->error;
     ReleaseDataport(dp);
    
     return(DS_NOTOK);  /* in all these cases,the procedure failed */
     }
    else return(DS_OK); 
 }
 else /* j == 1 : device has already been polled */
 {
  *error = DcErr_BadParameters;
  return(DS_NOTOK);
 }
 } 
 else /* bad keyboarding */
 {
  *error = DcErr_BadParameters; 
  return(DS_NOTOK);
 }

}

static long dev_initialisedev (ds, argin, argout, db_update, error)
Daemon ds;
struct DevDaemonStruct *argin;
DevLong *argout;
DevBoolean db_update;
long *error;
{
   long status;
   char mess[255];
   long data[5];
   int i,l;
   char device_name[60];
   db_resource resource4[2];
   unsigned int num_resource;

#if defined(EBUG)
   fprintf(stderr,"dev_initialisedev()\n");
   fprintf(stderr,"dev_initialisedev argin = %s\n",argin->dev_n);
#endif

   
   strcpy(device_name,argin->dev_n);
   l = strlen(device_name);
   for (i = 0;i < l;i++)
	device_name[i] = tolower(device_name[i]); 
   
  
  /***********************************************************************/
  /**  Put resources in database before loading the device on poller .   **/
  /**  Resources are a list of command(s) and a new poller frequency    **/
  /**  associated with device_name .                                    **/
  /***********************************************************************/

 
  if (db_update != '0')
  {    
   num_resource = 2;
   resource4[0].resource_name = "ud_command_list";
   resource4[0].resource_type = D_VAR_STRINGARR;
   resource4[0].resource_adr = &(argin->cmd_list);    
   resource4[1].resource_name = "ud_poll_interval";
   resource4[1].resource_type = D_LONG_TYPE;
   resource4[1].resource_adr = &(argin->poller_frequency);   

  if (db_putresource((argin->dev_n),resource4,num_resource,error)!=0)
  {
#ifdef PRINT
   printf("db_putresource failed : %s %s \n",device_name,dev_error_str(error));
   fflush(stdout);
#endif
   *error = DbErr_BadResourceType;
   return(DS_NOTOK);
   }
   /*****************************/
   /**  End of db_putresource  **/
   /*****************************/
  } 

   status = get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevInitialiseDevice;
#if defined(EBUG)
   fprintf(stderr,"dev_initialisedev argin=%s\n",device_name);
#endif
   strcpy(dipc->string,device_name);
   dipc->status=D_NEW;
   status=let_go_dp();

#if defined(EBUG)
   fprintf(stderr,"dipc->string=%s\n",dipc->string);
#endif

   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {
#if defined(EBUG)
     fprintf(stderr,"endf dev_initialisedev()\n");
#endif
     
     return(DS_NOTOK);
   }

   AccessDataport(dp);
   *argout = dipc->data[0];
   ReleaseDataport(dp);


#if defined(EBUG)
  fprintf(stderr,"end dev_initialisedev()\n");
#endif

 return(DS_OK);

}



/************************************************************************
 Function:      static long dev_saveconfi()

 Description:	routine to update the daemon (and the poller ?) .res

 Arg(s) In:	Daemon ds - update daemon to work with
	 	void *argin - none 

 Arg(s) Out:	void *argout - none 
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_saveconfi(ds, argin, argout, error)
Daemon ds;
DevVoid *argin;
DevVoid *argout;
long *error;
{
   unsigned int i,j,l;
   long status;
   char n_svr[60],svr_n[60],path_f[80],allocstr[170],f_tmp[30];
   char *svr_name,*name_svr,*ns,*pathfile,*str,*tmp_f;
   FILE *fileptr;
   DevVarStringArray cmd_list;
   DevLong poll_int;
   long res3_size = sizeof(res3) / sizeof(db_resource);
   long resource1_size = sizeof(resource1) / sizeof(db_resource);
   long resource2_size = sizeof(resource2) / sizeof(db_resource);
#ifdef unix  
   pid_t pid;
#else
   int pid;
#endif

   /*********************************************************/
   /**   retrieve the resource file name                   **/
   /**   with the devserver_class.server_name parameter    **/   
   /*********************************************************/

   tmp_f = f_tmp;
   fileptr = NULL; 
   str = allocstr;
   pathfile = path_f;
   name_svr = n_svr;
   ns = name_svr;
   svr_name = svr_n;
   sprintf(svr_name,"%s",config_flags.server_name);

   i = 0;
   j = 0;
   l = strlen(svr_name);
     
   while (svr_name[i] != '/') i++; 
   if (svr_name[i] == '/') i++;

   for ( ; i<l ; i++ )
   {
     name_svr[j] = svr_name[i];
     j++;
   }
   name_svr[j] = '\0';

   sprintf(name_svr,"%s.res",ns);
  
#if defined OSK 
  str[0]=0x30;
  str[1]=0x00;
  sprintf(str,"del /h0/%s",name_svr);
  system(str);
#endif

  /*******************************/
  /**   template file opening   **/
  /*******************************/
  
   pid = getpid();

#if defined unix
   sprintf(tmp_f,"/tmp/f_tmpres_%d",pid);
#endif

#if defined OSK
   sprintf(tmp_f,"/h0/f_tmpres_%d",pid);
#endif
   
   fileptr =(FILE *) fopen(tmp_f,"w+");
   if (fileptr == NULL)
   {
    *error = DevErr_CannotOpenErrorFile;
    return(DS_NOTOK);
   } 

  /****************************************/
  /**   template file resources adding   **/
  /****************************************/

   fprintf(fileptr,"#\n#\n%s/device:\t%s\n#\n#",(svr_name),(ds->devserver.name));

   /********************/
   /**  ud_poll_list  **/
   /********************/

   for (i = 0 ; i < (stringarray.length) ; i++)
   {
    if (i == 0) 
    {  
       if (((stringarray.length) - 1 ) == 0)    
          { fprintf(fileptr,"\n%s/ud_poll_list:\t%s\n#\n#\n#",(ds->devserver.name),(stringarray.sequence[i])); } 
       else 
           { fprintf(fileptr,"\n%s/ud_poll_list:\t%s \\",(ds->devserver.name),(stringarray.sequence[i])); } 
    }
    else /* (i != 0) */
    {
       if (i == ((stringarray.length)-1 ))
          { fprintf(fileptr,"\n\t\t\t\t\t%s\n#\n#\n#",(stringarray.sequence[i])); } 
       else  
          { fprintf(fileptr,"\n\t\t\t\t\t%s \\",(stringarray.sequence[i])); }  
    }
   }

   /***********************/
   /**  ud_command_list  **/
   /***********************/
     
 
   for (i = 0 ; i < (stringarray.length) ; i++)
   {
     resource1[0].resource_adr = &cmd_list;

     status = db_getresource(stringarray.sequence[i],resource1,resource1_size,error);
     if (status != DS_OK)
     {
#ifdef PRINT
   printf("db_getresource failed : %s %s \n",(stringarray.sequence[i],dev_error_str(error)));
   fflush(stdout);
#endif
     *error = DbErr_ResourceNotDefined;
     return(DS_NOTOK);
     }

     for (j = 0 ; j < (cmd_list.length) ; j++)
     {
      if (j == 0)  
      {
        if (((cmd_list.length) - 1) == 0)  
        {
	 if ( i == ((stringarray.length)-1) ) 
	   { fprintf(fileptr,"\n%s/ud_command_list:\t%s\n#\n#\n#",(stringarray.sequence[i]),(cmd_list.sequence[j])); }
         else
	   { fprintf(fileptr,"\n%s/ud_command_list:\t%s",(stringarray.sequence[i]),(cmd_list.sequence[j])); }
	}
	else
           { fprintf(fileptr,"\n%s/ud_command_lit:\t%s \\",(stringarray.sequence[i]),(cmd_list.sequence[j])); } 
      }
      else /* (j != 0) */ 
      {
        if (j == ((cmd_list.length)-1))
	{
         if (( i == (stringarray.length)-1))
	    { fprintf(fileptr,"\n\t\t\t\t%s\n#\n#\n#",(cmd_list.sequence[j])); }
         else
	    { fprintf(fileptr,"\n\t\t\t\t%s",(cmd_list.sequence[j])); }
	}
	else  
           { fprintf(fileptr,"\n\t\t\t\t%s \\",(cmd_list.sequence[j])); } 
      }
     }
  }
 
   /************************/
   /**  ud_poll_interval  **/
   /************************/
 
   for (i = 0 ; i < (stringarray.length) ; i++)
   {
     resource2[0].resource_adr = &poll_int;

     status = db_getresource(stringarray.sequence[i],resource2,resource2_size,error);;
     if (status != DS_OK)
     {
#ifdef PRINT
   printf("db_getresource failed : %s %s \n",(stringarray.sequence[i],dev_error_str(error)));
   fflush(stdout);
#endif
     *error = DbErr_ResourceNotDefined;
     return(DS_NOTOK);
     }
    fprintf(fileptr,"\n%s/ud_poll_interval:\t\t%d",(stringarray.sequence[i]),poll_int);
  } 



  /*******************************/
  /**   resource file copy in   **/
  /*******************************/
  
  res3[0].resource_adr = &pathfile;
  status = db_getresource("SYS/DaemonClass/Default",res3,res3_size,error); 
  if (status != DS_OK)
  {
#ifdef PRINT
   printf("db_getresource failed :  %s \n",dev_error_str(error));
   fflush(stdout);
#endif
  *error = DbErr_ResourceNotDefined;
  return(DS_NOTOK);
  }

  /*************************************************/
  /** string concatenation for system instruction **/
  /*************************************************/

  fclose(fileptr); 
  
 
#if defined unix 
  sprintf(str,"cp %s %s/%s",tmp_f,pathfile,name_svr);
#endif

#if defined OSK
  sprintf(str,"copy %s /h0/%s",tmp_f,name_svr);
#endif

  if ( system(str) != 0 )
  {
    *error = DevErr_DeviceHardwareError;
    return(DS_NOTOK);
  }

  str[0]=0x30;
  str[1]=0x00;


#if defined unix
  remove(tmp_f); 
#endif

#if defined OSK
  sprintf(str,"del %s ",tmp_f);
  system(str);
#endif


  return(DS_OK);


}


/************************************************************************
 Function:      static long dev_removedevice()

 Description:	routine to update the daemon (and the poller ?) .res

 Arg(s) In:	Daemon ds - update daemon to work with
	 	DevLong *argin - the daemon data id 

 Arg(s) Out:	DevVoid *argout - none 
		long *error - pointer to error code, in case routine fails
*************************************************************************/
static long dev_removedevice(ds, argin, argout, error)
Daemon ds;
DevLong *argin;
DevVoid *argout;
long *error;
{
 long status;
 char mess[255];
 long data[5];
   
   
   /********************************************/
   /**  Retrieve the name of the device id    **/
   /**  for the db_putresource procedure      **/
   /********************************************/

  status=get_ready_dp(); /* performs AccessDataport itself */
  dipc->command=DevGetDeviceName;
  dipc->data[0]=*argin;
  dipc->status=D_NEW;
  status=let_go_dp();
  status=get_response_dp(mess,data,error);
  if (status == DS_OK)
  {
     
  
  /*******************************/
  /*** ud_poll_list up to date ***/
  /*******************************/
  
  status = dev_poll_list_uptodate(ds,mess,error);
  if (status != DS_OK)
  {
#ifdef PRINT
     printf("dev_poll_list_uptodate failed : %s %s \n",mess,dev_error_str(error));
     fflush(stdout);
#endif
     return(DS_NOTOK);
  } 

  /*************************/
  /**  poller work with   **/
  /*************************/
  
  status=get_ready_dp();  /* performs AccessDataport itself */
  dipc->command=DevRemoveDevice;
  strcpy(dipc->string,mess);
  dipc->status=D_NEW;
  status=let_go_dp();
  status=get_response_dp(mess,data,error);
  if (status != DS_OK)
  {
     return(DS_NOTOK);
  } 
  return(DS_OK);  
 }
 else
 {
  *error = DbErr_DeviceNotDefined;
  return(DS_NOTOK);
 }


}  

  
static long dev_poll_list_uptodate(ds,argin,error)
Daemon ds;
char *argin;
long *error;
{
  unsigned int i,j,l,new_memory,num_resource;
  long status;
  char mess[255],pourcen[2];
  long data[5];
  DevLong arg;
  long pl_tab_size = sizeof(pl_tab) / sizeof(db_resource);
  char device_name[60],n_svr[60],svr_n[60],allocstr[100],path_f[80];
  char *str,*ns,*name_svr,*svr_name,*pf,*pathfile;
  char **polres;
  DevVarStringArray poll_list;

#if defined(EBUG)
     fprintf(stderr,"in dev_poll_list_uptodate()\n");
#endif
  
  poll_list.length = 0;
  poll_list.sequence = NULL;
  str = allocstr; 

  /*******************************/
  /*** ud_poll_list up to date ***/
  /*******************************/

  strcpy(device_name,argin);
  l = strlen(device_name);
  for ( i=0 ; i<l ; i++)
      device_name[i] =toupper(device_name[i]);     

   status = db_getresource((ds->devserver.name),pl_tab,pl_tab_size,error); 
   if (status != DS_OK)
   {
#ifdef PRINT
     printf("db_getresource failed :  %s \n",dev_error_str(error));
     fflush(stdout);
#endif
     *error = DbErr_ResourceNotDefined;
     return(DS_NOTOK);
   }

   j = 0;
   (poll_list.sequence) = (char **) calloc((stringarray.length),sizeof(DevString)); 
  
   for ( i=0 ; i<(stringarray.length) ; i++ )
   {
     if (strcmp(stringarray.sequence[i],device_name) == 0)
     {
	if ( i != (stringarray.length)-1 )  /* in case of the last */
	{
	  i++;
          poll_list.sequence[j] = (char *) malloc(strlen(stringarray.sequence[i])+1);
	  poll_list.sequence[j] = stringarray.sequence[i];
	  poll_list.length += 1;
        }
     }
     else
     {
      poll_list.sequence[j] = (char *) malloc(strlen(stringarray.sequence[i])+1);
      strcpy(poll_list.sequence[j],stringarray.sequence[i]);
      poll_list.length +=1;
     }
     j++;
   }  
  
   if (poll_list.length != 0)
   {
     new_memory = sizeof(poll_list) + sizeof(poll_list.sequence);
     for ( i=0 ; i<(poll_list.length) ; i++ )
         new_memory += strlen(poll_list.sequence[i] + 1);
     (stringarray.sequence) = (char **) realloc((stringarray.sequence),new_memory);
     stringarray.length = 0;
  
     for ( i=0 ; i<(poll_list.length) ; i++ )
     {
       stringarray.sequence[i] = (char *) malloc(strlen(poll_list.sequence[i]) + 1);
       strcpy(stringarray.sequence[i],poll_list.sequence[i]);
       stringarray.length += 1;
     }  

     for (i=0;i<(poll_list.length);i++)
     {
      free((poll_list.sequence)[i]);
     }
     free(poll_list.sequence);
 
   /*****************************************************/
   /**  ud_poll_list up to date with db_putresource .  **/
   /*****************************************************/

    pl_tab[0].resource_adr = &stringarray ;   
  
    if (db_putresource((ds->devserver.name),pl_tab,pl_tab_size,error)!=0)
    {
#ifdef PRINT
     printf("db_putresource failed : %s %s \n",(ds->devserver.name),dev_error_str(error));
     fflush(stdout);
#endif
     *error = DbErr_BadResourceType;
     return(DS_NOTOK);
    }  
  }
  else /* Case of stringarray has one and only one device : just memory freeing */
  {
    
    for ( i=0 ; i<(stringarray.length) ; i++ )
    {
     free((stringarray.sequence)[i]);
    }

    stringarray.length = 0;
    
    pl_tab[0].resource_adr = &stringarray ;   
    
    polres = (char**) polres_name;
    num_resource = 1;
    if (db_delresource((ds->devserver.name),polres,num_resource,error) != 0)
    {
#ifdef PRINT
     printf("db_delresource failed : %s %s \n",device_name,dev_error_str(error));
     fflush(stdout);
#endif
     *error = DbErr_ResourceNotDefined;
     return(DS_NOTOK);
    }
  }


#if defined(EBUG)
     fprintf(stderr,"endf dev_poll_list_uptodate()\n");
#endif
  return(DS_OK);

}  



/************************************************************************
 Function:      static long dev_startpolling()

 Description:	routine to initiate polling for a device

 Arg(s) In:	Daemon ds - update daemon to work with
		DevLong *argin - the ddid of the device to start polling
                                 ddid = 0 is equivalent to dev_on()

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_startpolling (ds, argin, argout, error)
Daemon ds;
DevLong *argin;
DevVoid *argout;
long *error;
{
   long status;
   char mess[255];
   long data[5];

#if defined(EBUG)
  fprintf(stderr,"dev_startpolling()\n");
#endif

#if defined(TRACE)
   printf("dev_startpolling \n");
#endif

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevStartPolling;
   dipc->data[0]=*argin;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_startpolling()\n");
#endif

     return(DS_NOTOK);
   }

#if defined(EBUG)
  fprintf(stderr,"end dev_startpolling()\n");
#endif

#if defined(TRACE)
   printf("end dev_startpolling \n");
#endif

   return(DS_OK);
}



/************************************************************************
 Function:      static long dev_stoppolling()

 Description:	routine to halt polling for a device

 Arg(s) In:	Daemon ds - update daemon to work with
		DevLong *argin - the ddid of the device to stop polling
                                 ddid = 0 is equivalent to dev_off()

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_stoppolling (ds, argin, argout, error)
Daemon ds;
DevLong *argin;
DevVoid *argout;
long *error;
{
   long status;
   char mess[255];
   long data[5];

#if defined(EBUG)
  fprintf(stderr,"dev_stoppolling()\n");
#endif

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevStopPolling;
   dipc->data[0]=*argin;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_stoppolling()\n");
#endif

     return(DS_NOTOK);
   }

#if defined(EBUG)
  fprintf(stderr,"end dev_stoppolling()\n");
#endif

   return(DS_OK);
}



/************************************************************************
 Function:      static long dev_pstatus()

 Description:	routine to retrieve poll status for a device

 Arg(s) In:	Daemon ds - update daemon to work with
		DevLong *argin - the ddid of the device to stop polling

 Arg(s) Out:	DevDaemonStatus *argout - the process status
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_pstatus(ds, argin, argout, error)
Daemon ds;
DevLong *argin;
DevDaemonStatus *argout;
long *error;
{
   long status;
   static char mess[255];
   long data[5];

#if defined(EBUG)
  fprintf(stderr,"dev_pstatus()\n");
#endif

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevPollStatus;
   dipc->data[0]=*argin;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_pstatus()\n");
#endif

     return(DS_NOTOK);
   }

   argout->BeingPolled       = (char)(data[0]);
   argout->PollFrequency     = (long)(data[1]);
   argout->TimeLastPolled    = (long)(data[2]);
   argout->PollMode          = (char)(data[3]);
   argout->DeviceAccessError = (char)(data[4]);

#if defined(EBUG)
  fprintf(stderr,"end dev_pstatus()\n");
#endif

   return(DS_OK);
}



/************************************************************************
 Function:      static long dev_longstatus()

 Description:	routine to retrieve daemon server status

 Arg(s) In:	Daemon ds - update daemon to work with

 Arg(s) Out:	DevString *argout - the daemon server status
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_longstatus (ds,argin,argout,error)
Daemon ds;
DevVoid *argin;
DevString *argout;
long *error;
{
   long status;
   static char mess[255];
   long data[5];

#if defined(EBUG)
  fprintf(stderr,"dev_longstatus()\n");
#endif

   status = get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevStatus;
   dipc->status=D_NEW;
   status = let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {
#if defined(EBUG)
     fprintf(stderr,"endf dev_longstatus()\n");
#endif
     return (DS_NOTOK);
   }

   *argout = mess; 

#if defined(EBUG)
  fprintf(stderr,"end dev_longstatus()\n");
#endif

   return(DS_OK);
}



/************************************************************************
 Function:      static long dev_dastatus()

 Description:	routine to retrieve device access status for a device

 Arg(s) In:	Daemon ds - update daemon to work with
		DevLong *argin - the ddid of the device to get status

 Arg(s) Out:	DevDaemonStatus *argout - the device access status
		long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_dastatus(ds, argin, argout, error)
Daemon ds;
DevLong *argin;
DevDaemonStatus *argout;
long *error;
{
   long status;
   static char mess[255];
   long data[5];

#if defined(EBUG)
  fprintf(stderr,"dev_dastatus()\n");
#endif

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevAccessStatus;
   dipc->data[0]=*argin;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_dastatus()\n");
#endif

     return(DS_NOTOK);
   }
   argout->LastCommandStatus  = dipc->data[0];
   argout->ErrorCode          = dipc->data[1];
   argout->ContinueAfterError = (char)(dipc->data[2]);

#if defined(EBUG)
  fprintf(stderr,"end dev_dastatus()\n");
#endif

   return(DS_OK);
}


/************************************************************************
 Function:      static long dev_changeinterval()

 Description:	function to change the poll interval for a device

 Arg(s) In:	Daemon ds - update daemon to work with
		DevDaemonData *argin - the ddid and its new poll interval

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_changeinterval(ds, argin, argout, error)
Daemon ds;
DevDaemonData *argin;
DevVoid *argout;
long *error;
{
   long status;
   static char mess[255];
   long data[5];
   unsigned int num_resource;
   db_resource resource4;


#if defined(EBUG)
  fprintf(stderr,"dev_changeinterval()\n");
#endif
   
   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevChangeInterval;
   dipc->data[0]=argin->ddid;
   dipc->data[1]=argin->long_data;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {
#if defined(EBUG)
     fprintf(stderr,"endf dev_changeinterval()\n");
#endif
     return(DS_NOTOK);
   }

   /********************************************/
   /**  Retrieve the name of the device id    **/
   /**  for the db_putresource procedure      **/
   /********************************************/
  

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevGetDeviceName;
   dipc->data[0]=argin->ddid;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status == DS_OK)
   {
     num_resource = 1;
     resource4.resource_name = "ud_poll_interval";
     resource4.resource_type = D_LONG_TYPE;
     resource4.resource_adr = &(argin->long_data);

     if (db_putresource(mess,&resource4,num_resource,error)!=0)
     {
#ifdef PRINT
   /*  printf("db_putresource failed : %s %s \n",devname,dev_error_str(error));
     fflush(stdout); */
#endif
     }
   }
   else
   {
      *error = DbErr_BadResourceType;
      return(DS_NOTOK);
   }

#if defined(EBUG)
  fprintf(stderr,"end dev_changeinterval()\n");
#endif

  return(DS_OK);

}




/************************************************************************
 Function:      static long dev_definemode()

 Description:	function to change the poll mode for a device

 Arg(s) In:	Daemon ds - update daemon to work with
		DevDaemonData *argin - the ddid and its new poll mode 

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
*************************************************************************/

static long dev_definemode(ds, argin, argout, error)
Daemon ds;
DevDaemonData *argin;
DevVoid *argout;
long *error;
{
   long status;
   long data[5];
   static char mess[255];

#if defined(EBUG)
  fprintf(stderr,"dev_definemode()\n");
#endif

   status=get_ready_dp(); /* performs AccessDataport itself */
   dipc->command=DevDefineMode;
   dipc->data[0]=argin->ddid;
   dipc->data[1]=argin->long_data;
   dipc->status=D_NEW;
   status=let_go_dp();
   status=get_response_dp(mess,data,error);
   if (status!=DS_OK)
   {

#if defined(EBUG)
     fprintf(stderr,"endf dev_definemode()\n");
#endif

     return(DS_NOTOK);
   }

#if defined(EBUG)
  fprintf(stderr,"end dev_definemode()\n");
#endif

   return(DS_OK);
}
