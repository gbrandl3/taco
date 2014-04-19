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
 * File:        MakeDP.cpp
 *
 * Project:     Device Servers 
 *
 * Description: 
 *
 * Author(s):   
 *              $Author: jkrueger1 $
 *
 * Original:    
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 22:41:09 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif

#ifdef _OSK
#define pid_t int 
#endif

void Make_Dataport_Name (char *dataport_name, size_t len, char *dataport_name_stub,pid_t proc_id)
{
	snprintf(dataport_name, len, "%s.%ld.DP",dataport_name_stub,(long)proc_id);
}
