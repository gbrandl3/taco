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
 * File:
 *
 * Description:
 *
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2005-07-25 08:48:43 $
 *
 */

#include "config.h"
#include <db_xdr.h>
/* Some C++ include files */
#include <string>
#include <dbClass.h>

DBServer	*dbm;

/**@defgroup dbServer The database server
 */

/**@defgroup dbServerInterface The database server interface
 * @ingroup dbServer
 */

/**@defgroup dbServerClasses The internal database server classes
 * @ingroup dbServer
 */

/**@ingroup dbServerInterface
 * To close the database to be able to reload a new database.
 * 
 * @return a pointer to an integer. This integer is simply an error code (0 if no error).
 */
int *db_clodb_1_svc(void)
{
	return reinterpret_cast<int *>(dbm->db_clodb_1_svc());
}

/**@ingroup dbServerInterface
 * Reopen the database after it has been updated by a dbm_update command in a single user 
 * mode or after the rebuilding from a backup file.
 * 
 * @return a pointer to an integer. This integer is simply an error code (0 if no error).
 */
int *db_reopendb_1_svc(void)
{
	return reinterpret_cast<int *>(dbm->db_reopendb_1_svc());
}

/**@ingroup dbServerInterface
 * To retrieve (from database) the command code associated to a command name (the string)
 * 
 * @param pcmd_name A pointer to string which is the command name
 * 
 * @returns  pointer to a structure of the cmd_que type, containing the command and error code
 */
cmd_que *db_cmd_query_1_svc(nam *pcmd_name)
{
	return dbm->db_cmd_query_1_svc(pcmd_name);
}

/**@ingroup dbServerInterface
 * list domain for all the device name defined in the NAMES and PS_NAMES tables
 * 
 * @return The domain name list
 */
db_res *devdomainlist_1_svc(void)
{
	return dbm->devdomainlist_1_svc();
}

/**@ingroup dbServerInterface
 * List the families for all the device defined in the NAMES and PS_NAMES tables for a given domain
 * 
 * @param domain The domain name
 *
 * @return The family name list
 */
db_res *devfamilylist_1_svc(nam * domain)
{
	return dbm->devfamilylist_1_svc(domain);
}

/**@ingroup dbServerInterface
 * List the members for all the devices defined in the NAMES and PS_NAMES tables for 
 * a given domain and family  
 * 
 * @param recev The domain name, the family name
 * 
 * @return The member name list
 */
db_res *devmemberlist_1_svc(db_res *recev)
{
	return dbm->devmemberlist_1_svc(recev);
}

/**@ingroup dbServerInterface
 * To retrieve (from database) the event code associated to a event name (the string)
 * 
 * @param pevent_name A pointer to string which is the event name
 *
 * @return a pointer to a structure of the event_que type 
 */
event_que *db_event_query_1_svc(nam *pevent_name)
{
	return dbm->db_event_query_1_svc(pevent_name);
}

/**@ingroup dbServerInterface
 * To retrieve the names of the exported devices device server. 
 *
 * @param fil_name A string to filter special device names in the whole list of  
 *	     the exported device
 * @param rqstp
 * 
 * @return a pointer to a structure of the db_res type. 
 */
db_res *db_getdevexp_1_svc(nam *fil_name,struct svc_req *rqstp)
{
	return dbm->db_getdevexp_1_svc(fil_name, rqstp);
}

/**@ingroup dbServerInterface
 * To store in the database (built from resources files) the host_name, the program 
 * number and the version number of the device server for a specific device
 * 
 * @param rece A pointer to a structure of the tab_dbdev type
 *
 * @returns an integer which is an error code. Zero means no error 
 */
int *db_devexp_1_svc(tab_dbdev *rece)
{
	return reinterpret_cast<int *>(dbm->db_devexp_1_svc(rece));
}



/**@ingroup dbServerInterface
 * To store in the database (built from resources files) the host_name, the program 
 * number, the version number and the process ID of the device server for a specific device
 * 
 * @param rece A pointer to a structure of the tab_dbdev_2 type 
 * 
 * @return an integer which is an error code. Zero means no error.
 */
int *db_devexp_2_svc(tab_dbdev_2 *rece)
{
	return reinterpret_cast<int *>(dbm->db_devexp_2_svc(rece));
}

/**@ingroup dbServerInterface
 * To store in the database (built from resources files) the host_name, the program number, 
 * the version number and the process ID of the device server for a specific device
 *
 * @param rece A pointer to a structure of the tab_dbdev_3 type
 *
 * @return an integer which is an error code. Zero means no error
 */
int *db_devexp_3_svc(tab_dbdev_3 *rece)
{
    return reinterpret_cast<int *>(dbm->db_devexp_3_svc(rece));
}

