#include <db_xdr.h>
/* Some C++ include files */
#include <string>
#include <dbClass.h>

DBServer	*dbm;

#warning remove the casts if this version is stable and change the db_xdr.h file


/****************************************************************************
*                                                                           *
*		Server code for dbm_close function                          *
*                               --------                                    *
*                                                                           *
*    Function rule : To close the database to be able to reload a new       *
*		     database.						    *
*                                                                           *
*    Argin :  No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to an integer. This integer is simply  *
*    an error code (0 if no error).					    * 
*                                                                           *
****************************************************************************/
int *db_clodb_1_svc(void)
{
    return reinterpret_cast<int *>(dbm->db_clodb_1_svc());
}

/****************************************************************************
*                                                                           *
*		Server code for dbm_reopendb function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : Reopen the database after it has been updated by a     *
*		     dbm_update command in a single user mode or after      *
*		     the rebuilding from a backup file.			    *
*									    *
*                                                                           *
*    Argin :  No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to an integer. This integer is simply  *
*    an error code (0 if no error).					    * 
*                                                                           *
****************************************************************************/
int *db_reopendb_1_svc(void)
{
    return reinterpret_cast<int *>(dbm->db_reopendb_1_svc());
}

/****************************************************************************
*                                                                           *
*		Server code for db_cmd_query function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : To retrieve (from database) the command code           *
*		     associated to a command name (the string)		    *
*                                                                           *
*    Argin : A pointer to string which is the command name		    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "cmd_que" type   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      int db_err;    The database error code                               *
*                        0 if no error                                      *
*      unsigned int xcmd_code;	The command code.			    *
*				0 if the database query fails		    *
*            }                                                              *
*                                                                           *
****************************************************************************/
cmd_que *db_cmd_query_1_svc(nam *pcmd_name)
{
    return dbm->db_cmd_query_1_svc(pcmd_name);
}

/****************************************************************************
*                                                                           *
*	Server code for the devdomainlist_1_svc function       	   	    *
*                           -------------------                       	    *
*                                                                           *
*    Method rule : To device domain list for all the device name defined    *
*		   in the NAMES and PS_NAMES tables			    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *devdomainlist_1_svc(void)
{
    return dbm->devdomainlist_1_svc();
}

/****************************************************************************
*                                                                           *
*	Server code for the devfamilylist_1_svc function    	    	    *
*                           -------------------                    	    *
*                                                                           *
*    Method rule : To device family list for all the device defined in the  *
*		   NAMES and PS_NAMES tables for a given domain		    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*                                                                           *
*    Argout : - family_list : The family name list 			    *
*                                                                           * 
*                                                                           *
****************************************************************************/
db_res *devfamilylist_1_svc(nam * domain)
{
    return dbm->devfamilylist_1_svc(domain);
}

/****************************************************************************
*                                                                           *
*	Server code for the devmemberlist_1_svc fucntion 	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To device member list for all the device defined in the  *
*		   NAMES and PS_NAMES tables for a given domain and family  *
*                                                                           *
*    Argin : - domain : The domain name					    *
*	     - family : The family name					    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *devmemberlist_1_svc(db_res *recev)
{
    return dbm->devmemberlist_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*		Server code for db_event_query function                     *
*                               ------------                                *
*                                                                           *
*    Function rule : To retrieve (from database) the command code           *
*		     associated to a command name (the string)		    *
*                                                                           *
*    Argin : A pointer to string which is the command name		    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "event_que" type *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      int db_err;    The database error code                               *
*                        0 if no error                                      *
*      unsigned int xevent_code;	The command code.		    *
*				0 if the database query fails		    *
*            }                                                              *
*                                                                           *
****************************************************************************/
event_que *db_event_query_1_svc(nam *pevent_name)
{
    return dbm->db_event_query_1_svc(pevent_name);
}

/****************************************************************************
*                                                                           *
*		Server code for db_getdevexp function                       *
*                               -------------                               *
*                                                                           *
*    Function rule : To retrieve the names of the exported devices          *
*                    device server.                                         *
*                                                                           *
*    Argin : A string to filter special device names in the whole list of   *
*	     the exported device					    *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;  A structure of the arr1 type with the device names    *
*      int db_err;    The database error code (0 if no error)               *
*            }                                                              *
*                                                                           *
****************************************************************************/
db_res *db_getdevexp_1_svc(nam *fil_name,struct svc_req *rqstp)
{
    return dbm->db_getdevexp_1_svc(fil_name, rqstp);
}

