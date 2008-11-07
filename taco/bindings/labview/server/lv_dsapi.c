#define BLA
/******************************************************************************
 *
 * File:	lv_dev_api.c
 *
 * Project:	TACO interface for LabView
 *
 * Description:	This file implements a thin layer between LabView and TACO
 *		in order to simplify executing commands on TACO objects.
 *		they can be called dynamically. To use this interface
 *		from LabView the user needs to use the call library interface
 *		function in the advanced pulldown menu. The TACO interface
 *		consists of the following calls :
 *
 *			lv_dev_putget(char *name, 
 *			              char *cmd,
 *			              void *in,
 *			              void *out,
 *		 	              long *error);
 *
 *			lv_dc_devget(char *name, 
 *			              char *cmd,
 *			              void *out,
 *		 	              long *error);
 *
 *			lv_dev_cmd_query(char *name,
 *			                 void *out,
 *			                 long *error);
 *
 *			lv_dc_cmd_query(char *name,
 *			                 void *out,
 *			                 long *error);
 *
 *			lv_dev_rpc_protocol(char *name,
 *			                    long protocol,
 *			                    long *errno);
 *
 *			lv_dev_rpc_timeout(char *name,
 *			                   long timeout,
 *			                   long *errno);
 *
 *			lv_dev_error_str(long errno);
 *
 *			lv_dev_free(char *name);
 *
 *		The input and output arguments should be adapted to the 
 *		appropriate command input and output binary types.
 *		Importing, querying and arguments types are handled
 *		automatically.
 *
 *		It also implements the LabView TACO device server interface
 *		
 *			lv_ds__init(char *server, char *name);
 *
 *		To activate the device server simply call lv_ds_init()
 *		from a thread. The LabView program replies to client
 *		requests by polling the input using the call
 *
 *			lv_ds__cmd_get();
 *
 *		If there is a client request then LabView must execute it
 *		and return the answer using the call 
 *
 *			lv_ds__cmd_put();
 *
 * Author(s):	Andy Gotz & Paolo Mangiagalli
 *
 * Original:	February 1999
 *
 * $Revision: 1.5 $
 *
 * $Date: 2008-11-07 17:01:06 $
 *
 * $Author: jkrueger1 $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2008/04/06 09:06:26  jkrueger1
 * Merge from branch JK
 *
 * Revision 1.3.4.1  2008/03/18 13:23:42  jkrueger1
 * make TACO 64 bit ready
 *
 * Revision 1.3  2006/11/21 16:38:19  jkrueger1
 * Add code from ESRF
 *
 * Revision 1.17  2005/06/29 08:39:39  goetz
 * going to add D_INT_FLOAT for the insertion device group
 *
 * Revision 1.16  2001/10/18  16:16:07  meyer
 * Added D_VARCHAR_ARRAY and DEV_OPAQUE_TYPE.
 *
 * Revision 1.15  2001/10/17  09:42:56  09:42:56  goetz (Andy Goetz)
 * *** empty log message ***
 * 
 * Revision 1.14  2001/08/17 11:37:21  goetz
 * added support for unsigned short types (same as signed short)
 *
 * Revision 1.13  2000/12/04 13:09:48  goetz
 * ported to Windows 95/98/NT based on TACO port of DSAPI 5.15 and ONCRPC
 *
 * Revision 1.11  20/0./2.  1.:6.:5.  1.:6.:5.  goetz (Andy Goetz)
 * added lv_db_getdevexp() call to interface - retrieve list of devices from
 * database which satisfy a user specified device name filter
 * 
 * Revision 1.10  2000/02/14 21:47:27  goetz
 * added type info to cmd_query call; added internal errors; pretty formatted
 *
 * Revision 1.9  2000/02/07 15:38:44  goetz
 * tested lv_dc_devget() on Linux and it works
 *
 * Revision 1.8  2000/02/07 13:50:28  goetz
 * added support for data collector (not tested), reorganised code to share
 * conversion routines
 *
 * Revision 1.7  2000/02/01 11:13:34  goetz
 * fixed some bugs and improved debugging messages
 *
 * Revision 1.6  2000/01/31 23:07:39  goetz
 * added LVIODouble and LVIOString commands, removed some bugs, added string arrays
 *
 * Revision 1.5  2000/01/29 07:54:10  goetz
 * added support for D_VAR_STRINGARR and IODouble + IOString (nty)
 *
 * Revision 1.4  1999/10/25 13:34:50  goetz
 * added lv_ds_cmd_get() and lv_ds_cmd_put() commands for device servers in G
 *
 * Revision 1.3  1999/10/19 14:22:06  goetz
 * passing double values out using DevReadValue commands works
 *
 * Revision 1.2  1999/10/18 19:10:57  goetz
 * lv_cmd_get() and lv_cmd_put() work without passing arguments to LabView
 *
 * Revision 1.1  1999/10/18 14:17:57  goetz
 * Initial revision
 *
 * Revision 1.1  1999/10/15 11:29:07  goetz
 * Initial revision
 *
 * Revision 1.2  1999/10/13 15:17:27  goetz
 * JMC's version for HP-UX
 *
 *
 * Copyleft 1999 by European Synchrotron Radiation Facility
 *                  Grenoble, France
 *
 ******************************************************************************/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#include <time.h>
#ifdef unix
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif /* unix */
#include <API.h>
#include <private/ApiP.h>
#include <Admin.h>
#include <BlcDsNumbers.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <DevErrors.h>
#include <maxe_xdr.h>
#include <LabView.h>
#ifdef _WINDOWS
#include <windows.h>
#endif /* _WINDOWS */

#ifdef hpux
#include <dl.h>
#endif /* hpux */
#if defined solaris || linux
#include <dlfcn.h>
#endif /* solaris || linux */
#include "extcode.h"

static long lv_dev_init();
static long lv_dev_search();
static long lv_dc_search();
static long lv_dev_import();
static long lv_dc_import();
static long lv_dev_cmd_args();
static long lv_dc_cmd_args();
static long lv_argin_convert();
static long lv_argout_prepare();
static long lv_argout_convert();
static long lv_startup();

#define LV_MAX_DEVICES 1000
#define LV_DEV_FREE 0
#define LV_DEV_IMPORTED 1
#define LV_DEV_QUERYED 2
#define LV_DC_IMPORTED 4
#define LV_DC_QUERYED 8

/* type definition for LV variable size data */

typedef struct {
  long dimSizes[1];
  char arg1[1];
} CharArray;

typedef CharArray **CharArrayHdl;

typedef struct {
  long dimSizes[1];
  short arg1[1];
} ShortArray;

typedef ShortArray **ShortArrayHdl;

typedef struct {
  long dimSizes[1];
  DevLong arg1[1];
} LongArray;

typedef LongArray **LongArrayHdl;

typedef struct {
  long dimSizes[1];
  DevULong arg1[1];
} ULongArray;

typedef ULongArray **ULongArrayHdl;

typedef struct {
  long dimSizes[1];
  unsigned char arg1[1];
} LVString;

typedef LVString **LVStringHdl;

typedef struct {
  long dimSizes[1];
  LVStringHdl arg1[1];
} LVStringArray;

typedef LVStringArray **LVStringArrayHdl;

typedef struct {
  long dimSizes[1];
  float arg1[1];
} FloatArray;

typedef FloatArray **FloatArrayHdl;

typedef struct {
  long dimSizes[1];
  double arg1[1];
} DblArray;

typedef DblArray **DblArrayHdl;

/* globals */

/* array of imported devices */

struct { char *name;
         long state;
         devserver ds;
         DevVarCmdArray dev_cmd_query;
#ifdef LV_DC
         datco *dc;
         DevVarCmdArray dc_cmd_query;
#endif /* DC */
} lv_device[LV_MAX_DEVICES];

#ifdef _WINDOWS
#define EXPORT  extern __declspec (dllexport)
#else
#define EXPORT
#endif /* _WINDOWS */

EXPORT long (*_dev_import)(char *,long,devserver*,DevLong*)=NULL;
EXPORT long (*_dev_cmd_query)(devserver,DevVarCmdArray*,DevLong*)=NULL;
EXPORT long (*_dev_putget)(devserver,long,void*,long,void*,long,DevLong*)=NULL;
EXPORT long (*_dev_xdrfree)(long,void*,DevLong*)=NULL;
EXPORT long (*_dev_free)(devserver,DevLong*)=NULL;
EXPORT long (*_dc_free)(dc_dev_free*,unsigned int,DevLong*)=NULL;
EXPORT long (*_dev_rpc_protocol)(devserver,long,DevLong*)=NULL;
EXPORT long (*_dev_rpc_timeout)(devserver,long,struct timeval*,DevLong*)=NULL;
EXPORT char* (*_dev_error_str)(long)=NULL;
#ifdef LV_DC
long (*_dc_import)(dc_dev_imp*,long,DevLong*)=NULL;
long (*_dc_dinfo)(char*,dc_devinf*,DevLong*)=NULL;
long (*_dc_devget)(datco*,long,void*,long,DevLong*)=NULL;
#endif /* DC */
EXPORT long (*_db_import)(DevLong*)=NULL;
EXPORT long (*_db_getresource)(char*,Db_resource,u_int,DevLong*)=NULL;
EXPORT long (*_db_getdevexp)(char*,char***,u_int*,DevLong*)=NULL;
EXPORT long (*_db_freedevexp)(char**)=NULL;
long (*_lv_ds__main)(char*,char*)=NULL;
long (*_lv_ds__cmd_get)()=NULL;
long (*_lv_ds__cmd_put)()=NULL;
long *_debug_flag=NULL;

/* initialise global pointers to NULL */

#ifdef hpux
shl_t taco_lib;
shl_t class_lib;
#endif /* hpux */
#if defined solaris || linux
void *taco_lib = NULL;
void *class_lib = NULL;
#endif /* solaris || linux  */

#ifdef LV_DEBUG_1
static long lv_debug = 1;
#else
static long lv_debug = 0;
#endif /* LV_DEBUG_1 */



/******************************************************************************
 *
 * Function:	lv_dev_init()
 *
 * Description:	Initialise all global values, load shared libraries and
 *		resolve the necessary dsapi symbols. Must be called once
 *		successfully in order for dev_putget to work.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_dev_init(DevLong *error)
{

#ifdef unix
	long status;	
	struct rlimit no_file;
/* 
 * set no. of open files per process to maximum, this will avoid the error
 * "too many files open" in LabView when working with lots of device servers
 * in TCP
 */
	getrlimit(RLIMIT_NOFILE, &no_file);
	no_file.rlim_cur = no_file.rlim_max;
	if(setrlimit(RLIMIT_NOFILE, &no_file) == -1) 
	{
		printf("lv_dev_init(): error setting no. of files to maximum\n");
	}
/*
 * first load all (TACO and C) shared libraries
 */
#ifdef hpux
	taco_lib = shl_load("/operation/dserver/lib/s700/libtaco.sl", BIND_IMMEDIATE  | DYNAMIC_PATH, 0);
	if (taco_lib == NULL) perror("lv_dev_init(): shl_load(libtaco.sl)");
	class_lib = shl_load("/operation/dserver/lib/s700/lv_dsclass.sl", BIND_IMMEDIATE  | DYNAMIC_PATH, 0);
	if (class_lib == NULL) perror("lv_dev_init(): shl_load(lv_dsclass.sl)");
#else
	taco_lib = dlopen("libtaco.so", RTLD_LAZY  | RTLD_GLOBAL);
	if (taco_lib == NULL) printf("lv_dev_init(): dlopen(libtaco.so) error %s\n",dlerror());
	class_lib = dlopen("lv_dsclass.so", RTLD_LAZY  );
	if (class_lib == NULL) printf("lv_dev_init(): dlopen(lv_dsclass.so) error %s\n",dlerror());
