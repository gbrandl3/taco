/*********************************************************************

 File:          ExpDsNumbers.h

 Project:       Device Servers with SUN-RPC

 Description:   Include file with the definitions for all
                EXPG dserver projects

 Author(s):     V.Rey Bakaikoa

 Original:      October 1993

 Updates:	04/06/96 R.Wilcke
		Added macros.h as include file;
		changed "<<" to DS_LSHIFT for FORTRAN compatibility.

		28/05/97 R.Wilcke
		Added definition of DevMstatBase;
		define DevCcdBase with same value as DevPrincetonBase.

		04/07/97 R.Wilcke
		name change DevMstatBase -> DevMachstatBase

		17/11/99 V.Rey Bakaikoa
		add Frelon Camera project

		22/02/00 D.Fernandez-Carreiras
		define DevFrelonBase with same value as DevCcdBase.

		06/07/04 Carlos Medrano & D.Fernandez-Carreiras 
		add DevFastTomoBase



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