/****************************************************************************
*                                                                           *
*		Server code for db_dev_export version 1 function            *
*                               -----------------------                     *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number and the version      *
*                    number of the device server for a specific device      *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures bm_devinfo *
*              bm_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*         Zero means no error                                               * 
*                                                                           *
*****************************************************************************/
int *db_devexp_1_svc(tab_dbdev *rece)
{
    return reinterpret_cast<int *>(dbm->db_devexp_1_svc(rece));
}



/****************************************************************************
*                                                                           *
*		Server code for db_dev_export version 2 function            *
*                               -----------------------                     *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number, the version         *
*                    number and the process ID of the device server for a   *
*                    specific device      				    *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures bm_devinfo *
*              bm_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*    Zero means no error                                                    * 
*                                                                           *
*****************************************************************************/
int *db_devexp_2_svc(tab_dbdev_2 *rece)
{
    return reinterpret_cast<int *>(dbm->db_devexp_2_svc(rece));
}

/****************************************************************************
*                                                                           *
*		Server code for db_dev_export version 3 function            *
*                               -----------------------                     *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number, the version         *
*                    number and the process ID of the device server for a   *
*                    specific device      				    *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures bm_devinfo *
*              bm_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*    Zero means no error                                                    * 
*                                                                           *
*****************************************************************************/
int *db_devexp_3_svc(tab_dbdev_3 *rece)
{
    return reinterpret_cast<int *>(dbm->db_devexp_3_svc(rece));
}

/****************************************************************************
*                                                                           *
*		Server code for db_dev_import function                      *
*                               -------------                               *
*                                                                           *
*    Function rule : To retrieve (from database) the host_name, the program *
*                    number and the version number for specific devices     *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*               u_int arr1_len;      The number of strings                  *
*               char **arr1_val;     A pointer to the array of strings      *
*                   }                                                       *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_resimp" type *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      tab_dbdev imp_dev;   A structure of the tab_dbdev type (see above)   *
*                           with the informations needed (host_name,        *
*                           program number and version number)              *
*      int db_imperr;    The database error code                            *
*                        0 if no error                                      *
*            }                                                              *
*                                                                           *
****************************************************************************/
db_resimp *db_devimp_1_svc(arr1 *de_name)
{
    return dbm->db_devimp_1_svc(de_name);
}

/****************************************************************************
*                                                                           *
*		Server code for db_svc_unreg function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : To unregister from database all the devices driven by  *
*                    a device server                                        *
*                                                                           *
*    Argin : The network device server name                                 *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an int which is an error code                    *
*       Zero means no error                                                 *
*                                                                           *
****************************************************************************/
int *db_svcunr_1_svc(nam *dsn_name)
{
    return reinterpret_cast<int *>(dbm->db_svcunr_1_svc(dsn_name));
}

/****************************************************************************
*                                                                           *
*		Server code for db_svc_check function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : To retrieve (and send back to client) the program      *
*                    number and version number for a device server          *
*                                                                           *
*    Argin : The network device server name                                 *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a stucture of the "svc_inf" type    *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*	char *ho_name;  host name					    *	
*   	u_int p_num;	the program number                                  *
*       u_int v_num;    the version number                                  *
*       int db_err;     an error code (0 if no error)                       *
*           }                                                               *
*                                                                           *
****************************************************************************/
svc_inf *db_svcchk_1_svc(nam *dsn_name)
{
    return dbm->db_svcchk_1_svc(dsn_name);
}

/****************************************************************************
*                                                                           *
*	Server code for the devserverlist_1_svc function       	   	    *
*                           -------------------                       	    *
*                                                                           *
*    Method rule : To retrieve device server list for all the devices       *
*		   defined in the NAMES table			  	    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *devserverlist_1_svc(void)
{
    return dbm->devserverlist_1_svc();
}

/****************************************************************************
*                                                                           *
*	Server code for the devpersnamelist_1_svc function    	    	    *
*                           ---------------------                    	    *
*                                                                           *
*    Method rule : To device family list for all the device defined in the  *
*		   NAMES and PS_NAMES tables for a given domain		    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*                                                                           *
*    Argout : - family_list : The family name list 			    *
*                                                                           * 
*                                                                           *
****************************************************************************/
db_res *devpersnamelist_1_svc(nam *server)
{
    return dbm->devpersnamelist_1_svc(server);
}

