#include <API.h>
#include <DevErrors.h>
#include <db_xdr.h>

// Some C++ include

#include <iostream>
#include <NdbmClass.h>
#include <fstream>

/* Some local functions declaration */

static long upd_name(char *,char *,int,long,long *);
static long del_name(device *,int *,char *,dena **,long *);
static long is_dev_in_db(db_dev_in_db *,long);
static long update_dev_list(device *,int);
static long upd_res(char *,long,char,long *);

/* Some global variables */

/* Variables defined in setacc_svc.c */

extern NdbmInfo dbgen;
extern db_res browse_back;




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
	static db_psdev_error sent_back;
	long list_nb;
	long i,last,ind;
	char *ptr,*ptr_cp,*ptr_cp2;
	char *lin,*tmp;
	char last_dev[40];
	char *ptr_free;

	list_nb = dev_list->res_val.arr1_len;
		
#ifdef DEBUG
	cout << "In upddev_1_svc function for " << list_nb << " device list(s)" << endl;
#endif

/* Initialize parameter sent back to client */

	sent_back.error_code = 0;
	sent_back.psdev_err = 0;
	
/* A loop on each device list */

	for (i = 0;i < list_nb;i++)
	{

		last = False;
				
/* Allocate memory for strtok pointers */

		lin = dev_list->res_val.arr1_val[i];

#ifdef DEBUG
		cout << "Device list = " << lin << endl;
#endif /* DEBUG */
		
		if ((ptr_cp = (char *)malloc(strlen(lin) + 1)) == NULL)
		{
			sent_back.psdev_err = i + 1;
			sent_back.error_code = DbErr_ClientMemoryAllocation;
			return(&sent_back);
		}
	
		if ((ptr = (char *)malloc(strlen(lin) + 1)) == NULL)
		{
			free(ptr_cp);
			sent_back.psdev_err = i + 1;
			sent_back.error_code = DbErr_ClientMemoryAllocation;
			return(&sent_back);
		}
		ptr_free = ptr;
		if ((ptr_cp2 = (char *)malloc(strlen(lin) + 1)) == NULL)
		{
			free(ptr_cp);
			free(ptr);
			sent_back.psdev_err = i + 1;
			sent_back.error_code = DbErr_ClientMemoryAllocation;
			return(&sent_back);
		}
		
/* Find the last device in the list. If there is no , character in the line,
   this means that there is only one device in the list */

		tmp = strrchr(lin,',');
		if (tmp == NULL)
		{
			tmp = strchr(lin,':');
			last = True;
		}
		strcpy(last_dev,tmp + 1);
	
/* Extract each device from the list and update table each time */

		strcpy(ptr_cp,lin);
		strcpy(ptr_cp2,lin);
		
		ind = 1;
		ptr = strtok(ptr_cp,",");
		
		if (upd_name(ptr,lin,ind,last,&sent_back.error_code) == -1)
		{
			free(ptr_cp);
			free(ptr);
			free(ptr_cp2);
			sent_back.psdev_err = i + 1;
			return(&sent_back);
		}

		ptr = strtok(ptr_cp2,","); /* Reinit. strtok internal pointer */
	
		while((ptr = strtok(NULL,",")) != NULL)
		{
			if (strcmp(ptr,last_dev) == 0)
				last = True;
			else
				last = False;
			
			ind++;	
			if (upd_name(ptr,lin,ind,last,&sent_back.error_code) == -1)
			{
				free(ptr_cp);
				free(ptr_free);
				free(ptr_cp2);
				sent_back.psdev_err = i + 1;
				return(&sent_back);
			}
		}
		
		free(ptr_cp);
		free(ptr_free);
		free(ptr_cp2);
	
	}

//
// Free memory and return data
//

	return(&sent_back);

}



