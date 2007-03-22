/*********************************************************************

 File:          ExpDsNumbers.h

 Project:       Device Servers with SUN-RPC

 Description:   Include file with the definitions for all
                EXPG dserver projects

 Author(s):     V.Rey Bakaikoa
		$Author: jkrueger1 $

 Original:      October 1993

 Version:	$Revision: 1.2 $

 Date:		$Date: 2007-03-22 15:49:49 $

 Copyright (c) 1993 by European Synchrotron Radiation Facility,
                       Grenoble, France
********************************************************************-*/

#ifndef _ExpDsNumbers_h
#define _ExpDsNumbers_h

#include <DserverTeams.h>
#include <macros.h>

/* Hexapode *********/
#define DevHexapodeBase       (ExpTeamNumber + DS_LSHIFT(1,DS_IDENT_SHIFT))
/* Rga **************/
#define DevRgaBase            (ExpTeamNumber + DS_LSHIFT(2,DS_IDENT_SHIFT))
/* Huber ************/
#define DevHuberBase          (ExpTeamNumber + DS_LSHIFT(3,DS_IDENT_SHIFT))
/* OptBerger ********/
#define DevOptBergerBase      (ExpTeamNumber + DS_LSHIFT(4,DS_IDENT_SHIFT))
/* Viraxis ********/
#define DevViraxisBase        (ExpTeamNumber + DS_LSHIFT(5,DS_IDENT_SHIFT))
/* Princeton CCD camera ******/
#define DevPrincetonBase      (ExpTeamNumber + DS_LSHIFT(6,DS_IDENT_SHIFT))
/* CCD cameras ******/
#define DevCcdBase	      (ExpTeamNumber + DS_LSHIFT(6,DS_IDENT_SHIFT))
/* VME general ******/
#define DevVmedsBase          (ExpTeamNumber + DS_LSHIFT(7,DS_IDENT_SHIFT))
/* Machine Status ******/
#define DevMachstatBase       (ExpTeamNumber + DS_LSHIFT(8,DS_IDENT_SHIFT))
/* Frelon camera ******/
#define DevFrelonBase         (ExpTeamNumber + DS_LSHIFT(6,DS_IDENT_SHIFT))
/* Stanford DG535 delay generator, Friedrich Schotte, 6 Oct 2000  */
#define DevDg535Base          (ExpTeamNumber + DS_LSHIFT(9,DS_IDENT_SHIFT))
/* X-ray chopper by Forschungszentrum Julich, Friedrich Schotte, 11 Oct 2000 */
#define DevXrayChopperBase    (ExpTeamNumber + DS_LSHIFT(10,DS_IDENT_SHIFT))
/* Must synchronization */
#define DevMustBase    (ExpTeamNumber + DS_LSHIFT(11,DS_IDENT_SHIFT))
/* Fast Tomo (id19) */
#define DevFastTomoBase    (ExpTeamNumber + DS_LSHIFT(12,DS_IDENT_SHIFT))

#endif	/* _ExpDsNumbers_h */
