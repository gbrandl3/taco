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
 * File:	dataport.h
 *
 * Author(s):	
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 12:54:15 $
 *
 ***************************************************************************/

/**
 * @defgroup dataportAPI Data port API
 * @ingroup API
 * These functions allow the access to the data port
 *
 * The dataport library allows uses to create and share data structures
 * on Unix and OS9 via shared memory. Synchronisation is achieved
 * with semaphores.
 */

#ifndef DATAPORT_H
#define DATAPORT_H

#if (OSK || _OSK)
/* OS-9 version */

#include <module.h>

typedef int Semaphore;

typedef struct {
     struct modhcom   header;

     Semaphore        sem;
     char             semname[32];
     int              pid;
     char             body;
} Dataport;

#else                                             /* Unix version */
#include <sys/types.h>

typedef struct {
     int              sema;
     int              shar;
     int              key;
     pid_t            creator_pid;
     char             body;
} Dataport;

#endif

#define	HASH_BASE	131
#define	HASH_MOD	500000000

#ifdef __cplusplus
extern "C" {
extern Dataport *CreateDataport(char *,long);
extern Dataport *OpenDataport(char *,long);
extern long AccessDataport(Dataport *);
extern long ReleaseDataport(Dataport *);
extern long CloseDataport(Dataport *,char *);
}
#else
extern Dataport *CreateDataport();
extern Dataport *OpenDataport();
extern long AccessDataport();
extern long ReleaseDataport();
extern long CloseDataport();
#endif /* __cplusplus */

#endif
