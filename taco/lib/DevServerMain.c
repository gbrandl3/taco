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
 * File       	: DevServerMain.c
 *
 * Project    	: Device Servers with sun-rpc
 *
 * Description	: Main programm for all device servers
 *
 * Author(s)  	: Jens Meyer
 * 		  $Author: jkrueger1 $
 *
 * Original   	: March 1991
 *
 * Version	: $Revision: 1.29 $
 *
 * Date		: $Date: 2008-10-22 08:31:22 $
 *
 *******************************************************************-*/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#ifdef unix
#       ifdef HAVE_PATHS_H
#               include <paths.h>
#       else
#               define _PATH_DEVNULL    "/dev/null"
#       endif
#       if HAVE_SYS_TYPES_H
#             include <sys/types.h>
#       endif
#       include <fcntl.h>
#       include <unistd.h>
#endif

#include <API.h>
#include <private/ApiP.h>
#include <Admin.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <DevErrors.h>
#include "db_setup.h"
#include "taco_utils.h"

#if defined WIN32
#include <rpc/Pmap_pro.h>
#include <rpc/pmap_cln.h>
#include <process.h>
/* #define NODATETIMEPICK to avoid compiler error. (I hate MicroSoft!)*/
#define NODATETIMEPICK
#include <commctrl.h>
#undef NODATETIMEPICK
#else
#include <rpc/pmap_clnt.h>
#endif	/* WIN32 */
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
extern long minimal_access;

static void _WINAPI devserver_prog_4	PT_( (struct svc_req *rqstp,SVCXPRT *transp) );


/****************************************
 *          Globals                     *
 ****************************************/

/*
 *  Type for global state flags for the current
 *  server or client status is defined in API.h 
 */

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

#if !defined(vxworks) && !defined(NOMAIN)
#if 0
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
#endif
/**/
#ifdef WIN32  /* WIN32 */

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
#define ISWIN32      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(ISWIN32) && (LOBYTE(LOWORD(GetVersion()))<4))
#define IS_WIN95 (BOOL)(!(ISWIN32) && !(IS_WIN32S)) && IS_WIN32

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

static BOOL /* RPC and TACO initialization (Device Server's main) */
application_main (int argc, char **argv)  /* Windows does not use main()! */

#else /* WIN32 */
#ifdef unix
/**@ingroup dsAPIintern
 * This function makes the current process to a daemon.
 *
 * @return Process ID of the child if the process became a daemon otherwise the process ID of itself.
 */
pid_t   become_daemon(void)
{
        pid_t   pid = getpid();
        int     fd_devnull = open(_PATH_DEVNULL, O_RDWR);
        if (fd_devnull < 0)
                return pid;
        while (fd_devnull <= 2)
        {
                int i = dup(fd_devnull);
                if (i < 0)
                        return pid;
                fd_devnull = i;
        }

        if ((pid = fork ()) > 0)
/*
 * Stop the parent process
 */
                exit(0);
        else if (pid == 0)
        {
/*
 * Child process and try to detach the terminal
 */
#if HAVE_SETSID
                setsid();
#elif defined (TIOCNOTTY)
                {
                        int i = open("/dev/tty", O_RDWR, 0);
                        if (i != -1)
                        {
                                ioctl(i, (int)TIOCNOTTY, (char *)NULL);
                                close(i);
                        }
                }
#endif
                dup2(fd_devnull, 0);
                dup2(fd_devnull, 1);
                dup2(fd_devnull, 2);
                pid = getpid();
        }
        return pid;
}
#endif

/**@ingroup dsAPI
 * Main routine for all device servers.
 *
 * Checks wether a device server with the same name is already running. 
 *
 * Opens a connection to the static database and to a message service.
 *
 * Registers the server on a transient program number to the portmapper.
 * 
 * @param argc the number of command line parameters including the name of the executeable itself
 * @param argv options and the personal name for the device server.
 * 
 * @return 1 DS_NOTOK
 */
