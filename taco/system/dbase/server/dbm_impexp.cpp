#include <API.h>

#include <DevErrors.h>

#include <db_xdr.h>
#include <fcntl.h>

/* Some C++ include files */

#include <iostream>
#include <NdbmClass.h>

/* Some local function definition */

int db_store_3(db_devinfo_3);
int db_store_2(db_devinfo_2);
int db_store(db_devinfo);

/* Variables defined elsewhere */

extern NdbmInfo dbgen;

/* Global variables */

db_resimp back;



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
	int i,db_err;
	u_int num_dev;
	static int errcode;

	num_dev = rece->tab_dbdev_len;
#ifdef DEBUG
	for (i=0;i<num_dev;i++)
	{
		cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << endl;
		cout << "Host name : " << rece->tab_dbdev_val[i].host_name << endl;
		cout << "Program number : " << rece->tab_dbdev_val[i].p_num << endl;
		cout << "Version number : " << rece->tab_dbdev_val[i].v_num << endl;
		cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << endl;
		cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << endl;
	}
#endif

/* Initialize error code sended back to client */

	errcode = 0;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}

/* Store values in database */

	for (i=0;i<num_dev;i++)
	{
		if ((db_err = db_store(rece->tab_dbdev_val[i])) != 0)
		{
			errcode = db_err;
			return(&errcode);
		}
	}

	return(&errcode);
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
	int i,db_err;
	u_int num_dev;
	static int errcode;

	num_dev = rece->tab_dbdev_len;
#ifdef DEBUG
	for (i=0;i<num_dev;i++)
	{
		cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << endl;
		cout << "Host name : " << rece->tab_dbdev_val[i].host_name << endl;
		cout << "Program number : " << rece->tab_dbdev_val[i].p_num << endl;
		cout << "Version number : " << rece->tab_dbdev_val[i].v_num << endl;
		cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << endl;
		cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << endl;
		cout << "Device server PID : " << rece->tab_dbdev_val[i].pid << endl;
	}
#endif

/* Initialize error code sended back to client */

	errcode = 0;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}

/* Store values in database */

	for (i=0;i<num_dev;i++)
	{
		if ((db_err = db_store_2(rece->tab_dbdev_val[i])) != 0)
		{
			errcode = db_err;
			return(&errcode);
		}
	}

	return(&errcode);
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
	int i,db_err;
	u_int num_dev;
	static int errcode;

	num_dev = rece->tab_dbdev_len;
#ifdef DEBUG
	for (i=0;i<num_dev;i++)
	{
		cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << endl;
		cout << "Host name : " << rece->tab_dbdev_val[i].host_name << endl;
		cout << "Program number : " << rece->tab_dbdev_val[i].p_num << endl;
		cout << "Version number : " << rece->tab_dbdev_val[i].v_num << endl;
		cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << endl;
		cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << endl;
		cout << "Device server PID : " << rece->tab_dbdev_val[i].pid << endl;
		cout << "Device server process name : " << rece->tab_dbdev_val[i].proc_name << endl;
	}
#endif

/* Initialize error code sended back to client */

	errcode = 0;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}

/* Store values in database */

	for (i=0;i<num_dev;i++)
	{
		if ((db_err = db_store_3(rece->tab_dbdev_val[i])) != 0)
		{
			errcode = db_err;
			return(&errcode);
		}
	}

	return(&errcode);
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
	int i,j,resu,num_dev,flags;
	datum key;
	datum content;
	device ret;
	register db_devinfo *stu_addr,*stu_addr1;	
	char *tbeg, *tend;
	int diff;
	int exit = 0;
	char ret_host_name[20];
	char ret_dev_name[24];
	int ret_pn;
	int ret_vn;
	char ret_dev_type[24];
	char ret_dev_class[24];
	char prgnr[12];
	

	num_dev = de_name->arr1_len;
 
#ifdef DEBUG
	for (i=0;i<num_dev;i++)
		cout << "Device name (in import function) : " << de_name->arr1_val[i] << endl;
#endif

