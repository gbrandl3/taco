/*+*******************************************************************

 File       	: DevServerMain.c

 Project    	: Device Servers with sun-rpc

 Description	: Main programm for all device servers

 Author(s)  	: Jens Meyer
 		  $Author: jkrueger1 $

 Original   	: March 1991

 Version	: $Revision: 1.2 $

 Date		: $Date: 2003-05-02 09:12:48 $

 Copyright (c) 1990-2002 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/

#include <API.h>
#include <private/ApiP.h>
#include <Admin.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <DevErrors.h>
#if defined _NT
#include <rpc/Pmap_pro.h>
#include <rpc/pmap_cln.h>
#include <process.h>
/* #define NODATETIMEPICK to avoid compiler error. (I hate MicroSoft!)*/
#define NODATETIMEPICK
#include <commctrl.h>
#undef NODATETIMEPICK
#else
#include <rpc/pmap_clnt.h>
#endif	/* _NT */
#ifdef _UCC
#include <errno.h>
#include <rpc/rpc.h>
#endif /* _UCC */
#ifdef vxworks
#include <taskLib.h>
#endif /* vxworks */
#include <stdlib.h>
/*
 * M. Diehl, 22.7.1999
 * includes added to make the gethostbyname-patch working, see later
 * not tested on these platforms (in fact, only on Linux)
 */

#if ( OSK | _OSK )
#include <inet/netdb.h>
#else
#if !defined vxworks
#if !defined WIN32
#include <netdb.h>
#else
#include <rpc/netdb.h>
#endif /* !WIN32 */
#endif /* !vxworks */
#endif /* OSK | _OSK */

/* end of added includes */

/*
 * For compatibility with version 3 clients.
 * In external module ds_prog_vers3.c
 */

void _WINAPI devserver_prog_1	PT_( (struct svc_req *rqstp,SVCXPRT *transp) );
long minimal_access      = WRITE_ACCESS;


static void _WINAPI devserver_prog_4	PT_( (struct svc_req *rqstp,SVCXPRT *transp) );
static long svc_check 		PT_( (long *error) );


/****************************************
 *          Globals                     *
 ****************************************/

/*
 *  Type for global state flags for the current
 *  server or client status is defined in API.h 
 */

/*
 *  Configuration flags
 */

extern configuration_flags      config_flags;

/*
 *  Communication sockets
 *  definition moved to dev_api.c
 */
 
static int	udp_socket;
static int	tcp_socket;

extern long synch_svc_udp_sock;
extern long synch_svc_tcp_sock;

/*
 * debug flag
 */

extern long debug_flag;

static SVCXPRT *transp;
static SVCXPRT *transp_tcp;
/*
 * VXWORKS and NOMAIN routine to create a device server - device_server()
 */

#if defined (vxworks) || (NOMAIN)
void device_server (char *server_name, char *pers_name, int nodb, int pn, int n_device, char** device_list)
{

        char    host_name [19];
        char    dsn_name [37];
        char    *proc_name;
        char    *display;

        char                    res_path [80];
        char                    res_name[80];
        DevVarStringArray       default_access;
        db_resource             res_tab;

        long    prog_number=0;
        long    status;
        long    error = 0;
        int     pid = 0;
        short   m_opt = False;
        short   s_opt = True;
        short   nodb_opt = False;
        short   sig;
        short   i;

/*
 * import database server  
 * BP: moved this to the front, as the new db_import_multi usage sets up
 * config_flags, so that server_name gets overwritten.
 */
	if ( db_import (&error) < 0 )
	{
	    dev_printerror_no (SEND,"db_import failed",error);
	    exit(-1);
	}		
/*
 *  read device server's class name and personal name
 *  check for lenght of names : server process name <= 23 char
 *                              personal name       <= 11 char
 */
        proc_name = server_name;

        if ( strlen(proc_name) > 23 )
        {
        	printf ( "Filename too long : server_name <= 23 char\n");
           	exit (-1);
        }

        if ( strlen(pers_name) > 11 )
        {
		printf ( "Personal DS_name too long : personal_dsname <= 11 char\n");
		exit (-1);
        }

	memset  (dsn_name,0,sizeof(dsn_name));
	strncat (dsn_name , proc_name, 23);
	strncat (dsn_name , "/", 1);
	strncat (dsn_name , pers_name, 11);
/*
 * option nodb means run device server without database
 */
	if (nodb > 0)
	{
		nodb_opt = True;
		config_flags.no_database = True;
		xdr_load_kernel(&error);
	}
/*
 * option pn specifies program number (to be used in conjunction with nodb)
 */
	prog_number = pn;
/*
 * unregister this program number from the portmapper - this is potentially
 * dangerous because it can unregister another running server. we assume
 * the -pn option is used by those who know what they are doing ...
 */
	if (prog_number != 0)
	{
		pmap_unset (prog_number, API_VERSION);
		pmap_unset (prog_number, ASYNCH_API_VERSION);
	}
/*
 * option -device means remaining command line arguments are device names
 */
	config_flags.device_no = n_device;
	config_flags.device_list = device_list;

#ifdef vxworks
/*
 * call rpcTaskInit() to initialise task-specific data structures 
 * required by RPC (cf. VxWorks Reference manual pg. 1-203). 
 * Failure to do so will result in the task crashing the first
 * time a call to is made to an RPC function
 */
	rpcTaskInit();
#endif /* vxworks */

#else /* vxworks || NOMAIN */

/**/
#ifdef _NT  /* _NT */

#include "resource.h"

#define TITLE_STR 	"TACO-Device Server: "
#define	MB_ERR		(MB_OK | MB_ICONEXCLAMATION)
#define	MB_INFO		(MB_OK | MB_ICONINFORMATION)
/* Makes it easier to determine appropriate code paths: */
#if defined (WIN32)
#define IS_WIN32 TRUE
#else
#define IS_WIN32 FALSE
#endif
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(IS_NT) && (LOBYTE(LOWORD(GetVersion()))<4))
#define IS_WIN95 (BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

	/***** Globals for Windows *****/

HINSTANCE ghAppInstance;  /* the application's module handle */
HWND ghWndMain;           /* the main window handle */
char* gszAppName;         /* the application's name */
MainWndTextDisplay gMWndTxtDisplay;  /* the main window backdrop text */
long (*DelayedStartup)() = NULL;    /* hook for delayed startup */
void (*OnShutDown)() =NULL;         /* hook for process shutdown */
	/* local globals for Windows */
static HANDLE ghWorkerThread;    /* worker's thread handle */
static DWORD gdwWorkerThreadId;  /* worker's thread ID */
static char **gArgv;   /* emulates argv for Windows */
static int gArgc=0;    /* emulates argc for Windows */
static struct {
	int width, height; 
	HDC hMemDC;} gMainWndBackdrop;  /* values needed for WM_PAINT */
static HWND hConsole;               /* the console window handle */


/* Prototypes of functions included in this code module: */

static ATOM  /* Window class registratiuon */
MyRegisterClass(CONST WNDCLASS*);