/**@ingroup dbServerInterface
 * To retrieve (from database) the host_name, the program number and the version 
 * number for specific devices 
 * 
 * @param de_name A pointer to a structure of the arr1 type 
 *
 * @returns a pointer to a structure of the db_resimp type 
 */
db_resimp *db_devimp_1_svc(arr1 *de_name)
{
	return dbm->db_devimp_1_svc(de_name);
}

/**@ingroup dbServerInterface
 * unregister from database all the devices driven by a device server 
 * 
 * @param dsn_name The network device server name 
 * 
 * @return an int which is an error code. Zero means no error
 */
int *db_svcunr_1_svc(nam *dsn_name)
{
	return reinterpret_cast<int *>(dbm->db_svcunr_1_svc(dsn_name));
}

/**@ingroup dbServerInterface
 * To retrieve (and send back to client) the program number and version number 
 * for a device server 
 * 
 * @param dsn_name The network device server name 
 * 
 * @return a pointer to a stucture of the svc_inf type 
 */
svc_inf *db_svcchk_1_svc(nam *dsn_name)
{
	return dbm->db_svcchk_1_svc(dsn_name);
}

/**@ingroup dbServerInterface
 * To retrieve device server list for all the devices defined in the NAMES table
 *
 * @return The domain name list
 */
db_res *devserverlist_1_svc(void)
{
	return dbm->devserverlist_1_svc();
}

/**@ingroup dbServerInterface
 * list all personal names for a given server
 * 
 * @param server The server name
 *
 * @return The personal name list
 */
db_res *devpersnamelist_1_svc(nam *server)
{
	return dbm->devpersnamelist_1_svc(server);
}

/**@ingroup dbServerInterface
 * list the hosts where device server should run 
 * 
 * @return The host name list
 */
db_res *hostlist_1_svc()
{
	return dbm->hostlist_1_svc();
}

/**@ingroup dbServerInterface
 * To store in the database (built from resources files) the host_name, the program 
 * number and the version number of the device server for a specific pseudo device 
 * 
 * @param rece A pointer to a structure of the psdev_reg_x type
 * 
 * @return a pointer to the error code 
 */
db_psdev_error *db_psdev_reg_1_svc(psdev_reg_x *rece)
{
	return dbm->db_psdev_reg_1_svc(rece);
}

/**@ingroup dbServerInterface
 * unregister a list of specified pseudo devices.
 *
 * @param rece A pointer to a structure of the arr1 type
 *
 * @return a pointer to the error code 
 */
db_psdev_error *db_psdev_unreg_1_svc(arr1 *rece)
{
	return dbm->db_psdev_unreg_1_svc(rece);
}

/**@ingroup dbServerInterface
 * retrieve resource domain list for all the resources defined in the database
 * 
 * @return The domain name list
 */
db_res *resdomainlist_1_svc(void)
{
	return dbm->resdomainlist_1_svc();
}

/**@ingroup dbServerInterface
 * retrieve all the family defined (in resources name) for a specific domain
 * 
 * @param domain The domain name
 *
 * @return The family name list
 */
db_res *resfamilylist_1_svc(nam* domain)
{
	return dbm->resfamilylist_1_svc(domain);
}

/**@ingroup dbServerInterface
 * retrieve all the member defined (in resources name) for a specific couple domain,family
 *
 * @param recev The domain name, the family name
 * 
 * @return The member name list
 */
db_res *resmemberlist_1_svc(db_res *recev)
{
	return dbm->resmemberlist_1_svc(recev);
}

/**@ingroup dbServerInterface
 * retrieve a list of resources for a given device
 *
 * @param recev The device domain name, the device family name, the device member name
 *
 * @return The resource name list
 */
db_res *resresolist_1_svc(db_res *recev)
{
	return dbm->resresolist_1_svc(recev);
}

/**@ingroup dbServerInterface
 * retrieve a resource value (as strings)
 *
 * @param recev The device domain name, the device family name, the device member name,
 *	     the resource name
 * 
 * @return The resource value list
 */
db_res *resresoval_1_svc(db_res *recev)
{
	return dbm->resresoval_1_svc(recev);
}

/**@ingroup dbServerInterface
 * retrieve from the database (built from resources files) a resource value
 *
 * @param rece A pointer to a structure of the arr1 type
 * @param rqstp
 *
 * @return a pointer to a structure of the db_res type.
 */
db_res *db_getres_1_svc(arr1 *rece, struct svc_req *rqstp)
{
	return dbm->db_getres_1_svc(rece, rqstp);
}

