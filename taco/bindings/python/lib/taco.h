/**********************************************
 * File:        taco.h
 *
 * Description: interface python - taco
 *
 * Author(s):   MCD
 *		$Author: jkrueger1 $
 *
 * Original:    December 99
 *
 * Date:	$Date: 2004-05-14 15:02:07 $
 *
 * Version:	$Revision: 1.2 $
 *
 ************************************************/

#ifndef _taco_py
#define _taco_py

long get_argin_single(DevArgument, long, PyObject *, char *);

long get_argin_array(DevArgument, long, PyObject *, char *);

long get_argin_special(DevArgument, long, PyObject *, char *);

long get_argout_single(DevArgument, long, PyObject **, char *);

long get_argout_array(DevArgument, long, PyObject **, char *);

long get_argout_special(DevArgument, long, PyObject **, char *);

long check_type(long, long *, long *, long *);

void display_single(DevArgument, long, char *);

void display_array(DevArgument, long, char *);

void display_special(DevArgument, long, char *);

long lenoftype(long);     

long Ctype2numeric(long);

#endif
