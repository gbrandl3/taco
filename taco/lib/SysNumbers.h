/*+*******************************************************************

 File:          SysNumbers.h

 Project:       Device Servers with SUN-RPC

 Description:   Include file with the definitions for all 
		software of the system group 

 Author(s):     Emmanuel Taurel

 Original:      January 1994


 $Revision: 1.2 $
 $Date: 2004-11-25 15:56:52 $

 $Author: andy_gotz $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

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
