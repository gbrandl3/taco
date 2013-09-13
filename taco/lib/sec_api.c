/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File       : sec_api.c
 * 
 * Project    : Device Servers with SUN-RPC
 *
 * Description: Interface for the security system
 *
 * Author(s)  : Jens Meyer
 * 		$Author: jensmeyer $
 *
 * Original   : December 1993
 *
 * Version    :	$Revision: 1.21 $
 *
 * Date       :	$Date: 2010-05-19 15:56:57 $
 *
 ********************************************************************-*/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevErrors.h>
#include <Admin.h>

#include "taco_utils.h"

#include <errno.h>

#ifdef unix
#	if HAVE_NETDB_H
#		include <netdb.h>
#	endif
#	if !defined vxworks
#		include <pwd.h>
#		include <grp.h>
#	endif /* !vxworks */
#	if !HAVE_SOCKLEN_T
#		define	_BSD_SOCKLEN_T_	/* Darwin 6.8 Problem? */
#	endif
#	if HAVE_SYS_SOCKET_H
#		include <sys/socket.h>
#	endif
#	if HAVE_ARPA_INET_H
#		include <arpa/inet.h>
#	endif
#	if HAVE_NETINET_Hx
#		include <netinet/in.h>
#	endif 
#endif /* unix */

#if ( OSK | _OSK )
#	include <inet/socket.h>
#	include <inet/netdb.h>
#endif /* OSK | _OSK */

/*
 * the following macros should be defined in macros.h but I do
 * not have access to it for the moment (checked by WDK)
 *
 * andy 23apr97
 */
#ifdef vxworks
#define VW_USER_ID      42
#define VW_USER_NAME    "vw_user"
#define VW_GROUP_ID     42
#define VW_GROUP_NAME   "vw_group"
#endif /* vxworks */

static long sec_check
PT_( (char *dev_name, long access_right, SecUserAuth user_auth, DevLong *error) );

static long sec_user_ident
PT_( (SecUserAuth user_auth, SecDefaultAccess sec_default_access, long i_nethost, DevLong *error) );

static long search_dev_name
PT_( (char *dev_name, char str_array[3][LONG_NAME_SIZE], DevLong *error) );

static long check_access_hierarchy
PT_( (char *dev_name, long access_right, char *name, char *res_path, DevLong *error) );

static long check_access_right
PT_( (char *name, long requested_access, DevVarStringArray *access_res, DevLong *error) );

static long create_client_id
PT_( (SecUserAuth user_auth, long *ret_client_id, DevLong *error) );

static long get_connection_id
PT_( (long *connection_id, DevLong *error) );

static long sec_verify_tcp_conn
PT_( (DevServerDevices *device) );


/*
 * A global variable to hold the unique client identification 
 */
static long	client_id = 0;

/*
 * A global variable to hold the security keys to check 
 * the client handles.
 */
static SecOpenConn 	conn_list = { 0, NULL};

/*
 * the following array have to be declared once per nethost
 * for multi-nethost. A user will have different security access
 * in different control system and therefore has to be initialised
 * once per control system i.e. nethost.
 */
short		*auth_flag;

/**
 * @ingroup secAPI
 * Get the user authentication.
 * Verify the access to the control system.
 * Check the requested access right for the device.
 * Return the client identification and the next free field in the list 
 * of connections to device servers.
 *  
 * @param dev_name name of the device to access.
 * @param requested_access requested access right.
 * @param ret_client_id will be set to a unique client identification
 * @param connection_id will be set to the next free field in the list of connections
 * @param error pointer to error code in case routine fails
 *
 * @retval DS_OK if the access is allowed 
 * @retval DS_NOTOK access denied.
 */
long _DLLFunc dev_security (char *dev_name, long requested_access,
			    long *ret_client_id, long *connection_id, DevLong *error)
{
	static SecUserAuth	user_auth;
	char			*user_name;
	char			ip_str[SHORT_NAME_SIZE];
#if !defined _UCC && !defined WIN32 && !defined vxworks
	struct passwd		*passwd_info;
	struct group		*group_info;
#endif /* !_UCC && !WIN32 && !vxworks */
#if !defined vxworks
	struct hostent		*host_info;
#else  /* !vxworks */
	union 			{ int    int_addr;
	  			  u_char char_addr[4];}  host_addr;
#endif /* !vxworks */
	register long		i;
	long			i_nethost;

	u_char			*ip_ptr;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\ndev_security() : entering routine \n");

	*error = 0;

/*
 * Verify whether the requested access represents a
 * defined access right.
 */

	for (i=0; i<SEC_LIST_LENGTH; i++)
	{
		if ( requested_access == DevSec_List[i].access_right )
		{
			break;
		}
	}
	if ( i == SEC_LIST_LENGTH )
	{
		*error = DevErr_UndefAccessRight;
		return (DS_NOTOK);
	}

/*
 * get the nethost index
 */
	if ((i_nethost = get_i_nethost_by_device_name(dev_name,error)) < 0)
	{
		return(DS_NOTOK);
	}

/*
 * Do the user authentication only once PER nethost !
 */

	if ( auth_flag[i_nethost] == False )
	{
/* 
 * get the login name and the user id from the passwd entry.
 */

#if defined unix && !defined vxworks
		if ( (passwd_info = getpwuid ( getuid() )) == NULL )
		{
			*error = DevErr_NoValidPasswdEntry;
			return (DS_NOTOK);
		}

/*
 * make local copy of user name returned by getpwuid to avoid it being overwritten
 */
		user_name =
		(char*)malloc(strlen(passwd_info->pw_name)+1);
		if(!user_name)
		    {
			*error=DevErr_InsufficientMemory;
			return DS_NOTOK;
		    }
		strcpy(user_name, passwd_info->pw_name);

		user_auth.user_name = user_name;
		user_auth.uid       = passwd_info->pw_uid;
#endif /* unix && !vxworks */

#if ( OSK | _OSK )
		user_auth.uid = getuid();
		if ( user_auth.uid == 0 )
		{
			user_auth.user_name = OS9_USER;
		}
		else
		{
			user_auth.user_name = "unknown";
		}
#endif /* OSK | _OSK */

#if defined (WIN32)
		user_auth.uid = PC_USER_ID;
		user_auth.user_name = PC_USER_NAME;
#endif /* WIN32 */

#if defined (vxworks)
		user_auth.uid = VW_USER_ID;
		user_auth.user_name = VW_USER_NAME;
#endif /* vxworks */

		dev_printdebug (DBG_SEC, "dev_security() : user_name = %s\n", user_auth.user_name);
		dev_printdebug (DBG_SEC, "dev_security() : uid       = %d\n", user_auth.uid);

/*
 * get the group name and the group id of a user
 */

#if defined unix && !defined vxworks
		if ( (group_info = getgrgid ( getgid() )) == NULL )
		{
			*error = DevErr_NoValidGroupEntry;
			return (DS_NOTOK);
		}

		user_auth.group_name = group_info->gr_name;
		user_auth.gid 	= group_info->gr_gid;
#endif /* unix  && !vxworks */

#if ( OSK | _OSK )
		user_auth.group_name = OS9_GROUP;
		user_auth.gid        = 0;
#endif /* OSK | _OSK */

#if defined (WIN32)
		user_auth.group_name = PC_GROUP_NAME;
		user_auth.gid        = PC_GROUP_ID;
#endif	/* WIN32 */

#if defined (vxworks)
		user_auth.group_name = VW_GROUP_NAME;
		user_auth.gid        = VW_GROUP_ID;
#endif	/* vxworks */

		dev_printdebug (DBG_SEC, "dev_security() : group_name = %s\n", user_auth.group_name);
		dev_printdebug (DBG_SEC, "dev_security() : gid        = %d\n", user_auth.gid);

/*
 * get the IP address of the users host
 */
		taco_gethostname (user_auth.host_name, HOST_NAME_LENGTH);
#if !defined vxworks
		if ( (host_info = gethostbyname (user_auth.host_name)) == NULL )
		{
			*error = DevErr_NoValidHostEntry;
			return (DS_NOTOK);
		}

		snprintf (ip_str, sizeof(ip_str), "%d.%d.%d.%d",(u_char) host_info->h_addr[0],
		    (u_char) host_info->h_addr[1],
		    (u_char) host_info->h_addr[2],
		    (u_char) host_info->h_addr[3]);
#else  /* !vxworks */
		host_addr.int_addr = hostGetByName(user_auth.host_name);
		snprintf (ip_str, sizeof(ip_str), "%d.%d.%d.%d",(u_char) host_addr.char_addr[0],
		    (u_char) host_addr.char_addr[1],
		    (u_char) host_addr.char_addr[2],
		    (u_char) host_addr.char_addr[3]);
#endif /* !vxworks */
		user_auth.ip = inet_network (ip_str);

		ip_ptr = (u_char *) &user_auth.ip;
		dev_printdebug (DBG_SEC, "dev_security() : host_name = %s\n", user_auth.host_name);
		dev_printdebug (DBG_SEC, "dev_security() : ip_str    = %s\n", ip_str);
		dev_printdebug (DBG_SEC, "dev_security() : ip        = %d.%d.%d.%d\n", 
#ifndef linux
		    ip_ptr[0], ip_ptr[1], ip_ptr[2], ip_ptr[3]);
#else
/*
 * Linux memory model is little-endian i.e. low order byte first
 */
		    ip_ptr[3], ip_ptr[2], ip_ptr[1], ip_ptr[0]);
#endif /* !linux */

/*
 * Create the unique client identification.
 */

		if ( create_client_id (user_auth, ret_client_id, error) == DS_NOTOK )
		{
			return (DS_NOTOK);
		}
		auth_flag[i_nethost] = True;
	}