/****************************************************************************
*                                                                           *
*	Server code for the hostlist_1_svc function       	   	    *
*                           --------------                       	    *
*                                                                           *
*    Method rule : To device host list where device server should run       *
*		   in the NAMES table			  		    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *hostlist_1_svc()
{
    return dbm->hostlist_1_svc();
}

/****************************************************************************
*                                                                           *
*		Server code for db_psdev_register function                  *
*                               -----------------            	            *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number and the version      *
*                    number of the device server for a specific device      *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures db_devinfo *
*              db_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*         Zero means no error                                               * 
*                                                                           *
*****************************************************************************/
db_psdev_error *db_psdev_reg_1_svc(psdev_reg_x *rece)
{
    return dbm->db_psdev_reg_1_svc(rece);
}

/****************************************************************************
*                                                                           *
*		Server code for db_psdev_unregister function                *
*                               -------------------                         *
*                                                                           *
*    Function rule : To retrieve (from database) the host_name, the program *
*                    number and the version number for specific devices     *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*               u_int arr1_len;      The number of strings                  *
*               char **arr1_val;     A pointer to the array of strings      *
*                   }                                                       *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_resimp" type *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      tab_dbdev imp_dev;   A structure of the tab_dbdev type (see above)   *
*                           with the informations needed (host_name,        *
*                           program number and version number)              *
*      int db_imperr;    The database error code                            *
*                        0 if no error                                      *
*            }                                                              *
*                                                                           *
****************************************************************************/
db_psdev_error *db_psdev_unreg_1_svc(arr1 *rece)
{
    return dbm->db_psdev_unreg_1_svc(rece);
}

/****************************************************************************
*                                                                           *
*	Server code for the resdomainlist_1_svc function     	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrive resource domain list for all the resources    *
*		   defined in the database				    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *                                                          *
*                                                                           *
****************************************************************************/
db_res *resdomainlist_1_svc(void)
{
    return dbm->resdomainlist_1_svc();
}

/****************************************************************************
*                                                                           *
*	Server code for the resfamilylist_1_svc function     	   	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrieve all the family defined (in resources name)   *
*		   for a specific domain				    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*                                                                           *
*    Argout : - family_list : The family name list 			    *
*                                                                           * 
*                                                                           *
****************************************************************************/
db_res *resfamilylist_1_svc(nam* domain)
{
    return dbm->resfamilylist_1_svc(domain);
}

