/* 
 * File	: 	db_io_put.c
 *
 * Author(s): 	Denis Beauvois
 * 		Gilbert Pepellin
 * 		Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:   	june, 1996
 *
 * Description: This file contains the necessary function for putting
 *  		informations in the database of the esrf
 *  		This function is added to the tcl interpreter
 *
 * Version:	$Revision: 1.1 $
 *
 * Date:	$Date: 2003-03-18 16:28:25 $
 *
 */
 

/*SUPPRESS763*/
#include <db_io.h>
#include <dev_io.h>
#include <debug.h>


/* ------------------------------------------- */
/* Function db_io_put                          */
/* ------------------------------------------- */

int db_io_put (clientdata, interp, argc, argv)
/*SUPPRESS761*/
               ClientData clientdata;
               Tcl_Interp *interp;
               int argc;
               char *argv[];
{


 /* db_putresource() interface */
 db_resource tabres; /* only one structure item */
 unsigned int res_num; /* will be initialized to 1 */
 long db_error;

 DevVarStringArray res_adr; /* only one item because only one resource */
 

 /* local variables that hold the arguments of db_io_put*/
 char *State, *Error, *DeviceName, *ResourceName, *NewVal;
 
 int nbval;
 char **Lval;  /* will be allocated by Tcl_SplitList. Need to be freeed */

 /* some working variables*/
 int ret;
  
 /* --------------------------- */
 /* Setting debug mode          */
 /* --------------------------- */
 if (GetDBDebugMode() != DB_DEBUG_0) TCL_TEST=True;
 
 /* ------------------------------*/
 /* Parse argument line           */
 /* ------------------------------*/ 
 if (TCL_TEST) {printf ("db_io_put. entering...\n");}
 if (TCL_TEST) {printf ("db_io_put. Start argument line parsing\n");}
 /* Usage: db_io_put status error dev_name res_name new_value */
 if (argc != 6) /*function name "db_io_put is counted as argument */
    {
		 if (TCL_TEST) {printf("Bad argument number. should be 6\n");}
     return TCL_ERROR;
    }
 

 State        = argv[1];
 Error        = argv[2];
 DeviceName   = argv[3];
 ResourceName = argv[4];
 NewVal       = argv[5];

 if (TCL_TEST) {printf ("db_io_put. Parsing argument line...DONE\n");}
 
 if (TCL_TEST) printf ("db_io_put. Initialization of dynamical variables...\n");
 
 Tcl_ResetResult(interp);
 Tcl_SetVar(interp,State,"0",0);
 Tcl_SetVar(interp,Error," ",0);

 ret = Tcl_SplitList(interp,NewVal,&nbval,&Lval);
 if(TCL_TEST) printf("db_io_put. Tcl_SplitList returned %d\n",ret);
 if(ret != TCL_OK) {
   printf("db_io_get. +++++++ Syntax error in res list\n");
 }
  
/* nbval is the number of values to be set for the resource ResourceName */
/* Lval[0] -> Lval[nbval -1 ] are these values */

/* Call db_putresource */
  res_adr.length   = nbval;
	res_adr.sequence = Lval;
	tabres.resource_name = ResourceName;
	tabres.resource_type = D_VAR_STRINGARR;
	tabres.resource_adr  = &res_adr;
	res_num = 1;
	ret = db_putresource(DeviceName, &tabres, res_num, &db_error);
	if(TCL_TEST) printf("db_io_put. db_putresource returned %d\n",ret);
	
	Tcl_Free( (char*)Lval);
return TCL_OK;

}
 
 
    
  
                                    
       
    
 
 
