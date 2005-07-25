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
 * File:        BlcDsNumbers.h
 *
 * Project:     Device Servers with SUN-RPC
 *
 * Description: Include file with the definitions for all 
 *		device server classes of the BLC group 
 *
 * Author(s):   Christine Dominguez
 *		$Author: jkrueger1 $
 *
 * Original:    July 1993
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:08:21 $
 *
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

#endif	/* _BlcDsNumbers_h */