#endif /* hpux */
#ifdef LV_DEBUG
	if (lv_debug) printf("taco handle 0x%08x\n",taco_lib);
	if (lv_debug) printf("class handle 0x%08x\n",class_lib);
#endif /* LV_DEBUG */
	if (taco_lib == NULL) 
	{
		*error = DevErr_LVCannotLoadLibrary;
		return(DS_NOTOK);
	}
#endif /* unix */
#ifdef _WINDOWS
	rpc_nt_init();
#endif /* _WINDOWS */
/*
 * and resolve the dsapi symbols
 */
#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_import", TYPE_UNDEFINED, &_dev_import);
#endif /* hpux */
#if defined solaris || linux
	_dev_import = dlsym(taco_lib, "dev_import");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_import = dev_import;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dev_import handle 0x%08x\n",_dev_import);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_free", TYPE_UNDEFINED, &_dev_free);
#endif /* hpux */
#if defined solaris || linux
	_dev_free = dlsym(taco_lib, "dev_free");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_free = dev_free;
#endif /* _windows */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dev_free handle 0x%08x\n",_dev_free);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dc_free", TYPE_UNDEFINED, &_dc_free);
#endif /* hpux */
#if defined solaris || linux
	_dc_free = dlsym(taco_lib, "dc_free");
#endif /* solaris || linux */
#ifdef NO_WINDOWS
	_dc_free = dc_free;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dc_free handle 0x%08x\n",_dc_free);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_cmd_query", TYPE_UNDEFINED, &_dev_cmd_query);
#endif /* hpux */
#if defined solaris || linux
	_dev_cmd_query = dlsym(taco_lib, "dev_cmd_query");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_cmd_query = dev_cmd_query;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dev_cmd_query handle 0x%08x\n",_dev_cmd_query);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_putget", TYPE_UNDEFINED, &_dev_putget);
#endif /* hpux */
#if defined solaris || linux
	_dev_putget = dlsym(taco_lib, "dev_putget");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_putget = dev_putget;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dev_putget handle 0x%08x\n",_dev_putget);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_xdrfree", TYPE_UNDEFINED, &_dev_xdrfree);
#endif /* hpux */
#if defined solaris || linux
	_dev_xdrfree = dlsym(taco_lib, "dev_xdrfree");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_xdrfree = dev_xdrfree;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dev_xdrfree handle 0x%08x\n",_dev_xdrfree);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_rpc_protocol", TYPE_UNDEFINED, &_dev_rpc_protocol);
#endif /* hpux */
#if defined solaris || linux
	_dev_rpc_protocol = dlsym(taco_lib, "dev_rpc_protocol");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_rpc_protocol = dev_rpc_protocol;
#endif /* _WINDOWS */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_rpc_timeout", TYPE_UNDEFINED, &_dev_rpc_timeout);
#endif /* hpux */
#if defined solaris || linux
	_dev_rpc_timeout = dlsym(taco_lib, "dev_rpc_timeout");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_rpc_timeout = dev_rpc_timeout;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dev_rpc_timeout handle 0x%08x\n",_dev_rpc_timeout);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dev_error_str", TYPE_UNDEFINED, &_dev_error_str);
#endif /* hpux */
#if defined solaris || linux
	_dev_error_str = dlsym(taco_lib, "dev_error_str");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dev_error_str = dev_error_str;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dev_error_str handle 0x%08x\n",_dev_error_str);
#endif /* LV_DEBUG */

#ifdef LV_DC
#ifdef hpux
	status = shl_findsym(&taco_lib, "dc_import", TYPE_UNDEFINED, &_dc_import);
#endif /* hpux */
#if defined solaris || linux
	_dc_import = dlsym(taco_lib, "dc_import");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dc_import = dc_import;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dc_import handle 0x%08x\n",_dc_import);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dc_dinfo", TYPE_UNDEFINED, &_dc_dinfo);
#endif /* hpux */
#if defined solaris || linux
	_dc_dinfo = dlsym(taco_lib, "dc_dinfo");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dc_info = dc_info;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dc_dinfo handle 0x%08x\n",_dc_dinfo);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "dc_devget", TYPE_UNDEFINED, &_dc_devget);
#endif /* hpux */
#if defined solaris || linux
	_dc_devget = dlsym(taco_lib, "dc_devget");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_dc_devget = dc_devget;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_dc_devget handle 0x%08x\n",_dc_devget);
#endif /* LV_DEBUG */
#endif /* DC */

#ifdef hpux
	status = shl_findsym(&taco_lib, "db_import", TYPE_UNDEFINED, &_db_import);
#endif /* hpux */
#if defined solaris || linux
	_db_import = dlsym(taco_lib, "db_import");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_db_import = db_import;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_db_import handle 0x%08x\n",_db_import);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "db_getresource", TYPE_UNDEFINED, &_db_getresource);
#endif /* hpux */
#if defined solaris || linux
	_db_getresource = dlsym(taco_lib, "db_getresource");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_db_getresource = db_getresource;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_db_getresource handle 0x%08x\n",_db_getresource);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "db_getdevexp", TYPE_UNDEFINED, &_db_getdevexp);
#endif /* hpux */
#if defined solaris || linux
	_db_getdevexp = dlsym(taco_lib, "db_getdevexp");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_db_getdevexp = db_getdevexp;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_db_getdevexp handle 0x%08x\n",_db_getdevexp);
#endif /* LV_DEBUG */

#ifdef hpux
	status = shl_findsym(&taco_lib, "db_freedevexp", TYPE_UNDEFINED, &_db_freedevexp);
#endif /* hpux */
#if defined solaris || linux
	_db_freedevexp = dlsym(taco_lib, "db_freedevexp");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_db_freedevexp = db_freedevexp;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_db_freedevexp handle 0x%08x\n",_db_freedevexp);
#endif /* LV_DEBUG */


#ifdef hpux
	status = shl_findsym(&taco_lib, "debug_flag", TYPE_UNDEFINED, &_debug_flag);
#endif /* hpux */
#if defined solaris || linux
	_debug_flag = dlsym(taco_lib, "debug_flag");
#endif /* solaris || linux */
#ifdef _WINDOWS
	_debug_flag = &debug_flag;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
	if (lv_debug) printf("_debug_flag handle 0x%08x\n",_debug_flag);
#endif /* LV_DEBUG */
/*	if (_debug_flag != NULL) *_debug_flag = 0xffffffff;*/

#ifndef _WINDOWS
	if (class_lib != NULL)
	{
#ifdef hpux
		status = shl_findsym(&class_lib, "lv_ds__main", TYPE_UNDEFINED, &_lv_ds__main);
#endif /* hpux */
#if defined solaris || linux
		_lv_ds__main = dlsym(class_lib, "lv_ds__main");
#endif /* solaris || linux */
#ifdef NO_WINDOWS
		_lv_ds__main = lv_ds__main;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
		if (lv_debug) printf("_lv_ds__main handle 0x%08x\n",_lv_ds__main);
#endif /* LV_DEBUG */

#ifdef hpux
		status = shl_findsym(&class_lib, "lv_ds__cmd_get", TYPE_UNDEFINED, &_lv_ds__cmd_get);
#endif /* hpux */
#if defined solaris || linux
		_lv_ds__cmd_get = dlsym(class_lib, "lv_ds__cmd_get");
#endif /* solaris || linux */
#ifdef NO_WINDOWS
		_lv_ds__cmd_get = lv_ds__cmd_get;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
		if (lv_debug) printf("_lv_ds__cmd_get handle 0x%08x\n",_lv_ds__cmd_get);
#endif /* LV_DEBUG */

#ifdef hpux
		status = shl_findsym(&class_lib, "lv_ds__cmd_put", TYPE_UNDEFINED, &_lv_ds__cmd_put);
#endif /* hpux */
#if defined solaris || linux
		_lv_ds__cmd_put = dlsym(class_lib, "lv_ds__cmd_put");
#endif /* solaris || linux */
#ifdef NO_WINDOWS
		_lv_ds__cmd_put == lv_ds__cmd_put;
#endif /* _WINDOWS */
#ifdef LV_DEBUG
		if (lv_debug) printf("_lv_ds__cmd_put handle 0x%08x\n",_lv_ds__cmd_put);
#endif /* LV_DEBUG */
	}
#endif /* _WINDOWS */
#ifdef unix
	if ((_dev_import != NULL) && (_dev_cmd_query != NULL) && 
		(_dev_putget != NULL))
#endif /* unix */
	{
/*
 * import database - this is needed by calls like db_getdevexp()
 */
		return((*_db_import)(error));
	}
#ifdef unix
	else
	{
		*error = -1;
	    	return(DS_NOTOK);
	}
#endif /* unix */
}

/******************************************************************************
 *
 * Function:	lv_dev_import()
 *
 * Description:	Import device with this name.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_dev_import(char *name, DevLong *error)
{
	long i, status, dev_status=-1;

#ifdef unix
	if (_dev_import == NULL)
#endif /* unix */
	{
		if (lv_dev_init(error) != DS_OK) return(DS_NOTOK);
	}

        if ((i = lv_dev_search(name)) == -1)
        {
/*
 * find next free slot in device[] table
 */
		for (i=0; i< LV_MAX_DEVICES; i++)
		{
			if (lv_device[i].state == LV_DEV_FREE) break;
		}
/*
 * no more free slots ?
 */
		if (i >= LV_MAX_DEVICES) return(DS_NOTOK);
       } 
#ifdef LV_DEBUG
        if (lv_debug) printf("going to call dev_import(), name = %s\n",name);
#endif /* LV_DEBUG */

#ifdef _WINDOWS
	dev_status = dev_import(name, 0, &(lv_device[i].ds), error);
#else
        dev_status = (*_dev_import)(name, 0, &(lv_device[i].ds), error);
#endif /* _WINDOWS */

#ifdef LV_DEBUG
	if (lv_debug) printf("lv_dev_import(): dev_import() returned %d\n",dev_status);	
#endif /* LV_DEBUG */

	if (dev_status == DS_OK)
	{
		lv_device[i].state |= LV_DEV_IMPORTED;

		lv_device[i].name = malloc(strlen(name)+1);
		sprintf(lv_device[i].name, "%s", name);
	
		lv_device[i].dev_cmd_query.length = 0;
		lv_device[i].dev_cmd_query.sequence = NULL;
		status = (*_dev_cmd_query)(lv_device[i].ds, &(lv_device[i].dev_cmd_query), error);
#ifdef LV_DEBUG
		if (lv_debug) printf("lv_dev_import(): dev_cmd_query() returned %d\n",status);	
#endif /* LV_DEBUG */
		if (status == DS_OK)
		{
			lv_device[i].state |= LV_DEV_QUERYED;
		}
	}
	if (dev_status == DS_OK) return(i);

	return (DS_NOTOK);
}

#ifdef LV_DC
/******************************************************************************
 *
 * Function:	lv_dc_import()
 *
 * Description:	Import device from data collector with this name.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_dc_import(char *name, DevLong *error)
{
	long i, j, status, dc_status=-1;
	static dc_dev_imp dc_dev_import;
	static dc_devinf dc_dev_info;
	static db_resource res_cmd;
	static long cmd, cmd_team, cmd_class, cmd_number;
	static char res_device[256], res_name[16], *cmd_name;

        if (_dc_import == NULL)
        {
                if (lv_dev_init(error) != DS_OK) return(DS_NOTOK);
        }

	if ((i = lv_dev_search(name)) == -1)
	{
/*
 * find next free slot in device[] table
 */
		for (i=0; i< LV_MAX_DEVICES; i++)
		{
			if (lv_device[i].state == LV_DEV_FREE) break;
		}
/*
 * no more free slots ?
 */
		if (i >= LV_MAX_DEVICES) return(DS_NOTOK);
	}