static BOOL  /* Init Application */
InitApplication(HINSTANCE);

static BOOL  /* Init current Instance */
InitInstance(HINSTANCE, int);

static LRESULT CALLBACK  /* Main window-procedure */
WndProc(HWND, UINT, WPARAM, LPARAM);

static LRESULT CALLBACK  /* About window-procedure */
About(HWND, UINT, WPARAM, LPARAM);

static BOOL 
CenterWindow (HWND, HWND);

static DWORD /* Worker thread entry-point */
WorkerThreadMain(LPDWORD lpdwParam);

static LRESULT CALLBACK /* Handler for WM_COMMAND events */
WM_COMMAND_handler( HWND hWnd, WPARAM wParam, LPARAM lParam);

static BOOL /* RPC and TACO initislization (Device Server's main) */
application_main (int argc, char **argv)  /* Windows does not use main()! */

#else /* _NT */
int main (int argc, char **argv)
/*+**********************************************************************
 Function   :   main()

 Description:   Main routine for all device servers.
	    :   Checks wether a device server with the
		same name is already running. 
	    :	Opens a connection to the static database and
		to a message service.
            :   Registers the server on a transient program
		number to the portmapper.

 Arg(s) In  :   char *argv[1] - personal name for the device server.

 Return(s)  :   exit(1) or exit(-1)
***********************************************************************-*/
#endif /* _NT */
{
/*        SVCXPRT *transp; */
/*        SVCXPRT *transp_tcp; */
        char    host_name [19];
        char    dsn_name [37];
        char    *proc_name;
        char    *display;

        char                    res_path [80];
        char                    res_name[80];
        DevVarStringArray       default_access;
        db_resource             res_tab;

        long    prog_number=0;
        long    status;
        long    error = 0;
        int     pid = 0;
        short   m_opt = False;
        short   s_opt = True;
        short   nodb_opt = False;
        short   sig;
        short   i,j;

	/*
	 *  read device server's class name and personal name
	 *  check for lenght of names : server process name <= 23 char
	 *                              personal name       <= 11 char
	 */
#ifndef _NT
	if (argc < 2)
	{
/*	   printf ("usage :  %s personal_name {-m} {-s} {-nodb} {-pn program #} {-device dev1 ...}\n",argv[0]);*/
	   printf ("usage :  %s personal_name {-nodb} {-pn program #} {-device device1 ...}\n",argv[0]);
	   exit (1);
	}
#else
	if(0 !=(proc_name = (char *)strrchr (argv[0],'\\'))) {
		if(0 == strncmp(proc_name+strlen(proc_name)-4,".exe",4))
    		proc_name[strlen(proc_name)-4]='\0';
	}
#endif  /* !_NT */

/*
 * make sure all config flags are set to zero before starting
 */
	memset(&config_flags, 0, sizeof(config_flags));
#if defined (unix)
  	proc_name = (char *)strrchr (argv[0], '/');
#endif	/*unix*/
#if ( OSK || _OSK )
#ifdef __cplusplus
	proc_name = (char *)strrchr (argv[0], '/');
#else
	proc_name = (char *)rindex (argv[0], '/');
#endif
#endif /* OSK || _OSK */

	if (proc_name == NULL)
      		proc_name = argv[0];
	else  
           	proc_name++;

	if ( strlen(proc_name) > 23 )
	{
		char msg[]="Filename to long : server_name <= 23 char\n";
#ifdef _NT
		MessageBox((HWND)NULL, msg, TITLE_STR, MB_INFO);
		return(FALSE);
#else
		printf (msg);
		exit (-1);
#endif
	}

	if ( strlen(argv[1]) > 11 )
	{
		char msg[]= "Personal DS_name to long : personal_dsname <= 11 char\n";
#ifdef _NT
		MessageBox((HWND)NULL, msg, TITLE_STR, MB_INFO);
		return(FALSE);
#else
		printf (msg); 
		exit (-1);
#endif
	}

/*
 *  read options for device server start
 */
	if (argc > 2)
	{
		for (i=2; i<(argc); i++)
		{
			if (strcmp (argv[i],"-m") == 0)
				m_opt = True;
			if (strcmp (argv[i],"-s") == 0)
				s_opt = True;
/*
 * option -nodb means run device server without database
 */
			if (strcmp (argv[i],"-nodb") == 0)
			{
				nodb_opt = True;
				config_flags.no_database = True;
				xdr_load_kernel(&error);
			}
/*
 * option -pn specifies program number (to be used in conjunction with -nodb)
 */
			if (strcmp (argv[i],"-pn") == 0)
			{
				sscanf(argv[i+1],"%d",&prog_number);
/*
 * unregister this program number from the portmapper - this is potentially
 * dangerous because it can unregister another running server. we assume
 * the -pn option is used by those who know what they are doing ...
 */
				if (prog_number != 0)
				{
					pmap_unset (prog_number, API_VERSION);
					pmap_unset (prog_number, ASYNCH_API_VERSION);
				}
				i++;
			}
/*
 * option -device means remaining command line arguments are device names
 */
			if (strcmp (argv[i],"-device") == 0)
			{
				config_flags.device_no = argc-i-1;
				config_flags.device_list = (char**)malloc(config_flags.device_no*sizeof(char));
				for (j=i+1; j<argc; j++)
				{
					config_flags.device_list[j-i-1] = (char*)malloc(strlen(argv[j])+1);
					sprintf(config_flags.device_list[j-i-1],"%s",argv[j]);
				}
				i = j;
			}
		}
	}
	memset  (dsn_name,0,sizeof(dsn_name));
	strncat (dsn_name , proc_name, 23);
	strncat (dsn_name , "/", 1);
	strncat (dsn_name , argv[1], 11);

#endif /* vxworks || NOMAIN */
/*
 *  get process ID, host_name 
 *  and create device server network name
 */

#if defined (_NT)
        pid = _getpid ();
#else
#if !defined (vxworks)
        pid = getpid ();
#else  /* !vxworks */
        pid = taskIdSelf ();
#endif /* !vxworks */
#endif
/*
 * M. Diehl, 22.7.1999
 * We have to take care here, since hostname might be set to the FQDN thus
 * gethostname() returns "host.and.complete.domain" which may easily exceed
 * the 19 characters reserved for it!
 * There are 3 possible solutions:
 * 1) General extension of sizeof(host_name) reasonably beyond 19 characters
 * 2) Extracting the hostname from the FQDN
 * 3) Switching to an IP-String
 *
 * Here are some ideas on that:
 *
 * 1) Is obviously the best way, especially as there are several limitations
 *    to SHORT_NAME_SIZE=32 in quite a number of files. However this implies
 *    some major issues with respect to DBM and platform independence.
 * 2) Means switching to the intended behaviour and is rather easy to
 *    achieve. However this will fail, if the host (on which the device
 *    server is running e.g.) is not in our search domain - which is
 *    a disadvantage of the current implementation anyway, I believe.
 * 3) Replacing the hostname by it's IP-String-Quad (if it won't fit into
 *    19 characters) is possible without any changes at DBM code and
 *    should work in all situations. The disadvantage however is,
 *    that db_tools show up with xxx.xxx.xxx.xxx values instead of
 *    well-known hostnames, so one might have to use nslookup.
 *
 * What follows realizes suggestion 3) using some code from sec_api.c
 * with respect to different IP-retrieving for VxWorks and the rest of the
 * world. However, one has to keep in mind, that a bunch of other stuff
 * will fail, if FQDN exceeds SHORT_NAME_SIZE=32 - which is not too hard!
 */
 
	if( gethostname(host_name, 19) != 0 )
	{
		char            hostname[200];     /* hopefully enough! */
		char		ip_str[SHORT_NAME_SIZE];
#if !defined(vxworks)
		struct hostent	*host_info;
#else  /* !vxworks */
		union 		{ int    int_addr;
	  			  u_char char_addr[4];}  host_addr;
#endif /* !vxworks */

		
		if( gethostname(hostname,sizeof(hostname)) != 0 )
		{
			printf("unable to retrieve hostname!\n");
			exit(-1);
		}
/*#ifdef EBUG		
		printf("long hostname found - FQDN = %s\n", hostname);
#endif*/

#if !defined(vxworks)
		if ( (host_info = gethostbyname(hostname)) == NULL )
		{
			printf("unable to get IP for host %s\n",
				hostname);
			exit(-1);
		}

		sprintf (ip_str,"%d.%d.%d.%d",
		    (u_char) host_info->h_addr[0],
		    (u_char) host_info->h_addr[1],
		    (u_char) host_info->h_addr[2],
		    (u_char) host_info->h_addr[3]);
#else  /* !vxworks */
		host_addr.int_addr = hostGetByName(hostname);
		sprintf (ip_str,"%d.%d.%d.%d",
		    (u_char) host_addr.char_addr[0],
		    (u_char) host_addr.char_addr[1],
		    (u_char) host_addr.char_addr[2],
		    (u_char) host_addr.char_addr[3]);
#endif /* !vxworks */

/*#ifdef EBUG
		printf("using IP-addr-quad %s\n", ip_str);
#endif*/
		strncpy(host_name,ip_str,19);
	}
/*#ifdef EBUG
	printf( "using hostname: %s\n", host_name);
#endif*/

	TOLOWER(dsn_name);
	TOLOWER(host_name);
	sprintf (config_flags.server_name,"%s", dsn_name); 
	sprintf (config_flags.server_host,"%s", host_name); 

	/*
	 * install signal handling for HPUX, SUN, OS9
	 */

	(void) signal(SIGINT,  main_signal_handler);
	(void) signal(SIGTERM, main_signal_handler);
        (void) signal(SIGABRT, main_signal_handler);

#if defined (unix)
	(void) signal(SIGQUIT, main_signal_handler);
/*
 * SIGHUP and SIGPIPE are now caught in the main signal handler
 * which will simply return. This is needed for asynchronous
 * clients and servers to detect a server/client going down.
 *
 * andy 8may97
 */
	(void) signal(SIGHUP,  main_signal_handler);
	(void) signal(SIGPIPE, main_signal_handler);
#endif /* unix */

#if defined (_NT)
        (void) signal(SIGBREAK,main_signal_handler);
#endif /* _NT */

#if ( OSK || _OSK )
	(void) signal(SIGQUIT, main_signal_handler);
#endif /* OSK || _OSK */


	if (nodb_opt == False)
	{
/*
 *  if database required then import database server  
 */
		if ( db_import (&error) < 0 )
	   	{
	   		dev_printerror_no (SEND,"db_import failed",error);
	   		exit(-1);
	   	}		
/*
 *  check wether an old server with the same name
 *  is mapped to portmap or still running
 */
		if ( svc_check(&error) < 0 )
	   	{
	   		dev_printerror_no (SEND,"svc_check()",error);
	   		exit(-1);
	   	}		
/*
 * If the security system is switched on, read the minimal
 * access right for version 3 clients from the 
 * security database.
 */
       		if ( config_flags.security == True )
	   	{
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
	      			exit (-1);
	      		}
/*
 * Transform the string array into an access right value.
 */
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
		                 	minimal_access = NO_ACCESS;
	      		}	 
	   		else 
		              	minimal_access = NO_ACCESS;

	   		free_var_str_array (&default_access);
	   	}
	}		
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
#if defined(_NT)
		MessageBox((HWND)NULL, msg, TITLE_STR, MB_ERR);
            	/*raise(SIGABRT);*/
		return (FALSE);
