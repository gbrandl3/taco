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
 * File:	dataportP.h
 *
 * Description: Private include file for the OS-9 release of the dataport 
 * 
 * Author(s):	
 *		$Author: jkrueger1 $
 *
 * Version:	$Version$
 * 
 * Date:	$Date: 2005-07-25 12:54:15 $
 *
 ****************************************************************************/

#ifndef DATAPORTP_H
#define DATAPORTP_H

#ifdef ERROR
#	undef ERROR
#	define	ERROR	-1
#endif

#ifndef FALSE
#	define	FALSE	0
#endif

#ifndef TRUE
#	define	TRUE	1
#endif

#define	ALM_SIGNAL	SIGWAKE

#endif