/*
 * try to import data collector device
 */
	dc_dev_import.device_name = name;
	dc_dev_import.dc_access = 0;
	dc_dev_import.dc_ptr = NULL;
	dc_dev_import.dc_dev_error = error;
        dc_status = (*_dc_import)(&dc_dev_import, 1, error);
#ifdef LV_DEBUG
	if (lv_debug) printf("lv_dc_import(): dc_import() returned %d\n",dc_status);	
#endif /* LV_DEBUG */

	if (dc_status == DS_OK)
	{
		lv_device[i].state |= LV_DC_IMPORTED;
		lv_device[i].dc = dc_dev_import.dc_ptr;

		if (lv_device[i].name == NULL)
		{
			lv_device[i].name = malloc(strlen(name)+1);
			sprintf(lv_device[i].name, "%s", name);
		}
	
		lv_device[i].dc_cmd_query.length = 0;
		lv_device[i].dc_cmd_query.sequence = NULL;
		status = (*_dc_dinfo)(name, &dc_dev_info, error);
#ifdef LV_DEBUG
		if (lv_debug) printf("lv_dc_import(): dc_dinfo() returned %d\n",status);	
#endif /* LV_DEBUG */
		if (status == DS_OK)
		{
			lv_device[i].dc_cmd_query.length = dc_dev_info.devinf_nbcmd;
			lv_device[i].dc_cmd_query.sequence = (DevCmdInfo*)malloc(sizeof(DevCmdInfo)*dc_dev_info.devinf_nbcmd);
			for (j=0; j<dc_dev_info.devinf_nbcmd;j++)
			{
				lv_device[i].dc_cmd_query.sequence[j].cmd = dc_dev_info.devcmd[j].devinf_cmd;
				lv_device[i].dc_cmd_query.sequence[j].in_name = NULL;
				lv_device[i].dc_cmd_query.sequence[j].in_type = D_VOID_TYPE;
				lv_device[i].dc_cmd_query.sequence[j].out_name = NULL;
				lv_device[i].dc_cmd_query.sequence[j].out_type = dc_dev_info.devcmd[j].devinf_argout;
/*
 * retrieve dc command name from CMDS table in static database
 */
				cmd = dc_dev_info.devcmd[j].devinf_cmd;
				cmd_team = (cmd>>DS_TEAM_SHIFT)&DS_TEAM_MASK;
				cmd_class = (cmd>>DS_IDENT_SHIFT)&DS_IDENT_MASK;
				cmd_number = (cmd<<DS_IDENT_SHIFT)>>DS_IDENT_SHIFT;
				sprintf(res_device,"CMDS/%d/%d",cmd_team,cmd_class);
				sprintf(res_name,"%d",cmd_number);
				res_cmd.resource_name = res_name;
				res_cmd.resource_type = D_STRING_TYPE;
				res_cmd.resource_adr = &cmd_name;
				cmd_name = NULL;
#ifdef LV_DEBUG
                		if (lv_debug) printf("lv_dc_import(): search database for %s/%s\n",res_device, res_name);
#endif /* LV_DEBUG */

				status = (*_db_getresource)(res_device,&res_cmd,1,error);
				if (cmd_name != NULL)
				{
#ifdef LV_DEBUG
                			if (lv_debug) printf("lv_dc_import(): db_getresource() returned command name %s (status=%d)\n",
					              cmd_name,status);
#endif /* LV_DEBUG */
					sprintf(lv_device[i].dc_cmd_query.sequence[j].cmd_name,"%s",cmd_name);
					free(cmd_name);
				}
			}
			lv_device[i].state |= LV_DC_QUERYED;
		}
		return(i);
	}
	if (dc_status == DS_OK) return(i);

	return (DS_NOTOK);
}
#endif /* DC */

/******************************************************************************
 *
 * Function:	lv_dev_free()
 *
 * Description:	Free device with this name.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

long lv_dev_free(char *name, DevLong *error)
{
	long idev, status;
	u_int i;
#ifdef LV_DC
	static dc_dev_free dc_dev_free;
#endif /* DC */

	if (_dev_free == NULL)
	{
		if (lv_dev_init(error) != DS_OK) return(DS_NOTOK);
	}
/*
 * find device in device[] table
 */
	if ((idev = lv_dev_search(name)) == DS_NOTOK)
	{
/*
 * device not imported do nothing
 */
		return(DS_OK);
	}
	if (lv_device[idev].state == LV_DEV_FREE)
	{
/*
 * device already freed do nothing
 */
		return(DS_OK);
	}
	
	if ((lv_device[idev].state & LV_DEV_IMPORTED) != 0)
	{
        	status = (*_dev_free)(lv_device[idev].ds, error);
#ifdef LV_DEBUG
		if (lv_debug) printf("lv_dev_free(): dev_free() returned %d\n",status);	
#endif /* LV_DEBUG */

		if (status == DS_OK)
		{
			free(lv_device[idev].name);
			lv_device[idev].name = NULL;
		
		}
	}
/*
 * free dev_cmd_query() structure
 */
	if ((lv_device[idev].state & LV_DEV_QUERYED) != 0)
	{
		for (i=0; i< lv_device[idev].dev_cmd_query.length; i++)
		{
			free(lv_device[idev].dev_cmd_query.sequence[i].in_name);
			free(lv_device[idev].dev_cmd_query.sequence[i].out_name);
		}
		if (lv_device[idev].dev_cmd_query.sequence != NULL)
		{
			free(lv_device[idev].dev_cmd_query.sequence);
			lv_device[idev].dev_cmd_query.sequence = NULL;
			lv_device[idev].dev_cmd_query.length = 0;
		}

	}

#ifdef LV_DC
/*
 * free dc device
 */
	if ((lv_device[idev].state & LV_DC_IMPORTED) != 0)
	{
		dc_dev_free.dc_ptr = lv_device[idev].dc;
		dc_dev_free.dc_dev_error = error;
		status = (*_dc_free)(&dc_dev_free, 1, error);
		if (lv_device[idev].name != NULL) 
		{
			free(lv_device[idev].name);
			lv_device[idev].name = NULL;
		}
	}
/*
 * free dc_cmd_query() structure
 */
	if ((lv_device[idev].state & LV_DC_QUERYED) != 0)
	{
		for (i=0; i< lv_device[idev].dc_cmd_query.length; i++)
		{
			free(lv_device[idev].dc_cmd_query.sequence[i].in_name);
			free(lv_device[idev].dc_cmd_query.sequence[i].out_name);
		}
		if (lv_device[idev].dc_cmd_query.sequence != NULL)
		{
			free(lv_device[idev].dc_cmd_query.sequence);
			lv_device[idev].dc_cmd_query.sequence = NULL;
			lv_device[idev].dc_cmd_query.length = 0;
		}

	}
#endif /* DC */
	lv_device[idev].state = LV_DEV_FREE;

	return (DS_OK);
}

/******************************************************************************
 *
 * Function:	lv_dev_search()
 *
 * Description:	Search lv_device[] array for an imported device of this name
 *
 * Returns:	i=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_dev_search(char *name)
{
	long i, found=0;

	for (i=0; i<LV_MAX_DEVICES; i++)
	{
		if (lv_device[i].state >= LV_DEV_IMPORTED)
		{
			if (strcmp(name,lv_device[i].name) == 0)
			{
				found = 1;
				break;
			}
		}
	}

	if (found == 1)
	{
		return(i);
	}
	else
	{
#ifdef LV_DEBUG
		if (lv_debug) printf("lv_dev_search():device %s not yet imported\n", name);
#endif /* LV_DEBUG */
		return(DS_NOTOK);

	}
}

/******************************************************************************
 *
 * Function:	lv_dev_cmd_args()
 *
 * Description:	Search lv_device[].cmd_query for input and out argument 
		types for the specified command
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_dev_cmd_args(long id, char *cmd, long *icmd, long *argin_type, long *argout_type, DevLong *error)
{
	long status, found=0;
	u_int i;

	if ((lv_device[id].state & LV_DEV_QUERYED) == 0)
	{
		lv_device[id].dev_cmd_query.length = 0;
		lv_device[id].dev_cmd_query.sequence = NULL;
		status = (*_dev_cmd_query)(lv_device[id].ds, &(lv_device[id].dev_cmd_query), error);
		if (status == DS_OK)
		{
			lv_device[i].state |= LV_DEV_QUERYED;
		}
		else
		{
			return(DS_NOTOK);
		}
	}
	for (i=0; i<lv_device[id].dev_cmd_query.length; i++)
	{
		if (strcmp(cmd,lv_device[id].dev_cmd_query.sequence[i].cmd_name) == 0)
		{
			*icmd = lv_device[id].dev_cmd_query.sequence[i].cmd;
			*argin_type = lv_device[id].dev_cmd_query.sequence[i].in_type;
			*argout_type = lv_device[id].dev_cmd_query.sequence[i].out_type;
			found = 1;
			break;
		}
	}

	if (found == 1)
	{
		return(DS_OK);
	}
	else
	{
#ifdef LV_DEBUG
	        if (lv_debug) printf("lv_dev_cmd_args(): command %s not found\n",cmd);
#endif /* LV_DEBUG */
		return(DS_NOTOK);

	}
}

#ifdef LV_DC
/******************************************************************************
 *
 * Function:	lv_dc_cmd_args()
 *
 * Description:	Search lv_device[].dc_cmd_query for input and out argument 
		types for the specified command in the data collector
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_dc_cmd_args(long id, char *cmd, long *icmd, long *argout_type,DevLong *error)
{
	long i, status, found=0;

	if ((lv_device[id].state & LV_DC_QUERYED) == 0)
	{
		return(DS_NOTOK);
	}
	for (i=0; i<lv_device[id].dc_cmd_query.length; i++)
	{
		if (strcmp(cmd,lv_device[id].dc_cmd_query.sequence[i].cmd_name) == 0)
		{
			*icmd = lv_device[id].dc_cmd_query.sequence[i].cmd;
			*argout_type = lv_device[id].dc_cmd_query.sequence[i].out_type;
			found = 1;
			break;
		}
	}

	if (found == 1)
	{
		return(DS_OK);
	}
	else
	{
#ifdef LV_DEBUG
	        if (lv_debug) printf("lv_dc_cmd_args(): command %s not found\n",cmd);
#endif /* LV_DEBUG */
		return(DS_NOTOK);

	}
}
#endif /* DC */

/******************************************************************************
 *
 * Function:	lv_argin_convert()
 *
 * Description:	Convert argin data from Labview to TACO type
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_argin_convert(long argin_type, void *argin_lv, void **argin_taco, DevLong *error)
    
{	
  	unsigned long i;
  /* fixed size data */
  	static DevShort argin_short;
  	static DevUShort argin_ushort;
  	static DevLong argin_long;
  	static DevULong argin_ulong;
  	static DevFloat argin_float;
  	static DevDouble argin_double;
  	static DevLongReadPoint argin_lrp;
  	static DevFloatReadPoint argin_frp;
  	static DevStateFloatReadPoint argin_sfrp;
  	static DevDoubleReadPoint argin_drp;
  /* variable size data */
  	static DevString argin_string;
  	static LVStringHdl argin_string_hdl;
  	static DevVarStringArray argin_string_array={0,NULL};
  	static LVStringArrayHdl argin_string_array_hdl;
        static DevVarCharArray argin_chararray;
        static CharArrayHdl argin_chararray_hdl;
  	static DevVarShortArray argin_shortarray;
  	static ShortArrayHdl argin_shortarray_hdl;
  	static DevVarLongArray argin_longarray;
  	static DevVarULongArray argin_ulongarray;
  	static LongArrayHdl argin_longarray_hdl;
  	static ULongArrayHdl argin_ulongarray_hdl;
  	static DevVarFloatArray argin_floatarray;
  	static FloatArrayHdl argin_floatarray_hdl;
  	static DevVarDoubleArray argin_dblarray;
  	static DblArrayHdl argin_dblarray_hdl;
  	static DevVarLongReadPointArray argin_lrparray;
  	static DevVarFloatReadPointArray argin_frparray;
  	static DevVarStateFloatReadPointArray argin_sfrparray;
  /* motor types */
	static DevMulMove argin_mulmove;
   	DevMotorLong argin_motlong;
  	static DevMotorFloat argin_motfloat;
  	static DevIntFloat argin_intfloat;