/* Initialize error code sended back to client */

	back.db_imperr = 0;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		back.db_imperr = DbErr_DatabaseNotConnected;
		back.imp_dev.tab_dbdev_len = 0;
		back.imp_dev.tab_dbdev_val = NULL;
		return(&back);
	}

/* Allocate memory for the array of db_devinfo structures */

	if ((back.imp_dev.tab_dbdev_val = (db_devinfo *)calloc(num_dev,sizeof(db_devinfo))) == NULL )
	{
		back.db_imperr = DbErr_ServerMemoryAllocation;
		back.imp_dev.tab_dbdev_len = 0;
		return(&back);
	}

/* A loop on every device to import */

	for (i=0;i<num_dev;i++)
	{

/* Initialise exit flag */

		exit = 0;

/* Try to retrieve the tuple in the NAMES table */

		key = gdbm_firstkey(dbgen.tid[0]);
		if (key.dptr == NULL)
		{
			free(back.imp_dev.tab_dbdev_val);
			back.db_imperr = DbErr_DatabaseAccess;
			back.imp_dev.tab_dbdev_len = 0;
			return(&back);
		}

		do
		{
			content = gdbm_fetch(dbgen.tid[0], key);

			if (content.dptr != NULL)
			{
		
				tbeg = content.dptr;
				if ((tend = strchr(tbeg, '|')) != NULL)
				{
					diff = (u_int)(tend++ - tbeg);
					strncpy(ret_dev_name, tbeg, diff);
					ret_dev_name[diff] = '\0';
		
					if (strcmp(ret_dev_name, de_name->arr1_val[i]) == 0)
					{
						exit = 1;
/* Unpack the content */
						tbeg = content.dptr;
						tend = strchr(tbeg, '|');
						diff = (u_int)(tend++ - tbeg);
						strncpy(ret_dev_name, tbeg, diff);
						ret_dev_name[diff] = '\0';
		
						tbeg = tend;

						tend = strchr(tbeg, '|');
						diff = (u_int)(tend++ - tbeg);
						strncpy(ret_host_name, tbeg, diff);
						ret_host_name[diff] = '\0';

						tbeg = tend;

						tend = strchr(tbeg, '|');
						diff = (u_int)(tend++ - tbeg);
						strncpy(prgnr, tbeg, diff);
						prgnr[diff] = '\0';
						ret_pn = atoi(prgnr);
		
						tbeg = tend;

						tend = strchr(tbeg, '|');
						diff = (u_int)(tend++ - tbeg);
						strncpy(prgnr, tbeg, diff);
						prgnr[diff] = '\0';
						ret_vn = atoi(prgnr);

						tbeg = tend;

						tend = strchr(tbeg, '|');
						diff = (u_int)(tend++ - tbeg);
						strncpy(ret_dev_type, tbeg, diff);
						ret_dev_type[diff] = '\0';

						tbeg = tend;

						tend = strchr(tbeg, '|');
						if (tend == NULL) 
							diff = strlen(tbeg);
						else
							diff = (u_int)(tend++ - tbeg);
						strncpy(ret_dev_class, tbeg, diff);
						ret_dev_class[diff] = '\0';

					} /* end of found */
				} /* end of not NULL content */
			} /* end of not NULL separator */

			if (exit == 0)
			{
		 		key = gdbm_nextkey(dbgen.tid[0], key); 
				if (key.dptr == NULL) 
					exit = 1;
		
			} /*end of exit if */
		} /* end of do */
		while (!exit);

/* In case of error */

		if ((content.dptr == NULL) || (key.dptr == NULL) || (ret_pn == 0))
		{
			for (j=0;j<i;j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				free(stu_addr->dev_name);
				free(stu_addr->host_name);
				free(stu_addr->dev_type);
				free(stu_addr->dev_class);
			}
			back.imp_dev.tab_dbdev_len = 0;
			if (key.dptr == NULL)
				back.db_imperr = DbErr_DeviceNotDefined;
			else if (content.dptr == NULL)
				back.db_imperr = DbErr_DatabaseAccess;
			else
				back.db_imperr = DbErr_DeviceNotExported;
			return(&back);
		}

/* Allocate memory for the host_name string */

		stu_addr1 = &(back.imp_dev.tab_dbdev_val[i]);
		if ((stu_addr1->host_name = (char *)malloc(strlen(ret_host_name) + 1)) == NULL)
		{
			for (j=0;j<i;j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				free(stu_addr->dev_name);
				free(stu_addr->host_name);
				free(stu_addr->dev_class);
				free(stu_addr->dev_type);
			}
			back.imp_dev.tab_dbdev_len = 0;
			back.db_imperr = DbErr_ServerMemoryAllocation;
			return(&back);
		}

/* Allocate memory for the device name string */

		if ((stu_addr1->dev_name = (char *)malloc(strlen(de_name->arr1_val[i]) + 1)) == NULL)
		{
			free(stu_addr1->host_name);
			for (j=0;j<i;j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				free(stu_addr->dev_name);
				free(stu_addr->host_name);
				free(stu_addr->dev_type);
				free(stu_addr->dev_class);
			}
			back.imp_dev.tab_dbdev_len = 0;
			back.db_imperr = DbErr_ServerMemoryAllocation;
			return(&back);
		}

/* Allocate memory for the device type string */

		if ((stu_addr1->dev_type = (char *)malloc(strlen(ret_dev_type) + 1)) == NULL)
		{
			free(stu_addr1->host_name);
			free(stu_addr1->dev_name);
			for (j=0;j<i;j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				free(stu_addr->dev_name);
				free(stu_addr->host_name);
				free(stu_addr->dev_type);
				free(stu_addr->dev_class);
			}
			back.imp_dev.tab_dbdev_len = 0;
			back.db_imperr = DbErr_ServerMemoryAllocation;
			return(&back);
		}

/* Allocate memory for the device class string */

		if ((stu_addr1->dev_class = (char *)malloc(strlen(ret_dev_class) + 1)) == NULL)
		{
			free(stu_addr1->host_name);
			free(stu_addr1->dev_name);
			free(stu_addr1->dev_type);
			for (j=0;j<i;j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				free(stu_addr->dev_name);
				free(stu_addr->host_name);
				free(stu_addr->dev_type);
				free(stu_addr->dev_class);
			}
			back.imp_dev.tab_dbdev_len = 0;
			back.db_imperr = DbErr_ServerMemoryAllocation;
			return(&back);
		}

/* Initialize structure sended back to client */

		strcpy(stu_addr1->host_name,ret_host_name);
		strcpy(stu_addr1->dev_name,de_name->arr1_val[i]);
		stu_addr1->p_num = ret_pn;
		stu_addr1->v_num = ret_vn;
		strcpy(stu_addr1->dev_class,ret_dev_class);
		strcpy(stu_addr1->dev_type,ret_dev_type);

	} /* end of for for devices */

	back.imp_dev.tab_dbdev_len = num_dev;
	return(&back);
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
	static int mis;
	register char *tmp,*sto,*temp;
	register int i;
	device dev;
	unsigned int diff;
	datum key;
	datum content, dev1;
	char dev_str[MAX_CONT];
	char key_str[MAX_KEY];
	int resu,d_num;
	int dev_numb = 1;
	char seqnr[4];
	int exit = 0;
	int flags;
	char proc_str[40];
	char pers_str[40];
	char p_num[20];
	char dev_class[40];
	int exit_loop;
	int old_d_num;

