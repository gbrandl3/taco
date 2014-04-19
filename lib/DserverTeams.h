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
 * File:        DserverTeams.h
 *
 * Project:     Device Servers with SUN-RPC
 *
 * Description: Include file with the definitions for all 
 *		device server programming teams.
 *
 * Author(s):   Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:    July 1993
 *
 * Version:	$Revision: 1.4 $
 *
 * Date: 	$Date: 2005-07-25 13:08:23 $
 *
 ********************************************************************-*/

#ifndef _DserverTeams_h
#define _DserverTeams_h

/*
 * Definitions to code and decode the error and command numbers.
 */

#define DS_TEAM_SHIFT	26
#define DS_IDENT_SHIFT	18
#define DS_CAT_SHIFT	12

#define DS_TEAM_MASK	63      /* value = 0x3f */
#define DS_IDENT_MASK	255     /* value = 0xff */
#define DS_CAT_MASK	63      /* value = 0x3f */

/**************  Device server development Teams definition  **************/

#define CntrlTeamNumber	DS_LSHIFT(1,DS_TEAM_SHIFT) /* CS - Control group */
#define DasTeamNumber	DS_LSHIFT(2,DS_TEAM_SHIFT) /* CS - Data acquisition */
#define ExpTeamNumber	DS_LSHIFT(3,DS_TEAM_SHIFT) /* Experiments-Programming */
#define CrgTeamNumber	DS_LSHIFT(4,DS_TEAM_SHIFT) /* External - CRG */
#define BlcTeamNumber	DS_LSHIFT(5,DS_TEAM_SHIFT) /* CS - Beam Line Control */
#define SysTeamNumber	DS_LSHIFT(6,DS_TEAM_SHIFT) /* CS - System Team */
#define MachTeamNumber	DS_LSHIFT(7,DS_TEAM_SHIFT) /* Machine - Theory Group */
#define FRMTeamNumber	DS_LSHIFT(8,DS_TEAM_SHIFT) /* FRM-II - Garching */
#define TestTeamNumber	DS_LSHIFT(63, DS_TEAM_SHIFT)	/* Only for tests */

/**************  Device server error category definition  **************/

#define WarningError	63      /* value = 0x3f : Just a warning */

#endif	/* _DserverTeams_h */
