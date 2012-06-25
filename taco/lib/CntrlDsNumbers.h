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
 * File:        CntrlDsNumbers.h
 *
 * Project:     Device Servers with SUN-RPC
 *
 * Description: Include file with the definitions for all 
 *		device server classes of the machine control 
 *		group.
 *
 * Author(s):   Jens Meyer
 *		$Author: jlpons $
 *
 * Original:    July 1993
 *
 * Version:	$Revision: 1.5 $

 * Date:	$Date: 2012-06-25 08:56:09 $
 *
 ********************************************************************-*/

#ifndef _CntrlDsNumbers_h
#define _CntrlDsNumbers_h

#include <DserverTeams.h>

/* Test service */
#define DevTestBase     (CntrlTeamNumber + DS_LSHIFT(1, DS_IDENT_SHIFT)) 
/* Aircondition Units Server (J. Meyer)*/
#define DevAirCUnitBase (CntrlTeamNumber + DS_LSHIFT(2, DS_IDENT_SHIFT))
/* Power Meter Server (P. Verdier) */
#define DevPowerMeterBase (CntrlTeamNumber + DS_LSHIFT(3, DS_IDENT_SHIFT))
/* Insertion device classes (C. Penel) */
#define DevIdBase (CntrlTeamNumber + DS_LSHIFT(4, DS_IDENT_SHIFT)) 
/* hazemeyer classes (J.M. Chaize) */
#define DevHazemeyerBase (CntrlTeamNumber + DS_LSHIFT(5, DS_IDENT_SHIFT)) 
/* XBPM class (A.Gotz) */
#define DevXBPMBase (CntrlTeamNumber + DS_LSHIFT(6, DS_IDENT_SHIFT))
/* XFeedback class (A.Gotz) */
#define DevXFDBKBase (CntrlTeamNumber + DS_LSHIFT(7, DS_IDENT_SHIFT))
/* Srct class (I.Fladmark) */
#define DevSRCTBase (CntrlTeamNumber + DS_LSHIFT(8, DS_IDENT_SHIFT))
/* VDSP71 Board for Voltage Monitor Server (P. Verdier) */
#define DevVdsp71Base (CntrlTeamNumber + DS_LSHIFT(9, DS_IDENT_SHIFT))
/* CalcSignals class (I.Fladmark) */
#define DevCalcSignalsBase (CntrlTeamNumber + DS_LSHIFT(10, DS_IDENT_SHIFT))
/* VLM class (J.M. Chaize) */
#define DevVLMBase (CntrlTeamNumber + DS_LSHIFT(11, DS_IDENT_SHIFT))
/* C5680 Streak Camera class (I. Fladmark) */
#define DevC5680Base (CntrlTeamNumber + DS_LSHIFT(12, DS_IDENT_SHIFT))
/* Fluid Circuit class (J. Meyer) */
#define DevFluidBase (CntrlTeamNumber + DS_LSHIFT(13, DS_IDENT_SHIFT))
/* Tagger Class (I. Fladmark) */
#define DevTaggerBase (CntrlTeamNumber + DS_LSHIFT(14, DS_IDENT_SHIFT))
/* Bpm Class (JM. Chaize) */
#define DevBpmBase (CntrlTeamNumber + DS_LSHIFT(15, DS_IDENT_SHIFT))
/* Cts Class (I. Fladmark) */
#define DevCTSBase (CntrlTeamNumber + DS_LSHIFT(16, DS_IDENT_SHIFT))
/* HpibSICL Class (I. Fladmark) */
#define DevHPIBSICLBase (CntrlTeamNumber + DS_LSHIFT(17, DS_IDENT_SHIFT))
/* Pgvarian Class (J. Meyer) */
#define DevPgvarianBase (CntrlTeamNumber + DS_LSHIFT(18, DS_IDENT_SHIFT))
/* Elesta device classes (C. Penel) */
#define DevElestaBase (CntrlTeamNumber + DS_LSHIFT(19, DS_IDENT_SHIFT))
/* Berger device classes (C. Penel) */
#define DevBergerBase (CntrlTeamNumber + DS_LSHIFT(20, DS_IDENT_SHIFT))
/* Axe device classes (C. Penel) */
#define DevAxeBase (CntrlTeamNumber + DS_LSHIFT(21, DS_IDENT_SHIFT))
/* Taper device classes (C. Penel) */
#define DevTaperBase (CntrlTeamNumber + DS_LSHIFT(22, DS_IDENT_SHIFT))  
/* IdSlit device classes (C. Penel) */
#define DevIdSlitBase (CntrlTeamNumber + DS_LSHIFT(23, DS_IDENT_SHIFT)) 
/* Revolver device classes (C. Penel) */
#define DevRevolverBase (CntrlTeamNumber + DS_LSHIFT(24, DS_IDENT_SHIFT)) 
/* Meca device classes (C. Penel) */
#define DevMecaBase (CntrlTeamNumber + DS_LSHIFT(25, DS_IDENT_SHIFT))
/* Power device classes (C. Penel) */
#define DevPowerBase (CntrlTeamNumber + DS_LSHIFT(26, DS_IDENT_SHIFT))
/* Bulstra device classes (C. Penel) */
#define DevBulstraBase (CntrlTeamNumber + DS_LSHIFT(27, DS_IDENT_SHIFT)) 
/* Lien device classes (C. Penel) */
#define DevLienBase (CntrlTeamNumber + DS_LSHIFT(28, DS_IDENT_SHIFT))
/* Alarms device classes (I. Fladmark) */
#define DevAlarmBase (CntrlTeamNumber + DS_LSHIFT(29, DS_IDENT_SHIFT))
/* West PID controller device class (J. Meyer) */
#define DevPidBase (CntrlTeamNumber + DS_LSHIFT(30, DS_IDENT_SHIFT))
/* AirCondition device class (I. Fladmark) */
#define DevAirConditionBase (CntrlTeamNumber + DS_LSHIFT(31, DS_IDENT_SHIFT)) 
/* Fluid device class (I. Fladmark) */
#define DevFLUIDBase (CntrlTeamNumber + DS_LSHIFT(32, DS_IDENT_SHIFT))
/* Dsp device class (J.M. Chaize) */
#define DevDspBase (CntrlTeamNumber + DS_LSHIFT(33, DS_IDENT_SHIFT))
/* Signal treatment class (J. Meyer) */
#define DevSignalBase (CntrlTeamNumber + DS_LSHIFT(34, DS_IDENT_SHIFT))
/* ESRF serial line protocol class (J. Meyer) */
#define DevProtocolBase (CntrlTeamNumber + DS_LSHIFT(35, DS_IDENT_SHIFT))
/* Anode power supply device class (J. Meyer) */
#define DevAnodeBase (CntrlTeamNumber + DS_LSHIFT(36, DS_IDENT_SHIFT))
/* FrontEnd Class error base */
#define DevFrontEndBase (CntrlTeamNumber + DS_LSHIFT(37, DS_IDENT_SHIFT))
/* Nucletude serial line protocol class */
#define DevNucletudeBase (CntrlTeamNumber + DS_LSHIFT(38, DS_IDENT_SHIFT))
/* Siemens serial line protocol class (J. Meyer) */
#define DevSiemensBase (CntrlTeamNumber + DS_LSHIFT(39, DS_IDENT_SHIFT))
/* RF PLC device class (J. Meyer) */
#define DevRfplcBase (CntrlTeamNumber + DS_LSHIFT(40, DS_IDENT_SHIFT))
/* RF Interlock class (J. Meyer) */
#define DevInterlockBase (CntrlTeamNumber + DS_LSHIFT(41, DS_IDENT_SHIFT))
/* Driver RF class (I. Fladmark) */
#define DevDriverRFBase (CntrlTeamNumber + DS_LSHIFT(42, DS_IDENT_SHIFT))
/* Tuner Loop class (I. Fladmark) */
#define DevTunerLoopBase (CntrlTeamNumber + DS_LSHIFT(43, DS_IDENT_SHIFT))
/* RF ion pump power supply device class (JL. Pons) */
#define DevIonPumpBase (CntrlTeamNumber + DS_LSHIFT(44, DS_IDENT_SHIFT))
/* RF Filament power supply device class (JL. Pons) */
#define DevFilamentBase (CntrlTeamNumber + DS_LSHIFT(45, DS_IDENT_SHIFT))
/* RF Focus power supply device class (JL. Pons) */
#define DevFocusBase (CntrlTeamNumber + DS_LSHIFT(46, DS_IDENT_SHIFT))
/* Sequencer class (JL. Pons) */
#define DevSequencerBase (CntrlTeamNumber + DS_LSHIFT(47, DS_IDENT_SHIFT))
/* Multi Signal class (I. Fladmark) */
#define DevMSignalBase (CntrlTeamNumber + DS_LSHIFT(48, DS_IDENT_SHIFT))
/*  Linac class (JM Chaize) */
#define DevLinacBase (CntrlTeamNumber + DS_LSHIFT(49, DS_IDENT_SHIFT))
/* RF HVPS device class (J. Meyer) */
#define DevHvpsBase (CntrlTeamNumber + DS_LSHIFT(50, DS_IDENT_SHIFT))
/* RF Klystron high-level device class (JL PONS) */
#define DevKlystronBase (CntrlTeamNumber + DS_LSHIFT(51, DS_IDENT_SHIFT))
/* RF Klystron tube device class (JL PONS) */
#define DevKlystronTubeBase (CntrlTeamNumber + DS_LSHIFT(52, DS_IDENT_SHIFT))
/* BunchClock device class (JM Chaize) */
#define DevBCBase (CntrlTeamNumber + DS_LSHIFT(53, DS_IDENT_SHIFT))
/* high level cavities class (JL Pons) */
#define DevCavitiesBase (CntrlTeamNumber + DS_LSHIFT(54, DS_IDENT_SHIFT))
/* Arc detector class (JL Pons) */
#define DevArcBase (CntrlTeamNumber + DS_LSHIFT(55, DS_IDENT_SHIFT))
/* Tuner class (J. Meyer) */
#define DevTunerBase (CntrlTeamNumber + DS_LSHIFT(56, DS_IDENT_SHIFT))
/* Gpsm (Fieldbus access) class (for 1000 tours BPM) (MC Dominguez) */
#define DevGpsmBase (CntrlTeamNumber + DS_LSHIFT(57, DS_IDENT_SHIFT))
/* RF Driver regulation loop (JL PONS) */
#define DevDrivLoopBase (CntrlTeamNumber + DS_LSHIFT(58, DS_IDENT_SHIFT))
/* GIP Industry Pack G64 board (JM Chaize) */
#define DevGIPBase (CntrlTeamNumber + DS_LSHIFT(59, DS_IDENT_SHIFT))
/* TSX17 plc interface between Plc class and RFplc class (B. Scaringella) */
#define DevTsx17Base (CntrlTeamNumber + DS_LSHIFT(60, DS_IDENT_SHIFT))
/* RF Phase regulation loop (JL PONS) */
#define DevPhaseLoopBase (CntrlTeamNumber + DS_LSHIFT(61, DS_IDENT_SHIFT))
/* Cobula (for 1000 tours BPM) (MC Dominguez) */
#define DevCobulaBase (CntrlTeamNumber + DS_LSHIFT(62, DS_IDENT_SHIFT))
/* NoBreak base for HQPS power management (JM Chaize) */
#define DevNoBreakBase (CntrlTeamNumber + DS_LSHIFT(63, DS_IDENT_SHIFT))
/* VIPBase base for Varian ionpump (JM Chaize) */
#define DevVIPBase (CntrlTeamNumber + DS_LSHIFT(64, DS_IDENT_SHIFT))
/* EMPHU base for EMPHU SYSTEM (C PENEL) */
#define DevEmphuBase (CntrlTeamNumber + DS_LSHIFT(65, DS_IDENT_SHIFT))
/* Serial line protocol for dose meter define by JF.Bouteille (J Meyer) */
#define DevJFBProtocolBase (CntrlTeamNumber + DS_LSHIFT(66, DS_IDENT_SHIFT))
/* MachStat base (JL Pons) */
#define DevMachStatBase (CntrlTeamNumber + DS_LSHIFT(67, DS_IDENT_SHIFT))
/* Mille Tour BPM high level class base (JL Pons) */
#define DevMTourBase (CntrlTeamNumber + DS_LSHIFT(68, DS_IDENT_SHIFT))
/* ADC icv101 base (JL Pons) */
#define DevAdcicv101Base (CntrlTeamNumber + DS_LSHIFT(69, DS_IDENT_SHIFT))
/* Global feedback base (JM Chaize) */
#define DevGfeedbackBase (CntrlTeamNumber + DS_LSHIFT(70, DS_IDENT_SHIFT))
/* Cavity Coolling loop base (JL Pons) */
#define DevCoolloopBase (CntrlTeamNumber + DS_LSHIFT(71, DS_IDENT_SHIFT))
/* Insertion Device base (A Gotz) */
#define DevIDBase (CntrlTeamNumber + DS_LSHIFT(72, DS_IDENT_SHIFT))
/* Transfer Efficiency Base */
#define DevTreffBase (CntrlTeamNumber + DS_LSHIFT(73, DS_IDENT_SHIFT))
/*  Horizontal Focussing Base */
#define DevHfoBase (CntrlTeamNumber + DS_LSHIFT(74, DS_IDENT_SHIFT))
/*  IP Module ADA08 Base */
#define DevADA08Base (CntrlTeamNumber + DS_LSHIFT(75, DS_IDENT_SHIFT))
/*  FeVac Base (High level class for FE vacuum) */
#define DevFeVacBase (CntrlTeamNumber + DS_LSHIFT(76, DS_IDENT_SHIFT))
/*  SYCODS (Booster Power Supply correction) */
#define DevSYCOBase (CntrlTeamNumber + DS_LSHIFT(77, DS_IDENT_SHIFT))
/* FrontEndLog Class base */
#define DevFrontEndLogBase (CntrlTeamNumber + DS_LSHIFT(78, DS_IDENT_SHIFT))
/* IPTaxi Class base */
#define DevIPTaxiBase (CntrlTeamNumber + DS_LSHIFT(79, DS_IDENT_SHIFT))
/* Transversal feedback Class base */
#define DevFeedTransBase (CntrlTeamNumber + DS_LSHIFT(80, DS_IDENT_SHIFT))
#define DevHDBBase (CntrlTeamNumber + DS_LSHIFT(81, DS_IDENT_SHIFT))
/* Cav78 Coolling Class base */
#define DevCAV78CoolBase (CntrlTeamNumber + DS_LSHIFT(82, DS_IDENT_SHIFT))
/* Stretching cell Class base */
#define DevSCBase (CntrlTeamNumber + DS_LSHIFT(83, DS_IDENT_SHIFT))
/* Radiation Survey Class base */
#define DevRadSurveyBase (CntrlTeamNumber + DS_LSHIFT(84, DS_IDENT_SHIFT))
/* Tsx57 Protocol Class base */
#define DevTsx57Base (CntrlTeamNumber + DS_LSHIFT(85, DS_IDENT_SHIFT))
/* Micos Motor Controller Class base */
#define DevMicosBase (CntrlTeamNumber + DS_LSHIFT(86, DS_IDENT_SHIFT))
/* Modbus protocol Class base */
#define DevModb_protBase (CntrlTeamNumber + DS_LSHIFT(87, DS_IDENT_SHIFT))
/* Oxford 700 cryo controller Class base */
#define DevOxford700Base (CntrlTeamNumber + DS_LSHIFT(88, DS_IDENT_SHIFT))
/* MecaT : Tango wrapper class for new Unulators */
#define MecaTBase (CntrlTeamNumber + DS_LSHIFT(89, DS_IDENT_SHIFT))
/* CavPhase : Taco class for RF cavity phase measurement */
#define CavPhaseBase (CntrlTeamNumber + DS_LSHIFT(90, DS_IDENT_SHIFT))
#endif	/* _CntrlDsNumbers_h */
