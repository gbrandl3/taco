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
 * File:        DaemonP.h
 *
 * Project:     Device Servers 
 *
 * Description: private include for the ServerClass
 *
 * Author(s):   Michael Schofield
 *              $Author: jkrueger1 $
 *
 * Original:    April 1992
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

#ifndef DAEMONP_H
#define DAEMONP_H

typedef struct _DaemonClassPart {
                                       int nada;
                                  }
                DaemonClassPart;

typedef struct _DaemonPart {
                                  int nada;
                             }
                DaemonPart;

typedef struct _DaemonClassRec {
                                      DevServerClassPart devserver_class;
                                      DaemonClassPart daemon_class;
                                 }
                DaemonClassRec;

extern DaemonClassRec daemonClassRec;

typedef struct _DaemonRec {
                                 DevServerPart devserver;
                                 DaemonPart daemon;
                            }
                DaemonRec;

#endif /* DAEMONP_H */
