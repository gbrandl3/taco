/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:	Starter.h
 * 
 * Project:	Device Servers
 * 
 * Description: Starter relevant operation class
 * 		
 * 
 * Author(s);	Emmanuel TAUREL
 * 
 * Original:	january 04 1995
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2005-07-25 13:08:28 $
 * 
 *********************************************************************/

#ifndef _STARTER_H
#define _STARTER_H


typedef struct _StarterClassRec *StarterClass;
typedef struct _StarterRec *Starter;

extern StarterClass starterClass;
extern Starter starter;

#define DevErr_NoProcessWithPid 9701
#define DevErr_CantFindExecutable 9702
#define DevErr_CantKillProcess 9703
#endif /* _STARTER_H */