/****************************************************************************
*                                                                           *
*		Code for upd_name function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To update the NAMES table in the database              *
*                                                                           *
*    Argin : - lin : The device name. The first time this function is called*
*		     for a device list, this buffer also contains the DS    *
*		     name and personal name				    *
*	     - ptr : The complete device list				    *
*	     - ind : The device indice list in the list (starting with 1)   *
*	     - last : A flag set to true if the device is the last one in   *
*		      the list						    *
*                                                                           *
*    Argout : p_err : Pointer for error code                                *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/

static long upd_name(char *lin,char *ptr,int ind,long last,long *p_err)
{
	static dena *tab_dena;
	device dev, ret;
	unsigned int diff;
	register char *temp,*tmp, *tbeg;
	int i,l;
	int flags;
	DBM *tup;
	static datum key, key_sto, key_sto2, key_2;
	static datum resu, content, cont_sto;
	static int ndev;
	int exit = 0;
	char prgnr[20];
	char seqnr[4];
	char temp_name[80];
	static long key_sto_base_length;

/* Allocate memory for the dbm-structures (key and content) */

	if (ind == 1)
	{
		key_sto.dptr = (char *)malloc(MAX_KEY);
		if (key_sto.dptr == NULL)
		{
			*p_err = DbErr_ServerMemoryAllocation;
			return(-1);
		}
		key_2.dptr = (char *)malloc(MAX_KEY);	
		if (key_2.dptr == NULL)
		{
			*p_err = DbErr_ServerMemoryAllocation;
			return(-1);

		}
		content.dptr = (char *)malloc(MAX_CONT);
		if (content.dptr == NULL)
		{
			*p_err = DbErr_ServerMemoryAllocation;
			return(-1);
		}

/* Get device server class */

		tmp = strchr(lin,'/');
		diff = (unsigned int)(tmp++ - lin);
		strncpy(dev.ds_class,lin,diff);
		dev.ds_class[diff] = '\0';
		strcpy(key_sto.dptr,dev.ds_class);
		strcat(key_sto.dptr,"|");

/* Get device server name */

		temp = strchr(tmp,'/');
		diff = (unsigned int)(temp++ - tmp);
		strncpy(dev.ds_name,tmp,diff);
		dev.ds_name[diff] = '\0';
		strcat(key_sto.dptr,dev.ds_name);
		strcat(key_sto.dptr, "|");
		key_sto.dsize = strlen(key_sto.dptr);
		key_sto_base_length = key_sto.dsize;

/* Get device name */

		temp = strchr(lin,':');
		diff = (unsigned int)(temp - lin);
		strcpy(dev.d_name,temp + 1);
		strcpy(content.dptr,temp + 1);
		strcat(content.dptr, "|");
	}
	else
	{
		strcpy(dev.d_name,lin);
		strcpy(content.dptr,dev.d_name);
		strcat(content.dptr,"|");
	}



/* Allocate memory for the dena structures array */

	if (ind == 1)
	{
		if ((tab_dena = (dena *)calloc(MAXDEV,sizeof(dena))) == NULL)
		{
			free(content.dptr);
			free(key_2.dptr);
			free(key_sto.dptr);
			*p_err = DbErr_ServerMemoryAllocation;
			return(-1);
		}
#ifdef DEBUG
		cout << "Memory allocated for the dena structures array" << endl;
#endif /* DEBUG */
	}
	
/* Call the del_name function */

	if (ind == 1)
	{
		if (del_name(&dev,&ndev,ptr,&tab_dena,p_err))
		{
			free(content.dptr);
			free(key_2.dptr);
			free(key_sto.dptr);
			free(tab_dena);
			return(-1);
		}
#ifdef DEBUG
		cout << "Device server class : " << dev.ds_class << endl;
		cout << "Device server name : " << dev.ds_name << endl;
		cout << "Device name : " << dev.d_name << endl;
		cout << "Device number (in device list) : " << ind << endl;
		
		cout << "Returned from the del_name function" << endl;
#endif /* DEBUG */
	}


