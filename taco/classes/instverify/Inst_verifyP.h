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
 * File:	Inst_verifyP.h
 *
 * Project:	Device Server Distribution
 *
 * Description:	Dummy class to test control system installations
 *
 * Author(s):   Jens Meyer
 *              $Author: jkrueger1 $
 *
 * Original:	10.5.95	
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 22:38:27 $
 */

#ifndef _INST_VERIFYP_H
#define _INST_VERIFYP_H



typedef struct _Inst_verifyClassPart
{
		DevLong	config_param; /* Configuration parameter for hardware */
} Inst_verifyClassPart;



typedef struct _Inst_verifyPart
{
		DevFloat	read_value; /* Read value of device */
		DevFloat	set_value; /* Set value of the device */
} Inst_verifyPart;



typedef struct _Inst_verifyClassRec
{
   DevServerClassPart	devserver_class;
				
   Inst_verifyClassPart	inst_verify_class;
} Inst_verifyClassRec;



extern Inst_verifyClassRec inst_verifyClassRec;


typedef struct _Inst_verifyRec
{
   DevServerPart 	devserver;
				
   Inst_verifyPart 	inst_verify;
} Inst_verifyRec;

#endif /*_INST_VERIFYP_H*/