#ifdef DEBUG
	cout << "Device server name (unregister function) : " << *dsn_name << endl;
#endif

/* Miscallaneous initialization */
	
	d_num = 0;
	mis = 0;
	sto = *dsn_name;
	flags = GDBM_REPLACE;
	exit_loop = False;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		mis = DbErr_DatabaseNotConnected;
		return(&mis);
	}

/* Get device server class */

	tmp = strchr(sto,'/');
	diff = (u_int)(tmp++ - sto);
	strncpy(dev.ds_class,sto,diff);
	dev.ds_class[diff] = '\0';

/* Get device server name */

	strcpy(dev.ds_name,tmp);

#ifdef DEBUG
	cout << "Device server class (unreg) : " << dev.ds_class << endl;
	cout << "Device server name (unreg) : " << dev.ds_name << endl;
#endif

/* Try to retrieve devices in database assuming that the input device server
   name is the device server PROCESS name. As there is no key build on
   the device server process name, do a full traversal of the database */

	do
	{

		old_d_num = d_num;
		for (key = gdbm_firstkey(dbgen.tid[0]);key.dptr != NULL;key = gdbm_nextkey(dbgen.tid[0], key))
		{

/* Extract personal name and sequence field from key */

			tmp = strchr(key.dptr,'|');
			tmp++;
			temp = strchr(tmp,'|');
			diff = (unsigned int)(temp++ - tmp);
			strncpy(pers_str,tmp,diff);
			pers_str[diff] = '\0';

			if (strcmp(dev.ds_name,pers_str) != 0)
				continue;

			strncpy(key_str,key.dptr,key.dsize);
			key_str[key.dsize] = '\0';

/* Get db content */

			content = gdbm_fetch(dbgen.tid[0],key);
			if (content.dptr == NULL)
			{
				if (gdbm_error(dbgen.tid[0]) != 0)
				{
					mis = DbErr_DatabaseAccess;
					return(&mis);
				}
			}

/* Extract program number */

			tmp = content.dptr;
			for (i = 0;i < 2;i++)
			{
				temp = strchr(tmp,'|');
				temp++;	
				tmp = temp;
			}
			temp = strchr(tmp,'|');
			diff = (unsigned int)(temp - tmp);
			strncpy(p_num,tmp,diff);
			p_num[diff] = '\0';

			if (strcmp(p_num,"0") == 0)
				continue;

/* Extract device class */

			tmp = content.dptr;
			for (i = 0;i < 5;i++)
			{
				temp = strchr(tmp,'|');
				temp++;	
				tmp = temp;
			}
			temp = strchr(tmp,'|');
			diff = (unsigned int)(temp - tmp);
			strncpy(dev_class,tmp,diff);
			dev_class[diff] = '\0';

/* Extract process name */

			tmp = content.dptr;
			for (i = 0;i < 7;i++)
			{
				temp = strchr(tmp,'|');
				temp++;	
				tmp = temp;
			}
			temp = strchr(tmp,'|');
			diff = (unsigned int)(temp - tmp);
			strncpy(proc_str,tmp,diff);
			proc_str[diff] = '\0';

			if (strcmp(dev.ds_class,proc_str) != 0)
				continue;

/* A device to be unregistered has been found, build the new database content */

			sto = content.dptr;
			tmp = strchr(sto,'|');
			tmp++;
			temp = strchr(tmp,'|');
			diff = (u_int)(temp - sto);
			strncpy(dev_str, sto, diff);
			dev_str[diff] = '\0';

			strcat(dev_str,"|");
			strcat(dev_str,"0");
			strcat(dev_str,"|");
			strcat(dev_str,"0");
			strcat(dev_str,"|");
			strcat(dev_str,"unknown");
			strcat(dev_str,"|");
			strcat(dev_str,dev_class);
			strcat(dev_str,"|");
			strcat(dev_str,"0");
			strcat(dev_str,"|");
			strcat(dev_str,proc_str);
			strcat(dev_str,"|");

/* Update database */

			dev1.dptr = dev_str;
			dev1.dsize = strlen(dev_str);
			key.dptr = key_str;
			key.dsize = strlen(key_str);

			if (gdbm_store(dbgen.tid[0], key, dev1, flags))
			{
				mis = DbErr_DatabaseAccess;
				return(&mis);
			}
			d_num++;
			break;
		}

		if (old_d_num == d_num)
			exit_loop = True;
	}
	while (exit_loop == False);