/* Check, if the only device server is to be removed */ 

	if (strcmp(dev.d_name,"%") != 0)
	{

/* Initialize the new tuple with the right pn and vn values */

		for (i = 0;i < ndev;i++)
		{
			if (strcmp(dev.d_name,tab_dena[i].devina) == 0)
				break;
		}

		if (i == ndev)
		{

/* Initialize the content for dbm-database */

			strcat(content.dptr, "not_exp");
			strcat(content.dptr, "|");
			strcat(content.dptr, "0");
			strcat(content.dptr, "|");
			strcat(content.dptr, "0");
			strcat(content.dptr, "|");
			strcat(content.dptr, "unknown");
			strcat(content.dptr, "|");
			strcat(content.dptr, "unknown");
			strcat(content.dptr, "|");
			strcat(content.dptr, "0");
			strcat(content.dptr, "|");
			strcat(content.dptr, "unknown");
			strcat(content.dptr, "|");
			content.dsize = strlen(content.dptr);
		}
		else
		{
			dev.pn = tab_dena[i].opn;
			dev.vn = tab_dena[i].ovn;
			dev.pid = tab_dena[i].opid;
			strcpy(dev.h_name,tab_dena[i].oh_name);
			strcpy(dev.d_class,tab_dena[i].od_class);
			strcpy(dev.d_type,tab_dena[i].od_type);
			strcpy(dev.proc_name,tab_dena[i].od_proc);
		
/* Initialize the content for dbm-database */

			strcat(content.dptr, tab_dena[i].oh_name);
			strcat(content.dptr, "|");
			sprintf(prgnr,"%d", tab_dena[i].opn);
			strcat(content.dptr, prgnr);
			strcat(content.dptr, "|");
			sprintf(prgnr, "%d", tab_dena[i].ovn);
			strcat(content.dptr, prgnr);
			strcat(content.dptr, "|");
			strcat(content.dptr, tab_dena[i].od_class);
			strcat(content.dptr, "|");
			strcat(content.dptr, tab_dena[i].od_type);
			strcat(content.dptr, "|");
			sprintf(prgnr,"%d", tab_dena[i].opid);
			strcat(content.dptr, prgnr);
			strcat(content.dptr, "|");
			strcat(content.dptr, tab_dena[i].od_proc);
			strcat(content.dptr, "|");
			content.dsize = strlen(content.dptr);
		}
		dev.indi = ind;
		sprintf(seqnr, "%d", ind);
		key_sto.dptr[key_sto_base_length] = '\0';
		strcat(key_sto.dptr, seqnr);
		strcat(key_sto.dptr, "|");
		key_sto.dsize = strlen(key_sto.dptr);

/* Insert tuple in NAMES table */
#ifdef DEBUG
		cout << "Insert tuple in NAMES table" << endl;
#endif /* DEBUG */

		flags = DBM_INSERT;

		key_sto2 = key_sto;
		cont_sto = content;

		if ((i = dbm_store(dbgen.tid[0],key_sto2,cont_sto,flags)) != 0)
		{
			free(content.dptr);
			free(key_2.dptr);
			free(key_sto.dptr);
			free(tab_dena);
			if (i == 1) 
				*p_err = DbErr_DoubleTupleInNames;
			else
				*p_err = DbErr_DatabaseAccess;
			return(-1);
		}

	}
	
/* Free memory if it is the last device in the list */

	if (last == True)
	{
		free(content.dptr);
		free(key_2.dptr);
		free(key_sto.dptr);
		free(tab_dena);
	}
	
/* Leave function */

	return(0);
	
}



/****************************************************************************
*                                                                           *
*		Code for del_name function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To delete (in the database) all the devices for a      *
*                    specific device server.                                *
*                                                                           *
*    Argin : - The device server network name                               *
*	     - The address of a buffer where is saved all the device's names*
*              in charge of a device server.                                *
*              The string follows this format :                             *
*              D.S. network name:device name 1,device name 2,...            *
*                                                                           *
*    Argout : - The number of device that have been deleted                 *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/


static long del_name(device *devi,int *pndev,char *ptr,dena **buf,long *p_err)
{
	int i,j,l,tp;
	register char *ptr1;
	datum key;
	datum content;
	device ret;
	char *tbeg, *tend;
	int diff;
	char seqnr[20];
	int exit_loop,seq;
	char key_buf[MAX_KEY];
	char *devname;
	int resu;
	long nb_dev;
	db_dev_in_db *ptr_dev;

/* Miscellaneous init. */

	i = 0;
	exit_loop = False;
	seq = 1;

