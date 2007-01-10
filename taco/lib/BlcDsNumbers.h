/*+*******************************************************************

 File:          BlcDsNumbers.h

 Project:       Device Servers with SUN-RPC

 Description:   Include file with the definitions for all 
		device server classes of the BLC group 

 Author(s):     Christine Dominguez

 Original:      July 1993


 $Revision: 1.4 $
 $Date: 2007-01-10 15:48:25 $

 $Author: andy_gotz $

 $Log: not supported by cvs2svn $
 Revision 3.85  2007/01/10 14:28:05  domingue
 C353 base (correction)

 Revision 3.84  2007/01/10 14:12:48  domingue
 add c353 device server base

 Revision 3.83  2006/06/29 15:36:14  goetz
 added Master and Slave bases

 Revision 3.82  2005/09/16 14:12:12  beteva
 added DevXbpmBase

 Revision 3.81  2005/02/25 15:52:58  beteva
 added CalcPython base

 Revision 3.80  2004/04/26 09:07:45  meyer
 Correted typing mistake

 Revision 3.79  2004/04/26 08:39:03  meyer
 Added generic python server

 Revision 3.78  2003/12/12 15:05:20  goetz
 added ImageIO base

 Revision 3.77  2003/12/12 09:57:08  dserver
 Add flex correlator (id10)

 Revision 3.76  2003/08/04 06:48:53  perez
 added wago base number

 Revision 3.75  2003/04/01 13:46:05  goetz
 added Mpa3 base number

 Revision 3.74  2003/01/28 15:53:00  ahoms
 added base number for Relay

 Revision 3.73  2002/11/13 19:52:14  goetz
 added Fitem base

 Revision 3.72  2002/11/06 13:46:46  goetz
 added DevPlotBase

 Revision 3.71  2002/11/05 20:23:46  goetz
 added DevSimulatorBase

 Revision 3.70  2002/07/22 08:20:18  perez
 added FIDL base

 Revision 3.69  2002/05/28 08:50:43  beteva
 added Hook base

 Revision 3.68  2002/04/03 12:22:10  perez
 added ISG Serial base

 Revision 3.67  2001/09/13 14:47:49  goetz
 added RedLion base number

 Revision 3.66  2001/08/30 05:19:32  goetz
 added Binem base

 Revision 3.65  2001/08/09 10:33:53  goetz
 added Modbus base

 Revision 3.64  2001/05/31 13:11:19  goetz
 fixed Beamline to BeamlineBase

 Revision 3.63  2001/05/31 13:02:16  goetz
 added Beamline base number

 Revision 3.62  2001/04/12 11:17:59  goetz
 added DevImpacBase for Impac pyrometer

 Revision 3.61  2001/03/29 19:11:29  goetz
 added Eurotherm base

 Revision 3.60  1901/02/12 10:47:04  dserver
 add zygo device server

 * Revision 3.59  20/1./1.  5.:2.:8.  5.:2.:8.  goetz (Andy Goetz)
 * added Linkam base number
 * 
 Revision 3.58  2001/01/17 14:51:16  goetz
 added Matlab base number

 Revision 3.57  2000/07/24 13:45:35  claustre
 added Scanner base number, first scanner server is for the MAR345 Image Plate detector

 Revision 3.56  2000/07/19 07:15:27  perez
 added Wafer base number

 Revision 3.55  2000/01/29 07:31:57  goetz
 added LabView base number

 Revision 3.54  2000/01/17 15:12:03  goetz
 added medoptics base number

 * Revision 3.53  99/10/11  13:55:14  13:55:14  goetz (Andy Goetz)
 * added DevXiaBase for Paolo Mangiagalli
 * 
 Revision 3.52  1999/05/18 13:09:17  goetz
 added Oms base

 Revision 3.51  1999/04/23 14:16:10  goetz
 added ImagePro base

 * Revision 3.50  98/08/19  11:16:17  11:16:17  goetz (Andy Goetz)
 * added DevCCDCameraBase for generic CCD device servers
 * 
 * Revision 3.49  98/08/12  14:15:58  14:15:58  beteva (A.Beteva)
 * Added PiezoControl base definition
 * 
 * Revision 3.48  98/07/24  09:57:20  09:57:20  goetz (Andy Goetz)
 * added DevIK320Base for Heidenhain IK320 VME-based high resolution encoder
 * 
 * Revision 3.47  98/06/26  12:56:05  12:56:05  perez (Manuel.Perez)
 * added DevPCExpBase for PHI'S Hemispherical electron analyser
 * 
 * Revision 3.46  1998/05/13  09:04:14  perez
 * added DevFastScanBase for Image Plate FastScanner on Windows NT
 *
 * Revision 3.45  1998/03/10  16:36:56  beteva
 * added LEYBOLD Instruments base definition
 *
 * Revision 3.44  97/12/04  16:18:00  16:18:00  goetz (Andy Goetz)
 * added DevSensiCamBase for Sensicam CCD camera on Windows 95
 * 
 * Revision 3.43  97/10/16  16:28:53  16:28:53  goetz (Andy Goetz)
 * added DevMarCCDBase for Mar CCD scanner on GNU/Linux
 * 
 Revision 3.42  1997/09/19 13:54:58  perez
 add DevPPSbase

 * Revision 3.41  1997/01/30  13:46:24  domingue
 * add DevIpmcBase
 *
 * Revision 3.40  96/10/15  15:20:02  15:20:02  beteva (A.Beteva)
 * added DevAdc150Base
 * 
 * Revision 3.39  96/05/14  08:39:46  08:39:46  meyer (J.Meyer)
 * Added brackets around base definition.
 * 
 * Revision 3.38  96/05/06  18:39:27  18:39:27  meyer (J.Meyer)
 * Fortran compatible include file.
 * 
 * Revision 3.37  96/02/12  12:16:25  12:16:25  veuillen (Jm.Veuillen)
 * left unchanged
 * 
 * Revision 3.36  96/02/01  13:54:08  13:54:08  perez ()
 * Add DevGalilBase
 * 
 * Revision 3.35  96/02/01  13:52:30  13:52:30  perez ()
 * Add Pgblazers by JMV.
 * 
 * Revision 3.34  95/05/29  10:21:56  10:21:56  domingue (MC Dominguez)
 * remove last Wxbpm base because was already there !!
 * 
 * Revision 3.33  95/05/29  10:19:44  10:19:44  domingue (M.C Dominguez)
 * add Wxbpm new base
 * 
 * Revision 3.32  94/10/25  17:42:03  17:42:03  domingue (M.C Dominguez)
 * add Maxe base for new commands
 * 
 * Revision 3.31  94/08/31  10:51:53  10:51:53  pepellin (Gilbert Pepellin)
 * Add DevWxbpmBase
 * 
 * Revision 3.30  94/05/26  10:24:17  10:24:17  domingue (M.C Dominguez)
 * Add DevIcvDacBase and SigApiBase 
 * 
 * Revision 3.29  94/04/18  15:07:25  15:07:25  dserver ()
 * VIP added by JMV
 * 
 * Revision 3.28  94/01/17  10:38:43  10:38:43  goetz (Andy Goetz)
 * added Serial Line class base number
 * 
 * Revision 1.1  94/01/05  09:35:02  09:35:02  goetz (Andy Goetz)
 * Initial revision
 * 
 * Revision 3.26  1993/12/14  08:46:00  domingue
 * Add Upm60 and Hbmsh servers
 *
 * Revision 3.25  1993/09/24  17:24:36  meyer
 * Release with error and command resources.
 *
 * Revision 1.1  93/09/08  13:03:20  13:03:20  domingue ( Marie-Christine Dominguez)
 * Initial revision
 * 

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

********************************************************************-*/