#else
		printf (msg);
            	kill (pid,SIGQUIT);
#endif
	}
/*
 *  make 3 tries to get transient progam number
 */
	synch_svc_udp_sock = -1;
	for (i=0; i<3; i++)
	{
		if (prog_number == 0)
			prog_number = gettransient(dsn_name);
		if( prog_number == 0 )
		{
	  		dev_printerror_no(SEND,"gettransient: no free programm nnumber\n",error);
	  		exit(-1);
		}
/*
 * Write the device server identification to the global
 * configuration structure.
 */
		config_flags.prog_number = prog_number;
		/*config_flags.vers_number = DEVSERVER_VERS;*/
		config_flags.vers_number = API_VERSION;

		if (!svc_register(transp, prog_number, API_VERSION, 
			  	devserver_prog_4, IPPROTO_UDP)) 
		{
			char msg[]="Unable to register server (UDP,4), retry...\n"; 
#if defined(_NT)
			MessageBox((HWND)NULL, msg, TITLE_STR, MB_ERR);
			return(FALSE);
#else
			printf (msg); 
#endif
		}
		else
		{
/*
 * keep the socket, we need it later for dev_synch();
 */
			udp_socket = transp->xp_sock;
			synch_svc_udp_sock = transp->xp_sock;
			break;
		}
	}
	if (synch_svc_udp_sock == -1)
	{
		char msg[]="Unable to register server (UDP,4), exiting...\n"; 
#if defined(_NT)
		MessageBox((HWND)NULL, msg, TITLE_STR, MB_ERR);
		return(FALSE);
#else
		printf (msg); 
		kill(pid, SIGQUIT);
#endif
	}

/*
 *  register tcp port
 */
	transp_tcp = svctcp_create(tcp_socket,0,0);
	if (transp_tcp == NULL) 
	{
		char msg[]= "Cannot create tcp service, exiting...\n";
#if defined(_NT)
		MessageBox((HWND)NULL, msg, TITLE_STR, MB_ERR);
            	/*raise(SIGABRT);*/
		return (FALSE);
#else
		printf (msg); 
		kill (pid,SIGQUIT);
#endif
	}

        if (!svc_register(transp_tcp, prog_number, API_VERSION,
			  devserver_prog_4, IPPROTO_TCP))
	{
		char msg[]= "Unable to register server (TCP,4), exiting...\n";
#if defined(_NT)
		MessageBox((HWND)NULL, msg, TITLE_STR, MB_ERR);
		raise(SIGABRT);
		return (FALSE);
#else
		printf (msg); 
		kill (pid,SIGQUIT);
#endif
	}

