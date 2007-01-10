/*+*******************************************************************

  File:					DevCcdCmds.h

  Project:      CCD Camera Server

  Description: 	Include file with the definitions for all 
	              commands numbers for the CCD cameras

  Author(s): 		David Fernandez, Holger Witsch

  Original:			September 2000

  $Author: andy_gotz $ 
  $Date: 2007-01-10 12:55:20 $   
  $Locker:  $ 
  $Name: not supported by cvs2svn $   
  $State: Exp $  

  $Log: not supported by cvs2svn $
  Revision 1.7  2006/03/08 14:26:12  claustre
  new commands for the marccd, Laurent C.

  Revision 1.6  2006/03/08 14:10:53  claustre
  David has added cmds for the frelon2k !!!

  Revision 1.5  2004/05/06 06:48:49  meyer
  Corrected #endif in file

  Revision 1.4  2004/05/05 17:15:35  ahoms
  Completed unification with Meteor2 commands.
  Using RCS log in header.

  Revision 1.3  2004/04/01 12:00:00  miraut
  New commands to modify output image size

  Revision 1.2  2004/03/19 12:00:00  claustre
  Unified with others ccds and with .res file
  /segfs/dserver/classes/ccd/res/ dir.

  Revision 1.1.2.3  2004/01/27 12:00:00  meyer
  Added DevCcdSetGain and DevCcdGetGain

  Revision 1.1.2.2  2002/10/03 12:00:00  meyer
  Added DevCcdSetMaxExposure and DevCcdGetMaxExposure

  Revision 1.1.2.1  2001/09/07 12:00:00  meyer
  Added DevCcdSetThreshold and DevCcdGetThreshold

  Revision 1.1.1.7  2004/03/18 12:00:00  miraut
  New commands to send Jpeg compressed images

  Revision 1.1.1.6  2003/12/03 12:00:00  claustre
  Added DevCcdDezinger special cmd for marccd

  Revision 1.1.1.5  2003/04/17 12:00:00  fernande
  Added DevCcdReadAll and DevCcdWriteAll

  Revision 1.1.1.4  2002/08/01 12:00:00  fernande
  MCD/DFC DevCcdStop replaces DevCcdAbortAcquis

  Revision 1.1.1.3  2001/10/18 12:00:00  witsch
  Added DevCcdGetBuffer and DevCcdGetBufferInfo

  Revision 1.1.1.2  2001/06/06 12:00:00  fernande
  include <ExpDsNumbers.h>

  Revision 1.1.1.1  2001/05/21 12:00:00  witsch
  DevCcdGetCurrent included (gets current frame number in ds)

  Revision 1.1  2000/10/06 12:00:00  witsch
  Initial revision: Some new commands included

  Copyright (c) 2000 by European Synchrotron Radiation Facility,
				        Grenoble, France

  Tab size is 2!

*********************************************************************/

#ifndef _CCDDEVCMDS_H
#define _CCDDEVCMDS_H

#include <ExpDsNumbers.h>	/* DF010606 */
#include <DevCmds.h>