/*
 * For all further imports, return the already created client_id.
 */
	else
	{
		*ret_client_id = client_id;
	}


/*
 * Do the identification and access right checks.
 */
	if ( sec_check (dev_name, requested_access, user_auth, error) == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

/*
 * get the next free place in the list of connections.
 */
	if ( get_connection_id (connection_id, error) == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "\ndev_security() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPIintern
 * Read the default access rights from the resource database.
 * Verify the user identification and control system access.
 * Check the requested access on the device. 
 * If nothing was specified, checks the default access right.
 *
 * @param dev_name name of the device to access.
 * @param access_right requested access right.
 * @param user_auth user authentication structure.
 *
 * @param error pointer to error code, in case routine fails
 * 
 * @retval DS_OK if access is allowed
 * @retval DS_NOTOK otherwise 
 */
static long sec_check (char *dev_name, long access_right, SecUserAuth user_auth,
		       DevLong *error)
{
        /* M. Diehl, 2.11.99
         * sec_default_access will either be alloc'ed or set to &default_ip!
         * hence we have to remember what we've done for later
         * freeing it if appropriate.
         */
         
	SecDefaultAccess	sec_default_access;
	short                   alloc_ip_list = 0;

	DevVarStringArray	default_access;
	db_resource   		res_tab;
	u_int           	res_tab_size = 1;
	char            	res_path [LONG_NAME_SIZE];

	char			res_name[LONG_NAME_SIZE];
	u_long			default_ip = 0;
	long			ret;
	short			i;
	long			i_nethost;
	u_char			*ip_ptr;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_check() : entering routine \n");

	*error = 0;
/*
 * get the nethost index
 */
	if ((i_nethost = get_i_nethost_by_device_name(dev_name,error)) < 0)
	{
		return(DS_NOTOK);
	}

	default_access.length   = 0;
	default_access.sequence = NULL;

	/*
	 * Read the default access values from the resource database
	 * and store the in the structure sec_default_access.
	 */

	snprintf (res_name, sizeof(res_name), "default");
	res_tab.resource_name = res_name;
	res_tab.resource_type = D_VAR_STRINGARR;
	res_tab.resource_adr  = &default_access;

/*
 * Add the nethost to the resource path
 * so that db_getresource() queries the appropriate static database
 */
 
	snprintf (res_path, sizeof(res_path), "//%s/SEC/MINIMAL/ACC_RIGHT",multi_nethost[i_nethost].nethost);
	
	if (db_getresource (res_path, &res_tab, res_tab_size, error)==DS_NOTOK)
	{
		return (DS_NOTOK);
	}

	/*
	 * Transform the string array into access right value
	 * and an array of IP addresses.
	 */

	if ( default_access.length > 0 )
	{
		for (i=0; i<SEC_LIST_LENGTH; i++)
		{
			if (strcmp (default_access.sequence[0], 
			    DevSec_List[i].access_name) == 0)
			{
				sec_default_access.access_right = DevSec_List[i].access_right;
				break;
			}
		}

		if ( i == SEC_LIST_LENGTH )
		{
			sec_default_access.access_right   = NO_ACCESS;
		}
	}
	else
	{
		sec_default_access.access_right   = NO_ACCESS;
		sec_default_access.ip_list_length = 1;
		sec_default_access.ip_addr_list   = &default_ip;
	}


	if ( default_access.length > 1 )
	{
		sec_default_access.ip_list_length = default_access.length - 1;
		sec_default_access.ip_addr_list   = (u_long *) calloc 
		    (sec_default_access.ip_list_length, sizeof (u_long));

                /* we need to know later whether this was calloc'ed
                 * or not to decide if we have to free it! */
                 
                if( sec_default_access.ip_addr_list != NULL )
                  alloc_ip_list = 1;
                else
                  ; /* since we don't catch this, we are in big trouble now! */

		for (i=1; (u_long)i<default_access.length; i++)
		{
			sec_default_access.ip_addr_list[i-1] = 
			    inet_network (default_access.sequence[i]);
		}
	}
	else
	{
		sec_default_access.ip_list_length = 1;
		sec_default_access.ip_addr_list   = &default_ip;
		alloc_ip_list = 0;   /* do not try to free() it later */
	}

	dev_printdebug (DBG_SEC, "sec_check() : def_access = %d\n", sec_default_access.access_right);
	for (i=0; i<sec_default_access.ip_list_length; i++)
	{
		ip_ptr = (u_char *)&sec_default_access.ip_addr_list[i];
		dev_printdebug (DBG_SEC, "sec_check() : def_ip = %d.%d.%d.%d\n",
#ifndef linux
		    ip_ptr[0], ip_ptr[1], ip_ptr[2], ip_ptr[3]);
#else
/*
 * Linux memory model is little-endian i.e. low order byte first
 */
		    ip_ptr[3], ip_ptr[2], ip_ptr[1], ip_ptr[0]);
#endif /* !linux */
	}
	/*
	 * free the allocated string array
	 */

	free_var_str_array (&default_access);

	/*
	 * Check the user identification and her/his network access rights.
	 */

	if (sec_user_ident (user_auth, sec_default_access, i_nethost, error) == DS_NOTOK)
	{
	       /* dont't leak the memory, if ip_addr_list was alloc'ed */
               if( alloc_ip_list == 1 )
                  free (sec_default_access.ip_addr_list);
                  
		return (DS_NOTOK);
	}

	/*
	 * Free the allocated pointer for the list of default IP
	 * addresses.
	 * But only if it was alloc'ed before.
	 */
	 
	if( alloc_ip_list == 1 )
		free (sec_default_access.ip_addr_list);

	/*
	 * Check the user access hierrarchy for DOMAIN/FAMILY/MEMBER.
	 */

/*
 * Prepare the resource path.
 */
 
	snprintf (res_path, sizeof(res_path), "//%s/SEC/USER/ACC_RIGHT",multi_nethost[i_nethost].nethost);
	
	ret = check_access_hierarchy (dev_name, access_right, user_auth.user_name, res_path, error);
	if (ret != DS_WARNING)
	{
		return (ret);
	}


	/*
	 * If no access right was specified for the user,
	 * check the group access hierarchy for DOMAIN/FAMILY/MEMBER.
	 */

	snprintf (res_path, sizeof(res_path), "//%s/SEC/GROUP/ACC_RIGHT",multi_nethost[i_nethost].nethost);

	ret = check_access_hierarchy (dev_name, access_right, 
	    user_auth.group_name, res_path, error);
	if (ret != DS_WARNING)
		return (ret);

	/*
	 * If no access right was specified for the group,
	 * check the default access right.
	 */

	if ( access_right > sec_default_access.access_right )
	{
		*error = DevErr_AccessDenied;
		return (DS_NOTOK);
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_check() : default access OK\n");
	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_check() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPIintern
 * Read the three resource tables (DOMAIN/FAMILY/MEMBER) for user or group access rights. 
 * Checks the user or group access rights specified in these tables in the order MEMBER, 
 * FAMILY, and DOMAIN.
 *
 * @param dev_name name of the device to access.
 * @param access_right requested access right.
 * @param name name of user or group.
 * @param res_path path to the resource tables SEC/USER/ACC_RIGHT/ or SEC/GROUP/ACC_RIGHT/
 *
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK if the access is allowed
 * @retval DS_NOTOK if access denied
 * @retval DS_WARNING if no access right specification.
 */
static long check_access_hierarchy (char *dev_name, long access_right, 
				    char *name, char *res_path, DevLong *error)
{
	db_resource   		res_tab[3];
	u_int           	res_tab_size = 3;
	char			res_names[3][LONG_NAME_SIZE];
	DevVarStringArray	domain_access;
	DevVarStringArray	family_access;
	DevVarStringArray	member_access;
	long			ret;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\ncheck_access_hierarchy() : entering routine \n");
	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_hierarchy() : name         = %s\n", name);
	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_hierarchy() : access_right = %d\n", access_right);
	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_hierarchy() : dev_name     = %s\n", dev_name);

	*error = 0;
	domain_access.length   = 0;
	domain_access.sequence = NULL;
	family_access.length   = 0;
	family_access.sequence = NULL;
	member_access.length   = 0;
	member_access.sequence = NULL;


	/*
	 * Create resource table with the resource names:
	 *	DOMAIN,
	 *	DOMAIN/FAMILY,
	 * and  DOMAIN/FAMILY/MEMBER
	 */

	if (search_dev_name (dev_name, res_names, error) == DS_NOTOK)
	{
		return (DS_NOTOK);
	}

	res_tab[0].resource_name = res_names[0];
	res_tab[1].resource_name = res_names[1];
	res_tab[2].resource_name = res_names[2];

	res_tab[0].resource_type = D_VAR_STRINGARR;
	res_tab[1].resource_type = D_VAR_STRINGARR;
	res_tab[2].resource_type = D_VAR_STRINGARR;

	res_tab[0].resource_adr  = &domain_access;
	res_tab[1].resource_adr  = &family_access;
	res_tab[2].resource_adr  = &member_access;


	/*
	 * Read the user access rights from the resource database.
	 */

	if (db_getresource (res_path, res_tab, res_tab_size, error)==DS_NOTOK)
	{
		return (DS_NOTOK);
	}

	/*
	 * Compare the requested access right with the permissions
	 * in the user resources.
	 */

	/*
	 * Check the rights specified for the device.
	 */
	ret = check_access_right (name, access_right, &member_access, error );
	if ( ret != DS_WARNING)
		{
		/* 
	 	 * Free variable string arrays, allcoated by db_getresource().
	 	 */
		free_var_str_array (&member_access);
		free_var_str_array (&family_access);
		free_var_str_array (&domain_access);
		return (ret);
		}
	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_hierarchy() : no access right for device\n");

	/*
	 * Check the rights specified for the device family.
	 */
	ret = check_access_right (name, access_right, &family_access, error );
	if ( ret != DS_WARNING)
		{
		/* 
	 	 * Free variable string arrays, allcoated by db_getresource().
	 	 */
		free_var_str_array (&member_access);
		free_var_str_array (&family_access);
		free_var_str_array (&domain_access);
		return (ret);
		}
	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_hierarchy() : no access right for family\n");

	/*
	 * Check the rights specified for the domain of the device.
	 */
	ret = check_access_right (name, access_right, &domain_access, error );
	if ( ret != DS_WARNING)
		{
		/* 
	 	 * Free variable string arrays, allcoated by db_getresource().
	 	 */
		free_var_str_array (&member_access);
		free_var_str_array (&family_access);
		free_var_str_array (&domain_access);
		return (ret);
		}
	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_hierarchy() : no access right for domain\n");

	/* 
	 * Free variable string arrays, allcoated by db_getresource().
	 */

	free_var_str_array (&member_access);
	free_var_str_array (&family_access);
	free_var_str_array (&domain_access);

	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_hierarchy() : leaving routine\n");

	return (DS_WARNING);
}

/**
 * @ingroup secAPIintern
 * Check whether the user has access to the control system.
 * The user identification information will be read and verified. 
 * If nothing is specified, the group identification information will be read and verified. 
 * If also nothing was specified, the default network access will be checked.
 *
 * @param user_auth user athentication structure.
 * @param sec_default_access default access rights.
 * @param i_nethost
 *
 * @param error pointer to error code. Will return error if no access was given.
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
static long sec_user_ident (SecUserAuth user_auth, 
			    SecDefaultAccess sec_default_access,
			    long i_nethost, DevLong *error)
{
	db_resource 		res_tab;
	char            	res_path [LONG_NAME_SIZE];
	DevVarStringArray	ident;

	long 			uid;
	long			gid;
	long			ip;
	u_char			ip1;
	u_char			ip2;
	u_long			mask;

	short			i, k, j;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_user_ident() : entering routine \n");

	*error = 0;
	ident.length   = 0;
	ident.sequence = NULL;

	/*
	 * Read the user identification from the resource database.
	 */

	res_tab.resource_name = user_auth.user_name;
	res_tab.resource_type = D_VAR_STRINGARR;
	res_tab.resource_adr  = &ident;

/*
 * Prepare the resource path.
 */
 
	snprintf (res_path, sizeof(res_path), "//%s/SEC/USER/IDENT",multi_nethost[i_nethost].nethost);

	if (db_getresource (res_path, &res_tab, 1, error) == DS_NOTOK)
	{
		return (DS_NOTOK);
	}

	/*
	 * Transforme the string array into UID
	 * and an array of IP addresses.
	 */

	if ( ident.length != 0 )
	{
		uid = atol (ident.sequence[0]);

		/*
	    * check the uid
	    */
		dev_printdebug (DBG_SEC, "sec_user_ident() : uid = %d res_uid = %d\n", user_auth.uid, uid);

		if ( uid != user_auth.uid )
		{
			*error = DevErr_UserIdIsNotCorrect;
			return (DS_NOTOK);
		}

	   /*
	    * Check the specified IP adresses for a user.
	    *
	    * Changed the way in which the byte comparisons are made
	    * in order to be compatible with Intel platforms (Linux + NT). 
	    * Intel uses a different memory model and byte addressing a
	    * a long word gives little-endian byte order on Intel as opposed
	    * to big-endian byte order which is required for network order
	    */
		if ( ident.length > 1 )
		{
			for (i=1; (u_long)i<ident.length; i++)
			{
				j = 0;
				ip = inet_network (ident.sequence[i]);
				for (k=0; k<4; k++)
				{
					mask = 0xff000000 >> (8*k);
					mask = mask ^ 0x00000000;
					ip2 = (ip & mask) >> (8*(3-k));
					mask = 0xff000000 >> (8*j);
					mask = mask ^ 0x00000000;
					ip1 = (user_auth.ip & mask) >> (8*(3-j));
        				/*dev_printdebug (DBG_TRACE | DBG_SEC,
            				"\nsec_user_ident() : compare ip1 %d with ip2 %d\n",ip1,ip2);*/
					if ( ip2 != 0 && ip2 != 255 )
					{

						if ( ip2 != ip1 )
						{
							j = 0;
							continue;
						}
						else
						{
							j++;
						}
					}
				}

				if ( j != 0 )
				{
					dev_printdebug (DBG_SEC, "sec_user_ident() : User network access OK!\n");
/*
 * Free the allocated variable string array.
 */
					free_var_str_array (&ident);
					return (DS_OK);
				}
			}

/*
 * Free the allocated variable string array
 */
			free_var_str_array (&ident);
			*error = DevErr_NetworkAccessDenied;
			return (DS_NOTOK);
		}

	}

	dev_printdebug (DBG_SEC, "sec_user_ident() : No user network access specified!\n");

/*
 * Free the allocated variable string array
 */

	free_var_str_array (&ident);

/*
 * Not sufficient user rights specified.
 * Check the group ID and the network access for the group.
 *
 * Read the group identification from the resource database.
 */
	res_tab.resource_name = user_auth.group_name;
	res_tab.resource_type = D_VAR_STRINGARR;
	res_tab.resource_adr  = &ident;

/*
 * Prepare the resource path.
 */
 
	snprintf (res_path, sizeof(res_path), "//%s/SEC/GROUP/IDENT",multi_nethost[i_nethost].nethost);

	if (db_getresource (res_path, &res_tab, 1, error) == DS_NOTOK)
	{
		return (DS_NOTOK);
	}

	/*
	 * Transforme the string array into GID
	 * and an array of IP addresses.
	 */

	if ( ident.length != 0 )
	{
		gid = atol (ident.sequence[0]);

/*
 * check the gid
 */
		dev_printdebug (DBG_SEC, "sec_user_ident() : gid = %d res_gid = %d\n", user_auth.gid, gid);

		if ( gid != user_auth.gid )
		{
			*error = DevErr_GroupIdIsNotCorrect;
			return (DS_NOTOK);
		}

		/*
	    * Check the specified IP adresses for a group.
	    *
	    * Changed the way in which the byte comparisons are made
	    * in order to be compatible with Intel platforms (Linux + NT). 
	    */
		if ( ident.length > 1 )
		{
			for (i=1; (u_long)i<ident.length; i++)
			{
				j = 0;
                                ip = inet_network (ident.sequence[i]);
                                for (k=0; k<4; k++)
                                {
                                        mask = 0xff000000 >> (8*k);
                                        mask = mask ^ 0x00000000;
                                        ip2 = (ip & mask) >> (8*(3-k));
					mask = 0xff000000 >> (8*j);
					mask = mask ^ 0x00000000;
					ip1 = (user_auth.ip & mask) >> (8*(3-j));
        				/*dev_printdebug (DBG_TRACE | DBG_SEC,
            				"\nsec_user_ident() : compare ip1 %d with ip2 %d\n",ip1,ip2);*/
                                        if ( ip2 != 0 && ip2 != 255 )
                                        {
                                                if ( ip2 != ip1 )
                                                {
							j = 0;
							continue;
						}
						else
						{
							j++;
						}
					}
				}

				if ( j != 0 )
				{
					dev_printdebug (DBG_SEC, "sec_user_ident() : Group network access OK!\n");
/*
 * Free the allocated variable string array.
 */
					free_var_str_array (&ident);
					return (DS_OK);
				}
			}

			/*
	 		 * Free the allocated variable string array
	 		 */
			free_var_str_array (&ident);

			*error = DevErr_NetworkAccessDenied;
			return (DS_NOTOK);
		}

	}

	dev_printdebug (DBG_SEC, "sec_user_ident() : No group network access specified!\n");

	/*
	 * Free the allocated variable string array
	 */

	free_var_str_array (&ident);

	/*
	 * Not sufficient user or group rights specified.
	 * Check the default network access.
	 */

	for (i=0; i<sec_default_access.ip_list_length; i++)
	{
		j = 0;
	   /*
            * Changed the way in which the byte comparisons are made
            * in order to be compatible with Intel platforms (Linux + NT).
            */
                ip = sec_default_access.ip_addr_list[i];
                for (k=0; k<4; k++)
                {
                	mask = 0xff000000 >> (8*k);
                        mask = mask ^ 0x00000000;
                        ip2 = (ip & mask) >> (8*(3-k));
			mask = 0xff000000 >> (8*j);
			mask = mask ^ 0x00000000;
			ip1 = (user_auth.ip & mask) >> (8*(3-j));

        		/*dev_printdebug (DBG_TRACE | DBG_SEC,
            		"\nsec_user_ident() : compare ip1 %d with ip2 %d\n",ip1,ip2);*/
                        if ( ip2 != 0 && ip2 != 255 )
			{
				if ( ip2 != ip1 )
				{
					j = 0;
					continue;
				}
				else
				{
					j++;
				}
			}
		}

		if ( j != 0 )
		{
			dev_printdebug (DBG_SEC, "sec_user_ident() : Default network access OK!\n");
			return (DS_OK);
		}
	}

	*error = DevErr_NetworkAccessDenied;
	return (DS_NOTOK);
}

/**
 * @ingroup secAPIintern
 * Extract the three parts: DOMAIN, DOMAIN/FAMILY, DOMAIN/FAMILY/MEMBER
 * from the device name and store them in a string array.
 *
 * @param dev_name device name. 
 *
 * @param str_array extracted parts of the device name.
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
static long search_dev_name (char *dev_name, char str_array[3][LONG_NAME_SIZE],
			     DevLong *error)
{
	char		name[LONG_NAME_SIZE];
	char            *ptr1;
	char            *ptr2;
	char            *ptr3;
	short           length;
	register long   i;
	register long   k;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsearch_dev_name() : entering routine \n");

/*
 * check if nethost ahs been specified e.g. "//nethost.domain/family/member"
 * if it has then advance the dev_name pointer to the start of the device
 * name without the nethost
 */
	if (strncmp(dev_name,"//",2) == 0)
	{
/*
 * advance the dev_name pointer to the start of the device name
 */
		for (dev_name += 2; *dev_name != '/'; dev_name++); 
		dev_name++;
	}

	snprintf (name, sizeof(name), "%s", dev_name);
	ptr1 = name;
	ptr2 = name;

	/*
	 *  get the domain and domain/family strings
	 */

	length = strlen (ptr1);
	k=0;

	for (i=0; i<length; i++, ptr1++)
	{
		if ( *ptr1 == '/' )
		{
			*ptr1 = '|';
			strncpy (str_array[k], ptr2, i);
			ptr3 = str_array[k];
			ptr3[i] = '\0';
			k++;
			i++;
			ptr1++;
		}
	}

	if ( k != 2)
	{
		*error = DevErr_DeviceTypeNotRecognised;
		return (DS_NOTOK);
	}

	snprintf (str_array[k], sizeof(str_array[k]), "%s", name);

	dev_printdebug (DBG_SEC, "search_dev_name() : resource names \n");
	dev_printdebug (DBG_SEC, "DO= %s  FA= %s  ME=%s\n", str_array[0], str_array[1], str_array[2]);
	dev_printdebug (DBG_TRACE | DBG_SEC, "search_dev_name() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPIintern
 * Search a specified access right for a user or a group (given by name) in a variable 
 * string array of the format:
 *			name1, 	 access_ right1, \
 *			name2,   access_ right2, \
 *			...................
 *
 * Verify the requested access if a maximum access right was found for the name.
 *
 * @param name name of user or group.
 * @param requested_access requested access right.
 * @param access_res variable string array of names and access rights.
 * 
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK if access allowed
 * @retval DS_NOTOK if access denied
 * @retval DS_WARNING if access right specification.
 */
static long check_access_right (char *name, long requested_access,
				DevVarStringArray *access_res, DevLong *error)
{
	long		max_access_right;
	char		*name_ptr;
	short		len;
	register long   i;
	register long   k;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\ncheck_access_right() : entering routine \n");

	*error = 0;

	/*
	 * search for the user name or group name in the resource array.
	 */
	for (i=0; (u_long)i<access_res->length; i=i+2)
	{
/*
 * make sure, all names are in lower letters.
 */
		name_ptr = str_tolower(access_res->sequence[i]);

/*
 * If a name was found in the resource array, compare the
 * requested access right with the one specified in the 
 * security database.
 */

		if ( strcmp(name, access_res->sequence[i]) == 0 )
		{
		/*
	         *  Search the minimum access right value,
	         *  DevSec_List.
	         */
			for (k=0; k<SEC_LIST_LENGTH; k++)
			{
				if (strcmp (access_res->sequence[i+1], 
				    DevSec_List[k].access_name) == 0)
				{
					max_access_right = DevSec_List[k].access_right;
					dev_printdebug (DBG_SEC, "check_access_right() : max_access_right = %d\n", max_access_right);
					dev_printdebug (DBG_SEC, "check_access_right() : requested_access = %d\n", requested_access);
					break;
				}
			}

			/* 
	       * If the string describing the access right is unknown.
	       */

			if ( k == SEC_LIST_LENGTH)
			{
				*error = DevErr_UndefAccessRightInRes;
				return (DS_NOTOK);
			}

			/*
	       * Now compare the two access rights!
	       */

			if ( requested_access <= max_access_right )
			{
				return (DS_OK);
			}
			else
			{
				*error = DevErr_AccessDenied;
				return (DS_NOTOK);
			}
		}
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "check_access_right() : leaving routine \n");

	/*
	 * return a warning to indicate that the search has to go on.
	 */

	return (DS_WARNING);
}

/**
 * @ingroup secAPIintern
 * Free a variable string array.  Normally allocated by db_getresource().
 *
 * @param str_array pointer to the variable string array.
 */
void _DLLFunc free_var_str_array (DevVarStringArray *str_array)
{
	short	i;

	for ( i=0; (u_long)i<str_array->length; i++ )
	{
		free (str_array->sequence[i]);
	}

	str_array->length = 0;
	free (str_array->sequence);
	str_array->sequence = NULL;
}

/**
 * @ingroup secAPIintern
 * Create a unique client ID from a timestamp, the process ID, the IP address, the UID and the GID. 
 * 
 * @param user_auth user authentication structure.
 *
 * @param ret_client_id will be set to the client ID
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
static long create_client_id (SecUserAuth user_auth, long *ret_client_id, 
			      DevLong *error)
{
	time_t 	time_stamp;
	long 	pid;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\ncreate_client_id() : entering routine \n");

	*error = 0;

	/*
	 * get the timestamp.
	 */

	time (&time_stamp);

	/*
	 * get the process ID.
	 */

#if !defined (WIN32)
#if !defined (vxworks)
        pid = getpid ();
#else  /* !vxworks */
        pid = taskIdSelf ();
#endif /* !vxworks */
#else  /* !WIN32 */
        pid = (long)_getpid();
#endif /* !WIN32 */

	/*
	 * create the client ID.
	 */

	client_id = time_stamp +
	    pid  +
	    user_auth.uid +
	    user_auth.gid +
	    user_auth.ip  /
	    5;

	*ret_client_id = client_id;

	dev_printdebug (DBG_SEC, "create_client_id() : client_id = %d\n", client_id);
	dev_printdebug (DBG_TRACE | DBG_SEC, "create_client_id() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPIintern
 * Get the connection ID for a new server connection, to store the security key.
 *
 * @param connection_id will be set to the connection number.
 * @param error pointer to error code, in case routine fails.
 * 
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
static long get_connection_id (long *connection_id, DevLong *error)
{
	long   	*new_connections;
	short    i;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nget_connection_id() : entering routine\n");

	*error = 0;

	/*
         * Search for the first free field in the connections list.
         */

	for (i=0; i<conn_list.max_no_of_conn; i++)
	{
		if (conn_list.sec_keys[i] == FREE_CONNECTION)
		{
			*connection_id = i;
			break;
		}
	}

	if ( i == conn_list.max_no_of_conn )
	{
		/*
	    * On SUN systems realloc does not work with NULL pointer.
	    * For this reason the first block has to be allocated
	    * with malloc()!
	    */

		if (conn_list.max_no_of_conn == 0)
		{
			new_connections = (long *) malloc (BLOCK_SIZE * sizeof(long));
		}
		else
		{
			/*
               * If no free field was found in the connections list, a new block
               * must be allocated.
               */

			new_connections = (long *) realloc (conn_list.sec_keys,
			    ((conn_list.max_no_of_conn+BLOCK_SIZE) * 
			    sizeof(long)));
		}

		if ( new_connections == NULL )
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}

		conn_list.sec_keys = new_connections;

		/*
            * Initialise the new allocated structures
            */

		memset ((char *)&conn_list.sec_keys[(_Int)conn_list.max_no_of_conn],
		    FREE_CONNECTION,
		    (BLOCK_SIZE*sizeof(long)));

		*connection_id = conn_list.max_no_of_conn;
		conn_list.max_no_of_conn = conn_list.max_no_of_conn + BLOCK_SIZE;
	}

	return (DS_OK);
}

/**
 * @ingroup secAPI
 * Free the reserved field for the security connection.  
 * For connections with old library versions the reserved field can be freed.
 *
 * @param connection_id connection number.
 * 
 * @param error pointer to error code, in case routine fails.
 * 
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long _DLLFunc free_connection_id_vers3 (long connection_id, DevLong *error)
{
	dev_printdebug (DBG_TRACE | DBG_SEC, "free_connection_id_vers3() : entering routine\n");

	conn_list.sec_keys[(_Int)connection_id] = FREE_CONNECTION;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nfree_connection_id_vers3() : leaving routine\n");

	return (*error=DS_OK);
}

/**
 * @ingroup secAPI
 * Create the security key from the requested access right, the unique client identification
 * and the RPC client handle to the device server.  
 * The security key is stored as reference in the list of connections with the given connection ID.
 *
 * @param ds device server client handle.
 * 
 * @param error pointer to error code, in case routine fails
 * 
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long _DLLFunc create_sec_key (devserver ds, DevLong *error)
{
	long	connection_id;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\ncreate_sec_key() : entering routine \n");

	/*
	 * Ignore any security key treatment in case of
	 * a connection to an old service version.
	 */

	if ( ds->vers_number > DEVSERVER_VERS )
	{
		connection_id = ds->ds_id >> CONNECTIONS_SHIFT;
		connection_id = connection_id & CONNECTIONS_MASK;

		/*
	    * Check whether the field in the list of connections
	    * indicated by connection_id is still free.
	    */

		if ( conn_list.sec_keys[(_Int)connection_id] != FREE_CONNECTION )
		{
			*error = DevErr_CannotStoreSecKey;
			return (DS_NOTOK);
		}

		/*
	    * Create and store security key
	    */

		conn_list.sec_keys[(_Int)connection_id] = ds->ds_id +
		    ds->dev_access +
		    ds->prog_number;

		dev_printdebug (DBG_SEC, "create_sec_key() : sec_key   = %d\n", conn_list.sec_keys[(_Int)connection_id]);
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "create_sec_key() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPI
 * Verify the information in the device server client handle, by creating a security key and
 * comparing this key with the stored key for the connection.
 * If the security key is valid, the client ID is returned.
 *
 * @param ds device server client handle.
 *
 * @param ret_client_id will be set to the client ID.
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long _DLLFunc verify_sec_key (devserver ds, long *ret_client_id, DevLong *error)
{
	long	sec_key;
	long	connection_id;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nverify_sec_key() : entering routine \n");

	/*
	 * Ignore any security key treatment in case of
	 * a connection to an old service version.
	 */

	if ( ds->vers_number > DEVSERVER_VERS )
	{

		/*
	    * Create a security key from the client handle
	    * and compare it with the stored one in the list
	    * of connections.
	    */

		sec_key = ds->ds_id +
		    ds->dev_access +
		    ds->prog_number;

		connection_id = ds->ds_id >> CONNECTIONS_SHIFT;
		connection_id = connection_id & CONNECTIONS_MASK;

		dev_printdebug (DBG_SEC, "verify_sec_key() : new sec_key = %d\n", sec_key);
		dev_printdebug (DBG_SEC, "verify_sec_key() : sec_key = %d  conn_id = %d\n", 
			 conn_list.sec_keys[(_Int)connection_id], connection_id);

		if ( sec_key != conn_list.sec_keys[(_Int)connection_id] )
		{
			*error = DevErr_SecurityKeyNotValid;
			return (DS_NOTOK);
		}
	}

	/*
	 * return the client ID
	 */

	*ret_client_id = client_id;

	dev_printdebug (DBG_TRACE | DBG_SEC, "verify_sec_key() : leaving routine \n");

	return (DS_OK);
}


/**
 * @ingroup secAPI
 * Free the field with the security key in the list of connections. 
 * The field can be reused with the next import of a device. 
 *
 * @param ds device server client handle.
 */
void _DLLFunc free_sec_key (devserver ds)
{
	long	connection_id;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nfree_sec_key() : entering routine \n");

	/*
	 * Ignore any security key treatment in case of
	 * a connection to an old service version.
	 */

	if ( ds->vers_number > DEVSERVER_VERS )
	{
		connection_id = ds->ds_id >> CONNECTIONS_SHIFT;
		connection_id = connection_id & CONNECTIONS_MASK;

		dev_printdebug (DBG_SEC, "free_sec_key() : sec_key = %d  conn_id = %d\n", 
		    conn_list.sec_keys[(_Int)connection_id], connection_id);

		conn_list.sec_keys[(_Int)connection_id] = FREE_CONNECTION;
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "free_sec_key() : leaving routine \n");
}

/**
 * @ingroup secAPI
 * Test for single user request. 
 * A single user request can be only imported, if the device is not yet in 
 * single user or administration mode.
 * Set the device in single user or administration mode if it was requested.
 * Store the client identification, connection_id access right and the peer address 
 * of the TCP socket for a single user.
 *
 * @param device structure for the exported device.
 * @param connection_id client connection to the device.
 * @param client_id client identification for the connection.
 * @param access_right requested access on the device.
 * @param rqstp RPC request structure.
 *
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long _DLLFunc sec_svc_import (DevServerDevices *device, long connection_id,
			      long client_id, long access_right,
			      struct svc_req *rqstp, DevLong *error)
{
#ifdef _XOPEN_SOURCE_EXTENDED
	unsigned int ulen = sizeof(device->si_peeraddr);
#else
	int          len  = sizeof(device->si_peeraddr);
#endif /* _XOPEN_SOURCE_EXTENDED */

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_svc_import() : entering routine \n");
	dev_printdebug (DBG_SEC, "sec_svc_import() : client_id     = %d\n", client_id);
	dev_printdebug (DBG_SEC, "sec_svc_import() : connection_id = %d\n", connection_id);
	dev_printdebug (DBG_SEC, "sec_svc_import() : access_right  = %d\n",access_right);

	*error = 0;

	/*
	 * Is the requested access a single user or administration access?
	 */

	switch (access_right)
	{
	case ADMIN_ACCESS:
		/*
		 * Reject import request, if the device is already in
		 * administration mode.
		 */

		if ( device->admin_user_flag == True )
		{
			/*
		    * Validate the registered administrator connection
		    * before rejecting access.
		    */

			if ( sec_verify_tcp_conn (device) == DS_OK)
			{
				*error = DevErr_DeviceIsLockedInAdminMode;
				return (DS_NOTOK);
			}
		}

		/*
		 * Set the device into administration mode for
		 * this connection.
		 */

		device->single_user_flag = True;
		device->admin_user_flag  = True;

		/*
		 * Store the connection data of the administrator
		 * client to verify accesses.
		 */

		device->si_client_id     = client_id;
		device->si_connection_id = connection_id;
		device->si_access_right  = access_right;

		/*
		 * Store the tcp socket descriptor of the connection 
		 * and the peer address of the connection, which describes
		 * the client.
		 * The pointer to the RPC request structure is NULL
		 * if this is a local request.
	         */

		if ( rqstp != NULL )
		{
			device->si_tcp_socket = rqstp->rq_xprt->xp_sock;
			getpeername ((int)device->si_tcp_socket, 
#if defined (_XOPEN_SOURCE_EXTENDED)
			    (struct sockaddr *)&(device->si_peeraddr), &ulen);
#elif defined (_UCC) 
			    (struct sockaddr *)&(device->si_peeraddr), &len);
#elif HAVE_SOCKLEN_T
			    (struct sockaddr *)&(device->si_peeraddr), (socklen_t*)&len);
#elif defined (WIN32)
			    (struct sockaddr *)&(device->si_peeraddr), &len);
#else
			    (void *)&(device->si_peeraddr), &len);
#endif /* _UCC || _solaris || linux */
		}
		else
		{
			device->si_tcp_socket = LOCAL_CONNECTION;
		}

		break;

	case SI_WRITE_ACCESS:
	case SI_SU_ACCESS:
		/* 
	         * Reject import request, if the device is already in 
		 * single user mode.
		 */
		if ( device->single_user_flag == True )
		{
			/*
		    * Validate the registered single user connection
		    * before rejecting access.
		    */

			if ( sec_verify_tcp_conn (device) == DS_OK)
			{
				*error = DevErr_DeviceIsLockedInSiMode;
				return (DS_NOTOK);
			}
		}

		/*
		 * Set the device into single user mode for
		 * this connection.
		 */

		device->single_user_flag = True;

		/*
		 * Store the connection data of the single user
		 * client to verify accesses.
		 */

		device->si_client_id     = client_id;
		device->si_connection_id = connection_id;
		device->si_access_right  = access_right;

		/*
		 * Store the tcp socket descriptor of the connection 
		 * and the peer address of the connection, which describes
		 * the client.
		 * The pointer to the RPC request structure is NULL
		 * if this is a local request.
	         */

		if ( rqstp != NULL )
		{
			device->si_tcp_socket = rqstp->rq_xprt->xp_sock;
			getpeername ((int)device->si_tcp_socket, 
#if defined (_XOPEN_SOURCE_EXTENDED)
			    (struct sockaddr *)&(device->si_peeraddr), &ulen);
#elif defined (_UCC) 
			    (struct sockaddr *)&(device->si_peeraddr), &len);
#elif HAVE_SOCKLEN_T
			    (struct sockaddr *)&(device->si_peeraddr), (socklen_t*)&len);
