/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:	sema.h
 *
 * Description:
 *
 * Author(s):	
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2005-07-25 12:54:15 $
 *
 *****************************************************************************/

#ifndef SEMA_H
#define SEMA_H

#define DEADLOCK_WAIT 15
#define OS9_DEADLOCK_WAIT	DEADLOCK_WAIT*256

int define_sema ();
int delete_sema ();
int get_sema ();
int release_sema ();

#endif