/* Remove all the devices already registered in the database with the same
   device server/personal name */
   
	do
	{

/* Build key */

		strcpy(key_buf,devi->ds_class);
		strcat(key_buf,"|");
		strcat(key_buf,devi->ds_name);
		strcat(key_buf,"|");
		sprintf(&(key_buf[strlen(key_buf)]),"%d",seq);
		strcat(key_buf,"|");

		key.dptr = key_buf;
		key.dsize = strlen(key_buf);

/* Try to get data out of database */

		content = dbm_fetch(dbgen.tid[0],key);

		if (content.dptr == NULL)
		{
			if (dbm_error(dbgen.tid[0]) == 0)
				exit_loop = True;
			else
			{
				*p_err = DbErr_DatabaseAccess;
		   		return(-1);
			}
		}
		else
		{

/* Realloc memory for the dena structures array if necessary */

			if ((i != 0) && (i & 0xF) == 0)
			{
				tp = i >> 4;
				*buf = (dena *)realloc(*buf,sizeof(dena) * ((tp + 1) * MAXDEV))	;
			}

/* Copy all the database information in a "dena" structure */

			tbeg = content.dptr;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy((*buf)[i].devina, tbeg, diff);
			(*buf)[i].devina[diff] = '\0';
		
			tbeg = tend;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy((*buf)[i].oh_name, tbeg, diff);
			(*buf)[i].oh_name[diff] = '\0';
		
			tbeg = tend;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy(seqnr, tbeg, diff);
			seqnr[diff] = '\0';
			(*buf)[i].opn = atoi(seqnr);
		
			tbeg = tend;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy(seqnr, tbeg, diff);
			seqnr[diff] = '\0';
			(*buf)[i].ovn = atoi(seqnr);

			tbeg = tend;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy((*buf)[i].od_type, tbeg, diff);
			(*buf)[i].od_type[diff] = '\0';       

			tbeg = tend;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy((*buf)[i].od_class, tbeg, diff);
			(*buf)[i].od_class[diff] = '\0';       

			tbeg = tend;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy(seqnr,tbeg,diff);
			seqnr[diff] = 0;
			(*buf)[i].opid = atoi(seqnr);

			tbeg = tend;
			tend = strchr(tbeg,'|');
			diff = (unsigned int)(tend++ - tbeg);
			strncpy((*buf)[i].od_proc,tbeg,diff);
			(*buf)[i].od_proc[diff] = '\0';
			i++;

/* Delete database entry */

			dbm_delete(dbgen.tid[0],key);
			seq++;
		}
	}while (exit_loop == False);

/* Compute how many devices are defined in the list */

	nb_dev = 1;
	l = strlen(ptr);
	for (j = 0;j < l;j++)
	{
		if (ptr[j] == ',')
			nb_dev++;
	}

/* Allocate memory for the structure array */

	if ((ptr_dev = (db_dev_in_db *)calloc(nb_dev,sizeof(db_dev_in_db))) == NULL)
	{
		*p_err = DbErr_ServerMemoryAllocation;
		return(-1);
	}
	
/* Init structure array */

	ptr1 = strtok(ptr,",");
	strcpy(ptr_dev[0].dev_name,ptr1);

	j = 0;	
	while(1)
	{
		if ((ptr1 = strtok(NULL,",")) == NULL)
			break;
		j++;
		strcpy(ptr_dev[j].dev_name,ptr1);
	}
	
/* In case of device in the device list which was not previously member of
   this device server, try to retrieve a tuple in the database for each device
   of the list */
   	
	resu = is_dev_in_db(ptr_dev,nb_dev);

/* Delete remaining device(s) from db */

	if (resu == 0)
	{
		for (j = 0;j < nb_dev;j++)
		{
			if (ptr_dev[j].found == True)
			{
				key.dptr = ptr_dev[j].key_buf;
				key.dsize = strlen(ptr_dev[j].key_buf);
				
				dbm_delete(dbgen.tid[0],key);
				if (update_dev_list(&(ptr_dev[j].dev_info),ptr_dev[j].seq + 1) == ERR_DEVNAME)
				{
					free(ptr_dev);
					*p_err = DbErr_DatabaseAccess;
					return(-1);
				}
			}
		}
	}
	else if (resu == ERR_DEVNAME)
	{
		free(ptr_dev);
		*p_err = DbErr_DatabaseAccess;
		return(-1);
	}

/* Leave function */

	free(ptr_dev);
	*pndev = i;
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for is_dev_in_db function                              *
*                        ------------                                       *
*                                                                           *
*    Function rule : To check if a device is known in the database	    *
*                                                                           *
*    Argin : - devname : The device name				    *
*                                                                           *
*    Argout : - p_dev : Pointer to a device structure with fields set if the*
*			device is defined in the database		    *
*	      - p_seq : The device sequence database entry field	    *
*	      - p_key : The database entry key				    *
*                                                                           *
*    This function returns 0 if there is a device in the datbase	    *
*    If the device is not defined in the database, the function returns     *
*    ERR_DEV_NOT_FOUND. In case of failure, the function returns ERR_DEVNAME*
*                                                                           *
****************************************************************************/