int main (int argc, char **argv)
#endif /* WIN32 */
{
	char    		host_name [HOST_NAME_LENGTH],
				dsn_name [DS_NAME_LENGTH + DSPERS_NAME_LENGTH + 1],
				*proc_name,
				*display,
				res_path [80],
				res_name[80],
				**device_list = NULL;
				char nethost[80];
#if 0
	pid_t			pid;
#endif
	DevVarStringArray	default_access;
	db_resource		res_tab;

	long			prog_number=0,
				status,
				error = 0;
	int			pid = 0;
	short			m_opt = False,
				s_opt = True,
				nodb_opt = False,
#ifdef unix
				daemon_opt = False,
				pid_file = False,
#endif
				device_no = 0,
				sig,
				i,
				j;

/*
 *  read device server's class name and personal name
 *  check for lenght of names : server process name < DS_NAME_LENGTH char
 *                              personal name       < DSPERS_NAME_LENGTH char
 */
#ifndef WIN32
	if (argc < 2)
	{
		fprintf( stderr, "Usage: %s personal_name [OPTIONS]\n\n",
			argv[0] );
		
		fprintf( stderr, "   -device device1 [device2 ... ] : "
			"only export these devices\n" );
		
		fprintf( stderr, "   -m                             : "
			"enable logging via message server\n" );
		
		fprintf( stderr, "   -nodb                          : "
			"do not use database server for resource handling\n" );
		
		fprintf( stderr, "   -pn program #                  : "
			"use this rpc program number\n" );
		
		fprintf( stderr, "   -s                             : "
			"use startup function from server\n" );
		fprintf( stderr, "   -n nethost                     : "
			"use nethost instead of NETHOST environment\n" );
#ifdef unix
		fprintf( stderr, "   -d                             : "
			"start as daemon\n");
		fprintf( stderr, "   -p pid_file                    : "
			"write a pid file\n");
#endif
		exit (1);
	}
#else
	if(0 !=(proc_name = (char *)strrchr (argv[0],'\\'))) 
	{
		if(strncmp(proc_name+strlen(proc_name)-4,".exe",4) == 0)
    			proc_name[strlen(proc_name)-4]='\0';
	}
#endif  /* !WIN32 */

/*
 * make sure all config flags are set to zero before starting
 */
	memset(config_flags, 0, sizeof(config_flags));
#if defined (unix)
	proc_name = (char *)strrchr (argv[0], '/');
#elif ( OSK || _OSK )
#	ifdef __cplusplus
	proc_name = (char *)strrchr (argv[0], '/');
#	else
	proc_name = (char *)rindex (argv[0], '/');
#	endif
#endif /* OSK || _OSK */

	if (proc_name == NULL)
		proc_name = argv[0];
	else  
		proc_name++;

/*
 *  read options for device server start
 */
	if (argc > 2)
	{
		for (i=2; i<(argc); i++)
		{
			if (strcmp (argv[i],"-m") == 0)
				m_opt = True;
			else if (strcmp (argv[i],"-s") == 0)
				s_opt = True;
#ifdef unix
			else if (strcmp (argv[i],"-d") == 0)
				daemon_opt = True;
#endif
			else if (strcmp (argv[i], "-n") == 0 && (i + 1) < argc)
			{
				snprintf(nethost, sizeof(nethost), "%s", argv[i + 1]);
				taco_setenv("NETHOST", nethost, 1);
				++i;
			}
/*
 * option -nodb means run device server without database
 */
			else if (strcmp (argv[i],"-nodb") == 0)
				nodb_opt = True;
/*
 * option -pn specifies program number (to be used in conjunction with -nodb)
 */
			else if (strcmp (argv[i],"-pn") == 0)
			{
				sscanf(argv[i+1],"%d",&prog_number);
				i++;
			}
/*
 * option -device means remaining command line arguments are device names
 */
			else if (strcmp (argv[i],"-device") == 0)
			{
				device_no = argc-i-1;
				device_list = (char**)malloc(device_no*sizeof(char));
				for (j=i+1; j<argc; j++)
				{
					device_list[j-i-1] = (char*)malloc(strlen(argv[j])+1);
					strcpy(device_list[j-i-1], argv[j]);
				}
				i = j;
			}
		}
	}
#ifdef unix
	if (daemon_opt == True)
		pid = become_daemon();
	else
		pid = getpid();
#endif
	status = device_server(proc_name, argv[1], m_opt, s_opt, nodb_opt, prog_number, device_no, device_list);
#ifdef WIN32
	if (status == FALSE)
		raise(SIGABRT);
#else
	if (status == DS_NOTOK)
		kill(pid,SIGQUIT);
#endif /* WIN32 */
	return status;
}
#endif /* vxworks || NOMAIN */

#ifdef WIN32
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
		snprintf (msg, sizeof(msg), "usage :  %s personal_name [-nodb] [-pn number] [-device device1 ...]\n",gszAppName);
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
#ifdef WIN32
	long error;
	void *test_heap;
#endif /* WIN32 */

#ifdef WIN32
	test_heap = malloc(4096);
	msize(test_heap,&error);
#endif /* WIN32 */

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

#ifdef WIN32
		test_heap = malloc(4096);
		msize(test_heap,&error);
#endif /* WIN32 */

	/*** go, connect to TACO ***/
	if(-1 == rpc_nt_init()) {
		MessageBox(NULL,"Initialisation of ONC RPC failed","Warning", MB_OK|MB_ICONWARNING);
		return (FALSE);        /* zuut! error rpc_nt_init */
	}
	
	if(!application_main(gArgc, gArgv)) {  /* TACO startup ok */
		MessageBox(NULL,"Initialisation of TACO failed","Warning", MB_OK|MB_ICONWARNING);
		return (FALSE);                    /* huugh! Taco startup not ok */
	}

	// Get/Set the debug level
	SetDebugLevel(GetDebugLevel());
	dprintf("Initial debug level is %d", giDebugLevel);

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
			svc_unregister(config_flags->prog_number, DEVSERVER_VERS);
			svc_unregister(config_flags->prog_number, API_VERSION);
			svc_unregister(config_flags->prog_number, ASYNCH_API_VERSION);

			if (transp != 0) svc_destroy(transp);
			if (transp_tcp != 0) svc_destroy(transp_tcp);
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

#endif  /* WIN32 */
