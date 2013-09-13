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
 * File:	NT_debug.h
 *
 * Description: Debug and printf support
 *
 * Author(s):	Andy Goetz
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:08:28 $
 *
 *********************************************************************************/

#ifndef _debug_h
#define _debug_h
#ifdef __cplusplus
extern "C" {
#endif

extern LRESULT CALLBACK DebugWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern HWND ghWndDebug;          // handle of debug/console window
extern HWND ghWndMain;           // the main window handle
extern char* gszAppName;         // the application's name
extern HINSTANCE ghAppInstance;  // the application's module handle

extern void cdecl DbgOut(LPSTR lpFormat, ...);
extern void LastErrorBox();
extern DWORD   gdEval;
extern int giDebugLevel;     // 0 is default 

extern void SetDebugLevel(int i);
extern int GetDebugLevel();
extern void __AssertMsg(LPSTR exp ,LPSTR file, int line);

#ifdef _DEBUG


    #define A_SSERT(exp) \
        ((exp) ? (void)0 : __AssertMsg((LPSTR)(#exp), (LPSTR)__FILE__, __LINE__))
    #define E_VAL(exp) \
        (gdEval=(DWORD)(exp), (gdEval) ? (void)0 : __AssertMsg((LPSTR)(#exp), (LPSTR)__FILE__, __LINE__), gdEval)

    #define dprintf                         DbgOut
    #define dprintf1 if (giDebugLevel >= 1) DbgOut
    #define dprintf2 if (giDebugLevel >= 2) DbgOut
    #define dprintf3 if (giDebugLevel >= 3) DbgOut
    #define dprintf4 if (giDebugLevel >= 4) DbgOut

#else // not _DEBUG

    #define A_SSERT(exp) 0
    #define E_VAL(exp) (exp)

    #define dprintf  if (0) ((int (*)(char *, ...)) 0)
    #define dprintf1 if (0) ((int (*)(char *, ...)) 0)
    #define dprintf2 if (0) ((int (*)(char *, ...)) 0)
    #define dprintf3 if (0) ((int (*)(char *, ...)) 0)
    #define dprintf4 if (0) ((int (*)(char *, ...)) 0)

#endif // _DEBUG

#ifdef __cplusplus
}
#endif

#endif  // _debug_h
