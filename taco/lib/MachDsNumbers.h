/*+*******************************************************************

 File:          MachNumbers.h

 Project:       Device Servers with SUN-RPC

 Description:   Include file with the definitions for all 
		software of the machine theory group 

 Author(s):     J. Meyer

 Original:      June 1994


 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

********************************************************************-*/
#include <DserverTeams.h>
#include <macros.h>

#ifndef _MachDsNumbers_h
#define _MachDsNumbers_h


/*------- Device server identification ---------*/

#define	AutoCorBase	(MachTeamNumber + DS_LSHIFT(1,DS_IDENT_SHIFT))
#define	AppliBase	(MachTeamNumber + DS_LSHIFT(2,DS_IDENT_SHIFT))
#define	MessageBase	(MachTeamNumber + DS_LSHIFT(3,DS_IDENT_SHIFT))
#define	FbpsBase	(MachTeamNumber + DS_LSHIFT(4,DS_IDENT_SHIFT))
/* base for Unidos dosemeter (J.M. Chaize) */
#define	UnidosBase	(MachTeamNumber + DS_LSHIFT(5,DS_IDENT_SHIFT))

#endif	/* _MachDsNumbers_h */