/*
 * keep the socket, we need it later
 */
	tcp_socket = transp_tcp->xp_sock;
	synch_svc_tcp_sock = transp_tcp->xp_sock;
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
	status = asynch_rpc_register(&error);
	if (status != DS_OK)
	{
		dev_printerror_no (SEND,"failed to register asynchronus rpc",error);
/* 
 * DO NOT abort server, continue (without asynchronous server) ...
 *
#if defined (_NT)
		raise(SIGABRT);
#else
		kill (pid,SIGQUIT);
#endif
 */
/* startup message service */
		if (m_opt ==True)
	    	{	
			display=getenv("DISPLAY");
			if(msg_import(dsn_name,host_name,prog_number,display,&error)!=DS_OK)
		    	{
			/* we dont care */
				printf("can not import message service\n");
		    	}	
	    	}
	}

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
		status = startup(config_flags.server_name,&error);
		if ( status != DS_OK )
		{
			dev_printerror_no (SEND,"startup failed",error);
#if defined(_NT)
			raise(SIGABRT);
#else
			kill (pid,SIGQUIT);
#endif
		}		

/*
 *  if ds__svcrun() is used, the server can return from
 *  the startup function with status=1 to avoid svc_run()
 *  and to do a proper exit.
 */
		if ( status == 1 )
		{
#if defined(_NT)
			raise(SIGABRT);
#else
			kill (pid,SIGQUIT);
#endif
		}
		config_flags.startup = True;
	}

#ifndef _NT
/*
 *  set server into wait status
 */
	svc_run();
	{
		char msg[]= "svc_run returned\n";
		printf (msg); 
		kill (pid,SIGQUIT);
	}
#else   /* _NT */
/*
 * show up the main dialog
 */
	return TRUE;
#endif
}

#ifdef _NT
/***********************************************************************
//
//  FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)
//
//  PURPOSE: Entry point for the application.
//
//  COMMENTS:
//
//	This function initializes the application and processes the
//	message loop.
//
 ***********************************************************************/
int APIENTRY WinMain(
					 HINSTANCE hInstance, 
					 HINSTANCE hPrevInstance, 
					 LPSTR lpCmdLine, 
					 int nCmdShow)
{
	MSG win_msg;
	HANDLE hAccelTable;

	/*
	 * parse NT's command-line into gArgv and gArgc
	 */
	{   /* parse block */
	char *token[64];
	char* command_line= GetCommandLine();
	int cnt=0;
	char fname[_MAX_FNAME];
	char* path;

	/* program name */
	path= strtok(command_line, " ");
	_splitpath( path, NULL, NULL, fname, NULL );
	gszAppName= malloc (strlen(fname)+1);
	strcpy(gszAppName, fname);
	token[cnt]= gszAppName;
		
	/* further arguments ? */
	if('\0' != *lpCmdLine) {  
		command_line= (char*) malloc (strlen(lpCmdLine)+1);
		strcpy(command_line, lpCmdLine);
		/* first argument */
		token[++cnt]= strtok(command_line, " ");
		do {
		/* next arguments */
		token[++cnt]= strtok(NULL, " ");
		} while(NULL != token[cnt]); 
	} else {
		cnt=1;
		}
	gArgc= cnt;

	if(gArgc < 2) {
		char msg[1024];
/*		sprintf (msg,"usage :  %s personal_name [-m] [-s]\n",gszAppName);*/
		sprintf (msg,"usage :  %s personal_name [-nodb] [-pn number] [-device device1 ...]\n",gszAppName);
		MessageBox((HWND)NULL, msg, TITLE_STR, MB_INFO);
		return(FALSE);
	}

	gArgv= (char**) malloc (sizeof(char**)*gArgc);
	while(cnt>0) {
		cnt--;
		gArgv[cnt]=token[cnt];
		}
	}   /* end of parse block */

	if (!hPrevInstance) {
		// Perform instance initialization:
		if (!InitApplication(hInstance)) {
			return (FALSE);
		}
	}

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return (FALSE);
	}

	hAccelTable = LoadAccelerators (hInstance, "DS_MAIN_ACC");

	// Main message loop:
	while (GetMessage(&win_msg, NULL, 0, 0)) {
		if (!TranslateAccelerator (win_msg.hwnd, hAccelTable, &win_msg)) {
			TranslateMessage(&win_msg);
			DispatchMessage(&win_msg);
		}
	}
	/* return (win_msg.wParam); old fashioned */
	ExitProcess(0);
}

//
//  FUNCTION: InitApplication(HANDLE)
//
//  PURPOSE: Initializes window data and registers window class 
//
//  COMMENTS:
//
//       In this function, we initialize a window class by filling out a data
//       structure of type WNDCLASS and calling either RegisterClass or 
//       the internal MyRegisterClass.
//
static BOOL 
InitApplication(HINSTANCE hInstance)
{
    WNDCLASS  wc;
    HWND      hwnd;

    // Win32 will always set hPrevInstance to NULL, so lets check
    // things a little closer. This is because we only want a single
    // version of this app to run at a time
    hwnd = FindWindow (gszAppName, NULL);
    if (hwnd) {
        // We found another version of ourself. Lets defer to it:
        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }
        SetForegroundWindow (hwnd);

        // If this app actually had any functionality, we would
        // also want to communicate any action that our 'twin'
        // should now perform based on how the user tried to
        // execute us.
        return FALSE;
        }

        // Fill in window class structure with parameters that describe
        // the main window.
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = (WNDPROC)WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInstance;
        wc.hIcon         = LoadIcon (hInstance, "DS_LARGE");
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

        // Since Windows95 has a slightly different recommended
        // format for the 'Help' menu, lets put this in the alternate menu like this:
        if (IS_WIN95) {
			wc.lpszMenuName  = "DS_MAIN_W95";
        } else {
			wc.lpszMenuName  = "DS_MAIN_NT";
        }
        wc.lpszClassName = gszAppName;

        // Register the window class and return success/failure code.
        if (IS_WIN95) {
	return MyRegisterClass(&wc);
        } else {
	return RegisterClass(&wc);
        }
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle, creates main window and starts worker thread
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable,
//        create and display the main program window, and start a worker
//        thread to receive ONC RPC calls.
//
static BOOL 
InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HCURSOR oldcr;
	HBITMAP hBmp, hBmpOld;
	BITMAP bm;    
	HDC hDC;
	HDC hMemDC; // virtual main window painting area
	RECT wnd, client, calc;
	TEXTMETRIC tm;
	int loop, xorg, yorg;
	HWND hWnd;
	char title[256];

	strcpy(title, TITLE_STR);
	strcat(title, gszAppName);
	
	ghAppInstance = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(gszAppName, 
		title, 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		return (FALSE);
	}
	ghWndMain= hWnd; // Store main window handle in our global variable

	InitCommonControls();  // init Common Controls library //

	// construct a Debug Console
	hConsole= ConstructDbgCon(hInstance, hWnd, gszAppName);

	/*** go, connect to TACO ***/
	if(-1 == rpc_nt_init()) {
		MessageBox(NULL,"Initialisation of ONC RPC failed","Warning", MB_OK|MB_ICONWARNING);
		return (FALSE);        /* zuut! error rpc_nt_init */
	}
	
	if(!application_main(gArgc, gArgv)) {  /* TACO startup ok */
		MessageBox(NULL,"Initialisation of TACO failed","Warning", MB_OK|MB_ICONWARNING);
		return (FALSE);                    /* huugh! Taco startup not ok */
	}