/* Initialization needed to retrieve the right tuples in the NAMES table
   and to update the tuples (program and version number) assuming the input
   name is a device server name*/

	if (d_num == 0)
	{
		do
		{
			strcpy(key_str, dev.ds_class);
			strcat(key_str, "|");
			strcat(key_str, dev.ds_name);
			strcat(key_str, "|");
			sprintf(seqnr, "%d", dev_numb);
			strcat(key_str, seqnr);
			strcat(key_str, "|");
			key.dptr = key_str;
			key.dsize = strlen(key_str);

/* Try to retrieve the tuples */

			content = gdbm_fetch(dbgen.tid[0], key); 

			if (content.dptr != NULL)
			{
				d_num++;

/* Extract device class */

				tmp = content.dptr;
				for (i = 0;i < 5;i++)
				{
					temp = strchr(tmp,'|');
					temp++;	
					tmp = temp;
				}
				temp = strchr(tmp,'|');
				diff = (unsigned int)(temp - tmp);
				strncpy(dev_class,tmp,diff);
				dev_class[diff] = '\0';

/* Build the new database content */

				sto = content.dptr;
				tmp = strchr(sto,'|');
				tmp++;
				temp = strchr(tmp,'|');
				diff = (u_int)(temp - sto);
				strncpy(dev_str, sto, diff);
				dev_str[diff] = '\0';

				strcat(dev_str,"|");
				strcat(dev_str,"0");
				strcat(dev_str,"|");
				strcat(dev_str,"0");
				strcat(dev_str,"|");
				strcat(dev_str,"unknown");
				strcat(dev_str,"|");
				strcat(dev_str,dev_class);
				strcat(dev_str,"|");
				strcat(dev_str,"0");
				strcat(dev_str,"|");

				tmp = content.dptr;
				for (i = 0;i < 7;i++)
				{
					temp = strchr(tmp,'|');
					temp++;	
					tmp = temp;
				}
				temp = strchr(tmp,'|');
				diff = (unsigned int)(temp - tmp);
				strncpy(proc_str,tmp,diff);
				proc_str[diff] = '\0';
			
				strcat(dev_str,proc_str);
				strcat(dev_str,"|");

/* Update database */

				dev1.dptr = dev_str;
				dev1.dsize = strlen(dev_str);

				if (gdbm_store(dbgen.tid[0], key, dev1, flags))
				{
					mis = DbErr_DatabaseAccess;
					return(&mis);
				}
				dev_numb++;
			}
			else
				exit = 1;
		} 
		while (!exit);
	}

