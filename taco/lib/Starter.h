
/*********************************************************************

 File:		Starter.h

 Project:	Device Servers

 Description:	Starter relevant operation class
		

 Author(s);	Emmanuel TAUREL

 Original:	january 04 1995

 Copyright(c) 1990 by European Synchrotron Radiation Facility, 
                      Grenoble, France

 *********************************************************************/

#ifndef _STARTER_H
#define _STARTER_H


typedef struct _StarterClassRec *StarterClass;
typedef struct _StarterRec *Starter;

extern StarterClass starterClass;
extern Starter starter;

#define DevErr_NoProcessWithPid 9701
#define DevErr_CantFindExecutable 9702
#define DevErr_CantKillProcess 9703
#endif /* _STARTER_H */

