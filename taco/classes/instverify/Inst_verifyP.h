
/*********************************************************************
 *
 * File:	Inst_verifyP.h
 *
 * Project:	Device Server Distribution
 *
 * Description:	Dummy class to test control system installations
 *
 * Author(s):	J.Meyer
 *
 * Original:	10.5.95	
 *
 * Copyright (c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#ifndef _INST_VERIFYP_H
#define _INST_VERIFYP_H



typedef struct _Inst_verifyClassPart
{
		DevLong	config_param; /* Configuration parameter for hardware */
} Inst_verifyClassPart;



typedef struct _Inst_verifyPart
{
		DevFloat	read_value; /* Read value of device */
		DevFloat	set_value; /* Set value of the device */
} Inst_verifyPart;



typedef struct _Inst_verifyClassRec
{
   DevServerClassPart	devserver_class;
				
   Inst_verifyClassPart	inst_verify_class;
} Inst_verifyClassRec;



extern Inst_verifyClassRec inst_verifyClassRec;


typedef struct _Inst_verifyRec
{
   DevServerPart 	devserver;
				
   Inst_verifyPart 	inst_verify;
} Inst_verifyRec;

#endif /*_INST_VERIFYP_H*/

