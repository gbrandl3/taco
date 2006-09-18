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
 * File:	Inst_verify.h
 *
 * Project:	Device Server Distribution
 *
 * Description:	Dummy class to test control system installations
 *		
 *
 * Author(s):	J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:	10.5.95
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 22:38:27 $
 */

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