/**@ingroup dbServerInterface
 * retrieve all the names of the devices driven by a device server. 
 * 
 * @param dev_name The name of the device server
 * 
 * @return a pointer to a structure of the db_res type.
 */
db_res *db_getdev_1_svc(nam *dev_name)
{
	return dbm->db_getdev_1_svc(dev_name);
}

/**@ingroup dbServerInterface
 * insert or update resources
 *
 * @param rece A pointer to a structure of the tab_putres type
 *
 * @return 0 if no error occurs. Otherwise an error code is returned
 */
int *db_putres_1_svc(tab_putres *rece)
{
	return reinterpret_cast<int *>(dbm->db_putres_1_svc(rece));
}

/**@ingroup dbServerInterface
 * delete resources from the database (built from resource files)
 *
 * @param rece A pointer to a structure of the arr1 type
 * @param rqstp
 * 
 * @return a pointer to a int. This int is the error code. It is set to 0 is everything 
 * is correct. Otherwise, it is initialised with the error code.
 */
int *db_delres_1_svc(arr1 *rece, struct svc_req *rqstp)
{
	return reinterpret_cast<int *>(dbm->db_delres_1_svc(rece));
}

/**@ingroup dbServerInterface
 * retrieve device informations from the database
 * 
 * @param dev The device name
 *
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_devinfo_svc *devinfo_1_svc(nam *dev)
{
	return dbm->devinfo_1_svc(dev);
}

/**@ingroup dbServerInterface
 * To retrieve all resources belonging to a device
 * 
 * @param recev The device name
 *
 * @return the resource list and an error code which is set if needed
 */
db_res *devres_1_svc(db_res *recev)
{
	return dbm->devres_1_svc(recev);
}

/**@ingroup dbServerInterface
 * To delete a device from the database
 * 
 * @param dev The device name
 * 
 * @return a pointer to an error code
 */
DevLong *devdel_1_svc(nam *dev)
{
	return dbm->devdel_1_svc(dev);
}

/**@ingroup dbServerInterface
 * To delete all the resources belonging to a device
 *
 * @param recev The device name
 *
 * @return a pointer to an error code
 */
db_psdev_error *devdelres_1_svc(db_res *recev)
{
	return dbm->devdelres_1_svc(recev);
}

/**@ingroup dbServerInterface
 * To get global information on the database
 * 
 * @returns a pointer to a structure with all the database info
 */
db_info_svc *info_1_svc(void)
{
	return dbm->info_1_svc();
}

/**@ingroup dbServerInterface
 * unregister a server from the database (to mark all its devices as not exported)
 *
 * @param recev A pointer to a structure where the first element is the device server name 
 *		and the second element is the device server personal name.
 *
 * @return a pointer to a long which will be set in case of error
 */
DevLong *unreg_1_svc(db_res *recev)
{
	return dbm->unreg_1_svc(recev);
}

/**@ingroup dbServerInterface
 * retrieve device server info from the database
 *
 * @param recev A pointer to a structure where the first element is the device server 
 *		name and the second element is the device server personal name.
 *
 * @return a pointer to a structure with all the device server information
 */
svcinfo_svc *svcinfo_1_svc(db_res *recev)
{
#ifdef DEBUG
#ifndef _solaris
	std::cerr << __FUNCTION__ << std::endl;
#endif /* !_solaris */
#endif
	return dbm->svcinfo_1_svc(recev);
}

/**@ingroup dbServerInterface
 * delete all the device belonging to a device server and if necessary also their resources
 *
 * @param recev A pointer to a structure where the first element is the device server name 
 *		and the second element is the device server personal name.
 *
 * @return a pointer to a long which will be set in case of error
 */
DevLong *svcdelete_1_svc(db_res *recev)
{
	return dbm->svcdelete_1_svc(recev);
}

/**@ingroup dbServerInterface
 * To retrieve poller info from a device name
 * 
 * @param dev  The device name
 *
 * @return a pointer to an error code
 */
db_poller_svc *getpoller_1_svc(nam *dev)
{
    return dbm->getpoller_1_svc(dev);
}

/**@ingroup dbServerInterface
 * updates the list of device for the servers
 * 
 * @param dev_list The lists of device names for the servers
 *
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_psdev_error *upddev_1_svc(db_res *dev_list)
{
	return dbm->upddev_1_svc(dev_list);
}

/**@ingroup dbServerInterface
 * update resource(s)
 * 
 * @param res_list The list of resources
 * 
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_psdev_error *updres_1_svc(db_res *res_list)
{
	return dbm->updres_1_svc(res_list);
}

/**@ingroup dbServerInterface
 * Read the password as resource from security file
 * 
 * @return password as a resource
 */
db_res *secpass_1_svc(void)
{
	return dbm->secpass_1_svc();
}
