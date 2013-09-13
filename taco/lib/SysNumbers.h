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
 * File:        SysNumbers.h
 *
 * Project:     Device Servers with SUN-RPC
 *
 * Description: Include file with the definitions for all 
 *		software of the system group 
 *
 * Author(s):   Emmanuel Taurel
 * 		$Author: jkrueger1 $
 *
 * Original:    January 1994
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:08:28 $
 *
 ********************************************************************-*/
#include <DserverTeams.h>

#ifndef _SysNumbers_h
#define _SysNumbers_h


/*------- Device server identification ---------*/

#define	DcBase		(SysTeamNumber + DS_LSHIFT(1, DS_IDENT_SHIFT))
#define DevMClockBase	(SysTeamNumber + DS_LSHIFT(2, DS_IDENT_SHIFT))

/*------- Daemon Device server (for new resources) -------*/

#define NewDevBase      (SysTeamNumber + DS_LSHIFT(3, DS_IDENT_SHIFT))

/*------- HDB Filler Device Server ----------------------*/

#define HdbFillerBase	(SysTeamNumber + DS_LSHIFT(4, DS_IDENT_SHIFT))

/*------- HDB Library base number ----------------------*/

#define HdbLibBase	(SysTeamNumber + DS_LSHIFT(5, DS_IDENT_SHIFT))

/*------- Bench base number ----------------------------*/

#define DevBenchBase	(SysTeamNumber + DS_LSHIFT(6, DS_IDENT_SHIFT))

/*------- Starter base number ----------------------------*/

#define DevStarterBase	(SysTeamNumber + DS_LSHIFT(7, DS_IDENT_SHIFT))

/*------- Tango base number for automatic numbering of Tango commands -*/

#define DevTangoBase	(SysTeamNumber + DS_LSHIFT(8, DS_IDENT_SHIFT))

/*------- Data collector warning codes ---------*/

#define DcWarnBase      (DcBase + DS_LSHIFT(WarningError, DS_CAT_SHIFT))


/*------- Data collector error codes -----------*/
#define	DcErrorBase	(DcBase)


#endif	/* _SysDsNumbers_h */