enum DevCcdCmds_type
{
    DevCcdNull = DevCcdBase,
    DevCcdStart,        /*   1 or in resource numbers: CMDS/3/6/1   */
    DevCcdStop,  	/*   2 or in resource numbers: CMDS/3/6/2   */
    DevCcdRead,         /*   3 or in resource numbers: CMDS/3/6/3   */
    DevCcdSetExposure,  /*   4 or in resource numbers: CMDS/3/6/4   */
    DevCcdGetExposure,  /*   5 or in resource numbers: CMDS/3/6/5   */
    DevCcdSetRoI,       /*   6 or in resource numbers: CMDS/3/6/6   */
    DevCcdGetRoI,       /*   7 or in resource numbers: CMDS/3/6/7   */
    DevCcdSetBin,       /*   8 or in resource numbers: CMDS/3/6/8   */
    DevCcdGetBin,       /*   9 or in resource numbers: CMDS/3/6/9   */
    DevCcdSetTrigger,   /*  10 or in resource numbers: CMDS/3/6/10  */
    DevCcdGetTrigger,   /*  11 or in resource numbers: CMDS/3/6/11  */
    DevCcdGetLstErrMsg, /*  12 or in resource numbers: CMDS/3/6/12  */
    DevCcdXSize,        /*  13 or in resource numbers: CMDS/3/6/13  */
    DevCcdYSize,        /*  14 or in resource numbers: CMDS/3/6/14  */
    DevCcdSetADC,       /*  15 or in resource numbers: CMDS/3/6/15  */
    DevCcdGetADC,       /*  16 or in resource numbers: CMDS/3/6/16  */
    DevCcdSetSpeed,     /*  17 or in resource numbers: CMDS/3/6/17  */
    DevCcdGetSpeed,     /*  18 or in resource numbers: CMDS/3/6/18  */
    DevCcdSetShutter,   /*  19 or in resource numbers: CMDS/3/6/19  */
    DevCcdGetShutter,   /*  10 or in resource numbers: CMDS/3/6/20  */
    DevCcdSetFrames,    /*  21 or in resource numbers: CMDS/3/6/21  */
    DevCcdGetFrames,    /*  22 or in resource numbers: CMDS/3/6/22  */
    DevCcdCommand,      /*  23 or in resource numbers: CMDS/3/6/23  */
    DevCcdDepth,        /*  24 or in resource numbers: CMDS/3/6/24  */
    DevCcdSetMode,      /*  25 or in resource numbers: CMDS/3/6/25  */
    DevCcdGetMode,      /*  26 or in resource numbers: CMDS/3/6/26  */
    DevCcdSetChannel,   /*  27 or in resource numbers: CMDS/3/6/27  */
    DevCcdGetChannel,   /*  28 or in resource numbers: CMDS/3/6/28  */
    DevCcdSetRingBuf,   /*  29 or in resource numbers: CMDS/3/6/29  */
    DevCcdGetRingBuf,   /*  30 or in resource numbers: CMDS/3/6/30  */
    DevCcdLive,         /*  31 or in resource numbers: CMDS/3/6/31  */
    DevCcdWriteFile,    /*  32 or in resource numbers: CMDS/3/6/32  */
    DevCcdReset,        /*  33 or in resource numbers: CMDS/3/6/33  */
    DevCcdGetIdent,     /*  34 or in resource numbers: CMDS/3/6/34  */
    DevCcdGetType,      /*  35 or in resource numbers: CMDS/3/6/35  */
    DevCcdSetKinWinSize,/*  36 or in resource numbers: CMDS/3/6/36  */
    DevCcdGetKinWinSize,/*  37 or in resource numbers: CMDS/3/6/37  */
    DevCcdSetKinetics,  /*  38 or in resource numbers: CMDS/3/6/38  */
    DevCcdGetKinetics,  /*  39 or in resource numbers: CMDS/3/6/39  */

    DevCcdCorrect,      /*  40 or in resource numbers: CMDS/3/6/40  */
    DevCcdSetFilePar,   /*  41 or in resource numbers: CMDS/3/6/41  */
    DevCcdGetFilePar,   /*  42 or in resource numbers: CMDS/3/6/42  */
    DevCcdHeader,       /*  43 or in resource numbers: CMDS/3/6/43  */
    DevCcdSetFormat,    /*  44 or in resource numbers: CMDS/3/6/44  */
    DevCcdGetFormat,    /*  45 or in resource numbers: CMDS/3/6/45  */
    DevCcdSetViewFactor,/*  46 or in resource numbers: CMDS/3/6/46  */
    DevCcdGetViewFactor,/*  47 or in resource numbers: CMDS/3/6/47  */
    DevCcdSetHwPar,     /*  48 or in resource numbers: CMDS/3/6/48  */
    DevCcdGetHwPar,     /*  49 or in resource numbers: CMDS/3/6/49  */

    DevCcdGetCurrent,   /*  50 or in resource numbers: CMDS/3/6/50  */
    DevCcdGetBuffer,    /*  51 or in resource numbers: CMDS/3/6/51  */
    DevCcdGetBufferInfo,/*  52 or in resource numbers: CMDS/3/6/52  */
    DevCcdReadAll,      /*  53 or in resource numbers: CMDS/3/6/53  */
    DevCcdWriteAll,     /*  54 or in resource numbers: CMDS/3/6/54  */
    DevCcdDezinger,     /*  55 or in resource numbers: CMDS/3/6/55  */

    DevCcdSetThreshold, /*  56 or in resource numbers: CMDS/3/6/56  */
    DevCcdGetThreshold, /*  57 or in resource numbers: CMDS/3/6/57  */
    DevCcdSetMaxExposure,/* 58 or in resource numbers: CMDS/3/6/58  */
    DevCcdGetMaxExposure,/* 59 or in resource numbers: CMDS/3/6/59  */
    DevCcdSetGain,      /*  60 or in resource numbers: CMDS/3/6/60  */
    DevCcdGetGain,      /*  61 or in resource numbers: CMDS/3/6/61  */

    DevCcdReadJpeg,     /*  62 or in resource numbers: CMDS/3/6/62  */  
    DevCcdRefreshTime,  /*  63 or in resource numbers: CMDS/3/6/63  */
    DevCcdOutputSize,   /*  64 or in resource numbers: CMDS/3/6/64  */ 
    DevCcdGetTGradient, /*  65 or in resource numbers: CMDS/3/6/65  */ 