static long is_dev_in_db(db_dev_in_db *ptr,long nb_dev)
{
	static datum key;
	static datum content;
	register char *tbeg;
	register char *tend;
	static unsigned int diff;
	char seq_str[20];
	char key_buf[MAX_KEY];
	device dev;
	long j;
	
	for (key = dbm_firstkey(dbgen.tid[0]); key.dptr != NULL;key = dbm_nextkey(dbgen.tid[0]))
	{

/* Extract dserver name */

		tend = strchr(key.dptr,'|');
		if (tend == NULL)
		{	
		   	return(ERR_DEVNAME);
		}
		diff = (unsigned int)(tend++ - key.dptr);
		strncpy(dev.ds_class,key.dptr,diff);
		dev.ds_class[diff] = '\0';

/* Extract personal name */

		tbeg = tend;
		tend = strchr(tbeg,'|');
		if (tend == NULL)
		{	
	   		return(ERR_DEVNAME);
	   	}
		diff = (unsigned int)(tend++ - tbeg);
		strncpy(dev.ds_name, tbeg, diff);
		dev.ds_name[diff] = '\0';

/* Extract sequence field */

		tbeg = tend;
		tend = strchr(tbeg,'|');
		if (tend == NULL)
		{	
	   		return(ERR_DEVNAME);
	   	}
		diff = (unsigned int)(tend++ - tbeg);
		strncpy(seq_str,tbeg,diff);
		seq_str[diff] = '\0';

/* Make a copy of the key */

		strncpy(key_buf,key.dptr,key.dsize);
		key_buf[key.dsize] = '\0';

/* Get db content */

		content = dbm_fetch(dbgen.tid[0], key);
		if (content.dptr != NULL)
		{
			tend = strchr(content.dptr,'|');
			if (tend == NULL)
			{	
	   			return(ERR_DEVNAME);
	   		}
			diff = (unsigned int)(tend++ - content.dptr);
			strncpy(dev.d_name,content.dptr,diff);
			dev.d_name[diff] = '\0';

/* Wanted device ? */

			for (j = 0;j < nb_dev;j++)
			{
				if (strcmp(dev.d_name,ptr[j].dev_name) == 0)
				{
					ptr[j].found = True;
					ptr[j].seq = atoi(seq_str);
					strcpy(ptr[j].key_buf,key_buf);
					ptr[j].dev_info = dev;
				}
			}
		}
	}
	return(0);
}




/****************************************************************************
*                                                                           *
*		Code for update_dev_list function                           *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To update a device server device list when one of the  *
*		     device is removed from the list. This means to update  *
*		     the sequence field of all the remaining devices	    *
*                                                                           *
*    Argin : - p_ret : The removed device structure with the device         *
*		       information					    *
*	     - seq : The sequence field of the first device to be updated   *
*                                                                           *
*    Argout : No argout							    *
*                                                                           *
*    This function returns 0 if there is no problem. Otherwise, it returns  *
*    ERR_DEVNAME							    *
*                                                                           *
*****************************************************************************/

