
/*+*******************************************************************

 File:          DserverTeams.h

 Project:       Device Servers with SUN-RPC

 Description:   Include file with the definitions for all 
		device server programming teams.

 Author(s):     Jens Meyer
 		$Author: jkrueger1 $

 Original:      July 1993

 Version:	$Revision: 1.3 $

 Date: 		$Date: 2003-05-16 13:52:03 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

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