#elif defined (WIN32)
			    (struct sockaddr *)&(device->si_peeraddr), &len);
#else
			    (void *)&(device->si_peeraddr), &len);
#endif /* _UCC || _solaris || linux */
		}
		else
		{
			device->si_tcp_socket = LOCAL_CONNECTION;
		}

		break;

	default:
		break;
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_svc_import() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPI
 * Free the single user lock, if one was set for this connection.
 *
 * @param device structure for the exported device.
 * @param connection_id client connection to the device.
 * @param client_id client identification for the connection.
 * @param access_right requested access on the device.
 *
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long _DLLFunc sec_svc_free (DevServerDevices *device, long connection_id,
			    long client_id, long access_right, DevLong *error)
{
	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_svc_free() : entering routine \n");
	dev_printdebug (DBG_SEC, "sec_svc_free() : client_id     = %d\n", client_id);
	dev_printdebug (DBG_SEC, "sec_svc_free() : connection_id = %d\n", connection_id);
	dev_printdebug (DBG_SEC, "sec_svc_free() : access_right  = %d\n", access_right);

	*error = 0;

	/*
	 * Is the device in single user or administration mode?
	 */

	if ( device->single_user_flag == True )
	{
		/*
	    * Is the client the single user or administrator?
	    */

		if ( device->si_client_id     == client_id &&
		    device->si_connection_id == connection_id &&
		    device->si_access_right  == access_right )
		{
			/*
	       * Free the single user or administration lock.
	       */

			device->admin_user_flag  = False;
			device->single_user_flag = False;
		}
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_svc_free() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPI
 * Check whether the granted access right is high enough to execute the command.
 *
 * @param device structure for the exported device.
 * @param connection_id client connection to the device.
 * @param client_id client identification for the connection.
 * @param access_right requested access on the device.
 * @param cmd requested command to execute.
 *
 * @param error pointer to error code, in case routine fails
 * 
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long _DLLFunc sec_svc_cmd (DevServerDevices *device, long connection_id,
			   long client_id, long access_right, long cmd,
			   DevLong *error)
{
#ifdef __cplusplus
	long ret;
#else
	DevServerClass 	ds_class;
	register	long i;
#endif /* __cplusplus */
	long		min_access_right = -1234L;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_svc_cmd() : entering routine \n");
	dev_printdebug (DBG_SEC, "sec_svc_cmd() : client_id     = %d\n", client_id);
	dev_printdebug (DBG_SEC, "sec_svc_cmd() : connection_id = %d\n", connection_id);
	dev_printdebug (DBG_SEC, "sec_svc_cmd() : access_right  = %d\n", access_right);

	*error = 0;

#ifdef __cplusplus
	try
	{
		min_access_right = device->device->GetMinAccessRight(cmd);
	}
	catch (const long &lError)
	{
	       *error = lError;
	       return DS_NOTOK;
	}
#else
	ds_class = device->ds->devserver.class_pointer;

	/*
	 * Search the minimum access right for the given command
	 * in the command list of the device.
	 */

	for (i = 0; i < ds_class->devserver_class.n_commands; i++)
		if (cmd == (ds_class->devserver_class.commands_list[(_Int)i].cmd))
		{
			min_access_right = ds_class->devserver_class.commands_list[(_Int)i].min_access;
			break;
		}
	if (min_access_right == -1234L)
	{
		*error = DevErr_CommandNotImplemented;
		return (DS_NOTOK);
	}
#endif /* __cplusplus */
/*
 * switch on the requested access and verify the different
 * access modes.
 */
	switch (access_right)
	{
		case ADMIN_ACCESS:
/*
 * Is the client the administrator?
 */
			if ( device->admin_user_flag  != True ||
			    device->si_client_id     != client_id ||
			    device->si_connection_id != connection_id ||
			    device->si_access_right  != access_right )
			{
					*error = DevErr_AdminAccessWasCanceled;
					return (DS_NOTOK);
			}
			break;

		case SI_WRITE_ACCESS:
		case SI_SU_ACCESS:
/*
 * Return error, if the device is in administration mode.
 */
			if (device->admin_user_flag == True)
			{
				*error = DevErr_DeviceIsLockedInAdminMode;
				return (DS_NOTOK);
			}

/*
 * Is the client the single user?
 */
			if ( device->single_user_flag != True ||
			    device->si_client_id     != client_id ||
			    device->si_connection_id != connection_id ||
			    device->si_access_right  != access_right )
			{
				*error = DevErr_SIAccessWasCanceled;
				return (DS_NOTOK);
			}
			break;

		default:
/*
 * Return error, if the device is in administration mode.
 */
			if (device->admin_user_flag == True)
			{
/*
 * Validate the registered administrator connection
 * before rejecting access.
 */
				if ( sec_verify_tcp_conn (device) == DS_OK)
				{
					*error = DevErr_DeviceIsLockedInAdminMode;
					return (DS_NOTOK);
				}
			}
/*
 * Return error, if the device is in single user mode,
 * but the client is not the single user and requests
 * a higher access right than READ_ACCESS.
 */
			if (device->single_user_flag == True && min_access_right > READ_ACCESS )
			{
/*
 * Validate the registered single user connection
 * before rejecting access.
 */
				if ( sec_verify_tcp_conn (device) == DS_OK)
				{
					*error = DevErr_DeviceIsLockedInSiMode;
					return (DS_NOTOK);
				}
			}
			break;
	}

/*
 * Now compare the minimum access right of the command
 * the maximum access right of the client.
 */
	if ( access_right < min_access_right )
	{
			*error = DevErr_CmdAccessDenied;
			return (DS_NOTOK);
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_svc_cmd() : leaving routine \n");
	return (DS_OK);
}


/**
 * @ingroup secAPI
 * A single user must use a tcp connection.
 * Change the connection described by clnt and svr_conn to TCP protocol.
 *
 * @param requested_access 
 * @param clnt Actual client handle. Will be changed to a TCP handle if necessary.
 *
 * @param svr_conn connection structure with protocol and socket information.
 * @param error pointer to error code, in case routine fails
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long _DLLFunc sec_tcp_connection (long requested_access, CLIENT **clnt,
				  server_connections *svr_conn, DevLong *error)
{
	struct  sockaddr_in     serv_adr;
#if !defined vxworks
	struct  hostent         *ht;
#else  /* !vxworks */
	int         		host_addr;
#endif /* !vxworks */
	CLIENT                  *tcp_clnt;
	char                    *hstring;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_tcp_connection() : entering routine \n");

	/*
	 * Execute the protocol change only if for a single user
	 * or administration access and if the protocol is not TCP.
	 */

	if ( requested_access != SI_WRITE_ACCESS &&
	    requested_access != SI_SU_ACCESS 	 &&
	    requested_access != ADMIN_ACCESS )
	{
		return (DS_OK);
	}

	/*
	 * For compatibility reasons with the old version 3
	 * return an error if a single user mode is requested
	 * on a connection to version 3 server.
	 */

	if ( svr_conn->vers_number == DEVSERVER_VERS )
	{
		*error = DevErr_NoSingleUserModeOnVersion3;
		return (DS_NOTOK);
	}

	/*
	 * Store the initial protocol, if this is the first single
	 * user access on the connection.
	 * If the protocol is already TCP, increase the counter of
	 * open single user connections and return.
	 */

	if ( svr_conn->rpc_protocol == D_TCP )
	{
		if ( svr_conn->open_si_connections == 0 )
		{
			svr_conn->rpc_protocol_before_si = svr_conn->rpc_protocol;
		}
		svr_conn->open_si_connections++;
		return (DS_OK);
	}

	/*
         *  Store the current RPC timeout off the old connection.
         */

	clnt_control (*clnt, CLGET_RETRY_TIMEOUT,
		      (char *) &svr_conn->rpc_retry_timeout);
	clnt_control (*clnt, CLGET_TIMEOUT, (char *) &svr_conn->rpc_timeout);

	/*
         * Before a new handle can be created,
         * verify whether it is possible to connect
         * to the remote host.
         */

	if ( rpc_check_host
	    ( svr_conn->server_host, error )
	    == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

#if !defined vxworks
	if ((ht = gethostbyname(svr_conn->server_host))
	    == NULL )
	{
		*error = DevErr_CannotCreateClientHandle;
		return (DS_NOTOK);
	}
	memcpy ( (char *)&serv_adr.sin_addr, ht->h_addr,
	    (unsigned int) ht->h_length );
#else  /* !vxworks */
	host_addr = hostGetByName(svr_conn->server_host);
	memcpy ( (char *)&serv_adr.sin_addr, &host_addr, 4);
#endif /* !vxworks */
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_port = 0;
	svr_conn->tcp_socket = RPC_ANYSOCK;

	tcp_clnt = clnttcp_create ( &serv_adr,
	    svr_conn->prog_number,
	    svr_conn->vers_number,
	    &( svr_conn->tcp_socket), 0, 0);

	if (tcp_clnt == NULL)
	{
		hstring = clnt_spcreateerror ("dev_rpc_protocol");
		dev_printerror (SEND,"%s",hstring);
		*error = DevErr_CannotCreateClientHandle;
		return (DS_NOTOK);
	}

/*
 * DO NOT destroy udp client handle , keep it for reuse
 *           - see dev_rpc_protocol()
 *
 *	clnt_destroy (*clnt);
 */
	svr_conn->udp_clnt = *clnt;

	svr_conn->clnt = tcp_clnt;
	*clnt          = tcp_clnt;
	svr_conn->tcp_clnt = tcp_clnt;

	/*
         *  Initialise the current RPC timeout to the new connection.
         */

	/* avoids retries at all costs - andy 20jan06 */
	/*clnt_control (*clnt, CLSET_RETRY_TIMEOUT,
		      (char *) &svr_conn->rpc_retry_timeout);*/
	clnt_control (*clnt, CLSET_RETRY_TIMEOUT,
		      (char *) &svr_conn->rpc_timeout);
	clnt_control (*clnt, CLSET_TIMEOUT, (char *) &svr_conn->rpc_timeout);

	/*
	 * Store the initial protocol, if this is the first single
	 * user access on the connection.
	 * Increase the counter of open single user connections 
	 * and return.
	 */
	if ( svr_conn->open_si_connections == 0 )
	{
		svr_conn->rpc_protocol_before_si = svr_conn->rpc_protocol;
	}
	svr_conn->open_si_connections++;
	svr_conn->rpc_protocol = D_TCP;

	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_tcp_connection() : leaving routine \n");

	return (DS_OK);
}

/**
 * @ingroup secAPI
 * A single user must use a tcp connection.
 * If the last single user access was closed on a connection, switch back to the initial protocol.
 *
 * @param ds device server client handle.
 * @param svr_conn connection structure with protocol and access counter.
 */
void _DLLFunc sec_free_tcp_connection (devserver ds, server_connections *svr_conn)
{
	DevLong 	error = 0;

	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_free_tcp_connection() : entering routine \n");

	/*
	 * Decrease the number of single user accesses on a connection 
	 * if a single user access will be freed.
	 */

	if ( ds->dev_access != SI_WRITE_ACCESS &&
	    ds->dev_access != SI_SU_ACCESS    &&
	    ds->dev_access != ADMIN_ACCESS )
	{
		return;
	}

	svr_conn->open_si_connections--;

	/*
	 * If the last single user access on a connection is freed,
	 * switch back to the initial protocol.
	 */

	if ( svr_conn->open_si_connections == 0 )
	{
		if ( dev_rpc_protocol (ds, (int)svr_conn->rpc_protocol_before_si, &error)
		    == DS_NOTOK )
		{
			dev_printerror_no (SEND, "sec_tcp_connection()", error);
		}
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_tcp_connection() : leaving routine \n");

	return;
}

/**
 * @ingroup secAPIintern
 * A single user must use a tcp connection.
 * The function validates the tcp connection to the client. If the connection was lost, the
 * single user or administration access on the device will be canceled.
 *
 * @param device structure for the exported device.
 * 
 * @retval DS_OK if connection valid 
 * @retval DS_NOTOK if connection no longer valid, single user access canceled.
 */
static long sec_verify_tcp_conn (DevServerDevices *device)
{
	struct sockaddr_in	peeraddr;
#ifdef _XOPEN_SOURCE_EXTENDED
	unsigned int		ulen = sizeof(peeraddr);
#else
	int			len = sizeof(peeraddr);
#endif /* XOPEN_SOURCE_EXTENDED */


	dev_printdebug (DBG_TRACE | DBG_SEC, "\nsec_verify_tcp_conn() : entering routine \n");

	/*
	 * Is the device in single user mode and
	 * is the connection not local?
	 */

	if ( device->single_user_flag == True )
	{
		if (device->si_tcp_socket != LOCAL_CONNECTION)
		{
			/*
	       * Is the socket still open and the peer address still
	       * the same?
	       */
			if ( getpeername ((int)device->si_tcp_socket,
#if defined (_XOPEN_SOURCE_EXTENDED)
			    (struct sockaddr *)&peeraddr, &ulen) == DS_NOTOK ||
#elif defined (_UCC) 
			    (struct sockaddr *)&peeraddr, &len) == DS_NOTOK ||
#elif HAVE_SOCKLEN_T
			    (struct sockaddr *)&peeraddr, (socklen_t*)&len) == DS_NOTOK ||
#elif defined (WIN32)
			    (struct sockaddr *)&peeraddr, &len) == DS_NOTOK ||
#else
			    (void *)&peeraddr, &len) == DS_NOTOK ||
#endif /* _UCC || _solaris || linux */
			    device->si_peeraddr.sin_addr.s_addr   != 

			    peeraddr.sin_addr.s_addr           ||
			    device->si_peeraddr.sin_port          != peeraddr.sin_port )
			{
				/*
		  * The connection is not valid any more.
		  * Cancel the single user mode.
		  */

				dev_printdebug (DBG_SEC, "sec_verify_tcp_conn() : Canceled single user mode!\n");
				dev_printdebug (DBG_SEC, "sec_verify_tcp_conn() : host = %X  ref_host = %X\n",
				    peeraddr.sin_addr.s_addr, device->si_peeraddr.sin_addr.s_addr);
				dev_printdebug (DBG_SEC, "sec_verify_tcp_conn() : port = %d  ref_port = %d\n",
				    peeraddr.sin_port, device->si_peeraddr.sin_port);
				dev_printdebug (DBG_SEC, "sec_verify_tcp_conn() : errno = %d\n", errno);

				device->admin_user_flag  = False;
				device->single_user_flag = False;

				return (DS_NOTOK);
			}
		}
	}

	dev_printdebug (DBG_TRACE | DBG_SEC, "sec_verify_tcp_conn() : leaving routine \n");

	return (DS_OK);
}