/* In case of trouble */
	
	if ((content.dptr == NULL) && (d_num == 0))
	{
		mis = DbErr_DeviceServerNotDefined;
		return(&mis);
	}

/* No error */
	return(&mis);

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

svc_inf *db_svcchk_1_svc(nam *dsn_nam)
{
	static svc_inf send;
	static char host_name[20];
	register char *tmp, *sto;
	device dev,dev1;
	datum key;
	datum content;
	char key_str[MAX_KEY];
	unsigned int diff;
	int dev_numb = 1;
	char seqnr[4];
	char *tbeg, *tend;
	char ret_host_name[20];
	char ret_dev_name[24];
	int ret_pn;
	int ret_vn;
	char ret_dev_type[24];
	char ret_dev_class[24];
	char prgnr[12];

#ifdef DEBUG
	cout << "Device server name (check function) : " << *dsn_nam << endl;
#endif /* DEBUG */

/* Miscalaneous initialization */

	sto = *dsn_nam;
	host_name[0] = '\0';
	send.ho_name = host_name;
	send.p_num = 1;
	send.v_num = 1;
	send.db_err = 0;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		send.db_err = DbErr_DatabaseNotConnected;
		return(&send);
	}

/* Get device server class */

	tmp = strchr(sto,'/');
	diff = (u_int)(tmp++ - sto);
	strncpy(dev.ds_class,sto,diff);
	dev.ds_class[diff] = '\0';

/* Get device server name */

	strcpy(dev.ds_name,tmp);

#ifdef DEBUG
	cout << "Device server class (check) : " << dev.ds_class << endl;
	cout << "Device server name (check) : " << dev.ds_name << endl;
#endif

/* Initialization needed to retrieve the right tuples in the NAMES table */

	strcpy(key_str, dev.ds_class);
	strcat(key_str, "|");
	strcat(key_str, dev.ds_name);
	strcat(key_str, "|");
	sprintf(seqnr,"%d", dev_numb);
	strcat(key_str, seqnr);
	strcat(key_str, "|");
	key.dptr = key_str;
	key.dsize = strlen(key_str);