static long update_dev_list(device *p_ret,int seq)
{
	datum key;
	datum content;
	char key_buf[MAX_KEY];
	char cont_sto[MAX_CONT];
	int exit_loop = False;
	int ind_in_key;
	int new_seq;

/* Miscellaneous init */

	strcpy(key_buf,p_ret->ds_class);
	strcat(key_buf,"|");
	strcat(key_buf,p_ret->ds_name);
	strcat(key_buf,"|");
	ind_in_key = strlen(key_buf);

cout << "before loop in update-dev_list function" << endl;
	do
	{
	

/* Build the key */

		sprintf(&(key_buf[ind_in_key]),"%d",seq);
		strcat(key_buf,"|");

		key.dptr = key_buf;
		key.dsize = strlen(key_buf);

/* Tried to get data from the database */

		content = dbm_fetch(dbgen.tid[0],key);
		if (content.dptr == NULL)
		{
			if (dbm_error(dbgen.tid[0]) == 0)
				exit_loop = True;
			else
			{
	   			return(ERR_DEVNAME);
			}
		}
		else
		{

/* Copy the old database content */

			strncpy(cont_sto,content.dptr,content.dsize);
			cont_sto[content.dsize] = '\0';

/* Delete the entry and store a new one with a modifed sequence field */

			if (dbm_delete(dbgen.tid[0],key) != 0)
			{
	   			return(ERR_DEVNAME);
			}

			new_seq = seq;
			new_seq--;
			sprintf(&(key_buf[ind_in_key]),"%d",new_seq);
			strcat(key_buf,"|");
		
			key.dptr = key_buf;
			key.dsize = strlen(key_buf);
			content.dptr = cont_sto;
			content.dsize = strlen(cont_sto);

			if (dbm_store(dbgen.tid[0],key,content,DBM_INSERT) != 0)
			{
	   			return(ERR_DEVNAME);
			}

			seq++;
		}
	}while (exit_loop == False);
cout << "after loop in update-dev_list function" << endl;	
	return(0);
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
	static db_psdev_error sent_back;
	long list_nb;
	long i,last,ind;
	char *ptr,*ptr_cp,*ptr_cp2;
	char *lin,*tmp;
	char last_dev[40];
	char pat[2];
	char *ptr_free;

	list_nb = res_list->res_val.arr1_len;
		
#ifdef DEBUG
	cout << "In updres_1_svc function for " << list_nb << " resource(s)" << endl;
#endif

/* Initialize parameter sent back to client */

	sent_back.error_code = 0;
	sent_back.psdev_err = 0;

/* A loop on each resources */

	for (i = 0;i < list_nb;i++)
	{
		
/* Allocate memory for strtok pointers */

		lin = res_list->res_val.arr1_val[i];

#ifdef DEBUG
		cout << "Resource list = " << lin << endl;
#endif /* DEBUG */

/* Only one update if the resource is a simple one */

		if (strchr(lin,SEP_ELT) == NULL)
		{
			if (upd_res(lin,1,False,&sent_back.error_code) == -1)
			{
				sent_back.psdev_err = i + 1;
				return(&sent_back);
			}
		}
		else
		{		
			if ((ptr_cp = (char *)malloc(strlen(lin) + 1)) == NULL)
			{
				sent_back.psdev_err = i + 1;
				sent_back.error_code = DbErr_ClientMemoryAllocation;
				return(&sent_back);
			}
	
			if ((ptr = (char *)malloc(strlen(lin) + 1)) == NULL)
			{
				free(ptr_cp);
				sent_back.psdev_err = i + 1;
				sent_back.error_code = DbErr_ClientMemoryAllocation;
				return(&sent_back);
			}
			ptr_free = ptr;
			if ((ptr_cp2 = (char *)malloc(strlen(lin) + 1)) == NULL)
			{
				free(ptr_cp);
				free(ptr);
				sent_back.psdev_err = i + 1;
				sent_back.error_code = DbErr_ClientMemoryAllocation;
				return(&sent_back);
			}
				
/* Extract each resource from the list and update table each time */

			strcpy(ptr_cp,lin);
			strcpy(ptr_cp2,lin);
		
			ind = 1;
			pat[0] = SEP_ELT;
			pat[1] = '\0';
			
			ptr = strtok(ptr_cp,pat);
		
			if (upd_res(ptr,ind,False,&sent_back.error_code) == -1)
			{
				free(ptr_cp);
				free(ptr);
				free(ptr_cp2);
				sent_back.psdev_err = i + 1;
				return(&sent_back);
			}

			ptr = strtok(ptr_cp2,pat); /* Reinit. strtok internal pointer */
	
			while((ptr = strtok(NULL,pat)) != NULL)
			{
				ind++;	
				if (upd_res(ptr,ind,True,&sent_back.error_code) == -1)
				{
					free(ptr_cp);
					free(ptr_free);
					free(ptr_cp2);
					sent_back.psdev_err = i + 1;
					return(&sent_back);
				}
			}
		
			free(ptr_cp);
			free(ptr_free);
			free(ptr_cp2);
	
		}
	}

//
// Free memory and return data
//

	return(&sent_back);

}




