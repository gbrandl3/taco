/*+*******************************************************************

 File:          DasDsNumbers.h

 Project:       Device Servers with SUN-RPC

 Description:   Include file with the definitions for all
                DAS projects

 Author(s):     A.Beteva

 Original:      Aug 1993

 $Revision: 1.2 $
 $Date: 2004-11-03 14:56:22 $

 $Author: andy_gotz $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France
********************************************************************-*/

#ifndef _DasDsNumbers_h
#define _DasDsNumbers_h

#include <DserverTeams.h>

/* ESRF-VDL ****************/
#define DevVdlBase       (DasTeamNumber + DS_LSHIFT(1, DS_IDENT_SHIFT))
/* ELTEC-IC40 **************/
#define DevIpcBase       (DasTeamNumber + DS_LSHIFT(2, DS_IDENT_SHIFT))
/* NOVELEC-MCCE ************/
#define DevMcceBase      (DasTeamNumber + DS_LSHIFT(3, DS_IDENT_SHIFT))
/*** ESRF - SKELETON *******/
#define DevSkelBase      (DasTeamNumber + DS_LSHIFT(4, DS_IDENT_SHIFT))
/*** LECROY 1151 - COUNTER */
#define DevCntBase       (DasTeamNumber + DS_LSHIFT(5, DS_IDENT_SHIFT))
/*** ESRF - VISTA (in old times it was TDC CI022 here) ******/
#define DevVistaBase     (DasTeamNumber + DS_LSHIFT(6, DS_IDENT_SHIFT))
/* CAEN V462 - GATEGEN *****/
#define DevGategenBase   (DasTeamNumber + DS_LSHIFT(7, DS_IDENT_SHIFT))
/*** EC740 TFG *************/
#define DevTfgBase       (DasTeamNumber + DS_LSHIFT(9, DS_IDENT_SHIFT))
/*** EC738 MCS *************/
#define DevMcsBase       (DasTeamNumber + DS_LSHIFT(10, DS_IDENT_SHIFT))
/*** VVHIST ****************/
#define DevVhistBase     (DasTeamNumber + DS_LSHIFT(11, DS_IDENT_SHIFT))
/*** HM - MM6326 ***********/
#define DevHmBase        (DasTeamNumber + DS_LSHIFT(12, DS_IDENT_SHIFT))
/*** Delay Line Detector ***/
#define DevDldBase       (DasTeamNumber + DS_LSHIFT(13, DS_IDENT_SHIFT))
/*** Multi Channel Analyser ***/
#define DevMcaBase     	 (DasTeamNumber + DS_LSHIFT(14, DS_IDENT_SHIFT))
/*** GPIB ******************/
#define DevGpibBase	 (DasTeamNumber + DS_LSHIFT(15, DS_IDENT_SHIFT))
/*** ESRF - VCT6 (counter/timer) ******/
#define DevVCT6Base	 (DasTeamNumber + DS_LSHIFT(16, DS_IDENT_SHIFT))
/*** ESRF - V110 (in old times it was TDC CI029 here) ******/
#define DevV110Base	 (DasTeamNumber + DS_LSHIFT(17, DS_IDENT_SHIFT))
/*** ESRF - XFR (data transfer) ******/
#define DevXfrBase	 (DasTeamNumber + DS_LSHIFT(18, DS_IDENT_SHIFT))
/*** ESRF - MemAcq (acq simulator) ******/
#define DevMemacqBase	 (DasTeamNumber + DS_LSHIFT(19, DS_IDENT_SHIFT))
/*** LeCroy - LTDC ******/
#define DevLTDCBase	 (DasTeamNumber + DS_LSHIFT(20, DS_IDENT_SHIFT))
/*** EMBL Xray scanner  ******/
#define DevXrayscanBase	 (DasTeamNumber + DS_LSHIFT(21, DS_IDENT_SHIFT))
/*** ESRF - VTDC4 (4 channel VME TDC)  ******/
#define DevVtdc4Base	 (DasTeamNumber + DS_LSHIFT(22, DS_IDENT_SHIFT))
/*** ESRF - VCCD3 (VME CCD3 camera i/f)  ******/
#define DevVccd3Base	 (DasTeamNumber + DS_LSHIFT(23, DS_IDENT_SHIFT))
/*** Scan based on Vct6 and Vpap device server ******/
#define DevScanBase	 (DasTeamNumber + DS_LSHIFT(24, DS_IDENT_SHIFT))
/*** V850 digital delay generator ******/
#define DevV850Base  (DasTeamNumber + DS_LSHIFT(25, DS_IDENT_SHIFT))
/*** DDA/XDS X-ray Digital Spectroscopy ******/
#define DevXdsBase  (DasTeamNumber + DS_LSHIFT(26, DS_IDENT_SHIFT))
/*** Barras Unix device server ***/
#define DevBarrasBase  (DasTeamNumber + DS_LSHIFT(27, DS_IDENT_SHIFT))
/*** C111 CompactPCI TDC card Linux device server ***/
#define DevC111Base  (DasTeamNumber + DS_LSHIFT(28, DS_IDENT_SHIFT))

#endif	/* _DasDsNumbers_h */
