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
 * File:        ud_server.h
 *
 * Project:     Device Servers
 *
 * Description: public include for the ServerClass
 *              
 * Author(s):   Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:    October 1990
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

#ifndef UD_SERVER_H
#define UD_SERVER_H

typedef struct _DaemonClassRec *DaemonClass;
typedef struct _DaemonRec *Daemon;

extern DaemonClass daemonClass;
extern Daemon ud_daemon;

#endif /* DAEMON_H */