/****************************************************************************
*                                                                           *
*		Code for upd_res function                                   *
*                        -------                                            *
*                                                                           *
*    Function rule : To update a resource in the appropriate table in       *
*                    database                                               *
*                                                                           *
*    Argin : - A pointer to the modified resource definition (without space *
*              and tab characters)                                          *
*            - The number of the resource in the array (one if the resource *
*              type is not an array)					    *
*            - A flag to inform the function that this resource is a member *
*              of an array                                                  *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/

static long upd_res(char *lin,long numb,char array,long *p_err)
{
	static char t_name[80];
	static reso res;
	reso ret;
	unsigned int diff;
	register char *temp,*tmp;
	int i,l,resu;
	int flags;
	static DBM *tab;
	datum key;
	datum content;
	char ind_name[20];
	char seqnr[4];
	int res_numb;
	static int res_pas;
	static int sec_res;
	int ctr = 0;
	int resu1;
	datum cont;
	datum key_array;
	char res_db[MAX_RES];
	int old_res_array;

	if (numb == 1)
	{
		
/* Get table name */

		temp = strchr(lin,'/');
		diff = (unsigned int)(temp++ - lin);
		strncpy(t_name,lin,diff);
		t_name[diff] = '\0';
		
		if (strcmp(t_name,"sec") == 0)
			sec_res = True;
		else
			sec_res = False;

/* Get family name */

		tmp = strchr(temp,'/');
		diff = (unsigned int)(tmp++ - temp);
		strncpy(res.fam,temp,diff);
		res.fam[diff] = '\0';

/* Get member name */

		temp = strchr(tmp,'/');
		diff = (unsigned int)(temp++ - tmp);
		strncpy(res.member,tmp,diff);
		res.member[diff] = '\0';

/* Get resource name */

		tmp = strchr(temp,':');
		diff = (unsigned int)(tmp - temp);
		strncpy(res.r_name,temp,diff);
		res.r_name[diff] = '\0';
		
/* If the resource belongs to Security domain, change every occurance of
   | by a ^ character */
   
   		if (sec_res == True)
		{
			l = strlen(res.r_name);
			for (i = 0;i < l;i++)
			{
				if (res.r_name[i] == '|')
					res.r_name[i] = SEC_SEP;
			}
		}

/* Get resource value (resource values are stored in the database as 
   case dependent strings */

		strcpy(res.r_val,tmp + 1);
	}
	else
		strcpy(res.r_val,lin);
		
/* For security domain, change every occurance of | by a ^ */

   	if (sec_res == True)
	{
		l = strlen(res.r_val);
		for (i = 0;i < l;i++)
		{
			if (res.r_val[i] == '|')
				res.r_val[i] = SEC_SEP;
		}
	}

/* Initialise resource number */

	res.indi = numb;

#ifdef DEBUG
	cout << "Table name : " << t_name << endl;
	cout << "Family name : " << res.fam << endl;
	cout << "Number name : " << res.member << endl;
	cout << "Resource name : " << res.r_name << endl;
	cout << "Resource value : " << res.r_val << endl;
	cout << "Sequence number : " << res.indi << endl << endl;
#endif /* DEBUG */

/* Select the right resource table in database */

	if (numb == 1)
	{
		for (i = 1;i < dbgen.TblNum;i++)
		{
			if (strcmp(t_name,dbgen.TblName[i].c_str()) == 0)
			{
				tab = dbgen.tid[i];
				break;
			}
		}

		if (i == dbgen.TblNum)
		{
			*p_err = DbErr_DomainDefinition;
			return(-1);
		}
	}

/* Try to retrieve the right tuple in table */

	key.dptr = (char *)malloc(MAX_KEY);
	content.dptr = (char *)malloc(MAX_CONT);

	res_numb = numb;

	strcpy(key.dptr, res.fam);
	strcat(key.dptr, "|");
	strcat(key.dptr, res.member);
	strcat(key.dptr, "|");
	strcat(key.dptr, res.r_name);
	strcat(key.dptr, "|");
	sprintf(seqnr,"%d", numb);
	strcat(key.dptr, seqnr);
	strcat(key.dptr, "|");
	key.dsize = strlen(key.dptr);


