#include <API.h>

#include <DevErrors.h>

#include <ctype.h>
#include <db_xdr.h>

/* Some C++ include files */

#include <iostream>
#include <NdbmClass.h>

/* Some local functions definition */

long reg_ps(char *,long,char *,long,long *);
long unreg_ps(char *,long *);

/* Variables defined in setacc_svc.c */

extern NdbmInfo dbgen;

/* Global variables */


/* Function declaration */

long reg_ps();
long unreg_ps();



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
	static db_psdev_error err;
	int num_psdev;
	psdev_elt *tmp;
	long error;
	register long i;

/* Miscellaneous init */

	num_psdev = rece->psdev_arr.psdev_arr_len;
	err.error_code = 0;
	err.psdev_err = 0;

#ifdef DEBUG
	cout << "Begin db_psdev_register" << endl;
	cout << "Host name : " << rece->h_name << endl;
	cout << "PID = " << rece->pid << endl;
	for (i = 0;i < num_psdev;i++)
	{
		tmp = &(rece->psdev_arr.psdev_arr_val[i]);
		cout << "Pseudo device name : " << tmp->psdev_name << endl;
		cout << "Refresh period : " << tmp->poll << endl;
	}
#endif

/* Return error code if the server is not connected */

	if (dbgen.connected == False)
	{
		err.error_code = DbErr_DatabaseNotConnected;
		return(&err);
	}

/* For each pseudo device, register it in the PS_NAMES table */

	for (i = 0;i < num_psdev;i++)
	{
		tmp = &(rece->psdev_arr.psdev_arr_val[i]);
		if (reg_ps(rece->h_name,rece->pid,tmp->psdev_name,
			   tmp->poll,&error) == -1)
		{
			err.error_code =  error;
			err.psdev_err = i + 1;
			return(&err);
		}
	}

/* Leave server */

#ifdef DEBUG
	cout << "End db_psdev_register" << endl;
#endif /* DEBUG */
	return(&err);
}



/****************************************************************************
*                                                                           *
*		Code for reg_ps function                		    *
*                        ------                         		    *
*                                                                           *
*    Function rule : To register a pseuo device in the PS_NAMES database    *
*		     table						    *
*									    *
*    Argin(s) : - h_name : The host name				    *
*		- pid : The process PID					    *
*		- ps_name : The pseudo device name			    *
*		- poll : The polling period				    *
*									    *
*    Argout(s) : - p_error : Pointer for an error code			    *
*									    *
*    This function returns 0 is everything OK. Otherwise, the returned value*
*    is -1 and the error code is set to the appropiate error.		    *
*                                                                           *
****************************************************************************/


