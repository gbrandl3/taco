/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright(c) 1994-2014 by European Synchrotron Radiation Facility,
 *                     Grenoble, France
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
 * File:        helloP.h
 *
 * Description: example device server
 *
 * Author(s):   $Author: jkrueger1 $
 *
 * Original:    December 99
 * 
 * Date:	$Date: 2013-05-17 08:05:49 $
 *
 * Version:	$Revision: 1.1 $
 */
#ifndef HELLO_P_H
#define HELLO_P_H

#include <DevServer.h>
#include <DevServerP.h>

typedef struct _HelloClassRec {
	DevServerClassPart devserver_class;
} HelloClassRec;

typedef HelloClassRec 	*HelloClass;

typedef struct _HelloRec {
	DevServerPart	devserver;
} HelloRec;

typedef HelloRec	*Hello;

extern HelloClass helloClass;
extern Hello	  hello;

#endif