#ifdef LV_DEBUG
	if (lv_debug) printf("lv_argin_convert(): argin_type %d\n",argin_type);
#endif /* LV_DEBUG */

  	switch (argin_type) {
    
  	case D_VOID_TYPE : 
    		*argin_taco = NULL;
#ifdef LV_DEBUG
    		if (lv_debug) printf("void argin\n");
#endif /* LV_DEBUG */
    		break;
    

  	case D_SHORT_TYPE : 
    		*argin_taco = &argin_short;
    		argin_short = *(DevShort*)argin_lv;
#ifdef LV_DEBUG
    		if (lv_debug) printf("short argin = %d\n", argin_short);
#endif /* LV_DEBUG */
    		break;
    
  	case D_USHORT_TYPE : 
    		*argin_taco = &argin_ushort;
    		argin_ushort = *(DevUShort*)argin_lv;
#ifdef LV_DEBUG
    		if (lv_debug) printf("unsigned short argin = %u\n", argin_ushort);
#endif /* LV_DEBUG */
    		break;
    
  	case D_LONG_TYPE : 
    		*argin_taco = &argin_long;
    		argin_long = *(DevLong*)argin_lv;
#ifdef LV_DEBUG
    		if (lv_debug) printf("long argin = %d\n", argin_long);
#endif /* LV_DEBUG */
    		break;
  
  	case D_ULONG_TYPE : 
    		*argin_taco = &argin_ulong;
    		argin_ulong = *(DevULong*)argin_lv;
#ifdef LV_DEBUG
    		if (lv_debug) printf("unsigned long argin = %ld\n", argin_ulong);
#endif /* LV_DEBUG */
    		break;
  
  	case D_FLOAT_TYPE : 
    		*argin_taco = &argin_float;
    		argin_float = *(DevFloat*)argin_lv;
#ifdef LV_DEBUG
    		if (lv_debug) printf("float argin = %f\n", argin_float);
#endif /* LV_DEBUG */
    		break;

  	case D_DOUBLE_TYPE : 
    		*argin_taco = &argin_double;
    		argin_double = *(DevDouble*)argin_lv;
#ifdef LV_DEBUG
    		if (lv_debug) printf("double argin = %f\n", argin_double);
#endif /* LV_DEBUG */
    		break;

  	case D_STRING_TYPE : 
    		*argin_taco = &argin_string;
    		argin_string_hdl = (LVStringHdl) argin_lv; 
    		argin_string =(char *)(*argin_string_hdl)->arg1; 
#ifdef LV_DEBUG
    		if (lv_debug) printf("string argin = %s\n", *argin_string);
#endif /* LV_DEBUG */
    		break;
    
  	case D_VAR_STRINGARR : 
    		*argin_taco = &argin_string_array;
    		argin_string_array_hdl = (LVStringArrayHdl) argin_lv; 
    		argin_string_array.length = (*argin_string_array_hdl)->dimSizes[0];
    		argin_string_array.sequence = (char**)realloc(argin_string_array.sequence,
                                  	argin_string_array.length*sizeof(char*));
    		for(i=0;i<argin_string_array.length;i++) {
       			argin_string_hdl = (*argin_string_array_hdl)->arg1[i];
       			argin_string_array.sequence[i] = (char*)malloc((*argin_string_hdl)->dimSizes[0]+1);
       			strncpy(argin_string_array.sequence[i],(*argin_string_hdl)->arg1,(*argin_string_hdl)->dimSizes[0]);
       			argin_string_array.sequence[i][(*argin_string_hdl)->dimSizes[0]] = 0;
#ifdef LV_DEBUG
       			if (lv_debug) printf("string array in [%d] = %s\n",i,argin_string_array.sequence[i]);
#endif /* LV_DEBUG */
    		}
    		break;
    
        case D_VAR_CHARARR :
        case D_OPAQUE_TYPE :
                *argin_taco = &argin_chararray;
                argin_chararray_hdl = (CharArrayHdl) argin_lv;
                argin_chararray.length = (*argin_chararray_hdl)->dimSizes[0];
                argin_chararray.sequence = (*argin_chararray_hdl)->arg1;
#ifdef LV_DEBUG
                if (lv_debug) printf("argin.length %d \n",argin_chararray.length
);
#endif /* LV_DEBUG */
                for(i=0;i<argin_chararray.length;i++) {
#ifdef LV_DEBUG
                        if (lv_debug) printf("argin.sequence[%d] = %d\n",i,argin_chararray.sequence[i]);
#endif /* LV_DEBUG */
                }
                break;

  	case D_VAR_SHORTARR :
  	case D_VAR_USHORTARR :
    		*argin_taco = &argin_shortarray;
    		argin_shortarray_hdl = (ShortArrayHdl) argin_lv; 
    		argin_shortarray.length = (*argin_shortarray_hdl)->dimSizes[0];
    		argin_shortarray.sequence = (*argin_shortarray_hdl)->arg1; 
#ifdef LV_DEBUG
    		if (lv_debug) printf("argin.length %d \n",argin_shortarray.length);
#endif /* LV_DEBUG */
    		for(i=0;i<argin_shortarray.length;i++) {
#ifdef LV_DEBUG
      			if (lv_debug) printf("argin.sequence[%d] = %d\n",i,argin_shortarray.sequence[i]);
#endif /* LV_DEBUG */
    		}
    		break;

  	case D_VAR_LONGARR :
    		*argin_taco = &argin_longarray;
    		argin_longarray_hdl = (LongArrayHdl) argin_lv; 
    		argin_longarray.length = (*argin_longarray_hdl)->dimSizes[0];
    		argin_longarray.sequence = (*argin_longarray_hdl)->arg1; 
#ifdef LV_DEBUG
    		if (lv_debug) printf("argin.length %d \n",argin_longarray.length);
#endif /* LV_DEBUG */
    		for(i=0;i<argin_longarray.length;i++) {
#ifdef LV_DEBUG
      			if (lv_debug) printf("argin.sequence[%d] = %d\n",i,argin_longarray.sequence[i]);
#endif /* LV_DEBUG */
    		}
    		break;

  	case D_VAR_ULONGARR :
    		*argin_taco = &argin_ulongarray;
    		argin_ulongarray_hdl = (ULongArrayHdl) argin_lv; 
    		argin_ulongarray.length = (*argin_ulongarray_hdl)->dimSizes[0];
    		argin_ulongarray.sequence = (*argin_ulongarray_hdl)->arg1; 
#ifdef LV_DEBUG
    		if (lv_debug) printf("argin.length %d \n",argin_ulongarray.length);
#endif /* LV_DEBUG */
    		for(i=0;i<argin_ulongarray.length;i++) {
#ifdef LV_DEBUG
      			if (lv_debug) printf("argin.sequence[%d] = %d\n",i,argin_ulongarray.sequence[i]);
#endif /* LV_DEBUG */
    		}
    		break;

  	case D_VAR_FLOATARR :
    		*argin_taco = &argin_floatarray;
    		argin_floatarray_hdl = (FloatArrayHdl) argin_lv; 
    		argin_floatarray.length = (*argin_floatarray_hdl)->dimSizes[0];
    		argin_floatarray.sequence = (*argin_floatarray_hdl)->arg1; 
#ifdef LV_DEBUG
    		if (lv_debug) printf("argin.length %d \n",argin_floatarray.length);
#endif /* LV_DEBUG */
    		for(i=0;i<argin_floatarray.length;i++) {
#ifdef LV_DEBUG
      			if (lv_debug) printf("argin.sequence[%d] = %f\n",i,argin_floatarray.sequence[i]);
#endif /* LV_DEBUG */
    		}
    		break;

  	case D_VAR_DOUBLEARR :
    		*argin_taco = &argin_dblarray;
    		argin_dblarray_hdl = (DblArrayHdl) argin_lv; 
    		argin_dblarray.length = (*argin_dblarray_hdl)->dimSizes[0];
    		argin_dblarray.sequence = (*argin_dblarray_hdl)->arg1; 
#ifdef LV_DEBUG
    		if (lv_debug) printf("argin.length %d \n",argin_dblarray.length);
#endif /* LV_DEBUG */
    		for(i=0;i<argin_dblarray.length;i++) {
#ifdef LV_DEBUG
      			if (lv_debug) printf("argin.sequence[%d] = %f\n",i,argin_dblarray.sequence[i]);
#endif /* LV_DEBUG */
    		}
    		break;

  	case D_MOTOR_LONG :
    		*argin_taco = &argin_motlong;
    		argin_longarray_hdl = (LongArrayHdl) argin_lv; 
    		argin_motlong.axisnum = (*argin_longarray_hdl)->arg1[0];
    		argin_motlong.value = (*argin_longarray_hdl)->arg1[1];
#ifdef LV_DEBUG
    		if (lv_debug) printf("axisnum = %d value = %d\n",argin_motlong.axisnum,
              		argin_motlong.value );
#endif /* LV_DEBUG */
    		break;

  	case D_MOTOR_FLOAT :
    		if (lv_debug) printf("Entering D_MOTOR_FLOAT\n");
    		*argin_taco = &argin_motfloat;
    		argin_floatarray_hdl = (FloatArrayHdl) argin_lv; 
    		argin_motfloat.axisnum = (DevLong)(*argin_floatarray_hdl)->arg1[0];
    		argin_motfloat.value = (*argin_floatarray_hdl)->arg1[1];
#ifdef LV_DEBUG
    		if (lv_debug) printf("axisnum = %d value = %f\n",argin_motfloat.axisnum,
              		argin_motfloat.value );
#endif /* LV_DEBUG */
    		break;

  	case D_MULMOVE_TYPE :
    		*argin_taco = &argin_floatarray;
    		argin_floatarray_hdl = (FloatArrayHdl) argin_lv; 
    		for(i=0;i<8;i++) {
      			argin_mulmove.action[i] = (DevLong)(*argin_floatarray_hdl)->arg1[i];
      			argin_mulmove.delay[i] = (DevLong)(*argin_floatarray_hdl)->arg1[8+i];
      			argin_mulmove.position[i] = (*argin_floatarray_hdl)->arg1[16+i];
#ifdef LV_DEBUG
      			if (lv_debug) printf("action[%d] = %d position[%d] = %f \n",
                		i,argin_mulmove.action[i],
                		i,argin_mulmove.delay[i]);
#endif /* LV_DEBUG */

  	case D_INT_FLOAT_TYPE :
    		*argin_taco = &argin_intfloat;
    		argin_floatarray_hdl = (FloatArrayHdl) argin_lv; 
    		argin_intfloat.state = (DevLong)(*argin_floatarray_hdl)->arg1[0];
    		argin_intfloat.value = (*argin_floatarray_hdl)->arg1[1];
#ifdef LV_DEBUG
    		if (lv_debug) printf("state = %d value = %f\n",argin_intfloat.state,
              		argin_intfloat.value );
#endif /* LV_DEBUG */
    		break;

    		}
    		break;
    
  	default : 
#ifdef LV_DEBUG
    		if (lv_debug) printf("lv_argin_convert(): argin type (%d) not recognised\n", argin_type);
#endif /* LV_DEBUG */
    		return(-1);
  	}
  	return(DS_OK);
}

