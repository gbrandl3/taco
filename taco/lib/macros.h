/*+*******************************************************************
 * 
 * File:          macros.h
 *  
 *  Project:       Device Servers with sun-rpc
 * 
 * Description:   Public macro definitions.
 * 		Macros for function declarations and prototypes.
 *
 * Author(s):     Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:      July 1994
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:		$Date: 2003-05-22 08:09:18 $
 *
 * Copyright (c) 1990 by European Synchrotron Radiation Facility,
 *                       Grenoble, France
 *
 ********************************************************************-*/

#ifndef taco_macros_h
#define taco_macros_h

/*
 * convert NULL-terminated string to lower characters
 */
#define TOLOWER(a) {char* i; i=a; while ( '\0'!=*i ) { *i=tolower(*i);i++; }}

#if !defined(FORTRAN)

#if defined (WIN32)
#undef __STDC__
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <winsock.h>
#include <rpc/rpc.h>
#include <process.h>

/*
 *  macro emulates BSD Socket function inet_network
 *  which does not exist for WinSock API.
 */
#define inet_network(addr) htonl(inet_addr(addr))

#define NETMNG   1
#define __STDC__ 1


#else /* UNIX */

#ifdef NETMNG
#undef NETMNG
#endif

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#ifndef _UCC
#include <unistd.h>
#endif


#ifndef _sunrpc
#define PORTMAP
#include <rpc/rpc.h>
#endif /* _sunrpc */

#ifdef _UCC
#include <inet/socket.h>
#ifdef __cplusplus
extern "C" {
int close(int);
int kill(int,int);
int getpid(void);
int _errmsg(...);
int intercept(...);
int sleep(unsigned int);
int os9exec(...);
int gettimeofday(struct timeval *,struct timezone *);

}
#endif /* __cplusplus */
#endif /* _UCC */

#endif /* (_NT) && (_WIN32) */
#endif /* FORTRAN */

/*
 * macro to extract prototypes in function declarations if
 * the ANSI C standad is not used.
 */

#if defined (__STDC__) || (__STDCPP__)
#define PT_(A) A
#else
#define PT_(A) ()
#endif /* __STDC__ || __STDCPP__ */


/*
 *  general definitions
 */

#define False 0
#define True 1
#ifndef NULL
#define NULL 0
#endif /* NULL */

#define DS_OK           0
#define DS_NOTOK        (-1)
#define DS_WARNING      1
#define DS_TRUE         1
#define DS_FALSE        0

/*
 * For the OS9 compiler cc68 size_t is not defined.
 * Define it here!
 */

#if !defined(FORTRAN)
#ifdef OSK
typedef unsigned int	size_t;
#endif /* OSK */
#endif /* FORTRAN */

/*
 * Definitions for name length
 */

#define LONG_NAME_SIZE			80
#define SHORT_NAME_SIZE			32

#define MAX_RESOURCE_FIELD_LENGTH	19
/*
 *	Definitions for PC_USER_AUTHENTICATION
 */
#define PC_USER_ID	33
#define PC_USER_NAME	"pc_user"
#define PC_GROUP_ID	33
#define PC_GROUP_NAME	"pc_group"

/*
 * Macros used for the Netmanage PC software !
 */

#if !defined (_NT)  /* UNIX definitions */
#if !defined (linux)
#define _LPfd_set	struct fd_set *
#else /* !(linux) */
#define _LPfd_set	fd_set *
#endif /* !(linux) */
#define	_Int		long
#define _WINAPI
#define TIMEVAL(a)  	a
#define MAXU_INT    	~0
#define _DLLFunc        
#define HWALK(p)
#define PRINTF(a)	printf(a)

#else /* (_NT) */
#define _LPfd_set	struct fd_set *
#define	_Int		short
#define _WINAPI     	WINAPI	   /* needed for MSVC++ */
#define TIMEVAL(a)    	(struct timeval*)&a	  /* needed for NetManage's RPC clnt_call() */
#define MAXU_INT    	0xFFFFFFFF

/* Macros for NT DLL symbol definition */
#define DllExport       __declspec( dllexport )
#define DllImport       __declspec( dllimport )
#ifdef _DLL           /* export definitions for NT Dlls */
#define _DLLFunc   	DllExport
#else 
#define _DLLFunc
#endif /* (_DLL) */

/*
 * Heap Walk utility: very useful for debugging !!
 */
extern _DLLFunc size_t 	msize PT_( (void* pointer, int* error) );
#if defined (_HWALK)
static int 		msz_error;
static size_t 		msz_size;

#define HWALK(p) msz_size= msize(p,&msz_error); \
   if(msz_size ==0)    \
      printf("Heap Walk ERR= %d (File: %s, Line %d)\n",msz_error,__FILE__,__LINE__); \
   else    \
      printf("Heap Size = %u (File %s, Line %d)\n", msz_size,__FILE__,__LINE__)
#else
#define HWALK(p)
#endif     /* _HWALK */   

/* 
 * these macros are used to display ASCII strings on the
 * NT console. (See DevServerMain.c and NT_debug.c 
 * for creation of the console).
 */
/*
extern HANDLE hConOut;
#define conout(a) { DWORD dwNumWritten; \
					WriteConsole(hConOut,a,strlen(a),&dwNumWritten,NULL);}
*/

#ifdef WIN32
#include <NT_debug.h>
#define PRINTF(a)      MessageBox(NULL,a,NULL,MB_OK|MB_ICONASTERISK);
#endif  /* WIN32 */

#endif /* (_NT) */



/*
 * macros to define left/right bit shifts in a way that can be used both
 * in C and FORTRAN programs:
 *
 * DS_LSHIFT(x,y) shifts (x) left  by (y) bit positions
 * DS_RSHIFT(x,y) shifts (x) right by (y) bit positions
 */

#if !defined(FORTRAN)
#define DS_LSHIFT(x,y) ((x) << (y))
#define DS_RSHIFT(x,y) ((x) >> (y))
#else /* FORTRAN */
#define DS_LSHIFT(x,y) ISHFT(x,y)
#define DS_RSHIFT(x,y) ISHFT(x,-(y))
#endif /* FORTRAN */

#if !defined(FORTRAN)
#if defined (__STDC__) || (__STDCPP__) && !defined (NETMNG)
typedef void *DevArgument;
#else
typedef char *DevArgument;
#endif /* __STDC__ || __STDCPP__ */
#endif /* FORTRAN */

#endif /* _macros_h */
