/**********************************************
 * File:        taco.h
 *
 * Description: interface python - taco
 *
 * Author(s):   MCD
 *
 * Original:    December 99
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.7  2002/03/06  15:35:02  15:35:02  meyer (J.Meyer)
 * Corrected version numbering
 * 
 * Revision 2.5  2001/10/17  18:42:59  18:42:59  meyer (J.Meyer)
 * Corrected command ececution for LINUX
 * 
 * Revision 2.4  2001/07/18  16:14:01  16:14:01  meyer (J.Meyer)
 * Corrected dev_getresource().
 * 
 * Revision 2.2  2001/04/27  14:37:16  14:37:16  meyer (J.Meyer)
 * Corrected modules to work only with one shared library
 * 
 * Revision 2.1  2001/02/22  16:58:01  16:58:01  meyer (J.Meyer)
 * TACO server and client as one package!
 * 
 * Revision 1.6  2000/07/31  14:46:53  14:46:53  meyer (J.Meyer)
 * Added compile options for numerical python (NUMPY)
 * 
 * Revision 1.5  2000/07/24  13:31:47  13:31:47  meyer (J.Meyer)
 * Changed class name from Device to TacoDevice.
 * 
 * Revision 1.4  2000/07/20  16:52:43  16:52:43  meyer (J.Meyer)
 * Clean-up of code
 * 
 * Revision 1.3  2000/07/11  10:30:51  10:30:51  meyer (J.Meyer)
 * Corrected memory leaks in taco.c
 * 
 * Revision 1.1  2000/06/30  10:23:56  10:23:56  meyer (J.Meyer)
 * Initial revision
 * 
 *  
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
