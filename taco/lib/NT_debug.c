static char RcsId[] ="$Header: /home/jkrueger1/sources/taco/backup/taco/lib/NT_debug.c,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";
/*
 debug.c

 Functions to support a popup console/debug window

 Author:	$Author: jkrueger1 $
  
 Version:	$Version$

 Date:		$Date: 2003-03-14 12:22:07 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                      Grenoble, France

*/

#include <windows.h>
#include <stdio.h>
#include "resource.h"       // main symbols
#include "NT_debug.h"

//
// Constants
//
#define MAXLISTLINES    300     // max list lines we keep
#define IDC_LIST        1       // listbox id

//
// local data
//
static HWND hWndDebugList= NULL;

//
// global data
//
HWND ghWndDebug= NULL;    // handle of debug/console window
int giDebugLevel= 0;      // default is debuglevel == 0
HWND ghWndMain;           // the main window handle
char* gszAppName;         // the application's name
HINSTANCE ghAppInstance;  // the application's module handle
DWORD   gdEval;

//
// prototypes
//
#ifdef _DEBUG
static BOOL assert_globals();
#endif

//
// Set the current debug level
//

void SetDebugLevel(int i)
{
    HMENU hMenu;
    int m;
	char buf[32];

#ifdef _DEBUG
	if(!assert_globals()) return;
#endif

	hMenu = GetMenu(ghWndMain); 
	for (m=IDM_DEBUG0; m<=IDM_DEBUG4; m++) {
		CheckMenuItem(hMenu, m, MF_UNCHECKED);
	}
	CheckMenuItem(hMenu, i + IDM_DEBUG0, MF_CHECKED);
	giDebugLevel = i;

    //
    // Save the debug level
    //
    sprintf(buf, "%d", giDebugLevel);
	WriteProfileString(gszAppName, "debug", buf);
}

//
// return saved debug level from .INI file
//

int GetDebugLevel()
{
#ifdef _DEBUG
	if(!assert_globals()) return giDebugLevel;
#endif

	return GetProfileInt(gszAppName, "debug", 1);
}

//
// Show a message box with assertion failure info in it
//

void __AssertMsg(LPSTR exp, LPSTR file, int line)
{
    char buf[256];
    int i;

#ifdef _DEBUG
	if(!assert_globals()) return;
#endif

    wsprintf(buf, 
             "Exp: %s\nFile: %s, Line %d",
             (LPSTR)exp,
             (LPSTR)file,
             line);
    i = MessageBox(ghWndMain,
                   buf,
                   "Assertion failure", 
                   MB_OK | MB_ICONEXCLAMATION);
}

//
// function to add a string to the end of the debug list
//

void cdecl DbgOut(LPSTR lpFormat, ...) 
{
    int i;
    char buf[256];

#ifdef _DEBUG
	if(!assert_globals()) return;
#endif

    //
    // See if we have a debug window or not.
    // If not then try to create one.
    //

    if (!ghWndDebug) 
	{
	
    //
    // define the window`class we want to register
    //
	WNDCLASS wc;

	wc.lpszClassName    = "Debug";
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = LoadIcon(ghAppInstance,"Icon");
    wc.lpszMenuName     = NULL;
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);
    wc.hInstance        = ghAppInstance;
    wc.lpfnWndProc      = DebugWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    
    RegisterClass(&wc);

	//
	// create the debug console
	//
    wsprintf(buf, "%s - Console", (LPSTR)gszAppName);
    ghWndDebug = CreateWindow("Debug",
			buf,
			WS_OVERLAPPED /*| WS_VISIBLE | WS_CAPTION | WS_SYSMENU*/
			| WS_BORDER | WS_THICKFRAME /*| WS_MINIMIZEBOX*/ ,
			GetSystemMetrics(SM_CXSCREEN) / 2-100,
			GetSystemMetrics(SM_CYSCREEN) * 3 / 4-75,
			GetSystemMetrics(SM_CXSCREEN) / 2,
			GetSystemMetrics(SM_CYSCREEN) / 4,
			ghWndMain,
			0,
			ghAppInstance,
			NULL
			);

	if (!ghWndDebug) {
		LastErrorBox();
		return;
		}
    }

    //
    // format the string
    //