    DevCcdGetChanges,  /*  66 or in resource numbers: CMDS/3/6/66  */ 
    DevCcdCalibrate,   /*  67 or in resource numbers: CMDS/3/6/67  */ 
    DevCcdSetThumbnail1, /*  68 or in resource numbers: CMDS/3/6/68  */
    DevCcdSetThumbnail2, /*  69 or in resource numbers: CMDS/3/6/69  */
    DevCcdWriteThumbnail1, /*  70 or in resource numbers: CMDS/3/6/70  */
    DevCcdWriteThumbnail2, /*  71 or in resource numbers: CMDS/3/6/71  */
};


/* 
 * commands for CCD cameras
   !!! These bit definitions MUST be in the same order as the above enum !!!
 */

#define DevCcdStart         DevCcdBase +  1
#define DevCcdStop          DevCcdBase +  2
#define DevCcdRead          DevCcdBase +  3
#define DevCcdSetExposure   DevCcdBase +  4
#define DevCcdGetExposure   DevCcdBase +  5
#define DevCcdSetRoI        DevCcdBase +  6
#define DevCcdGetRoI        DevCcdBase +  7
#define DevCcdSetBin        DevCcdBase +  8
#define DevCcdGetBin        DevCcdBase +  9
#define DevCcdSetTrigger    DevCcdBase + 10
#define DevCcdGetTrigger    DevCcdBase + 11
#define DevCcdGetLstErrMsg  DevCcdBase + 12
#define DevCcdXSize         DevCcdBase + 13
#define DevCcdYSize         DevCcdBase + 14
#define DevCcdSetADC        DevCcdBase + 15
#define DevCcdGetADC        DevCcdBase + 16
#define DevCcdSetSpeed      DevCcdBase + 17
#define DevCcdGetSpeed      DevCcdBase + 18

#define DevCcdSetShutter    DevCcdBase + 19
#define DevCcdGetShutter    DevCcdBase + 20

#define DevCcdSetFrames     DevCcdBase + 21
#define DevCcdGetFrames     DevCcdBase + 22

#define DevCcdCommand       DevCcdBase + 23

#define DevCcdGetDepth      DevCcdBase + 24

#define DevCcdSetMode       DevCcdBase + 25 /* numbers not follwing ! */
#define DevCcdGetMode       DevCcdBase + 26

#define DevCcdSetChannel    DevCcdBase + 27
#define DevCcdGetChannel    DevCcdBase + 28

#define DevCcdSetRingBuf    DevCcdBase + 29
#define DevCcdGetRingBuf    DevCcdBase + 30

#define DevCcdLive          DevCcdBase + 31
#define DevCcdWriteFile     DevCcdBase + 32

#define DevCcdReset         DevCcdBase + 33
#define DevCcdGetIdent      DevCcdBase + 34
#define DevCcdGetType       DevCcdBase + 35

#define DevCcdSetKinWinSize DevCcdBase + 36
#define DevCcdGetKinWinSize DevCcdBase + 37
#define DevCcdSetKinetics   DevCcdBase + 38
#define DevCcdGetKinetics   DevCcdBase + 39

#define DevCcdCorrect       DevCcdBase + 40
#define DevCcdSetFilePar    DevCcdBase + 41
#define DevCcdGetFilePar    DevCcdBase + 42
#define DevCcdHeader        DevCcdBase + 43
#define DevCcdSetFormat     DevCcdBase + 44
#define DevCcdGetFormat     DevCcdBase + 45
#define DevCcdSetViewFactor DevCcdBase + 46
#define DevCcdGetViewFactor DevCcdBase + 47
#define DevCcdSetHwPar      DevCcdBase + 48
#define DevCcdGetHwPar      DevCcdBase + 49

#define DevCcdGetCurrent    DevCcdBase + 50
#define DevCcdGetBuffer     DevCcdBase + 51
#define DevCcdGetBufferInfo DevCcdBase + 52
#define DevCcdReadAll       DevCcdBase + 53
#define DevCcdWriteAll      DevCcdBase + 54
#define DevCcdDezinger      DevCcdBase + 55

#define DevCcdSetThreshold  DevCcdBase + 56
#define DevCcdGetThreshold  DevCcdBase + 57
#define DevCcdSetMaxExposure  DevCcdBase + 58
#define DevCcdGetMaxExposure  DevCcdBase + 59
#define DevCcdSetGain       DevCcdBase + 60
#define DevCcdGetGain       DevCcdBase + 61

#define DevCcdReadJpeg      DevCcdBase + 62
#define DevCcdRefreshTime   DevCcdBase + 63
#define DevCcdOutputSize    DevCcdBase + 64 
#define DevCcdGetTGradient  DevCcdBase + 65 

#define DevCcdReadChanges   DevCcdBase + 66 
#define DevCcdCalibrate   DevCcdBase + 67 

/* Specific cmds for the marccd */
#define DevCcdSetThumbnail1   DevCcdBase + 68 
#define DevCcdSetThumbnail2   DevCcdBase + 69 
#define DevCcdWriteThumbnail1   DevCcdBase + 70 
#define DevCcdWriteThumbnail2   DevCcdBase + 71 

#endif /* _CCDDEVCMDS_H */
