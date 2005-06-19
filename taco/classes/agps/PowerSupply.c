static char RcsId[] = " $Header: /segfs/dserver/dev/classes/powersupply/ag/src/RCS/PowerSupply.c,v 1.1 2003/12/10 20:57:52 goetz Exp $ ";

/*********************************************************************

 File:		PowerSupply.c

 Project:	Device Servers

 Description:	Source code to implement PowerSupplyClass. This class
		is at present a placeholder for its subclasses. It will
		be used in the future for implementing any common methods
		which could be required for subclasses of the PowerSupply
		Class. At present there are no common methods, powersupplies
		only share a common data structure.
		

 Author(s);	Andy Goetz

 Original:	October 1990

 $Log: PowerSupply.c,v $
 Revision 1.1  2003/12/10 20:57:52  goetz
 Initial revision

 Revision 1.8  2003/08/20 10:54:35  chaize
 add DevSetTest command

 Revision 1.7  2003/08/20 08:33:16  chaize
 cleanup makefile

 Revision 1.6  1997/01/08 15:19:30  goetz
 added conditional statements around HWALK() so that it is defined for NT only

 * Revision 1.5  96/09/17  17:18:07  17:18:07  klotz (W.D. Klotz)
 * WD Klotz: checked NT build
 * 
 * Revision 1.3  94/01/31  11:27:27  11:27:27  goetz (Andy Goetz)
 * JMC's version - the correct one
 * ,.
 * 
 * Revision 1.2  94/01/26  19:29:28  19:29:28  goetz (Andy Goetz)
 * added resource initialising in the DevMethodInitialise and a method for 
 * comparing the read and set values and return a boolean.
 * 
 * Revision 1.1  94/01/12  16:45:48  16:45:48  goetz (Andy Goetz)
 * Initial revision
 * 
 * Revision 1.1  92/04/01  14:10:45  14:10:45  goetz (Andy Goetz)
 * Initial revision
 * 
 * Revision 1.3  90/11/15  09:51:09  09:51:09  goetz (Andy Goetz)
 * *** empty log message ***
 * 
 * Revision 1.2  90/11/02  15:40:11  15:40:11  goetz (Andy Goetz)
 * object_create, class_initialise, and state_handler now really static
 * 
 * Revision 1.1  90/11/02  11:09:01  11:09:01  goetz (Andy Goetz)
 * Initial revision
 * 

 Copyleft (c) 1990 by European Synchrotron Radiation Facility, 
                      Grenoble, France



 *********************************************************************/

#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <Admin.h>

#include <DevServerP.h>
#include <PowerSupplyP.h>
#include <PowerSupply.h>

#include <math.h>

#define MAX_NOISE 0.1
/*
 * public methods
 */

static long class_initialise();
static long object_create();
static long object_initialise();
static long state_handler();
static long check_read_value();

static DevMethodListEntry methods_list[] = {
 {DevMethodClassInitialise, class_initialise},
 {DevMethodCreate, object_create},
 {DevMethodPSInitialise, object_initialise},
 {DevMethodStateHandler, state_handler},
 {DevMethodCheckReadValue, check_read_value},
};

PowerSupplyClassRec powerSupplyClassRec = {
   /* n_methods */        sizeof(methods_list)/sizeof(DevMethodListEntry),
   /* methods_list */     methods_list,
};

PowerSupplyClass powerSupplyClass = (PowerSupplyClass)&powerSupplyClassRec;

/*
 * public commands
 */

static DevCommandListEntry commands_list[]={0,NULL,0,0,0};

static long n_commands = 0;

/*
 * reserve space for a default copy of the powerSupply object
 */

static PowerSupplyRec powerSupplyRec;
static PowerSupply powerSupply = (PowerSupply)&powerSupplyRec;

/*
 * databases resources initialised by the PowerSupply class
 */
static db_resource res_powersupply[] = {
                {"delta_i", D_FLOAT_TYPE},
                {"time_constant", D_LONG_TYPE},
                                   };

