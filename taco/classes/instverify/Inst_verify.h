
/*********************************************************************
 *
 * File:	Inst_verify.h
 *
 * Project:	Device Server Distribution
 *
 * Description:	Dummy class to test control system installations
 *		
 *
 * Author(s):	J.Meyer
 *
 * Original:	10.5.95
 *
 * Copyright(c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#ifndef _INST_VERIFY_H
#define _INST_VERIFY_H



/*************************************************************************
 * Include here the file which contains the definition of Devinst_verifyBase,
 * may be  CntrlDsNumbers.h, DasDsNumbers.h or BlcDsNumbers.h.
 * See DSN096.
 *************************************************************************
#include <CntrlDsNumbers.h>
*/

typedef struct _Inst_verifyClassRec *Inst_verifyClass;
typedef struct _Inst_verifyRec *Inst_verify;

extern Inst_verifyClass inst_verifyClass;

/*************************************************************************
 * Example of definition of commands which are not belonging to DevCmds.h
 * See DSN096.
 *************************************************************************
 
#define DevCommand1            DevInst_verifyBase + 1
#define DevCommand2            DevInst_verifyBase + 2
*/
/*************************************************************************
 * Example of definition of errors which are not belonging to DevErrors.h
 * See DSN096.
 *************************************************************************

#define DevErr_SpecialError1    DevInst_verifyBase + 1
#define DevErr_SpecialError2    DevInst_verifyBase + 2
 */

#endif /*_INST_VERIFY_H*/
