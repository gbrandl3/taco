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
 * $Revision: 1.1 $
 ************************************************/

#ifndef _taco_py

#define _taco_py
extern long get_argin_single(DevArgument ds_argin, long ds_in, 
                      PyObject *item, char* mymess);
extern long get_argin_array(DevArgument ds_argin, long ds_in, 
                      PyObject *mytuple, char* mymess);
extern long get_argin_special(DevArgument ds_argin, long ds_in, 
                      PyObject *mytuple, char* mymess);
extern long get_argout_single(DevArgument ds_argout, long ds_out, 
                      PyObject **item, char* mymess);
extern long get_argout_array(DevArgument ds_argout, long ds_out, 
                      PyObject **mylist, char* mymess);
extern long get_argout_special(DevArgument ds_argout, long ds_out, 
                      PyObject **mylist, char* mymess);
extern long check_type(long ds_ty, long *is_a_single, 
                long *is_an_array, long *is_a_special);
extern void display_single(DevArgument ds_argin,long ds_in,char * ms);
extern void display_array(DevArgument ds_argin,long ds_in, char* ms);
extern void display_special(DevArgument ds_argin,long ds_in,char * ms);
extern long lenoftype(long ds_out);     
extern long Ctype2numeric(long ds_out);

#endif