/* Try to retrieve the tuples */

	content = gdbm_fetch(dbgen.tid[0], key);                                

	if (content.dptr == NULL)
		send.db_err = DbErr_DeviceServerNotDefined;
	else
	{
		tbeg = content.dptr;
		tend = strchr(tbeg, '|');
		diff = (u_int)(tend++ - tbeg);
		strncpy(ret_dev_name, tbeg, diff);
		ret_dev_name[diff] = '\0';
		
		tbeg = tend;

		tend = strchr(tbeg, '|');
		diff = (u_int)(tend++ - tbeg);
		strncpy(ret_host_name, tbeg, diff);
		ret_host_name[diff] = '\0';

		tbeg = tend;

		tend = strchr(tbeg, '|');
		diff = (u_int)(tend++ - tbeg);
		strncpy(prgnr, tbeg, diff);
		prgnr[diff] = '\0';
		ret_pn = atoi(prgnr);
		
		tbeg = tend;

		tend = strchr(tbeg, '|');
		diff = (u_int)(tend++ - tbeg);
		strncpy(prgnr, tbeg, diff);
		prgnr[diff] = '\0';
		ret_vn = atoi(prgnr);

		tbeg = tend;

		tend = strchr(tbeg, '|');
		diff = (u_int)(tend++ - tbeg);
		strncpy(ret_dev_type, tbeg, diff);
		ret_dev_type[diff] = '\0';

		tbeg = tend;

		tend = strchr(tbeg, '|');
		if (tend == NULL)
			diff = strlen(tbeg);
		else
			diff = (u_int)(tend++ - tbeg);
		strncpy(ret_dev_class, tbeg, diff);
		ret_dev_class[diff] = '\0';
		strcpy(host_name, ret_host_name);

		send.p_num = ret_pn;
		send.v_num = ret_vn;
		send.ho_name = host_name;
	}

/* Leave function */

	return(&send);

}



/****************************************************************************
*                                                                           *
*		Code for db_store function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To store in the NAMES table of the database the        *
*                    number and the version number of the device server     *
*                    in charge of a device                                  *
*                                                                           *
*    Argin : - A db_devinf structure (with device name, host name, program  *
*              number and version number)                                   *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/


int db_store(db_devinfo dev_stu)
{
	datum key, key_sto;
	datum content, cont_sto;
	register db_devinfo *staddr;
	int exit = 0;
	char prgnr[12];
	char *tbeg, *tend;
	int diff;
	int flags;
	char ret_host_name[20];
	char ret_dev_name[24];

/* Miscellaneous initialisation */

	staddr = &dev_stu;
	exit = 0;

/* Try to retrieve the right tuple in the NAMES table */

	key_sto.dptr = (char *)malloc(MAX_KEY);
	cont_sto.dptr = (char *)malloc(MAX_CONT);
	
	key = gdbm_firstkey(dbgen.tid[0]);
	if (key.dptr == NULL)
	{
		free(key_sto.dptr);
		free(cont_sto.dptr);
		return(DbErr_DatabaseAccess);
	}

	strncpy(key_sto.dptr, key.dptr, key.dsize);
	key_sto.dptr[key.dsize] = '\0';
	key_sto.dsize = key.dsize;

	do
	{
		content = gdbm_fetch(dbgen.tid[0], key);

		if (content.dptr != NULL)
		{
			tbeg = content.dptr;
			if ((tend = strchr(tbeg, '|')) != NULL)
			{
				diff = (u_int)(tend++ - tbeg);
				strncpy(ret_dev_name, tbeg, diff);
				ret_dev_name[diff] = '\0';
		
				if (strcmp(ret_dev_name, staddr->dev_name) == 0)
				{
					exit = 1;
				}
			} /* not NULL separator */
		
		} /* not NULL content*/

		if (exit == 0)
		{
			key = gdbm_nextkey(dbgen.tid[0], key);
			if (key.dptr == NULL) 
				exit = 1;
			else
			{
				strncpy(key_sto.dptr, key.dptr, key.dsize);
				key_sto.dptr[key.dsize] = '\0';
				key_sto.dsize = key.dsize;
			} /* end of else */
			
		} /* end of exit if */
	} /* end of do */
	while (!exit);

/* Different result cases */

	if (key.dptr == NULL || key_sto.dptr == NULL)
	{
		free(key_sto.dptr);
		free(cont_sto.dptr);
		return(DbErr_DeviceNotDefined);
	}