/*#ifdef _DEBUG*/
	// Get/Set the debug level
	SetDebugLevel(GetDebugLevel());
	dprintf("Initial debug level is %d", giDebugLevel);
/*#endif // _DEBUG*/

	/* init the main window and it's backdrop */
	SetCapture(hWnd);
	oldcr= SetCursor(LoadCursor((HANDLE)NULL, IDC_WAIT));
	/* Load the bit map that will be displayed as background */
	hBmp= LoadBitmap(ghAppInstance, MAKEINTRESOURCE(IDB_BITMAP3));
	/* Calculate bitmap size using a BITMAP structure */
	GetObject(hBmp, sizeof(BITMAP), &bm);
	/* make a compatible virtual memory image  */
	hDC= GetDC(hWnd);
	hMemDC= CreateCompatibleDC(hDC);
	hBmpOld= SelectObject(hMemDC, hBmp);
	/* calculate the window size for a given client rect size */
	GetWindowRect(hWnd, &wnd);
	GetClientRect(hWnd, &client);
	calc.right= wnd.right-wnd.left-client.right+bm.bmWidth;
	calc.bottom= wnd.bottom-wnd.top-client.bottom+bm.bmHeight;
	/* resize the frame window  */
	SetWindowPos( 
		hWnd,
		HWND_TOP, 
		0, 
		0, 
		calc.right, 
		calc.bottom, 
		SWP_NOMOVE | SWP_NOZORDER);
	// display version text in main window 
	SetBkMode(hMemDC, TRANSPARENT);
	SetTextColor(hMemDC, RGB(255,255,255));
	GetTextMetrics(hMemDC, &tm);
	// origin of version text
	xorg= (int)(bm.bmWidth*0.12);
	yorg= (int)(bm.bmHeight*0.45);
	// loop counts the lines to place
	for(loop= 0; loop < gMWndTxtDisplay.lines; loop++) {
		TextOut(
			hMemDC,
			xorg,
			yorg,
			gMWndTxtDisplay.text[loop],
			strlen(gMWndTxtDisplay.text[loop]));
			yorg+=(tm.tmHeight+tm.tmExternalLeading);
	}
	// save the next three: - we need them for hWnd::WM_PAINT!
	gMainWndBackdrop.width= bm.bmWidth;
	gMainWndBackdrop.height=bm.bmHeight;
	gMainWndBackdrop.hMemDC= hMemDC;
	// ok all initialisation done
	ReleaseDC(hWnd, hDC);
	SetCursor(oldcr);
	ReleaseCapture();
		
	// Show the main window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	if (NULL != DelayedStartup) {
		if(-1 == (*DelayedStartup)()) { /* do delayed startup */
			MessageBox(NULL,"Delayed startup failed","Warning", MB_OK|MB_ICONWARNING);
			return (FALSE);        
		}
	}

	/* create and start the worker thread */
	ghWorkerThread= CreateThread(
		NULL,                   /* no security attributes                */
		0,                      /* use default stack size                */
		(LPTHREAD_START_ROUTINE) WorkerThreadMain,/* Thread entry-point  */
		&hWnd,                  /* argument to thread function           */
		0,                      /* use default creation flags            */
		&gdwWorkerThreadId);    /* returns thread identifier             */
	if( NULL == ghWorkerThread) {
		MessageBox(hWnd, "CreateThread error","Error",MB_OK|MB_ICONERROR);
		return (FALSE);
	}
	return (TRUE);
}

//
//  FUNCTION: MyRegisterClass(CONST WNDCLASS*)
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
// 	This function and its usage is only necessary if you want this code
// 	to be compatible with Win32 systems prior to the 'RegisterClassEx'
//	function that was added to Windows 95. It is important to call this function
// 	so that the application will get 'well formed' small icons associated
// 	with it.
//
static ATOM 
MyRegisterClass(CONST WNDCLASS *lpwc)
{
	HANDLE  hMod;
	FARPROC proc;
	WNDCLASSEX wcex;

	hMod = GetModuleHandle ("USER32");
	if (hMod != NULL) {

#if defined (UNICODE)
		proc = GetProcAddress (hMod, "RegisterClassExW");
#else
		proc = GetProcAddress (hMod, "RegisterClassExA");
#endif

		if (proc != NULL) {

			wcex.style         = lpwc->style;
			wcex.lpfnWndProc   = lpwc->lpfnWndProc;
			wcex.cbClsExtra    = lpwc->cbClsExtra;
			wcex.cbWndExtra    = lpwc->cbWndExtra;
			wcex.hInstance     = lpwc->hInstance;
			wcex.hIcon         = lpwc->hIcon;
			wcex.hCursor       = lpwc->hCursor;
			wcex.hbrBackground = lpwc->hbrBackground;
            wcex.lpszMenuName  = lpwc->lpszMenuName;
			wcex.lpszClassName = lpwc->lpszClassName;

			// Added elements for Windows 95:
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.hIconSm = LoadIcon(wcex.hInstance, "DS_SMALL");
			
			return (*proc)(&wcex);//return RegisterClassEx(&wcex);
		}
	}
	return (RegisterClass(lpwc));
}

/*****************************************************************************************
//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  MESSAGES:
//
//	WM_COMMAND - process the application menu
//  WM_NCRBUTTONUP - User has clicked the right button on the application's system menu
//  WM_RBUTTONDOWN - Right mouse click -- put up context menu here if appropriate
//  WM_DISPLAYCHANGE - message sent to Plug & Play systems when the display changes
//	WM_PAINT - Paint the main window
//	WM_DESTROY - post a quit message and return
//
//
 *****************************************************************************************/