//    wvsprintf(buf, lpFormat, (LPSTR)(&lpFormat+1));
    vsprintf(buf, lpFormat, (LPSTR)(&lpFormat+1));

    //
    // stop the listbox repaints while we mess with it
    //
    SendMessage(hWndDebugList, WM_SETREDRAW, (WPARAM) FALSE, (LPARAM) 0);

    //
    // get the item count
    //
    i = (int) SendMessage(hWndDebugList, LB_GETCOUNT, (WPARAM) 0, (LPARAM) 0);
    if (i == LB_ERR) i = 0;

    //
    // scrub a few if we have too many
    //
    while (i >= MAXLISTLINES) {
        SendMessage(hWndDebugList, LB_DELETESTRING, (WPARAM) 0, (LPARAM) 0);
        i--;
    }

    //
    // add the new one on at the end and scroll it into view
    //
    i = (int) SendMessage(hWndDebugList, LB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPSTR) buf);
    SendMessage(hWndDebugList, LB_SETCURSEL, (WPARAM) i, (LPARAM) 0);

    //
    // enable the repaint now
    //
    SendMessage(hWndDebugList, WM_SETREDRAW, (WPARAM) TRUE, (LPARAM) 0);
}


//
// Format the last error and display in a MessgaeBox
//

void LastErrorBox()
{
	LPVOID lpMsgBuf;
 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	// Display the string.
	MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
	return;
}


//
// Measure an item in our debug listbox
//

static void MeasureDebugItem(HWND hWnd, LPMEASUREITEMSTRUCT lpMIS)
{
    TEXTMETRIC tm;
    HDC hDC;


    hDC = GetDC(hWnd);
    GetTextMetrics(hDC, &tm);
    ReleaseDC(hWnd, hDC);
    lpMIS->itemHeight = tm.tmHeight;
}

//
// Display an item in our debug listbox
//

void DrawDebugItem(HWND hWnd, LPDRAWITEMSTRUCT lpDI)
{
    HBRUSH hbrBkGnd;
    RECT rc;
    HDC hDC;
    char buf[256];
    
    hDC = lpDI->hDC;
    rc = lpDI->rcItem;

    switch (lpDI->itemAction) {

    case ODA_SELECT:
    case ODA_DRAWENTIRE:

        //
        // erase the rectangle
        //
        hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(hDC, &rc, hbrBkGnd);
        DeleteObject(hbrBkGnd);

            //
            // show the text in our standard font
            //
            SetBkMode(hDC, TRANSPARENT);
    
            SendMessage(lpDI->hwndItem, 
                        LB_GETTEXT, 
                        lpDI->itemID, 
                        (LPARAM)(LPSTR)buf);

            ExtTextOut(hDC, 
                       rc.left+2, rc.top,
                       ETO_CLIPPED,
                       &rc, 
                       buf,
                       lstrlen(buf), 
                       NULL);
    
        break;
    
    }
}

//
// Window procedure for debug window
//

LRESULT CALLBACK DebugWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   HMENU hMenu;
   PAINTSTRUCT ps;

#ifdef _DEBUG
	if(!assert_globals()) return 0;
#endif

    switch(msg) {
    case WM_CREATE:

        //
        // Create the listbox 
        //
        hWndDebugList = CreateWindow("Listbox",
                            "",
                            WS_CHILD | WS_VISIBLE | WS_VSCROLL
                             | LBS_DISABLENOSCROLL
                             | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED
                             | LBS_NOINTEGRALHEIGHT,
                            0,
                            0,
                            0,
                            0,
                            hWnd,
                            (HMENU)IDC_LIST,
                            ghAppInstance,
                            (LPSTR)NULL
                            );

        break;

    case WM_SIZE:
        SetWindowPos(hWndDebugList,
                     NULL,
                     0, 0,
                     LOWORD(lParam), HIWORD(lParam),
                     SWP_NOZORDER);
        break;

    case WM_SETFOCUS:
        SetFocus(hWndDebugList);
        break;

    case WM_MEASUREITEM:
        MeasureDebugItem(hWnd, (LPMEASUREITEMSTRUCT)lParam);
        return (LRESULT) TRUE;

    case WM_DRAWITEM:
        DrawDebugItem(hWnd, (LPDRAWITEMSTRUCT) lParam);
        break;

    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        ghWndDebug = NULL;
        break;

	case WM_SHOWWINDOW:
		hMenu = GetMenu(ghWndMain); 
		if( wParam) {
			CheckMenuItem(hMenu, ID_VIEW_CONSOLE, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_VIEW_CONSOLE, MF_UNCHECKED);
		}
        return DefWindowProc(hWnd, msg, wParam, lParam);
		break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
        break;
    }
    return 0;
}


//
// assert globals that have to be set in code that uses
// the utilities defined in this module.
//
#ifdef _DEBUG
static BOOL assert_globals()
{
	if(NULL == gszAppName)  {
		MessageBox(NULL,"Debug/Console library: 'gszAppName' not initalised",                    
			"Assertion failure", 
            MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	} else if (0 == ghWndMain) {
		MessageBox(NULL,"Debug/Console library: 'ghWndMain' not initalised",                    
			"Assertion failure", 
            MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	} else if(0 == ghAppInstance) {
		MessageBox(NULL,"Debug/Console library: 'ghAppInstance' not initalised",                    
			"Assertion failure", 
            MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	} else
		return TRUE;
}
#endif
