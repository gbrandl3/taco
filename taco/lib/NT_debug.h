//
// Debug and printf support
//
/*
  $Header: /home/jkrueger1/sources/taco/backup/taco/lib/NT_debug.h,v 1.2 2004-11-25 15:47:34 andy_gotz Exp $

 Author:	$Author: andy_gotz $

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-11-25 15:47:34 $

*/

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