static LRESULT CALLBACK 
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
    POINT pnt;
	HMENU hMenu;
	BOOL fChanged;
	/*
	 * WARNING: db_setup.h uses the macro 'SIZE' which is in conflict with windef.h !!!
	 * To get around this problem we use 'SIZEL', which describes the same structure as
	 * window's 'SIZE' and is typedefed in windef.h. (Took me a long time to find...wdk)
	 */
	SIZEL screenSize;
	char helpFile[256];

	strcpy(helpFile, gszAppName);
	strcat(helpFile,".hlp");

	switch (message) { 

		case WM_COMMAND:
			if (WM_COMMAND_handler( hWnd, wParam, lParam)) {
				return (0);
			} else {
				return (DefWindowProc(hWnd, message, wParam, lParam));
			}


		case WM_NCRBUTTONUP: // RightClick on windows non-client area...
			if (IS_WIN95 && SendMessage(hWnd, WM_NCHITTEST, 0, lParam) == HTSYSMENU)
			{
				/* The user has clicked the right button on the applications
				// 'System Menu'. Here is where you would alter the default
				// system menu to reflect your application. Notice how the
				// explorer deals with this. For this app, we aren't doing
				// anything 
				*/
				return (DefWindowProc(hWnd, message, wParam, lParam));
			} else {
				// Nothing we are interested in, allow default handling...
				return (DefWindowProc(hWnd, message, wParam, lParam));
			}

        case WM_RBUTTONDOWN: // RightClick in windows client area...
            pnt.x = LOWORD(lParam);
            pnt.y = HIWORD(lParam);
            ClientToScreen(hWnd, (LPPOINT) &pnt);
		// This is where you would determine the appropriate 'context'
		// menu to bring up. Since this app has no real functionality,
		// we will just bring up the 'Help' menu:
            hMenu = GetSubMenu (GetMenu (hWnd), 2);
            if (hMenu) {
                TrackPopupMenu (hMenu, 0, pnt.x, pnt.y, 0, hWnd, NULL);
            } else {
				/* Couldn't find the menu... */
                MessageBeep(0);
            }
            break;


		case WM_DISPLAYCHANGE: // Only comes through on plug'n'play systems 
			fChanged = (BOOL)wParam;
			screenSize.cx = LOWORD(lParam);
			screenSize.cy = HIWORD(lParam);
			
			if (fChanged) {
				/* The display 'has' changed. screenSize reflects the */
				/* new size. */
				MessageBox (GetFocus(), "Display Changed", gszAppName, 0);
			} else {
				/* The display 'is' changing. screenSize reflects the */
				/* original size. */
				MessageBeep(0);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint (hWnd, &ps);
			BitBlt(
				hdc,
				0, 0,
				gMainWndBackdrop.width,
				gMainWndBackdrop.height,
				gMainWndBackdrop.hMemDC,
				0,0,
				SRCCOPY);
				EndPaint (hWnd, &ps);
			break;   
			
		case WM_CLOSE:
		case WM_DESTROY:
		   /* 
		    * Tell WinHelp we don't need it any more...
			*/
	        WinHelp (hWnd, helpFile, HELP_QUIT,(DWORD)0);
		   /*
			* terminate and clean up
			*/
			if( NULL != OnShutDown) (*OnShutDown)();
		   /*
			* unregister from static data base 
			*/
			unregister_server();
		   /*
			* RPC and potmapper stuff.
			*/
			svc_unregister(config_flags.prog_number, DEVSERVER_VERS);
			svc_unregister(config_flags.prog_number, API_VERSION);
			svc_unregister(config_flags.prog_number, ASYNCH_API_VERSION);

			svc_destroy(transp);
			svc_destroy(transp_tcp);
		   /*
			* RPC & Winsock cleanup.
			*/
    		rpc_nt_exit();
		   /* 
			* WIN32: end the process and all its threads.
			*/
			PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
			break;
	}
	return (0);
}

//
//  FUNCTION: About(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for "About" dialog box
// 		This version allows greater flexibility over the contents of the 'About' box,
// 		by pulling out values from the 'Version' resource.
//
//  MESSAGES:
//
//	WM_INITDIALOG - initialize dialog box
//	WM_COMMAND    - Input received
//
//
static LRESULT CALLBACK 
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static  HFONT hfontDlg;		// Font for dialog text
	static	HFONT hFinePrint;	// Font for 'fine print' in dialog
	DWORD   dwVerInfoSize;		// Size of version information block
	LPSTR   lpVersion;			// String pointer to 'version' text
	DWORD   dwVerHnd=0;			// An 'ignored' parameter, always '0'
	UINT    uVersionLen;
	WORD    wRootLen;
	BOOL    bRetCode;
	int     i;
	char    szFullPath[256];
	char    szResult[256];
	char    szGetName[256];
	DWORD	dwVersion;
	char	szVersion[40];
	DWORD	dwResult;

	switch (message) {
        case WM_INITDIALOG:
			ShowWindow (hDlg, SW_HIDE);
			hfontDlg = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				VARIABLE_PITCH | FF_SWISS, "");
			hFinePrint = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				VARIABLE_PITCH | FF_SWISS, "");
			CenterWindow (hDlg, GetWindow (hDlg, GW_OWNER));
			GetModuleFileName (ghAppInstance, szFullPath, sizeof(szFullPath));

			// Now lets dive in and pull out the version information:
			dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
			if (dwVerInfoSize) {
				LPSTR   lpstrVffInfo;
				HANDLE  hMem;
				hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
				lpstrVffInfo  = GlobalLock(hMem);
				GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);
				// The below 'hex' value looks a little confusing, but
				// essentially what it is, is the hexidecimal representation
				// of a couple different values that represent the language
				// and character set that we are wanting string values for.
				// 040904E4 is a very common one, because it means:
				//   US English, Windows MultiLingual characterset
				// Or to pull it all apart:
				// 04------        = SUBLANG_ENGLISH_USA
				// --09----        = LANG_ENGLISH
				// ----04E4 = 1252 = Codepage for Windows:Multilingual
				lstrcpy(szGetName, "\\StringFileInfo\\040904E4\\");	 
				wRootLen = lstrlen(szGetName); // Save this position
			
				// Set the title of the dialog:
				lstrcat (szGetName, "ProductName");
				bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
					(LPSTR)szGetName,
					(LPVOID)&lpVersion,
					(UINT *)&uVersionLen);
				lstrcpy(szResult, "About ");
				lstrcat(szResult, lpVersion);
				SetWindowText (hDlg, szResult);

				// Walk through the dialog items that we want to replace:
				for (i = IDV_FIRST; i <= IDV_LAST; i++) {
					GetDlgItemText(hDlg, i, szResult, sizeof(szResult));
					szGetName[wRootLen] = (char)0;
					lstrcat (szGetName, szResult);
					uVersionLen   = 0;
					lpVersion     = NULL;
					bRetCode      =  VerQueryValue((LPVOID)lpstrVffInfo,
						(LPSTR)szGetName,
						(LPVOID)&lpVersion,
						(UINT *)&uVersionLen);

					if ( bRetCode && uVersionLen && lpVersion) {
					// Replace dialog item text with version info
						lstrcpy(szResult, lpVersion);
						SetDlgItemText(hDlg, i, szResult);
					} else {
						dwResult = GetLastError();
						wsprintf (szResult, "Error %lu", dwResult);
						SetDlgItemText (hDlg, i, szResult);
					}
					SendMessage (GetDlgItem (hDlg, i), WM_SETFONT, 
						(UINT)((i==IDV_LAST)?hFinePrint:hfontDlg),
						TRUE);
				} // for (i = IDV_FIRST; i <= IDV_LAST; i++)


				GlobalUnlock(hMem);
				GlobalFree(hMem);

			} else {
				// No version information available.
			} // if (dwVerInfoSize)

            SendMessage (GetDlgItem (hDlg, IDC_LABEL), WM_SETFONT,
				(WPARAM)hfontDlg,(LPARAM)TRUE);
            SendMessage (GetDlgItem (hDlg, IDC_LABEL1), WM_SETFONT,
				(WPARAM)hfontDlg,(LPARAM)TRUE);

			/* We are  using GetVersion rather then GetVersionEx
			// because earlier versions of Windows NT and Win32s
			// didn't include GetVersionEx:
			 */
			dwVersion = GetVersion();

			if (dwVersion < 0x80000000) {
				// Windows NT
				wsprintf (szVersion, "Microsoft Windows NT %u.%u (Build: %u)",
					(DWORD)(LOBYTE(LOWORD(dwVersion))),
					(DWORD)(HIBYTE(LOWORD(dwVersion))),
                    (DWORD)(HIWORD(dwVersion)) );
			} else if (LOBYTE(LOWORD(dwVersion))<4) {
				// Win32s
                wsprintf (szVersion, "Microsoft Win32s %u.%u (Build: %u)",
					(DWORD)(LOBYTE(LOWORD(dwVersion))),
					(DWORD)(HIBYTE(LOWORD(dwVersion))),
                    (DWORD)(HIWORD(dwVersion) & ~0x8000) );
			} else {
				// Windows 95
                wsprintf (szVersion, "Microsoft Windows 95 %u.%u",
                    (DWORD)(LOBYTE(LOWORD(dwVersion))),
                    (DWORD)(HIBYTE(LOWORD(dwVersion))) );
			}

		    SetWindowText (GetDlgItem(hDlg, IDC_OSVERSION), szVersion);
			ShowWindow (hDlg, SW_SHOW);
			return (TRUE);

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, TRUE);
				DeleteObject (hfontDlg);
				DeleteObject (hFinePrint);
				return (TRUE);
			}
			break;
	}

    return FALSE;
}