/******************************************************************************
 *
 * Function:	lv_argout_prepare()
 *
 * Description:	Prepare TACO argout pointer to receive command output.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_argout_prepare(long argout_type, void **argout_taco, DevLong *error)
{	
  /* fixed size data */
  	static DevShort argout_short;
  	static DevUShort argout_ushort;
  	static DevLong argout_long;
  	static DevULong argout_ulong;
  	static DevFloat argout_float;
  	static DevDouble argout_double;
  	static DevLongReadPoint argout_lrp;
  	static DevFloatReadPoint argout_frp;
  	static DevStateFloatReadPoint argout_sfrp;
  	static DevDoubleReadPoint argout_drp;
  	static char *argout_char_ptr;
  /* variable size data */
  	static DevString argout_string;
  	static DevVarStringArray argout_string_array;
        static DevVarCharArray argout_chararray;
  	static DevVarShortArray argout_shortarray;
  	static DevVarLongArray argout_longarray;
  	static DevVarULongArray argout_ulongarray;
  	static DevVarFloatArray argout_floatarray;
  	static DevVarDoubleArray argout_dblarray;
  	static DevVarLongReadPointArray argout_lrparray;
  	static DevVarFloatReadPointArray argout_frparray;
  	static DevVarStateFloatReadPointArray argout_sfrparray;
  /* motor types */
  	static DevMulMove argout_mulmove;
  	static DevMotorLong argout_motlong;
  	static DevMotorFloat argout_motfloat;

  /*
   * prepare output arguments
   */
  	switch (argout_type) {
    
  	case D_VOID_TYPE :
    		*argout_taco = NULL;
    		break;
   
  	case D_SHORT_TYPE : 
    		*argout_taco = &argout_short;
    		break;

  	case D_USHORT_TYPE : 
    		*argout_taco = &argout_ushort;
    		break;
   
  	case D_LONG_TYPE : 
    		*argout_taco = &argout_long;
    		break;
    
  	case D_ULONG_TYPE : 
    		*argout_taco = &argout_ulong;
    		break;
    
  	case D_FLOAT_TYPE : 
    		*argout_taco = &argout_float;
    		break;

  	case D_DOUBLE_TYPE : 
    		*argout_taco = &argout_double;
    		break;

  	case D_LONG_READPOINT : 
    		*argout_taco = &argout_lrp;
    		break;

  	case D_FLOAT_READPOINT : 
    		*argout_taco = &argout_frp;
    		break;

  	case D_DOUBLE_READPOINT : 
    		*argout_taco = &argout_drp;
    		break;

  	case D_STATE_FLOAT_READPOINT : 
    		*argout_taco = &argout_sfrp;
    		break;

  	case D_STRING_TYPE : 
    		*argout_taco = &argout_string;
    		argout_string = NULL;
    		break;

  	case D_VAR_STRINGARR : 
    		*argout_taco = &argout_string_array;
    		argout_string_array.length = 0;
    		argout_string_array.sequence = NULL;
    		break;

        case D_VAR_CHARARR :
        case D_OPAQUE_TYPE :
                *argout_taco = &argout_chararray;
                argout_chararray.length = 0;
                argout_chararray.sequence = NULL;
                break;

  	case D_VAR_SHORTARR : 
  	case D_VAR_USHORTARR : 
    		*argout_taco = &argout_shortarray;
    		argout_shortarray.length = 0;
    		argout_shortarray.sequence = NULL;
    		break;
    
  	case D_VAR_LONGARR : 
  	case D_VAR_ULONGARR : 
    		*argout_taco = &argout_longarray;
    		argout_longarray.length = 0;
    		argout_longarray.sequence = NULL;
    		break;
    
  	case D_VAR_FLOATARR : 
    		*argout_taco = &argout_floatarray;
    		argout_floatarray.length = 0;
    		argout_floatarray.sequence = NULL;
    		break;
    
  	case D_VAR_DOUBLEARR : 
    		*argout_taco = &argout_dblarray;
    		argout_dblarray.length = 0;
    		argout_dblarray.sequence = NULL;
    		break;
    
  	case D_VAR_LRPARR : 
    		*argout_taco = &argout_lrparray;
    		argout_lrparray.length = 0;
    		argout_lrparray.sequence = NULL;
    		break;
    
  	case D_VAR_FRPARR : 
    		*argout_taco = &argout_frparray;
    		argout_frparray.length = 0;
    		argout_frparray.sequence = NULL;
    		break;
    
  	case D_VAR_SFRPARR : 
    		*argout_taco = &argout_sfrparray;
    		argout_sfrparray.length = 0;
    		argout_sfrparray.sequence = NULL;
    		break;
    
  	default : 
#ifdef LV_DEBUG
    		if (lv_debug) printf("lv_argout_prepare(): argout type (%d) not recognised\n", argout_type);
#endif /* LV_DEBUG */
    		return(-1);
  	}
 	 
  	return(DS_OK);
}

/******************************************************************************
 *
 * Function:	lv_argout_convert()
 *
 * Description:	Convert TACO argout type to corresponding Labview type. Free
 *		any memory allocated by TACO xdr routines.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_argout_convert(long argout_type, void *argout_taco, void *argout_lv, DevLong *error)
{	
  	static char *argout_char_ptr;
  	long string_len, status;
	unsigned int i;
  /* variable size data */
  	static LVStringHdl argout_string_hdl;
  	static LVStringArrayHdl argout_string_array_hdl;
	static CharArrayHdl argout_chararray_hdl;
  	static ShortArrayHdl argout_shortarray_hdl;
  	static short *argout_short_ptr;
  	static LongArrayHdl argout_longarray_hdl;
  	static ULongArrayHdl argout_ulongarray_hdl;
  	static DevLong *argout_long_ptr;
  	static FloatArrayHdl argout_floatarray_hdl;
  	static DevFloat *argout_float_ptr;
  	static DblArrayHdl argout_dblarray_hdl;
  	static DevDouble *argout_double_ptr;

  	status = 0;
  /*
   * return output argument to labview caller
   */
  	switch (argout_type) {
    
  	case D_VOID_TYPE : 
    		break;
    
  	case D_SHORT_TYPE : 
    		*(DevShort*)argout_lv = *(DevShort*)argout_taco;
    		break;
    
  	case D_USHORT_TYPE : 
    		*(DevUShort*)argout_lv = *(DevUShort*)argout_taco;
    		break;
    
  	case D_LONG_TYPE : 
    		*(DevLong*)argout_lv = *(DevLong*)argout_taco;
    		break;
    
  	case D_ULONG_TYPE : 
    		*(DevLong*)argout_lv = *(DevULong*)argout_taco;
    		break;
    
  	case D_FLOAT_TYPE : 
    		*(DevFloat*)argout_lv = *(DevFloat*)argout_taco;
    		break;
    
  	case D_DOUBLE_TYPE : 
    		*(DevDouble*)argout_lv = *(DevDouble*)argout_taco;
    		break;
    
  	case D_LONG_READPOINT : 
    		argout_longarray_hdl = (LongArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(iL, 1L, (UHandle*) &argout_longarray_hdl,2 )) != noErr )
      			return(status);
    		argout_long_ptr = (*argout_longarray_hdl)->arg1;
    		*(argout_long_ptr + 0) = ((DevLongReadPoint*)argout_taco)->set;
    		*(argout_long_ptr + 1) = ((DevLongReadPoint*)argout_taco)->read;
    		(*argout_longarray_hdl)->dimSizes[0]= 2;   
    		break;  

  	case D_FLOAT_READPOINT : 
    		argout_floatarray_hdl = (FloatArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fS, 1L, (UHandle*) &argout_floatarray_hdl,2 )) != noErr )
      			return(status);
    		argout_float_ptr = (*argout_floatarray_hdl)->arg1;
    		*(argout_float_ptr + 0) = ((DevFloatReadPoint*)argout_taco)->set;
    		*(argout_float_ptr + 1) = ((DevFloatReadPoint*)argout_taco)->read;
    		(*argout_floatarray_hdl)->dimSizes[0]= 2;   
    		break;  

  	case D_STATE_FLOAT_READPOINT : 
    		argout_floatarray_hdl = (FloatArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fS, 1L, (UHandle*) &argout_floatarray_hdl,3 )) != noErr )
      			return(status);
    		argout_float_ptr = (*argout_floatarray_hdl)->arg1;
 
    		*(argout_float_ptr + 0) = (float)((DevStateFloatReadPoint*)argout_taco)->state;
    		*(argout_float_ptr + 1) = ((DevStateFloatReadPoint*)argout_taco)->set;
    		*(argout_float_ptr + 2) = ((DevStateFloatReadPoint*)argout_taco)->read;
    		(*argout_floatarray_hdl)->dimSizes[0]= 3;   
    		break;  

  	case D_DOUBLE_READPOINT : 
    		argout_dblarray_hdl = (DblArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fD, 1L, (UHandle*) &argout_dblarray_hdl,2 )) != noErr )
      			return(status);
    		argout_double_ptr = (*argout_dblarray_hdl)->arg1;
    		*(argout_double_ptr + 0) = ((DevDoubleReadPoint*)argout_taco)->set;
    		*(argout_double_ptr + 1) = ((DevDoubleReadPoint*)argout_taco)->read;
    		(*argout_dblarray_hdl)->dimSizes[0]= 2;   
    		break;  

  	case D_STRING_TYPE :
    		argout_string_hdl = (LVStringHdl)argout_lv;
    		string_len=strlen( *(char**)argout_taco);
#ifdef LV_DEBUG
    		if (lv_debug) printf("string length %d\n",string_len);
#endif /* LV_DEBUG */
    		if ( ( status = NumericArrayResize(iB, 1L, (UHandle*) &argout_string_hdl,string_len+1 )) != noErr )
      			return(status);
    		strncat((*argout_string_hdl)->arg1, *(char**)argout_taco, string_len);
#ifdef LV_DEBUG
    		if (lv_debug) printf("string %s\n",*(char**)argout_taco);
