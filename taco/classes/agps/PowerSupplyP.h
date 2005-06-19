/*static char RcsId[] = " $Header: /segfs/dserver/classes/powersupply/include/RCS/PowerSupplyP.h,v 1.7 2005/01/12 20:14:41 goetz Exp $";*/
/*static char SccsId[] = " @(#) PowerSupplyP.h 1.2 90/04/27 14:48:19 ";*/
/*********************************************************************

 File:		PowerSupplyP.h

 Project:	Device Servers

 Description:	Private include file for applications using the
		power supply device server

 Author(s):	Andy Goetz

 Original:	March 1990

 $Log: PowerSupplyP.h,v $
 Revision 1.7  2005/01/12 20:14:41  goetz
 commented out token at end of endif

 Revision 1.6  2004/03/29 06:02:18  chaize
 put channel name in capital letters in SRSextupole.c

 Revision 1.5  2003/08/20 10:54:38  chaize
 add DevSetTest command

 Revision 1.4  2003/08/20 08:35:39  chaize
 cleanup makefile

 * Revision 1.3  94/01/31  11:29:16  11:29:16  goetz (Andy Goetz)
 * JMC's version - the correct one
 * 
 * Revision 1.2  94/01/26  19:31:53  19:31:53  goetz (Andy Goetz)
 * added three new fields to the object field.
 * 
 * Revision 1.1  93/03/24  12:54:56  12:54:56  goetz (Andy Goetz)
 * Initial revision
 * 
 * Revision 1.1  90/08/03  10:19:02  10:19:02  goetz (Andy Goetz)
 * Initial revision
 * 


 Copyright (c) 1990 by European Synchrotron Radiation Facility, 
                       Grenoble, France

		       All Rights Reserved

 *********************************************************************/

#ifndef _PowerSupplyP_h
#define _PowerSupplyP_h

typedef struct _PowerSupplyClassPart {
                                       int nada;
                                     }
                PowerSupplyClassPart;

typedef struct _PowerSupplyPart {
                                  DevType type;
                                  float set_val;
                                  float read_val;
                                  int channel;
                                  int n_ave;
                                  int fault_val;
                                  float cal_val;
                                  float conv_val;
                                  char *conv_unit;
                                  float set_offset;
                                  float read_offset;
                                  float set_u_limit;
                                  float set_l_limit;
                                  float idot_limit;
                                  int polarity;
                                  float delta_i;
                                  long time_const;
                                  long last_set_t;
                                }
                PowerSupplyPart;

typedef struct _PowerSupplyClassRec {
                                     DevServerClassPart devserver_class;
                                   }
               PowerSupplyClassRec;

extern PowerSupplyClassRec powerSupplyClassRec;

typedef struct _PowerSupplyRec {
                                 DevServerPart devserver;
                                 PowerSupplyPart powersupply;
                               }
                PowerSupplyRec;

#define DevMethodCheckReadValue 1000
#define DevMethodPSInitialise   1001

#endif /* _PowerSupplyP_h */