//
//   FUNCTION: CenterWindow(HWND, HWND)
//
//   PURPOSE: Centers one window over another. 
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
// 		This functionwill center one window over another ensuring that 
//		the placement of the window is within the 'working area', meaning 
//		that it is both within the display limits of the screen, and not 
//		obscured by the tray or other framing elements of the desktop.
static BOOL 
CenterWindow (HWND hwndChild, HWND hwndParent)
{
	RECT    rChild, rParent, rWorkArea;
	int     wChild, hChild, wParent, hParent;
	int     xNew, yNew;
	BOOL 	bResult;

	// Get the Height and Width of the child window
	GetWindowRect (hwndChild, &rChild);
	wChild = rChild.right - rChild.left;
	hChild = rChild.bottom - rChild.top;

	// Get the Height and Width of the parent window
	GetWindowRect (hwndParent, &rParent);
	wParent = rParent.right - rParent.left;
	hParent = rParent.bottom - rParent.top;

	// Get the limits of the 'workarea'
	bResult = SystemParametersInfo(
		SPI_GETWORKAREA,	// system parameter to query or set
		sizeof(RECT),
		&rWorkArea,
		0);
	if (!bResult) {
		rWorkArea.left = rWorkArea.top = 0;
		rWorkArea.right = GetSystemMetrics(SM_CXSCREEN);
		rWorkArea.bottom = GetSystemMetrics(SM_CYSCREEN);
	}

	// Calculate new X position, then adjust for workarea
	xNew = rParent.left + ((wParent - wChild) /2);
	if (xNew < rWorkArea.left) {
		xNew = rWorkArea.left;
	} else if ((xNew+wChild) > rWorkArea.right) {
		xNew = rWorkArea.right - wChild;
	}

	// Calculate new Y position, then adjust for workarea
	yNew = rParent.top  + ((hParent - hChild) /2);
	if (yNew < rWorkArea.top) {
		yNew = rWorkArea.top;
	} else if ((yNew+hChild) > rWorkArea.bottom) {
		yNew = rWorkArea.bottom - hChild;
	}

	// Set it, and return
	return SetWindowPos (hwndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

static LRESULT CALLBACK
WM_COMMAND_handler( HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	BOOL bGotHelp;
	static BOOL bHideCon= TRUE;
	char helpFile[256];

	wmId    = LOWORD(wParam); // Remember, these are...
	wmEvent = HIWORD(wParam); // ...different for Win32!

	strcpy(helpFile, gszAppName);
	strcat(helpFile,".hlp");

	//Parse the menu selections:
	switch (wmId) {

		case IDM_ABOUT:
			DialogBox(ghAppInstance, "AboutBox", hWnd, (DLGPROC)About);
			break;

		case IDM_EXIT:
			DestroyWindow (hWnd);
			break;

        case IDM_HELPTOPICS: // Only called in Windows 95
			bGotHelp = WinHelp (hWnd, helpFile, HELP_FINDER,(DWORD)0);
			if (!bGotHelp) {
				MessageBox (GetFocus(),"Unable to activate help",
                gszAppName, MB_OK|MB_ICONHAND);
			}
			break;

		case IDM_HELPCONTENTS: // Not called in Windows 95
            bGotHelp = WinHelp (hWnd, helpFile, HELP_CONTENTS,(DWORD)0);
		    if (!bGotHelp) {
           		MessageBox (GetFocus(),"Unable to activate help",
                gszAppName, MB_OK|MB_ICONHAND);
			}
			break;

		case IDM_HELPSEARCH: // Not called in Windows 95
 		    if (!WinHelp(hWnd, helpFile, HELP_PARTIALKEY,
				(DWORD)(LPSTR)"")) {
				MessageBox (GetFocus(), "Unable to activate help",
				gszAppName, MB_OK|MB_ICONHAND);
			}
			break;
		
		case IDM_HELPHELP: // Not called in Windows 95
			if(!WinHelp(hWnd, (LPSTR)NULL, HELP_HELPONHELP, 0)) {
				MessageBox (GetFocus(), "Unable to activate help",
				gszAppName, MB_OK|MB_ICONHAND);
			}
			break;

		case IDM_DEBUG0:
			dprintf2("Checked debug level 0");
			SetDebugLevel(0);
			break;

		case IDM_DEBUG1:
			dprintf2("Checked debug level 1");
			SetDebugLevel(1);
			break;

		case IDM_DEBUG2:
			dprintf2("Checked debug level 2");
			SetDebugLevel(2);
			break;

		case IDM_DEBUG3:
			dprintf2("Checked debug level 3");
			SetDebugLevel(3);
			break;

		case IDM_DEBUG4:
			dprintf2("Checked debug level 4");
			SetDebugLevel(4);
			break;

		case ID_FILE_EXIT:
			SendMessage(ghWndMain, WM_DESTROY, 0, 0);
			break;

		case ID_VIEW_CONSOLE:
			if( bHideCon) {
				ShowWindow(hConsole, SW_SHOW);
				bHideCon= FALSE;
				dprintf2("Checked console view");
			} else {
				ShowWindow(hConsole, SW_HIDE);
				bHideCon= TRUE;
				dprintf2("Unchecked console view");
			}
			break;

		// Here are all the other possible menu options,
		// all of these are currently disabled:
		case IDM_NEW:
		case IDM_OPEN:
		case IDM_SAVE:
		case IDM_SAVEAS:
		case IDM_UNDO:
		case IDM_CUT:
		case IDM_COPY:
		case IDM_PASTE:
		case IDM_LINK:
		case IDM_LINKS:

		/*
		 * all other unknown command
		 */
		default:
			return (FALSE);
	}  /* switch */
	return (TRUE);
}

DWORD
WorkerThreadMain(LPDWORD lpdwParam)
{
	svc_run();
	PRINTF("FATAL ERROR: svc_run returned");
	rpc_nt_exit();
	return 0;      // will terminate rpc worker thread
}

#endif  /* _NT */

/*+**********************************************************************
 Function   :   static void devserver_prog_4()

 Description:   Entry point for received RPCs.
	    :   Switches to the wished remote procedure.

 Arg(s) In  :   struct svc_rep *rqstp - RPC request handle
	    :   SVCXPRT *transp       - Service transport handle

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/

static void _WINAPI devserver_prog_4 (struct svc_req *rqstp, SVCXPRT *transp) 
{
	char	*help_ptr;
	int	pid = 0;

	union {
		_dev_import_in	rpc_dev_import_4_arg;
		_dev_free_in 	rpc_dev_free_4_arg;
		_server_data 	rpc_dev_putget_4_arg;
		_server_data 	rpc_dev_put_4_arg;
                _dev_query_in   rpc_dev_query_cmd_4_arg;
/*
 * RPC ADMIN service disabled temporarily, to be reimplemented later
 *
 * - andy 26nov96
 *
 *		long 		rpc_admin_import_4_arg;
 *		_server_admin 	rpc_admin_4_arg;
 */
		_server_data 	rpc_dev_putget_raw_4_arg;
		_server_data 	rpc_dev_put_asyn_4_arg;
	} argument;

	char *result;
	xdrproc_t xdr_argument,xdr_result;
#ifdef __cplusplus
	DevRpcLocalFunc local;
#else
	char *(*local)();
#endif

	/*
	 *  call the right server routine
	 */

	/*printf("devserver_prog_4(): called with rqstp->rq_proc %d\n",rqstp->rq_proc);*/
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
		local = (DevRpcLocalFunc) rpc_dev_putget_4;
#else
		local = (char *(*)()) rpc_dev_putget_4;
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
/* event query */
	case RPC_DEV_EVENT_QUERY:
        	xdr_argument = (xdrproc_t)xdr__dev_query_in;
        	xdr_result = (xdrproc_t)xdr__dev_query_out;
#ifdef __cplusplus
        	local = (DevRpcLocalFunc) rpc_dev_event_query_4;
#else
        	local = (char *(*)()) rpc_dev_event_query_4;
#endif
        	break;
/* end event query */


/*
 * RPC ADMIN service disabled temporarily, to be reimplemented later
 *
 * - andy 26nov96
 *
 *	case RPC_ADMIN_IMPORT:
 *		xdr_argument = xdr_long;
 *		xdr_result = xdr_long;
 *		local = (char *(*)()) rpc_admin_import_4;
 *		break;
 *
 *	case RPC_ADMIN:
 *		xdr_argument = (xdrproc_t)xdr__server_admin;
 *		xdr_result = xdr_long;
 *		local = (char *(*)()) rpc_admin_4;
 *		break;
 */

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
		dev_printerror (SEND,"%s",
		"svcerr_decode : server couldn't decode incoming arguments");
		svcerr_decode(transp);
		return;
	}


	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, xdr_result, (caddr_t)result)) 
	{
		dev_printerror (SEND,"%s",
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
		dev_printerror (SEND,"%s",
		"svc_freeargs : server couldn't free arguments !!");
		return;
	}
}