#endif /* LV_DEBUG */
    		(*argout_string_hdl)->dimSizes[0]= string_len;   
    		(*_dev_xdrfree)(D_STRING_TYPE,argout_taco,error);
    		break;
    
  	case D_VAR_STRINGARR :
    		argout_string_array_hdl = (LVStringArrayHdl)argout_lv;
    		if ( (status=NumericArrayResize(iL, 1L, (UHandle*)&argout_string_array_hdl, ((DevVarStringArray*)argout_taco)->length)) != noErr)
      			return(status);

    		(*argout_string_array_hdl)->dimSizes[0] = ((DevVarStringArray*)argout_taco)->length;

    		for (i=0; i<(*argout_string_array_hdl)->dimSizes[0]; i++)
    		{
      			string_len=strlen(((DevVarStringArray*)argout_taco)->sequence[i]);
      			argout_string_hdl = (*argout_string_array_hdl)->arg1[i];
      			if((status=NumericArrayResize(iB, 1L, (UHandle*)&argout_string_hdl, strlen(((DevVarStringArray*)argout_taco)->sequence[i]))) != noErr)
        			return(status);
      			(*argout_string_hdl)->dimSizes[0] = strlen(((DevVarStringArray*)argout_taco)->sequence[i]);
      			strncpy((*argout_string_hdl)->arg1,((DevVarStringArray*)argout_taco)->sequence[i],string_len);
#ifdef LV_DEBUG
    			if (lv_debug) printf("string array out[%d] = %s\n",i,((DevVarStringArray*)argout_taco)->sequence[i]);
#endif /* LV_DEBUG */
    		}
    		(*_dev_xdrfree)(D_VAR_STRINGARR,argout_taco,error);
    		break;

        case D_VAR_CHARARR :
        case D_OPAQUE_TYPE :
                argout_chararray_hdl = (CharArrayHdl)argout_lv;
                if ( ( status = NumericArrayResize(uB, 1L, (UHandle*) &argout_chararray_hdl,((DevVarCharArray*)argout_taco)->length )) != noErr )
                        return(status);
                argout_char_ptr = (*argout_chararray_hdl)->arg1;

                for(i=0;i<((DevVarCharArray*)argout_taco)->length;i++) {
                        *(argout_char_ptr + i) = ((DevVarCharArray*)argout_taco)
->sequence[i];
#ifdef LV_DEBUG
                        if (lv_debug) printf("Char[%d] = %d\n",i,*(argout_char_ptr + i));
#endif /* LV_DEBUG */
                }
                (*argout_chararray_hdl)->dimSizes[0]= ((DevVarCharArray*)argout_taco)->length;
                        if ( argout_type == D_VAR_CHARARR )
                        {
                        (*_dev_xdrfree)(D_VAR_CHARARR,argout_taco,error);
                        }
                        else
                        {
                                (*_dev_xdrfree)(D_OPAQUE_TYPE,argout_taco,error)
;
                        }
                break;

  	case D_VAR_SHORTARR : 
  	case D_VAR_USHORTARR : 
    		argout_shortarray_hdl = (ShortArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(iW, 1L, (UHandle*) &argout_shortarray_hdl,((DevVarShortArray*)argout_taco)->length )) != noErr )
      			return(status);
    		argout_short_ptr = (*argout_shortarray_hdl)->arg1;
 
    		for(i=0;i<((DevVarShortArray*)argout_taco)->length;i++) {
      			*(argout_short_ptr + i) = ((DevVarShortArray*)argout_taco)->sequence[i];
#ifdef LV_DEBUG
      			if (lv_debug) printf("Short[%d] = %d\n",i,*(argout_short_ptr + i));
#endif /* LV_DEBUG */
    		}
    		(*argout_shortarray_hdl)->dimSizes[0]= ((DevVarShortArray*)argout_taco)->length;   
    		(*_dev_xdrfree)(D_VAR_SHORTARR,argout_taco,error);
    		break;  
    
  	case D_VAR_LONGARR : 
    		argout_longarray_hdl = (LongArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(iL, 1L, (UHandle*) &argout_longarray_hdl,((DevVarLongArray*)argout_taco)->length )) != noErr )
      			return(status);
    		argout_long_ptr = (*argout_longarray_hdl)->arg1;
 
    		for(i=0;i<((DevVarLongArray*)argout_taco)->length;i++) {
      			*(argout_long_ptr + i) = ((DevVarLongArray*)argout_taco)->sequence[i];
#ifdef LV_DEBUG
      			if (lv_debug) printf("Long[%d] = %d\n",i,*(argout_long_ptr + i));
#endif /* LV_DEBUG */
    		} 
    		(*argout_longarray_hdl)->dimSizes[0]= ((DevVarLongArray*)argout_taco)->length;   
    		(*_dev_xdrfree)(D_VAR_LONGARR,argout_taco,error);
    		break;  

  	case D_VAR_FLOATARR : 
    		argout_floatarray_hdl = (FloatArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fS, 1L, (UHandle*) &argout_floatarray_hdl,((DevVarFloatArray*)argout_taco)->length )) != noErr )
      			return(status);
    		argout_float_ptr = (*argout_floatarray_hdl)->arg1;
 
    		for(i=0;i<((DevVarFloatArray*)argout_taco)->length;i++) {
      			*(argout_float_ptr + i) = ((DevVarFloatArray*)argout_taco)->sequence[i];
#ifdef LV_DEBUG
      			if (lv_debug) printf("Float[%d] = %f\n",i,*(argout_float_ptr + i)); 
#endif /* LV_DEBUG */
    		}
    		(*argout_floatarray_hdl)->dimSizes[0]= ((DevVarFloatArray*)argout_taco)->length;   
    		(*_dev_xdrfree)(D_VAR_FLOATARR,argout_taco,error);
    		break;  

  	case D_VAR_DOUBLEARR : 
    		argout_dblarray_hdl = (DblArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fD, 1L, (UHandle*) &argout_dblarray_hdl,((DevVarDoubleArray*)argout_taco)->length )) != noErr )
      			return(status);
    		argout_double_ptr = (*argout_dblarray_hdl)->arg1;
 
    		for(i=0;i<((DevVarDoubleArray*)argout_taco)->length;i++) {
      			*(argout_double_ptr + i) = ((DevVarDoubleArray*)argout_taco)->sequence[i];
#ifdef LV_DEBUG
      			if (lv_debug) printf("Double_out[%d] = %f\n",i,*(argout_double_ptr + i));
#endif /* LV_DEBUG */
    		}
    		(*argout_dblarray_hdl)->dimSizes[0]= ((DevVarDoubleArray*)argout_taco)->length;   
    		(*_dev_xdrfree)(D_VAR_DOUBLEARR,argout_taco,error);
    		break;  

  	case D_VAR_LRPARR : 
    		argout_longarray_hdl = (LongArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(iL, 1L, (UHandle*) &argout_longarray_hdl,((DevVarLongReadPointArray*)argout_taco)->length*2 )) != noErr )
      			return(status);
    		argout_long_ptr = (*argout_longarray_hdl)->arg1;
 
    		for(i=0;i<((DevVarLongReadPointArray*)argout_taco)->length;i++) {
      			*(argout_long_ptr + i*2) = ((DevVarLongReadPointArray*)argout_taco)->sequence[i].set;
      			*(argout_long_ptr + i*2 + 1) = ((DevVarLongReadPointArray*)argout_taco)->sequence[i].read;
#ifdef LV_DEBUG
      			if (lv_debug) printf("set[%d] = %d read[%d] = %d",i,*(argout_long_ptr + i*2),
                		i,*(argout_long_ptr + i*2 + 1)); 
#endif /* LV_DEBUG */
    		}
    		(*argout_longarray_hdl)->dimSizes[0]= ((DevVarLongReadPointArray*)argout_taco)->length*2;
    		(*_dev_xdrfree)(D_VAR_LRPARR,argout_taco,error);
    		break;

  	case D_VAR_FRPARR : 
    		argout_floatarray_hdl = (FloatArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fS, 1L, (UHandle*) &argout_floatarray_hdl,((DevVarFloatReadPointArray*)argout_taco)->length*2 )) != noErr )
      			return(status);
    		argout_float_ptr = (*argout_floatarray_hdl)->arg1;
 
    		for(i=0;i<((DevVarFloatReadPointArray*)argout_taco)->length;i++) {
      			*(argout_float_ptr + i*2) = ((DevVarFloatReadPointArray*)argout_taco)->sequence[i].set;
      			*(argout_float_ptr + i*2 + 1) = ((DevVarFloatReadPointArray*)argout_taco)->sequence[i].read;
#ifdef LV_DEBUG
      			if (lv_debug) printf("set[%d] = %f read[%d] = %f",i,*(argout_float_ptr + i*2),
                		i,*(argout_float_ptr + i*2 + 1)); 
#endif /* LV_DEBUG */
    		}
    		(*argout_floatarray_hdl)->dimSizes[0]= ((DevVarFloatReadPointArray*)argout_taco)->length*2;
    		(*_dev_xdrfree)(D_VAR_FRPARR,argout_taco,error);
    		break;  
  
  	case D_VAR_SFRPARR : 
    		argout_floatarray_hdl = (FloatArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fS, 1L, (UHandle*) &argout_floatarray_hdl,((DevVarStateFloatReadPointArray*)argout_taco)->length*3 )) != noErr )
      			return(status);
    		argout_float_ptr = (*argout_floatarray_hdl)->arg1;
 
    		for(i=0;i<((DevVarStateFloatReadPointArray*)argout_taco)->length;i++) {
      			*(argout_float_ptr + i*3) = ((DevVarStateFloatReadPointArray*)argout_taco)->sequence[i].state;
      			*(argout_float_ptr + i*3 + 1) = ((DevVarStateFloatReadPointArray*)argout_taco)->sequence[i].set;
      			*(argout_float_ptr + i*3 + 2) = ((DevVarStateFloatReadPointArray*)argout_taco)->sequence[i].read;
#ifdef LV_DEBUG
      			if (lv_debug) printf("state[%d] = %f set[%d] = %f read[%d] = %f",
                		i,*(argout_float_ptr + i*3),
                		i,*(argout_float_ptr + i*3 + 1),
                		i,*(argout_float_ptr + i*3 + 2)); 
#endif /* LV_DEBUG */
    		}
    		(*argout_floatarray_hdl)->dimSizes[0]= ((DevVarStateFloatReadPointArray*)argout_taco)->length*3;   
    		(*_dev_xdrfree)(D_VAR_SFRPARR,argout_taco,error);
    		break;  

  	case D_INT_FLOAT_TYPE : 
    		argout_floatarray_hdl = (FloatArrayHdl)argout_lv;
    		if ( ( status = NumericArrayResize(fS, 1L, (UHandle*) &argout_floatarray_hdl,2 )) != noErr )
      			return(status);
    		argout_float_ptr = (*argout_floatarray_hdl)->arg1;
    		*(argout_float_ptr + 0) = ((DevIntFloat*)argout_taco)->state;
    		*(argout_float_ptr + 1) = ((DevIntFloat*)argout_taco)->value;
    		(*argout_floatarray_hdl)->dimSizes[0]= 2;   
    		break;  

  	}
  
  	return(status);
}
/******************************************************************************
 *
 * Function:	lv_dev_putget()
 *
 * Description:	Execute command on device of given name. If not imported then
 *		import it.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_dev_putget(char *name, char *cmd, void *argin_lv, void *argout_lv, DevLong *error)
    
{	
	unsigned int i;
  	long idevice, status, icmd, argin_type, argout_type;
  	static void *argin_taco, *argout_taco;
  	static DevString argin_string;
  	static DevVarStringArray argin_string_array={0,NULL};

  /*
   * is device already imported ?
   */

  	idevice = lv_dev_search(name);
  /*
   * if not then import it
   */
  	if (idevice == DS_NOTOK)
  	{
      		if ((idevice=lv_dev_import(name,error)) == DS_NOTOK) 
      		{
	 		return (-1);
      		}
  	}
	else
	{
		if ((lv_device[idevice].state & LV_DEV_IMPORTED) == 0)
		{
      			if ((idevice=lv_dev_import(name,error)) == DS_NOTOK) 
      			{
	 			return (-1);
      			}
		}
	}
  /*
   * get command value and input and output types
   */
  	status = lv_dev_cmd_args(idevice,cmd,&icmd,&argin_type,&argout_type,error);
  
  	if (status == DS_NOTOK)
    	{
#ifdef LV_DEBUG
      		if (lv_debug) printf("lv_dev_putget(): command not recognised\n");
#endif /* LV_DEBUG */
		*error = -2;
      		return(DS_NOTOK);
    	}
  
#ifdef LV_DEBUG
  	if (lv_debug) printf("lv_dev_putget(%s,%s,argin=%d,argout=%d)\n", name,cmd,argin_type,argout_type);
#endif /* LV_DEBUG */
  /*
   * prepare input arguments
   */
  	if (lv_argin_convert(argin_type, argin_lv, &argin_taco, error) != DS_OK) 
	{
		printf("lv_argin_convert() %d\n", error);
		return(DS_NOTOK);
	}
  
  /*
   * prepare output arguments
   */
  	if (lv_argout_prepare(argout_type, &argout_taco, error) != DS_OK) 
	{
		printf("lv_argout_prepare() %d\n", error);
		return(DS_NOTOK);
	}
  
  	status = (*_dev_putget)(lv_device[idevice].ds,icmd,argin_taco,argin_type,
			      argout_taco,argout_type,error);

  	if (argin_type == D_VAR_STRINGARR)
  	{
    		for(i=0;i<((DevVarStringArray*)argin_taco)->length;i++) {
       			free(((DevVarStringArray*)argin_taco)->sequence[i]);
#ifdef LV_DEBUG
       			if (lv_debug) printf("free string array in\n");
#endif /* LV_DEBUG */
    		}
  	}

  	if(status != DS_OK)
  	{
#ifdef LV_DEBUG
    		if (lv_debug) printf("lv_dev_putget(): dev_putget error=%d\n",*error);
#endif /* LV_DEBUG */
    return(DS_NOTOK);
  	}
  /*
   * return output argument to labview caller
   */
  	if (lv_argout_convert(argout_type, argout_taco, argout_lv, error) != DS_OK) return(DS_NOTOK);

  	return(DS_OK);
}