long reg_ps(char *h_name,long pid,char *ps_name,long poll,long *p_error)
{
	datum key,content;
	register long i;
	long l;
	char *ps_name_low;
	char key_buf[MAX_KEY];
	char cont_buf[MAX_CONT];
	register char *tbeg;
	register char *tend;
	unsigned int diff;
	char d_name[40];

/* Make a copy of the pseudo device name in lowercase letter */

	l = strlen(ps_name);
	if ((ps_name_low = (char *)malloc(l + 1)) == (char *)NULL)
	{
		*p_error = DbErr_ServerMemoryAllocation;
		return(-1);
	}
	for (i = 0;i < l;i++)
		ps_name_low[i] = tolower(ps_name[i]);
	ps_name_low[i] = '\0';

/* First, check that the name used for the pseudo device is not already used
   for a real device */

	for (key = dbm_firstkey(dbgen.tid[0]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[0]))
	{
		content = dbm_fetch(dbgen.tid[0],key);
		if (content.dptr == NULL)
		{
			free(ps_name_low);
			*p_error = DbErr_DatabaseAccess;
			return(-1);
		}
		tbeg = content.dptr;
		tend = strchr(tbeg,'|');
		diff = (unsigned int)(tend - tbeg);
		strncpy(d_name,tbeg,diff);
		d_name[diff] = '\0';

		if (strcmp(d_name,ps_name_low) == 0)
			break;
	}

	if (key.dptr == NULL)
	{
		if (dbm_error(dbgen.tid[0]) != 0)
		{
			free(ps_name_low);
			*p_error = DbErr_DatabaseAccess;
			return(-1);
		}
	}
	else
	{
		free(ps_name_low);
		*p_error = DbErr_NameAlreadyUsed;
		return(-1);
	}

/* Now, try to retrieve a tuple in the PS_NAMES table with the same pseudo
   device name */

	strcpy(key_buf,ps_name_low);

	key.dptr = key_buf;
	key.dsize = strlen(key_buf);

	content = dbm_fetch(dbgen.tid[dbgen.ps_names_index],key);
	if (content.dptr == NULL)
	{

/* In case of error */

		if (dbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
		{
			free(ps_name_low);
			*p_error = DbErr_DatabaseAccess;
			return(-1);
		}

/* Insert a new record in database */

		else
		{
			strcpy(cont_buf,h_name);
			strcat(cont_buf,"|");
			sprintf(&(cont_buf[strlen(cont_buf)]),"%d",pid);
			strcat(cont_buf,"|");
			sprintf(&(cont_buf[strlen(cont_buf)]),"%d",poll);
			strcat(cont_buf,"|");

			content.dptr = cont_buf;
			content.dsize = strlen(cont_buf);
			
			if (dbm_store(dbgen.tid[dbgen.ps_names_index],key,content,DBM_INSERT) != 0)
			{
				free(ps_name_low);
				*p_error = DbErr_DatabaseAccess;
				return(-1);
			}
		}
	}

	else
	{

/* Update database information */

		strcpy(cont_buf,h_name);
		strcat(cont_buf,"|");
		sprintf(&(cont_buf[strlen(cont_buf)]),"%d",pid);
		strcat(cont_buf,"|");
		sprintf(&(cont_buf[strlen(cont_buf)]),"%d",poll);
		strcat(cont_buf,"|");

		content.dptr = cont_buf;
		content.dsize = strlen(cont_buf);
			
		if (dbm_store(dbgen.tid[dbgen.ps_names_index],key,content,DBM_REPLACE) != 0)
		{
			free(ps_name_low);
			*p_error = DbErr_DatabaseAccess;
			return(-1);
		}
	}

/* Free memory and leave function */

	free(ps_name_low);
	return(0);

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
	static db_psdev_error err;
	u_int num_psdev;
	register long i;
	long error;

/* Miscellaneous init */

	num_psdev = rece->arr1_len;
	err.error_code = 0;
	err.psdev_err = 0;

#ifdef DEBUG
	cout << "Begin db_psdev_unregister" << endl;
	for (i = 0;i < num_psdev;i++)
		cout << "Pseudo device name : " << rece->arr1_val[i] << endl;
#endif

/* Return error code if the server is not connected */

	if (dbgen.connected == False)
	{
		err.error_code = DbErr_DatabaseNotConnected;
		return(&err);
	}

/* For each pseudo device, unregister it in the PS_NAMES table */

	for (i = 0;i < num_psdev;i++)
	{
		if (unreg_ps(rece->arr1_val[i],&error) == -1)
		{
			err.error_code =  error;
			err.psdev_err = i + 1;
			return(&err);
		}
	}

/* Leave server */

#ifdef DEBUG
	cout << "End db_psdev_unregister" << endl;
#endif /* DEBUG */
	return(&err);
}



/****************************************************************************
*                                                                           *
*		Code for unreg_ps function                		    *
*                        --------                         		    *
*                                                                           *
*    Function rule : To unregister pseuo device from the database PS_NAMES  *
*		     table						    *
*									    *
*    Argin(s) : - ps_name : The pseudo device name			    *
*									    *
*    Argout(s) : - p_error : Pointer for an error code			    *
*									    *
*    This function returns 0 is everything OK. Otherwise, the returned value*
*    is -1 and the error code is set to the appropiate error.		    *
*                                                                           *
****************************************************************************/


long unreg_ps(char *ps_name,long *p_error)
{
	register long i;
	long l;
	char *ps_name_low;
	datum key,content;
	char key_buf[MAX_KEY];

/* Make a copy of the pseudo device name in lowercase letter */

	l = strlen(ps_name);
	if ((ps_name_low = (char *)malloc(l + 1)) == (char *)NULL)
	{
		*p_error = DbErr_ServerMemoryAllocation;
		return(-1);
	}
	for (i = 0;i < l;i++)
		ps_name_low[i] = tolower(ps_name[i]);
	ps_name_low[i] = '\0';

/* Retrieve a tuple in the PS_NAMES table with the same pseudo device name */

	strcpy(key_buf,ps_name_low);

	key.dptr = key_buf;
	key.dsize = strlen(key_buf);

	content = dbm_fetch(dbgen.tid[dbgen.ps_names_index],key);

	if (content.dptr == NULL)
	{
		if (dbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
		{
			free(ps_name_low);
			*p_error = DbErr_DatabaseAccess;
			return(-1);
		}
		else
		{
			free(ps_name_low);
			*p_error = DbErr_DeviceNotDefined;
			return(-1);
		}
	}
	else
	{

/* Remove pseudo device */

		if (dbm_delete(dbgen.tid[dbgen.ps_names_index],key) != 0)
		{
			free(ps_name_low);
			*p_error = DbErr_DatabaseAccess;
			return(-1);
		}

	}

/* Free memory and leave function */

	free(ps_name_low);
	return(0);

}