/****************************************************************************
*                                                                           *
*	Server code for the resmemberlist_1_svc function 	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrieve all the family defined (in resources name)   *
*		   for a specific couple domain,family			    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*	     - family : The family name					    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *resmemberlist_1_svc(db_res *recev)
{
    return dbm->resmemberlist_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*	Server code for the resresolist_1_svc function 	    	    	    *
*                           -----------------                     	    *
*                                                                           *
*    Method rule : To retrieve a list of resources for a given device	    *
*                                                                           *
*    Argin : - domain : The device domain name				    *
*	     - family : The device family name				    *
*	     - member : The device member name				    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                    	    *
*                                                                           *
****************************************************************************/
db_res *resresolist_1_svc(db_res *recev)
{
    return dbm->resresolist_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*	Server code for the resresoval_1_svc function     	    	    *
*                           ----------------                     	    *
*                                                                           *
*    Method rule : To retrieve a resource value (as strings)		    *
*                                                                           *
*    Argin : - domain : The device domain name				    *
*	     - family : The device family name				    *
*	     - member : The device member name				    *
*	     - resource : The resource name				    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                    	    *
*                                                                           *
****************************************************************************/
db_res *resresoval_1_svc(db_res *recev)
{
    return dbm->resresoval_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*		Server code for db_getresource function                     *
*                               --------------                              *
*                                                                           *
*    Function rule : To retrieve from the database (builded from resources  *
*		     files) a resource value                                *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*              u_int arr1_len;     The number of strings                    *
*              char **arr1_val;    A pointer to the array of strings        *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;   A structure of the arr1 type (see above) with the    *
*                     resources values information transferred as strings   *
*      int db_err;    The database error code                               *
*                     0 if no error                                         *
*          }                                                                *
*                                                                           *
*****************************************************************************/
db_res *db_getres_1_svc(arr1 *rece, struct svc_req *rqstp)
{
    return dbm->db_getres_1_svc(rece, rqstp);
}

/****************************************************************************
*                                                                           *
*		Server code for db_getdevlist function                      *
*                               -------------                               *
*                                                                           *
*    Function rule : To retrieve all the names of the devices driven by a   *
*                    device server.                                         *
*                                                                           *
*    Argin : The name of the device server                                  *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;   A structure of the arr1 type (see above) with the    *
*                     devices names                                         *
*      int db_err;    The database error code                               *
*                     0 if no error                                         *
*            }                                                              *
*                                                                           *
****************************************************************************/
db_res *db_getdev_1_svc(nam *dev_name)
{
    return dbm->db_getdev_1_svc(dev_name);
}

/****************************************************************************
*                                                                           *
*		Server code for db_putresource function                     *
*                               --------------                              *
*                                                                           *
*    Function rule : To insert or update resources    		    	    *
*                                                                           *
*    Argin : A pointer to a structure of the tab_putres type		    *
*            The definition of the tab_putres type is :                     *
*	     struct {							    *
*		u_int tab_putres_len;	The number of resources to be       *
*					updated or inserted		    *
*		putres tab_putres_val;	A pointer to an array of putres     *
*					structure. Each putres structure    *
*					contains the resource name and      *
*                                       the resource value		    *
*		    }							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This funtion returns 0 if no error occurs. Otherwise an error code is  *
*    returned								    *
*                                                                           *
****************************************************************************/
int *db_putres_1_svc(tab_putres *rece)
{
    return reinterpret_cast<int *>(dbm->db_putres_1_svc(rece));
}

/****************************************************************************
*                                                                           *
*		Server code for db_delresource function                     *
*                               --------------                              *
*                                                                           *
*    Function rule : To delete resources from the database (builded from    *
*		     resource files)					    *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*              u_int arr1_len;     The number of strings                    *
*              char **arr1_val;    A pointer to the array of strings        *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a int. This int is the error code   *
*    It is set to 0 is everything is correct. Otherwise, it is initialised  *
*    with the error code.						    *
*                                                                           *
*****************************************************************************/
int *db_delres_1_svc(arr1 *rece, struct svc_req *rqstp)
{
    return reinterpret_cast<int *>(dbm->db_delres_1_svc(rece));
}

/****************************************************************************
*                                                                           *
*	Server code for the devinfo_1_svc function       	   	    *
*                           -------------                       	    *
*                                                                           *
*    Method rule : To return device information 			    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                           *
*    This function returns a pointer to a structure with all device info    *
*    and an error code which is set if needed				    *
*                                                                           *
****************************************************************************/
db_devinfo_svc *devinfo_1_svc(nam *dev)
{
    return dbm->devinfo_1_svc(dev);
}

/****************************************************************************
*                                                                           *
*	Server code for the devres_1_svc function 	    	    	    *
*                           ------------                     	    	    *
*                                                                           *
*    Method rule : To retrieve all resources belonging to a device	    *
*                                                                           *
*    Argin : - name : The device name				    	    *
*                                                                           *
*    Argout : No argout							    *
*                                                                    	    *
*    This function returns the resource list and an error code which is set *
*    if needed								    *
*                                                                           *
****************************************************************************/
db_res *devres_1_svc(db_res *recev)
{
    return dbm->devres_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*	Server code for the devdel_1_svc function       	   	    *
*                           ------------                       	    	    *
*                                                                           *
*    Method rule : To delete a device from the database			    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to an error code			    *
*                                                                           *
****************************************************************************/
DevLong *devdel_1_svc(nam *dev)
{
    return dbm->devdel_1_svc(dev);
}

/****************************************************************************
*                                                                           *
*	Server code for the devdelres_1_svc function       	   	    *
*                           ---------------                       	    *
*                                                                           *
*    Method rule : To delete all the resources belonging to a device	    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to an error code			    *
*                                                                           *
****************************************************************************/
db_psdev_error *devdelres_1_svc(db_res *recev)
{
    return dbm->devdelres_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*	Server code for the info_1_svc function       	   	    	    *
*                           ----------                       	    	    *
*                                                                           *
*    Method rule : To get global information on the database		    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to a structure with all the database   *
*    info								    *
*                                                                           *
****************************************************************************/
db_info_svc *info_1_svc(void)
{
    return dbm->info_1_svc();
}

/****************************************************************************
*                                                                           *
*	Server code for the unreg_1_svc function     	    	    	    *
*                           -----------                     	    	    *
*                                                                           *
*    Method rule : To unregister a server from the database (to mark all its*
*		   devices as not exported)				    *
*                                                                           *
*    Argin : - recev : A pointer to a structure where the first element     *
*		       is the device server name and the second element     *
*		       is the device server personal name.		    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                    	    *
*    This function returns a pointer to a long which will be set in case of *
*    error								    *
*                                                                           *
****************************************************************************/
DevLong *unreg_1_svc(db_res *recev)
{
    return dbm->unreg_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*	Server code for the svcinfo_1_svc function     	    	    	    *
*                           -------------                     	    	    *
*                                                                           *
*    Method rule : To return device server info to client		    *
*                                                                           *
*    Argin : - recev : A pointer to a structure where the first element     *
*		       is the device server name and the second element     *
*		       is the device server personal name.		    *
*                                                                           *
*    argout : - no argout						    *
*                                                                    	    *
*    this function returns a pointer to a structure with all the device     *
*    server information							    *
*                                                                           *
****************************************************************************/
svcinfo_svc *svcinfo_1_svc(db_res *recev)
{
	std::cerr << __FUNCTION__ << std::endl;
    return dbm->svcinfo_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*	Server code for the svcdelete_1_svc function     	    	    *
*                           ---------------                     	    *
*                                                                           *
*    Method rule : To return delete all the device belonging to a device    *
*		   server and if necessary also their resources		    *
*                                                                           *
*    Argin : - recev : A pointer to a structure where the first element     *
*		       is the device server name and the second element     *
*		       is the device server personal name.		    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                    	    *
*    This function returns a pointer to a structure with all the device     *
*    server information							    *
*                                                                           *
****************************************************************************/
DevLong *svcdelete_1_svc(db_res *recev)
{
    return dbm->svcdelete_1_svc(recev);
}

/****************************************************************************
*                                                                           *
*	Server code for the getpoller_1_svc function       	   	    *
*                           ---------------                       	    *
*                                                                           *
*    Method rule : To retrieve poller info from a device name	    	    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to an error code			    *
*                                                                           *
****************************************************************************/
db_poller_svc *getpoller_1_svc(nam *dev)
{
    return dbm->getpoller_1_svc(dev);
}

/****************************************************************************
*                                                                           *
*	Server code for the upddev_1_svc function       	   	    *
*                           ------------                       	    	    *
*                                                                           *
*    Method rule : To update device list(s)				    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                           *
*    This function returns a pointer to a structure with all device info    *
*    and an error code which is set if needed				    *
*                                                                           *
****************************************************************************/
db_psdev_error *upddev_1_svc(db_res *dev_list)
{
    return dbm->upddev_1_svc(dev_list);
}

/****************************************************************************
*                                                                           *
*	Server code for the updres_1_svc function       	   	    *
*                           ------------                       	    	    *
*                                                                           *
*    Method rule : To update resource(s)				    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                           *
*    This function returns a pointer to a structure with all device info    *
*    and an error code which is set if needed				    *
*                                                                           *
****************************************************************************/
db_psdev_error *updres_1_svc(db_res *res_list)
{
    return dbm->updres_1_svc(res_list);
}

/****************************************************************************
*                                                                           *
*	Server code for the secpass_1_svc function       	   	    *
*                           -------------                       	    *
*                                                                           *
*    Method rule : To device domain list for all the device name defined    *
*		   in the NAMES and PS_NAMES tables			    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *secpass_1_svc(void)
{
    return dbm->secpass_1_svc();
}