#ifdef LV_DC
/******************************************************************************
 *
 * Function:	lv_dc_devget()
 *
 * Description:	Retrieve command result from data collector cache for device 
 *		of given name. If not dc imported then import it.
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_dc_devget(char *name, char *cmd, void *argout_lv, DevLong *error)
{	
  	static long i, idevice, status, icmd, argin_type, argout_type;
	static void *argout_taco;
/*
 * is device already imported ?
 */
 
  	idevice = lv_dev_search(name);
/*
 * if not then import it
 */
  	if (idevice == DS_NOTOK)
  	{
      		if ((idevice=lv_dc_import(name,error)) == DS_NOTOK)
      		{
         		return (DS_NOTOK);
      		}
  	}
	else
	{
		if ((lv_device[idevice].state & LV_DC_IMPORTED) == 0)
		{
      			if ((idevice=lv_dc_import(name,error)) == DS_NOTOK)
      			{
         			return (DS_NOTOK);
      			}
		}
	}
/*
 * get command value and input and output types
 */
  	status = lv_dc_cmd_args(idevice,cmd,&icmd,&argout_type,error);
 
  	if (status == DS_NOTOK)
    	{
#ifdef LV_DEBUG
      		if (lv_debug) printf("lv_dc_devget(): command not recognised\n");
#endif /* LV_DEBUG */
		*error = -2;
      		return(DS_NOTOK);
    	}
 
#ifdef LV_DEBUG
  	if (lv_debug) printf("lv_dc_devget(%s\t,%s )\n", name,cmd);
#endif /* LV_DEBUG */
/*
 * prepare output arguments
 */
	lv_argout_prepare(argout_type, &argout_taco, error);

/*
 * now call dc_devget()
 */
	status = (*_dc_devget)(lv_device[idevice].dc, icmd, argout_taco, argout_type, error);
	if (status != DS_OK)
	{
		return (DS_NOTOK);
	}
/*
 * return output to LabView
 */
	lv_argout_convert(argout_type, argout_taco, argout_lv, error);

	return(DS_OK);
}
#endif /* DC */


/******************************************************************************
 *
 * Function:	lv_in_out_to_string()
 *
 * Description:	Convert input and output types to string (used by lv_cmd_query())
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

static long lv_in_out_to_string(char *cmd_in_out, long in_type, long out_type)
    
{	

   	switch (in_type) 
   	{
      	case D_VOID_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(void");
         	break;
      	case D_SHORT_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(short");
         	break;
      	case D_USHORT_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(ushort");
         	break;
      	case D_LONG_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(long");
         	break;
      	case D_FLOAT_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(float");
         	break;
      	case D_DOUBLE_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(double");
         	break;
      	case D_STRING_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(string");
         	break;
      	case D_VAR_STRINGARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(string array");
         	break;
        case D_VAR_CHARARR :
                sprintf(cmd_in_out+strlen(cmd_in_out),"(char array");
                break;
        case D_OPAQUE_TYPE :
                sprintf(cmd_in_out+strlen(cmd_in_out),"(opaque");
                break;
      	case D_VAR_SHORTARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(short array");
         	break;
      	case D_VAR_USHORTARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(ushort array");
         	break;
      	case D_VAR_LONGARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(long array");
         	break;
      	case D_VAR_ULONGARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(ulong array");
         	break;
      	case D_VAR_FLOATARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(float array");
         	break;
      	case D_VAR_DOUBLEARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(double array");
         	break;
      	case D_MOTOR_LONG : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(motor long");
         	break;
      	case D_MOTOR_FLOAT : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(motor float");
         	break;
      	case D_MULMOVE_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(multiple move");
         	break;
      	default : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),"(UNKNOWN");
   	}

   	switch (out_type) 
   	{
      	case D_VOID_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",void)");
         	break;
      	case D_SHORT_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",short)");
         	break;
      	case D_USHORT_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",ushort)");
         	break;
      	case D_LONG_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",long)");
         	break;
      	case D_FLOAT_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",float)");
         	break;
      	case D_DOUBLE_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",double)");
         	break;
      	case D_LONG_READPOINT : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",long readpoint)");
         	break;
      	case D_FLOAT_READPOINT : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",float readpoint)");
         	break;
      	case D_STATE_FLOAT_READPOINT : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",state float readpoint)");
         	break;
      	case D_DOUBLE_READPOINT : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",double readpoint)");
         	break;
      	case D_STRING_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",string)");
         	break;
      	case D_VAR_STRINGARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",string array)");
         	break;
      	case D_VAR_CHARARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",char array)");
         	break;
      	case D_OPAQUE_TYPE : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",opaque)");
         	break;
      	case D_VAR_SHORTARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",short array)");
         	break;
      	case D_VAR_USHORTARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",ushort array)");
         	break;
      	case D_VAR_LONGARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",long array)");
         	break;
      	case D_VAR_ULONGARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",ulong array)");
         	break;
      	case D_VAR_FLOATARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",float array)");
         	break;
      	case D_VAR_DOUBLEARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",double array)");
         	break;
      	case D_VAR_LRPARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",long readpoint array)");
         	break;
      	case D_VAR_FRPARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",float readpoint array)");
        	break;
      	case D_VAR_SFRPARR : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",state floatreadpoint array)");
         	break;
      	default : 
         	sprintf(cmd_in_out+strlen(cmd_in_out),",UNKNOWN)");
   	}

   	return(DS_OK);
}
/******************************************************************************
 *
 * Function:	lv_dev_cmd_query()
 *
 * Description:	Return list of commands and their types implemented for this 
 *		device server
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_dev_cmd_query(char *name, void *argout_lv, DevLong *error)
    
{	
  	long i, idevice, status, string_len;
  	LVStringHdl argout_string_hdl;
  	LVStringArrayHdl argout_string_array_hdl;
  	static char cmd_in_out[256];
  /*
   * is device already imported ?
   */

  	idevice = lv_dev_search(name);
  /*
   * if not then import it
   */
  	if (idevice == DS_NOTOK)
  	{
      		if ((idevice=lv_dev_import(name,error)) == DS_NOTOK) 
      		{
	 		return (-1);
      		}
  	}
	else
	{
		if ((lv_device[idevice].state & LV_DEV_IMPORTED) == 0)
		{
      			if ((idevice=lv_dev_import(name,error)) == DS_NOTOK) 
      			{
	 			return (-1);
      			}
		}
	}

    	argout_string_array_hdl = (LVStringArrayHdl)argout_lv;
    	if ( (status=NumericArrayResize(iL, 1L, (UHandle*)&argout_string_array_hdl,  lv_device[idevice].dev_cmd_query.length)) != noErr)
      		return(status);

    	(*argout_string_array_hdl)->dimSizes[0] =  lv_device[idevice].dev_cmd_query.length;

    	for (i=0; i<(*argout_string_array_hdl)->dimSizes[0]; i++)
    	{
      		sprintf(cmd_in_out,"%s ",lv_device[idevice].dev_cmd_query.sequence[i].cmd_name);
      		lv_in_out_to_string(cmd_in_out, lv_device[idevice].dev_cmd_query.sequence[i].in_type,
                          	lv_device[idevice].dev_cmd_query.sequence[i].out_type);
      		string_len=strlen (cmd_in_out);
      		argout_string_hdl = (*argout_string_array_hdl)->arg1[i];
      		if((status=NumericArrayResize(iB, 1L, (UHandle*)&argout_string_hdl, string_len)) != noErr)
        		return(status);
      		(*argout_string_hdl)->dimSizes[0] = string_len;
      		strncpy((*argout_string_hdl)->arg1, cmd_in_out, string_len);
    	}

  	return(DS_OK);
}

#ifdef LV_DC
/******************************************************************************
 *
 * Function:	lv_dc_cmd_query()
 *
 * Description:	Return list of commands registered for this device in the data 
 *		collector for this device 
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_dc_cmd_query(char *name, void *argout_lv, DevLong *error)
{	
  	long i, idevice, status, icmd, argin_type, argout_type, string_len;
  	LVStringHdl argout_string_hdl;
  	LVStringArrayHdl argout_string_array_hdl;
  	char cmd_in_out[256];
  /*
   * is device already imported ?
   */

  	idevice = lv_dev_search(name);
  /*
   * if not then import it
   */
  	if (idevice == DS_NOTOK)
  	{
      		if ((idevice=lv_dc_import(name,error)) == DS_NOTOK) 
      		{
	 		return (-1);
      		}
  	}
	else
	{
  		if ((lv_device[idevice].state & LV_DC_IMPORTED) == 0) 
  		{
      			if ((idevice=lv_dc_import(name,error)) == DS_NOTOK) 
      			{
	 			return (-1);
      			}
  		}
	}

    	argout_string_array_hdl = (LVStringArrayHdl)argout_lv;
    	if ( (status=NumericArrayResize(iL, 1L, (UHandle*)&argout_string_array_hdl,  lv_device[idevice].dc_cmd_query.length)) != noErr)
      		return(status);

    	(*argout_string_array_hdl)->dimSizes[0] =  lv_device[idevice].dc_cmd_query.length;

    	for (i=0; i<(*argout_string_array_hdl)->dimSizes[0]; i++)
    	{
      		sprintf(cmd_in_out,"%s ",lv_device[idevice].dc_cmd_query.sequence[i].cmd_name);
      		lv_in_out_to_string(cmd_in_out, lv_device[idevice].dc_cmd_query.sequence[i].in_type,
                          	lv_device[idevice].dc_cmd_query.sequence[i].out_type);
      		string_len=strlen (cmd_in_out);
      		argout_string_hdl = (*argout_string_array_hdl)->arg1[i];
      		if((status=NumericArrayResize(iB, 1L, (UHandle*)&argout_string_hdl, string_len)) != noErr)
        		return(status);
      		(*argout_string_hdl)->dimSizes[0] = string_len;
      		strncpy((*argout_string_hdl)->arg1,cmd_in_out,string_len);
    	}

  	return(DS_OK);
}
#endif /* DC */
/******************************************************************************
 *
 * Function:	lv_db_getdevexp()
 *
 * Description:	Return list of exported devices in the database which satisfy 
 *		the filter
 *
 * Returns:	DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_db_getdevexp(char *filter, void *argout_lv, DevLong *error)
{	
  	long i, status, string_len;
	u_int	n_devices; 
  	LVStringHdl argout_string_hdl;
  	LVStringArrayHdl argout_string_array_hdl;
  	char **devexp_list=NULL;

	if (_db_getdevexp == NULL)
	{
		if (lv_dev_init(error) != DS_OK) return(DS_NOTOK);
	}
	if (devexp_list != NULL)
	{
		(*_db_freedevexp)(devexp_list);
	}
	status = (*_db_getdevexp)(filter, &devexp_list, &n_devices, error);

	if (status != DS_OK && n_devices > 0) return(DS_NOTOK);

    	argout_string_array_hdl = (LVStringArrayHdl)argout_lv;
    	if ( (status=NumericArrayResize(iL, 1L, (UHandle*)&argout_string_array_hdl,  n_devices)) != noErr)
      		return(status);

    	(*argout_string_array_hdl)->dimSizes[0] =  n_devices;

    	for (i=0; i<(*argout_string_array_hdl)->dimSizes[0]; i++)
    	{
      		string_len=strlen (devexp_list[i]);
      		argout_string_hdl = (*argout_string_array_hdl)->arg1[i];
      		if((status=NumericArrayResize(iB, 1L, (UHandle*)&argout_string_hdl, string_len)) != noErr)
        		return(status);
      		(*argout_string_hdl)->dimSizes[0] = string_len;
      		strncpy((*argout_string_hdl)->arg1,devexp_list[i],string_len);
    	}

  	return(DS_OK);
}
/******************************************************************************
 *
 * Function:    lv_dev_protocol()
 *
 * Description: Set the protocol of a device to TCP or UDP
 *
 * Returns:     DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_dev_protocol(char *name, char *protocol, DevLong *error)
{
  	long idevice, status;
/*
 * check input arguments
 */
  	if (name == NULL || protocol == NULL) 
  	{
#ifdef LV_DEBUG
     		if (lv_debug) printf("lv_dev_protocol(): error in input parameter !\n");
#endif /* LV_DEBUG */ 
     		return (DS_NOTOK);
  	}
  /*
   * is device already imported ?
   */
  	idevice = lv_dev_search(name);
  /*
   * if not then import it
   */
  	if (idevice == DS_NOTOK)
  	{
      		if ((idevice=lv_dev_import(name,error)) == DS_NOTOK) 
      		{
	 		return (-1);
      		}
  	}
	else
	{
  		if ((lv_device[idevice].state & LV_DEV_IMPORTED) == 0) 
  		{
      			if ((idevice=lv_dev_import(name,error)) == DS_NOTOK) 
      			{
	 			return (-1);
      			}
  		}
	}

  	status = 0;
  	if (strcmp(protocol,"tcp") == 0) 
  	{
      		if (_dev_rpc_protocol != NULL) 
      		{
         		status = (*_dev_rpc_protocol)(lv_device[idevice].ds,D_TCP,error);
      		}
  	}
  	else
  	{
     		if (_dev_rpc_protocol != NULL)
     		{
        		status = (*_dev_rpc_protocol)(lv_device[idevice].ds,D_UDP,error);
     		} 
  	}
  	return (status);
}