/*+**********************************************************************
 Function   :   static long svc_check()

 Description:   Checks wether a device server with 
		the same name is already running.

 Arg(s) In  :   none

 Arg(s) Out :   long *error - Will contain an appropriate error
			      code if the corresponding call
			      returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

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
 *
 *  DON'T - removed this check it is an OS9 anachronism (andy 2/5/2000)
 *
 *	if (strcmp (config_flags.server_host,host_name) != 0)
 *	   {
 *	   *error = DevErr_ServerRegisteredOnDifferentHost;
 *	   return (-1);
 *	   }
 */

/*
 * force version number to latest API_VERSION [4] , new servers do not
 * support the old DSERVER_VERSION [1] anymore
 */
	vers_number = API_VERSION;

    /*
     *  old server still exists ?
	  */

	clnt = clnt_create (host_name, prog_number,vers_number,"udp");
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

#ifdef _UCC
extern int ds_rpc_svc_fd; /* global variable - client rpc file descriptor */
#define getdtablesize() FD_SETSIZE
#ifdef __STDC__
void svc_run(void)
#else
void svc_run()
/*+**********************************************************************
 Function   :   void svc_run ()

 Description:   OS9 function to wait forever for rpc requests to a server.
		When a request arrives it is dispatched to the server via
		the rpc call svc_getreqset(). 

		This version specific to OS9 has been modelled on the
		example in the "OS9 Network File System/Remote Procedure
		Call User's Manual" pg 2-21 modified by P.Makijarvi. 
		It solves the problem of exiting on receiving a signal.


 Arg(s) In  :   none

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/
#endif /* __STDC__ */
{
	fd_set readfds;
	int dtbsz = getdtablesize();
	long i;

	FD_ZERO(&readfds);

	/*printf("svc_run(): arrived ...\n");*/
	
	for (;;)
	{
		readfds = svc_fdset;

		/*printf("svc_run(): readfds 0x%08x before select()\n",readfds);*/
		switch (select(dtbsz,&readfds,(fd_set*)0,(fd_set*)0,(struct timeval*)0))
		{
			case -1 : if ((errno == EINTR) || (errno == EOS_SIGNAL))
				  /*perror("svc_run(): select() returns with error");*/
			 	  continue;

				  /*return;*/

			case 0 : break;

			default : 
/*
 * determine which file descriptor the client has called from, this is
 * needed by OS9 in case it has to close the socket e.g. if dev_free()
 */
				  for (i=0; i<FD_SETSIZE; i++)
				  {
				  	if (FD_ISSET(i,&readfds)) ds_rpc_svc_fd = i;
				  }

				  /*printf("svc_run(): readfds 0x%08x after select(), ds_rpc_svc_fd=%d\n",readfds,ds_rpc_svc_fd);*/
				  svc_getreqset(&readfds);
		}

	}
}
#endif /* _UCC */