/* Test to see if host name in database is the same.
   Finally, update the tuple in database */

	else
	{
		strcpy(cont_sto.dptr, staddr->dev_name);
		strcat(cont_sto.dptr, "|");
		strcat(cont_sto.dptr, staddr->host_name);
		strcat(cont_sto.dptr, "|");
		sprintf(prgnr, "%d", staddr->p_num);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		sprintf(prgnr, "%d", staddr->v_num);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr ,staddr->dev_type);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr, staddr->dev_class);
		strcat(cont_sto.dptr ,"|0|unknown");
		cont_sto.dsize = strlen(cont_sto.dptr);

		flags = GDBM_REPLACE;

		if (gdbm_store(dbgen.tid[0], key_sto, cont_sto, flags))
		{
			free(key_sto.dptr);
			free(cont_sto.dptr);
			return(DbErr_DatabaseAccess);
		}
	} /* end of else */

	free(cont_sto.dptr);
	free(key_sto.dptr);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for db_store_2 function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To store in the NAMES table of the database the        *
*                    number and the version number of the device server     *
*                    in charge of a device                                  *
*                                                                           *
*    Argin : - A db_devinf structure (with device name, host name, program  *
*              number and version number)                                   *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/


int db_store_2(db_devinfo_2 dev_stu)
{
	datum key, key_sto;
	datum content, cont_sto;
	register db_devinfo_2 *staddr;
	int exit = 0;
	char prgnr[12];
	char *tbeg, *tend;
	int diff;
	int flags;
	char ret_host_name[20];
	char ret_dev_name[24];

/* Miscellaneous initialisation */

	staddr = &dev_stu;
	exit = 0;

/* Try to retrieve the right tuple in the NAMES table */

	key_sto.dptr = (char *)malloc(MAX_KEY);
	cont_sto.dptr = (char *)malloc(MAX_CONT);
	
	key = gdbm_firstkey(dbgen.tid[0]);
	if (key.dptr == NULL)
	{
		free(key_sto.dptr);
		free(cont_sto.dptr);
		return(DbErr_DatabaseAccess);
	}

	strncpy(key_sto.dptr, key.dptr, key.dsize);
	key_sto.dptr[key.dsize] = '\0';
	key_sto.dsize = key.dsize;

	do
	{
		content = gdbm_fetch(dbgen.tid[0], key);

		if (content.dptr != NULL)
		{
			tbeg = content.dptr;
			if ((tend = strchr(tbeg, '|')) != NULL)
			{
				diff = (u_int)(tend++ - tbeg);
				strncpy(ret_dev_name, tbeg, diff);
				ret_dev_name[diff] = '\0';
		
				if (strcmp(ret_dev_name, staddr->dev_name) == 0)
				{
					exit = 1;
				}
			} /* not NULL separator */
		
		} /* not NULL content*/

		if (exit == 0)
		{
			key = gdbm_nextkey(dbgen.tid[0], key);
			if (key.dptr == NULL) 
				exit = 1;
			else
			{
				strncpy(key_sto.dptr, key.dptr, key.dsize);
				key_sto.dptr[key.dsize] = '\0';
				key_sto.dsize = key.dsize;
			} /* end of else */
			
		} /* end of exit if */
	} /* end of do */
	while (!exit);

/* Different result cases */

	if (key.dptr == NULL || key_sto.dptr == NULL)
	{
		free(key_sto.dptr);
		free(cont_sto.dptr);
		return(DbErr_DeviceNotDefined);
	}

/* Test to see if host name in database is the same.
   Finally, update the tuple in database */

	else
	{
		strcpy(cont_sto.dptr, staddr->dev_name);
		strcat(cont_sto.dptr, "|");
		strcat(cont_sto.dptr, staddr->host_name);
		strcat(cont_sto.dptr, "|");
		sprintf(prgnr, "%d", staddr->p_num);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		sprintf(prgnr, "%d", staddr->v_num);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr ,staddr->dev_type);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr, staddr->dev_class);
		strcat(cont_sto.dptr ,"|");
		sprintf(prgnr, "%d", staddr->pid);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr,"unknown");
		strcat(cont_sto.dptr ,"|");
		cont_sto.dsize = strlen(cont_sto.dptr);

		flags = GDBM_REPLACE;

		if (gdbm_store(dbgen.tid[0], key_sto, cont_sto, flags))
		{
			free(key_sto.dptr);
			free(cont_sto.dptr);
			return(DbErr_DatabaseAccess);
		}
	} /* end of else */

	free(cont_sto.dptr);
	free(key_sto.dptr);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for db_store_3 function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To store in the NAMES table of the database the        *