/******************************************************************************
 *
 * Function:    lv_dev_timeout()
 *
 * Description: Set the timeout (in milliseconds) of a device 
 *
 * Returns:     DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_dev_timeout(char *name, long timeout, DevLong *error)

{
  	long idevice, status;
  	struct timeval tout;
/*
 * check input arguments
 */
  	if (name == NULL || timeout == 0) 
  	{
#ifdef LV_DEBUG
     		if (lv_debug) printf("lv_dev_timeout(): error in input parameter !\n");
#endif /* LV_DEBUG */ 
     		return (DS_NOTOK);
  	}
  /*
   * is device already imported ?
   */
  	idevice = lv_dev_search(name);
  /*
   * if not then import it
   */
  	if (idevice == DS_NOTOK)
  	{
      		if ((idevice=lv_dev_import(name,error)) == DS_NOTOK)
      		{
         		/*return (mgPrivErrSentinel);*/
         		return (mgErrSentinel);
      		}
  	}
	else
	{
		if ((lv_device[idevice].state & LV_DEV_IMPORTED) == 0)
      		{
			if ((idevice=lv_dev_import(name,error)) == DS_NOTOK)
      			{
         			/*return (mgPrivErrSentinel);*/
         			return (mgErrSentinel);
      			}
		}
	}

  	status = DS_OK;

  	tout.tv_sec = timeout / 1000;
  	tout.tv_usec = (timeout - tout.tv_sec*1000)*1000;

  	if (_dev_rpc_timeout != NULL) 
  	{
     		status = (*_dev_rpc_timeout)(lv_device[idevice].ds,CLSET_TIMEOUT,&tout,error);
  	}
  
  	return (status);
}
/******************************************************************************
 *
 * Function:    lv_ds_init()
 *
 * Description: Create and initialise LabView device server
 *
 * Returns:     DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_ds_init(char *server_name, char *pers_name, DevLong *error)
{
#ifdef unix
    	if (taco_lib == NULL && class_lib == NULL) 
#endif /* unix */
	{
		if (lv_dev_init(error) != DS_OK) return(DS_NOTOK);
	}

#ifdef LV_DEBUG
        if (lv_debug) printf("lv_ds__init(): server %s name %s\n",server_name,pers_name);
#endif /* LV_DEBUG */

        if (_lv_ds__main != NULL) (*_lv_ds__main)(server_name, pers_name);

	return(DS_OK);
}     

/******************************************************************************
 *
 * Function:    lv_ds__cmd_get()
 *
 * Description: Check to see if there are any pending client requests
 *
 * Returns:     DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_ds_cmd_get(long *icmd, void *argin)
{
	long str_len; 
	DevLong error;
	unsigned int i;
	void *ds_argin;
	static DevVarDoubleArray *dblarray_in;
	static DevVarStringArray *strarray_in;
	DblArrayHdl dblarray_hdl;
	LVStringArrayHdl strarray_hdl;
	LVStringHdl str_hdl;
	double *dbl_ptr;

	*icmd = 0;

#ifdef unix
        if (taco_lib == NULL && class_lib == NULL) 
#endif /* unix */
	{
		if (lv_dev_init(&error) != DS_OK) return(DS_NOTOK);
	}

        if (_lv_ds__cmd_get != NULL) 
	{
		*icmd = (*_lv_ds__cmd_get)(&ds_argin);
		if ((*icmd == LVSetValue) || (*icmd == LVIODouble))
		{
			dblarray_in = (DevVarDoubleArray*)ds_argin;
    			dblarray_hdl = (DblArrayHdl) argin;
			if ( ( NumericArrayResize(fD, 1L, (UHandle*) &dblarray_hdl,dblarray_in->length )) != noErr )
      				return(-1);
    			(*dblarray_hdl)->dimSizes[0]= dblarray_in->length;
    			dbl_ptr = (*dblarray_hdl)->arg1;

    			for(i=0;i<dblarray_in->length;i++) {
      				*(dbl_ptr + i) = dblarray_in->sequence[i];
#ifdef LV_DEBUG
      				if (lv_debug) printf("Double in [%d] = %f\n",i,*(dbl_ptr + i));
#endif /* LV_DEBUG */
    			}
		}
		if (*icmd == LVIOString)
		{
			strarray_in = (DevVarStringArray*)ds_argin;
    			strarray_hdl = (LVStringArrayHdl) argin;
			if ( ( NumericArrayResize(iL, 1L, (UHandle*) &strarray_hdl,strarray_in->length )) != noErr )
      				return(-1);

    			(*strarray_hdl)->dimSizes[0] = strarray_in->length;

    			for (i=0; i<(*strarray_hdl)->dimSizes[0]; i++)
    			{
      				str_len=strlen(strarray_in->sequence[i]);
      				str_hdl = (*strarray_hdl)->arg1[i];
      				if(NumericArrayResize(iB, 1L, (UHandle*)&str_hdl, str_len) != noErr)
        				return(-1);
      				(*str_hdl)->dimSizes[0] = strlen(strarray_in->sequence[i]);
      				strncpy((*str_hdl)->arg1,strarray_in->sequence[i],str_len);
#ifdef LV_DEBUG
				if (lv_debug) printf("String in [%d] = %s\n",i,strarray_in->sequence[i]);
#endif /* LV_DEBUG */
    			}
		}
	}

	return(0);
}     

/******************************************************************************
 *
 * Function:    lv_ds__cmd_put()
 *
 * Description: Send reply to client
 *
 * Returns:     DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_ds_cmd_put(long icmd, void *argout)
{
	DevLong error;
	unsigned int i;
	static DevVarDoubleArray dblarray_out;
	static DevVarStringArray strarray_out = {0, NULL};
	DblArrayHdl dblarray_hdl;
	LVStringArrayHdl strarray_hdl;
	LVStringHdl str_hdl;

#ifdef unix
        if (taco_lib == NULL && class_lib == NULL) lv_dev_init(&error);
#endif /* unix */
#ifdef _WINDOWS
		lv_dev_init(&error);
#endif /* _WINDOWS */

        if (_lv_ds__cmd_put != NULL) 
	{
		if (icmd == LVSetValue || icmd == LVState || icmd == LVStatus)
		{
			(*_lv_ds__cmd_put)(NULL);
		}
		if ((icmd == LVReadValue) || (icmd == LVIODouble))
		{
			dblarray_hdl = (DblArrayHdl) argout;
    			dblarray_out.length = (*dblarray_hdl)->dimSizes[0];
    			dblarray_out.sequence = (*dblarray_hdl)->arg1;
#ifdef LV_DEBUG
			if (lv_debug) printf("argout.length %d \n",dblarray_out.length);
    			for(i=0;i<dblarray_out.length;i++) {
      				if (lv_debug) printf("Double out [%d] = %f\n",i,dblarray_out.sequence[i]);
			}
#endif /* LV_DEBUG */
			(*_lv_ds__cmd_put)(&dblarray_out);
    		}
		if (icmd == LVIOString)
		{
			strarray_hdl = (LVStringArrayHdl) argout;
    			strarray_out.length = (*strarray_hdl)->dimSizes[0];
    			strarray_out.sequence = (char**)realloc(strarray_out.sequence,
			                        strarray_out.length*sizeof(char*));
    			for(i=0;i<strarray_out.length;i++) {
       				str_hdl = (*strarray_hdl)->arg1[i];
       				strarray_out.sequence[i] = (char*)malloc((*str_hdl)->dimSizes[0]+1);
       				strncpy(strarray_out.sequence[i],(*str_hdl)->arg1,(*str_hdl)->dimSizes[0]);
				strarray_out.sequence[i][(*str_hdl)->dimSizes[0]] = 0;
#ifdef LV_DEBUG
      				if (lv_debug) printf("String out [%d] = %s\n",i,strarray_out.sequence[i]);
#endif /* LV_DEBUG */
    			}
#ifdef LV_DEBUG
			if (lv_debug) printf("argout.length %d \n",strarray_out.length);
#endif /* LV_DEBUG */
			(*_lv_ds__cmd_put)(&strarray_out);
    			for(i=0;i<strarray_out.length;i++) {
				free(strarray_out.sequence[i]);
			}
    		}
	}

	return(DS_OK);
}     

/******************************************************************************
 *
 * Function:    lv_ds_debug()
 *
 * Description: Set LabView device server api debugging flag
 *
 * Returns:     DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT long lv_ds_debug(long debug)

{
   	lv_debug = debug;

   	return (DS_OK);
}

/******************************************************************************
 *
 * Function:    lv_dev_err_str()
 *
 * Description: Return device error string corresponding to lv_error. String
 *		is " " if lv_error==0
 *
 * Returns:     DS_OK=success , DS_NOTOK=fail
 *
 ******************************************************************************/

EXPORT char *lv_dev_error_str(long error_no)
{
   	static char *error_str=NULL, *char_ptr;
   	static int prev_error_no;
	DevLong error;


   	if (prev_error_no > 0) if (error_str != NULL) free(error_str);

	if (error_no < 0)
	{
		switch (error_no) {

		case -1 : 
			error_str = "Cannot load LabView + TACO libraries (hint check you $LD_LIBRARY_PATH or $SHLIB_PATH)";
			break;
		case -2 : 
			error_str = "Command not available for this device";
			break;
		default : 
			error_str = "Error not recognised";
			break;
		}
	}
	else
	{
		if (error_no == 0)
		{
			error_str = " ";
		}
		else
		{
			if (_dev_error_str == NULL) lv_dev_init(&error);

   			if (_dev_error_str != NULL) 
			{
				error_str = (*_dev_error_str)(error_no);
			}
			else
			{
				error_str = "Cannot load LabView + TACO libraries (hint check you $LD_LIBRARY_PATH or $SHLIB_PATH)";
			}
		}
	}
  
	prev_error_no = error_no;

   	return(error_str);
}