/* If the resource value is %, remove all the resources.
   If this function is called for a normal resource, I must also 
   remove all the old resources with the old name. This is necessary if there
   is an update of a resource which was previously an array */

	if ((strcmp(res.r_val,"%") == 0) || (array == False))
	{
		key_array.dptr = (char *)malloc(MAX_KEY);
		while(1)
		{
			strcpy(key.dptr, res.fam);
			strcat(key.dptr, "|");
			strcat(key.dptr, res.member);
			strcat(key.dptr, "|");
			strcat(key.dptr, res.r_name);
			strcat(key.dptr, "|");
			sprintf(seqnr,"%d", res_numb);
			strcat(key.dptr, seqnr);
			strcat(key.dptr, "|");
			key.dsize = strlen(key.dptr);

			cont = dbm_fetch(tab,key);
			if (cont.dptr == NULL)
			{
				if (dbm_error(tab) == 0)
					break;
				else
				{
					dbm_clearerr(tab);
					free(key.dptr);
					free(content.dptr);
					free(key_array.dptr);
					*p_err = DbErr_DatabaseAccess;
					return(-1);
				}
			}
			ctr++;
			strncpy(res_db,cont.dptr,cont.dsize);
			res_db[cont.dsize] = '\0';

/* The resource already exists in db. Check if one element with indoce 2 also
   exists. It it is the case, the resource is an array */

			if (ctr == 1)
			{
				strcpy(key_array.dptr, res.fam);
				strcat(key_array.dptr, "|");
				strcat(key_array.dptr, res.member);
				strcat(key_array.dptr, "|");
				strcat(key_array.dptr, res.r_name);
				strcat(key_array.dptr, "|2|");
				key_array.dsize = strlen(key_array.dptr);

				cont = dbm_fetch(tab,key_array);
				if (cont.dptr == NULL)
				{
					if (dbm_error(tab) == 0)
						old_res_array = False;
					else
					{
						dbm_clearerr(tab);
						free(key.dptr);
						free(content.dptr);
						free(key_array.dptr);
						*p_err = DbErr_DatabaseAccess;
						return(-1);
					}
				}
				else
					old_res_array = True;
			}

			dbm_delete(tab,key);
			res_numb++;
		}

		free(key_array.dptr);
		if (strcmp(res.r_val,"%") == 0)
		{
			free(key.dptr);
			free(content.dptr);
			return(0);
		}
	}

/* Insert a new tuple */

	strcpy(key.dptr, res.fam);
	strcat(key.dptr, "|");
	strcat(key.dptr, res.member);
	strcat(key.dptr, "|");
	strcat(key.dptr, res.r_name);
	strcat(key.dptr, "|");
	sprintf(seqnr,"%d", numb);
	strcat(key.dptr, seqnr);
	strcat(key.dptr, "|");
	key.dsize = strlen(key.dptr);

	strcpy(content.dptr, res.r_val);
	content.dsize = strlen(res.r_val);
	flags = DBM_REPLACE;

	if ((i = dbm_store(tab,key,content,flags)) != 0)
	{
		if (i == 1)
			*p_err = DbErr_DoubleTupleInRes;
		else
			*p_err = DbErr_DatabaseAccess;
		free(key.dptr);
		free(content.dptr);
		return(-1);
	}
	
	free(key.dptr);
	free(content.dptr);
	return(0);

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


db_res *secpass_1_svc()
{
	long i,j;
	char pass[80];
	char *base;
	
#ifdef DEBUG
	cout << "In secpass_1_svc function" << endl;
#endif

//
// Initialize structure sent back to client
//

	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	
	pass[0] = '\0';

//
// Build security file name
//

	base = (char *)getenv("DBM_DIR");
	string f_name(base);
	f_name.append("/.sec_pass");	

//
// Try to open the file
//

	ifstream f(f_name.c_str());
	
	if (!f)
	{
		browse_back.db_err = DbErr_NoPassword;
		return(&browse_back);
	}
	
//
// Get password
//

	f.getline(pass,sizeof(pass));
	
	if (strlen(pass) == 0)
	{
		browse_back.db_err = DbErr_NoPassword;
		return(&browse_back);
	}

//
// Init data sent back to client 
//	
	
	try
	{
		browse_back.res_val.arr1_val = new char * [1];
		browse_back.res_val.arr1_val[0] = new char [strlen(pass) + 1];
	
		strcpy(browse_back.res_val.arr1_val[0],pass);
	}
	catch (bad_alloc)
	{
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	browse_back.res_val.arr1_len = 1;

	
//
// Return data
//

	return(&browse_back);
	
}