static int res_powersupply_size = sizeof(res_powersupply)/sizeof(db_resource);

/********************************/
/* PowerSupply class initialise */
/********************************/

static long class_initialise(error)
long *error;
{
   int iret = 0;

/*
 * PowerSupplyClass is a subclass of the DevServerClass
 */

   powerSupplyClass->devserver_class.superclass = devServerClass;
   powerSupplyClass->devserver_class.class_name = (char*)malloc(sizeof("PowerSupplyClass"));
#ifdef _NT
   HWALK(powerSupplyClass->devserver_class.class_name);
#endif
   sprintf(powerSupplyClass->devserver_class.class_name,"PowerSupplyClass");
   powerSupplyClass->devserver_class.class_inited = 1;

/*
 * initialise powerSupply with default values. these will be used
 * for every PowerSupply object created.
 */

   powerSupply->devserver.class_pointer = (DevServerClass)powerSupplyClass;

   return(iret);
}

/*****************************/
/* PowerSupply create method */
/*****************************/

static long object_create(name, ds_ptr, error)
char *name;
DevServer *ds_ptr;
long *error;
{
   int iret = 0;
   PowerSupply ps;

   ps = (PowerSupply)malloc(sizeof(PowerSupplyRec));
#ifdef _NT
   HWALK(ps);
#endif

/*
 * initialise device with default object
 */

   *(PowerSupplyRec*)ps = *(PowerSupplyRec*)powerSupply;

/*
 * finally initialise the non-default values
 */

   ps->devserver.name = (char*)malloc(strlen(name));
#ifdef _NT
   HWALK(ps->devserver.name);
#endif
   sprintf(ps->devserver.name,"%s",name);

   *ds_ptr = (DevServer)ps;

   return(iret);
}

/****************************************/
/* PowerSupply object initialise method */
/****************************************/

static long object_initialise(ds, error)
PowerSupply ds;
long *error;
{
/*
 * this method does only one thing at present and that is initialise
 * a powersupply type object with two values from the database
 */
   register int ires;

   ires = 0;
   res_powersupply[ires].resource_adr = &(ds->powersupply.delta_i);
   ires++;
   res_powersupply[ires].resource_adr = &(ds->powersupply.time_const);

   if (db_getresource(ds->devserver.name,res_powersupply,res_powersupply_size,error))
   {
      printf("powersupply::object_initialise(): db_getresource() failed, erro %d\n",error);
      return(DS_NOTOK);
   }

   return(DS_OK);
}


/***********************************/
/* PowerSupply class state_handler */
/***********************************/

static long state_handler( ds, cmd, error)
PowerSupply ds;
DevCommand cmd;
long *error;
{
    
/*
 * does nothing for the moment
 */

   return(0);
}

/***************************************/
/* PowerSupply check_read_value method */
/***************************************/

static long check_read_value(ps,alarm,error)
PowerSupply ps;
DevBoolean *alarm;
long *error;
{
/*
 * this method will check wether the read value lies within the
 * tolerance of the desired set value - if it doesn't then it
 * returns an alarm in the boolean value alarm. this method expects 
 * the first parameter will be a PowerSupply object. the second
 * parameter will be the alarm flag to return.
 *
 * it assumes that the powersupply has been read recently and that
 * all information necessary for doing the check is passed via the
 * object itself
 */

   time_t now;
   float delta;

/*
 * first assume there is no alarm condition
 */
   *alarm = 0;
/*
 * only do the check if the delta_i value is initialised to a +ve value
 */
   if (ps->powersupply.delta_i > 0)
   {
/*
 * first check the time when the last set value was done is more than
 * the time_const time away
 */
      time(&now);

      if ((now - ps->powersupply.last_set_t) > ps->powersupply.time_const)
       
      {
         delta = ps->powersupply.set_val - ps->powersupply.read_val;
         if (fabs(delta) > ps->powersupply.delta_i)
         {
            *alarm = 1;
         }
      }
   }
   return(DS_OK);
}