*                    number and the version number of the device server     *
*                    in charge of a device                                  *
*		     This function is for the version 3 of the db_dev_export*
*		     call						    *
*                                                                           *
*    Argin : - A db_devinf structure (with device name, host name, program  *
*              number, version number and process name)                     *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/

int db_store_3(db_devinfo_3 dev_stu)
{
	datum key, key_sto;
	datum content, cont_sto;
	register db_devinfo_3 *staddr;
	int exit = 0;
	char prgnr[12];
	char *tbeg, *tend;
	int diff;
	int flags;
	char ret_host_name[20];
	char ret_dev_name[24];

/* Miscellaneous initialisation */

	staddr = &dev_stu;
	exit = 0;

/* Try to retrieve the right tuple in the NAMES table */

	key_sto.dptr = (char *)malloc(MAX_KEY);
	cont_sto.dptr = (char *)malloc(MAX_CONT);
	
	key = gdbm_firstkey(dbgen.tid[0]);
	if (key.dptr == NULL)
	{
		free(key_sto.dptr);
		free(cont_sto.dptr);
		return(DbErr_DatabaseAccess);
	}

	strncpy(key_sto.dptr, key.dptr, key.dsize);
	key_sto.dptr[key.dsize] = '\0';
	key_sto.dsize = key.dsize;

	do
	{
		content = gdbm_fetch(dbgen.tid[0], key);

		if (content.dptr != NULL)
		{
			tbeg = content.dptr;
			if ((tend = strchr(tbeg, '|')) != NULL)
			{
				diff = (u_int)(tend++ - tbeg);
				strncpy(ret_dev_name, tbeg, diff);
				ret_dev_name[diff] = '\0';
		
				if (strcmp(ret_dev_name, staddr->dev_name) == 0)
					exit = 1;
			}
		
		}

		if (exit == 0)
		{
			key = gdbm_nextkey(dbgen.tid[0], key);
			if (key.dptr == NULL) 
				exit = 1;
			else
			{
				strncpy(key_sto.dptr, key.dptr, key.dsize);
				key_sto.dptr[key.dsize] = '\0';
				key_sto.dsize = key.dsize;
			} 
			
		}
	}
	while (!exit);

/* Different result cases */

	if ((key.dptr == NULL) || (key_sto.dptr == NULL))
	{
		free(key_sto.dptr);
		free(cont_sto.dptr);
		return(DbErr_DeviceNotDefined);
	}

/* Finally, update the tuple in database */

	else
	{
		strcpy(cont_sto.dptr, staddr->dev_name);
		strcat(cont_sto.dptr, "|");
		strcat(cont_sto.dptr, staddr->host_name);
		strcat(cont_sto.dptr, "|");
		sprintf(prgnr, "%d", staddr->p_num);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		sprintf(prgnr, "%d", staddr->v_num);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr ,staddr->dev_type);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr, staddr->dev_class);
		strcat(cont_sto.dptr ,"|");
		sprintf(prgnr, "%d", staddr->pid);
		strcat(cont_sto.dptr, prgnr);
		strcat(cont_sto.dptr ,"|");
		strcat(cont_sto.dptr, staddr->proc_name);
		strcat(cont_sto.dptr ,"|");
		cont_sto.dsize = strlen(cont_sto.dptr);

		flags = GDBM_REPLACE;

		if (gdbm_store(dbgen.tid[0], key_sto, cont_sto, flags))
		{
			free(key_sto.dptr);
			free(cont_sto.dptr);
			return(DbErr_DatabaseAccess);
		}
	}

	free(cont_sto.dptr);
	free(key_sto.dptr);
	return(0);

}