#ifndef _BlcDsNumbers_h
#define _BlcDsNumbers_h

#include <DserverTeams.h>

/*------- M4 device server---------*/
#define DevM4Base     	      (BlcTeamNumber + DS_LSHIFT(1, DS_IDENT_SHIFT)) 
/*------- Upm60 device server---------*/
#define DevUpm60Base          (BlcTeamNumber + DS_LSHIFT(2, DS_IDENT_SHIFT))
/*------- Hbm with share memory device server---------*/
#define DevHbmshBase          (BlcTeamNumber + DS_LSHIFT(3, DS_IDENT_SHIFT))
/*------- Serial line device server---------*/
#define DevSerBase            (BlcTeamNumber + DS_LSHIFT(4, DS_IDENT_SHIFT))
/*------- Varian ion pump device server---------*/
#define DevVipBase            (BlcTeamNumber + DS_LSHIFT(5, DS_IDENT_SHIFT))
/*------- IcvDac device server---------*/
#define DevIcvDacBase	      (BlcTeamNumber + DS_LSHIFT(6, DS_IDENT_SHIFT))
/*------- SigApi (Signal monitoring) ---------*/
#define SigApiBase	      (BlcTeamNumber + DS_LSHIFT(7, DS_IDENT_SHIFT))
/*------- Wxbpm device server --------*/
#define DevWxbpmBase	      (BlcTeamNumber + DS_LSHIFT(8, DS_IDENT_SHIFT))
/*------- Maxe device server  new commands--------*/
#define DevMaxeBase	      (BlcTeamNumber + DS_LSHIFT(9, DS_IDENT_SHIFT))
/*------- Balzers pressure gauge device server---------*/
#define DevPgbalzersBase      (BlcTeamNumber + DS_LSHIFT(10, DS_IDENT_SHIFT))
/*------- Galil DC motor controller device server ---------*/
#define DevGalilBase   	      (BlcTeamNumber + DS_LSHIFT(11, DS_IDENT_SHIFT))
/*------- Adc150 (ICV150 ADC) device server ---------*/
#define DevAdc150Base         (BlcTeamNumber + DS_LSHIFT(12, DS_IDENT_SHIFT))
/*------- Ipmc device server ---------*/
#define DevIpmcBase           (BlcTeamNumber + DS_LSHIFT(13, DS_IDENT_SHIFT))
/*------- Medical Chair device server ---------*/
#define DevPPSBase            (BlcTeamNumber + DS_LSHIFT(14, DS_IDENT_SHIFT))
/*------- Mars CCD scanner on GNU/Linux--------*/
#define DevMarCCDBase         (BlcTeamNumber + DS_LSHIFT(15, DS_IDENT_SHIFT))
/*------- LYBOLD Instruments device server --------*/
#define DevLeyboldBase       (BlcTeamNumber + DS_LSHIFT(16, DS_IDENT_SHIFT))
/*------- Sensicam CCD camera on Windows 95--------*/
#define DevSensiCamBase       (BlcTeamNumber + DS_LSHIFT(17, DS_IDENT_SHIFT))
/*------- Image Plate FastScanner on Windows NT--------*/
#define DevFastScanBase       (BlcTeamNumber + DS_LSHIFT(18, DS_IDENT_SHIFT))
/*------- PHI's Hemispherical Electron Analyser on Windows95--------*/
#define DevPCExpBase       (BlcTeamNumber + DS_LSHIFT(19, DS_IDENT_SHIFT))
/*------- Heidenhain IK320 high resolution VME encoder on Linux/m68k --------*/
#define DevIK320Base       (BlcTeamNumber + DS_LSHIFT(20, DS_IDENT_SHIFT))
/*------- PiezoControl device server --------*/
#define DevPiezoCtrlBase   (BlcTeamNumber + DS_LSHIFT(21, DS_IDENT_SHIFT))
/*------- CCD Camera device server(s) --------*/
#define DevCCDCameraBase   (BlcTeamNumber + DS_LSHIFT(22, DS_IDENT_SHIFT))
/*------- ImagePro image analysis device server --------*/
#define DevImageProBase   (BlcTeamNumber + DS_LSHIFT(23, DS_IDENT_SHIFT))
/*------- Oregon stepper motor device server --------*/
#define DevOmsBase   (BlcTeamNumber + DS_LSHIFT(24, DS_IDENT_SHIFT))
/*------- Xia device server for Paolo Mangiagalli -------*/
#define DevXiaBase   (BlcTeamNumber + DS_LSHIFT(25, DS_IDENT_SHIFT))
/*-------MedOptics device server -------*/
#define DevMedOpticsBase   (BlcTeamNumber + DS_LSHIFT(26, DS_IDENT_SHIFT))
/*-------LabView device server -------*/
#define DevLabViewBase   (BlcTeamNumber + DS_LSHIFT(27, DS_IDENT_SHIFT))
/*-------MEDEA wafer handler device server -------*/
#define DevWaferBase   (BlcTeamNumber + DS_LSHIFT(28, DS_IDENT_SHIFT))
/*-------Scanner detector device server(s) -------*/
#define DevScannerBase   (BlcTeamNumber + DS_LSHIFT(29, DS_IDENT_SHIFT))
/*-------Matlab detector device server(s) -------*/
#define DevMatlabBase   (BlcTeamNumber + DS_LSHIFT(30, DS_IDENT_SHIFT))
/*-------Linkam sample stage device server(s) -------*/
#define DevLinkamBase   (BlcTeamNumber + DS_LSHIFT(31, DS_IDENT_SHIFT))
/*-------Zygo device server(s) -------*/
#define DevZygoBase   (BlcTeamNumber + DS_LSHIFT(32, DS_IDENT_SHIFT))
/*-------Eurotherm device server -------*/
#define DevEurothermBase   (BlcTeamNumber + DS_LSHIFT(33, DS_IDENT_SHIFT))
/*-------Impac pyrometer device server -------*/
#define DevImpacBase   (BlcTeamNumber + DS_LSHIFT(34, DS_IDENT_SHIFT))
/*-------Beamline device server -------*/
#define DevBeamlineBase   (BlcTeamNumber + DS_LSHIFT(35, DS_IDENT_SHIFT))
/*-------Modbus device server -------*/
#define DevModbusBase   (BlcTeamNumber + DS_LSHIFT(36, DS_IDENT_SHIFT))
/*-------Binem device server -------*/
#define DevBinemBase   (BlcTeamNumber + DS_LSHIFT(37, DS_IDENT_SHIFT))
/*-------Redlion device server -------*/
#define DevRedLionBase   (BlcTeamNumber + DS_LSHIFT(38, DS_IDENT_SHIFT))
/*-------ISG Serial device server -------*/
#define DevIsgSerialBase   (BlcTeamNumber + DS_LSHIFT(39, DS_IDENT_SHIFT))
/*------- Hook device server -------*/
#define DevHookBase   (BlcTeamNumber + DS_LSHIFT(40, DS_IDENT_SHIFT))
/*------- FIDL device server -------*/
#define DevFidlBase   (BlcTeamNumber + DS_LSHIFT(41, DS_IDENT_SHIFT))
/*------- Simulator device server -------*/
#define DevSimulatorBase   (BlcTeamNumber + DS_LSHIFT(42, DS_IDENT_SHIFT))
/*------- Plot device server -------*/
#define DevPlotBase   (BlcTeamNumber + DS_LSHIFT(43, DS_IDENT_SHIFT))
/*------- Fitem device server -------*/
#define DevFitemBase   (BlcTeamNumber + DS_LSHIFT(44, DS_IDENT_SHIFT))
/*------- Relay device server -------*/
#define DevRelayBase   (BlcTeamNumber + DS_LSHIFT(45, DS_IDENT_SHIFT))
/*------- Mpa3 device server -------*/
#define DevMpa3Base   (BlcTeamNumber + DS_LSHIFT(46, DS_IDENT_SHIFT))
/*------- Wago device server -------*/
#define DevWcBase   (BlcTeamNumber + DS_LSHIFT(47, DS_IDENT_SHIFT))
/*------- Flex Correlator device server -------*/
#define DevFlexBase   (BlcTeamNumber + DS_LSHIFT(48, DS_IDENT_SHIFT))
/*------- Image input output device server -------*/
#define DevImageIO   (BlcTeamNumber + DS_LSHIFT(49, DS_IDENT_SHIFT))
/*------- Generic Python server -------*/
#define DevPythonBase   (BlcTeamNumber + DS_LSHIFT(50, DS_IDENT_SHIFT))
/*------- Calculation Python server -------*/
#define DevCalcPythonBase   (BlcTeamNumber + DS_LSHIFT(51, DS_IDENT_SHIFT))
/*------- Xbpm Taco server -------*/
#define DevXbpmBase   (BlcTeamNumber + DS_LSHIFT(52, DS_IDENT_SHIFT))
/*------- Batch Queue System Master -------*/
#define DevMasterBase   (BlcTeamNumber + DS_LSHIFT(53, DS_IDENT_SHIFT))
/*------- Batch Queue System Slave -------*/
#define DevSlaveBase   (BlcTeamNumber + DS_LSHIFT(54, DS_IDENT_SHIFT))
/*------- C353 device server -------*/
#define DevC353Base   (BlcTeamNumber + DS_LSHIFT(55, DS_IDENT_SHIFT))

#endif	/* _BlcDsNumbers_h */
