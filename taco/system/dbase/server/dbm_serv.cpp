#include <API.h>

#include <DevErrors.h>

#include <malloc.h>
#include <sys/socket.h>
#include <db_xdr.h>
#include <fcntl.h>

/* Some C++ include files */

#include <iostream>
#include <NdbmClass.h>
#include <string>

/* Funcion declarations */

int db_find(char *,char *,char **,char **,int *);
int db_devlist(char *,int *,db_res *);
int db_del(char *,char **);
int db_reinsert(arr1 *,char **,int);

/* Variables defined elsewhere */

extern NdbmInfo dbgen;
extern u_short udp_port;

/* Define external variables (to be able to free memery in server stub code */

db_res send_back;
db_res dev_back;



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


db_res *db_getres_1_svc(arr1 *rece,struct svc_req *rqstp)
{
	int i,j;
	int k = 0;
	u_int num_res,diff,err_db;
	char *ptrc,*temp;
	char tab_name[80];
	char rest[80];
	struct sockaddr_in so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
	socklen_t  so_size;   /* from POSIX draft - already used by GLIBC */
#else
	int so_size;
#endif
	u_short prot;
	char *tmp1;
	int k1 = 1;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		send_back.db_err = DbErr_DatabaseNotConnected;
		send_back.res_val.arr1_len = 0;
		send_back.res_val.arr1_val = NULL;
		return(&send_back);
	}

/* Retrieve the protocol used to send this request to server */

	so_size = sizeof(so);

#ifdef sun
	if (rqstp->rq_xprt->xp_port == udp_port)
                prot = IPPROTO_UDP;
        else
                prot = IPPROTO_TCP;
#else
	so_size = sizeof(so);
	if (getsockname(rqstp->rq_xprt->xp_sock,(struct sockaddr *)&so,&so_size) == -1)
	{
		send_back.db_err = DbErr_TooManyInfoForUDP;
		send_back.res_val.arr1_len = 0;
		return(&send_back);
	}

	if (so.sin_port == udp_port)
		prot = IPPROTO_UDP;
	else
		prot = IPPROTO_TCP;
#endif /*solaris */

	num_res = rece->arr1_len;

#ifdef DEBUG
	for(i=0;i<num_res;i++)
	{
		cout << "Resource name : " << rece->arr1_val[i] << endl;
	}
#endif

/* Initialize send_back structure error code */

	send_back.db_err = 0;

/* Allocate memory for the array of string sended back to client */

 	if((send_back.res_val.arr1_val = (char **)calloc(num_res,sizeof(nam))) == NULL)
	{
		send_back.db_err = DbErr_ServerMemoryAllocation;
		send_back.res_val.arr1_len = 0;
		return(&send_back);
	}

/* Allocate memory for the temporary buffer used in the db_find function */

	if ((tmp1 = (char *)malloc((size_t)SIZE)) == NULL)
	{
		send_back.db_err = DbErr_ServerMemoryAllocation;
		send_back.res_val.arr1_len = 0;
		return(&send_back);
	}

/* A loop on the resource's number to be looked for */

	for(i=0;i<num_res;i++)
	{
		ptrc = rece->arr1_val[i];

/* Find the table name (DOMAIN) */
		
		temp = (char *) strchr(ptrc,'/');
		diff = (u_int)(temp - ptrc);
		strncpy(tab_name,ptrc,diff);
		tab_name[diff] = '\0';

		strcpy(rest,temp + 1);

/* Try to find the resource value from database */

		if((err_db = db_find(tab_name,rest,&send_back.res_val.arr1_val[i],&tmp1,&k1)) != 0 )
		{

			for (j=0;j<=i;j++)
			{
				free(send_back.res_val.arr1_val[j]);
			}
			free(tmp1);
			send_back.db_err = err_db;
			send_back.res_val.arr1_len = 0;
			return(&send_back);
		}

/* Compute an estimation of the network packet size (Only if the UDP protocol
   has been used to send this request to the server) */

		if (prot == IPPROTO_UDP)
		{
			if ((k = strlen(send_back.res_val.arr1_val[i]) + k) > SIZE - 1000)
			{
				for (j=0;j<=i;j++)
				{
					free(send_back.res_val.arr1_val[j]);
				}
				free(tmp1);
				send_back.db_err = DbErr_TooManyInfoForUDP;
				send_back.res_val.arr1_len = 0;
				return(&send_back);
			}

		}

	}

/* Initialize the structure sended back to client */

	send_back.res_val.arr1_len = num_res;

/* Free memory */

	free(tmp1);

/* Exit server */

	return(&send_back);
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
	char *temp;
	int dev_num,err_db,i;

#ifdef DEBUG
	cout << "Device server name (getdevlist) : " << *dev_name << endl;
#endif

/* Initialize error code sended back to client */

	dev_back.db_err = 0;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		dev_back.db_err = DbErr_DatabaseNotConnected;
		dev_back.res_val.arr1_len = 0;
		dev_back.res_val.arr1_val = NULL;
		return(&dev_back);
	}

/* Call database function */

	if ((err_db = db_devlist(*dev_name,&dev_num,&dev_back)) != 0)
	{
		dev_back.db_err = err_db;
		dev_back.res_val.arr1_len = 0;
		return(&dev_back);
	}

#ifdef DEBUG
	for (i=0;i<dev_num;i++)
	{
		cout << "Device name : " << dev_back.res_val.arr1_val[i] << endl;
	}
#endif

/* Exit server */

	return(&dev_back);

}



/****************************************************************************
*                                                                           *
*		Code for db_find function                                   *
*                        -------                                            *
*                                                                           *
*    Function rule : To retrieve a resource value in the database           *
*                                                                           *
*    Argin : - The table name where the ressource can be retrieved          *
*            - A part of the resource name (FAMILY/MEMBER/RES.)             *
*            - The adress where to put the resource value (as a string)     *
*	     - The buffer's address used to store temporary results         *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/

int db_find(char *tab_name,char *p_res_name,char **out,char **adr_tmp1,int *k1)
{
	reso res, ret;
	unsigned int diff;
	char *temp, *tmp;
	int k,sec_res;
	int ctr = 0;
	DBM *tab;
	datum key;
	datum resu, resu_out;
	int res_numb = 1;
	char ind_name[20];
	int exit = 0;
	char indnr[4];
	int i;

#ifdef DEBUG
	cout << "Table name : " << tab_name << endl;
#endif

/* Set a flag if the resource belongs to security domain */

	if (strcmp(tab_name,"sec") == 0)
		sec_res = True;
	else
		sec_res = False;

/* Get family name */

	tmp = (char *) strchr(p_res_name,'/');
	diff = (u_int)(tmp++ - p_res_name);
	strncpy(res.fam,p_res_name,diff);
	res.fam[diff] = '\0';

/* Get member name */

	temp = (char *) strchr(tmp,'/');
	diff = (u_int)(temp++ - tmp);
	strncpy(res.member,tmp,diff);
	res.member[diff] = '\0';

/* Get resource name */

	strcpy(res.r_name,temp);
	
/* For security domain, change all occurances of | by ^ (| is the field
   separaor in NDBM !) */
   
	if (sec_res == True)
	{
		k = strlen(res.r_name);
		for (i = 0;i < k;i++)
		{
			if (res.r_name[i] == '|')
				res.r_name[i] = SEC_SEP;
		}
	}

#ifdef DEBUG
	cout << "Family name : " << res.fam << endl;
	cout << "Member name : " << res.member << endl;
	cout << "Resource name : " << res.r_name << endl;
#endif

/* Select the right resource table in the right database */

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (strcmp(tab_name,dbgen.TblName[i].c_str()) == 0)
		{
			tab = dbgen.tid[i];
			break;
		}
	}

	if (i == dbgen.TblNum)
		return(DbErr_DomainDefinition);


/* Try to retrieve the right tuple in table and loop in the case of an
   array of resources */

	if ((resu_out.dptr = (char *)malloc(MAX_CONT)) == NULL)
	{
		printf("Error in malloc for resu\n");
		return(DbErr_ServerMemoryAllocation);
	}
	if ((key.dptr = (char *)malloc(MAX_KEY)) == NULL)
	{
		free(resu_out.dptr);
		printf("Error in malloc for key\n");
		return(DbErr_ServerMemoryAllocation);
	}


	do
	{

		strcpy(key.dptr, res.fam);
		strcat(key.dptr,"|");
		strcat(key.dptr, res.member);
		strcat(key.dptr,"|");
		strcat(key.dptr, res.r_name);
		strcat(key.dptr,"|");
		sprintf(indnr,"%d",res_numb);
		strcat(key.dptr, indnr);
		strcat(key.dptr, "|");
		key.dsize = strlen(key.dptr);


		resu = dbm_fetch(tab, key);                                 
		if (resu.dptr != NULL)
		{
			strncpy(resu_out.dptr, resu.dptr, resu.dsize);
			resu_out.dptr[resu.dsize] = '\0';
			if (ctr)
			{

/* Copy the new array element in the result buffer. If the temporary buffer
   is full, realloc memory for it. */

				k = strlen(*adr_tmp1);
				if (k > ((*k1 * SIZE) - LIM))
				{
					if ((*adr_tmp1 = (char *)realloc(*adr_tmp1,(size_t)((*k1 + 1) * SIZE))) == NULL)
					{
						free(key.dptr);
						free(resu_out.dptr);
						return(DbErr_ServerMemoryAllocation);
					}
					(*k1)++;
				}
				(*adr_tmp1)[k] = SEP_ELT;
				(*adr_tmp1)[k + 1] = 0;
				strcat(*adr_tmp1,resu_out.dptr);
			}
			else
				strcpy(*adr_tmp1,resu_out.dptr);
			ctr++;
			res_numb++;


		} 
		else
		{
			exit = 1;
		}
	}
	while (!exit);


/* If it is a normal resource,so copy the resource value to the result buffer */


	if (ctr == 1)
	{
		if ((*out = (char *)malloc(strlen(*adr_tmp1) + 1)) == NULL)
		{
			free(key.dptr);
			free(resu_out.dptr);
			printf("Error in malloc for out\n");
			return(DbErr_ServerMemoryAllocation);
		}
		strcpy(*out,*adr_tmp1);
	}

/* For an array of resource */

	if (ctr > 1)
	{
		k = strlen(*adr_tmp1);
		if ((*out = (char *)malloc(strlen(*adr_tmp1) + 10)) == NULL)
		{
			free(key.dptr);
			free(resu_out.dptr);
			return(DbErr_ServerMemoryAllocation);
		}

		(*out)[0] = INIT_ARRAY;
		sprintf(&((*out)[1]),"%d",ctr);
		k = strlen(*out);
		(*out)[k] = SEP_ELT;
		(*out)[k + 1] = 0;
		strcat(*out,*adr_tmp1);
	}
	
/* Return if database error */

/*
	if (resu == NULL && ctr != 0)
	{
		free(key.dptr);
		free(resu_out.dptr);
		return(DbErr_DatabaseAccess);
	}
*/

/* Initialize resource value to N_DEF if the resource is not defined in the
   database */

	if (resu.dptr == NULL && ctr == 0)
	{
		if ( (*out = (char *)malloc(10)) == NULL)
		{
			free(key.dptr);
			free(resu_out.dptr);
			return(DbErr_ServerMemoryAllocation);
		}
		strcpy(*out,"N_DEF");
	}
	
/* For resource of the SEC domain, change all occurences of the ^ character
   to the | character */

	if (sec_res == True)
	{
		k = strlen(*out);   
   		for (i = 0;i < k;i++)
		{
			if ((*out)[i] == SEC_SEP)
				(*out)[i] = '|';
		}
	}
		
/* Reset the temporary buffer */

	(*adr_tmp1)[0] = 0;

	free(key.dptr);
	free(resu_out.dptr);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for db_devlist function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To retrieve all the devices name for a particular      *
*                    device server                                          *
*                                                                           *
*    Argin : - The  device server name                                      *
*            - The adress  of the db_res structure tobe initialized with    *
*              the devices names                                            *
*              (see the definition of the db_res structure above)           *
*                                                                           *
*    Argout : - The number of devices managed by the devices server         *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/


int db_devlist(char *dev_na,int *dev_num,db_res *back)
{

	register char **ptra;
	register int d_num;
	int i,j,tp;
	char *tmp;
	char *tbeg, *tend;
	unsigned int diff;
	device dev,dev1;
	datum key;
	datum resu;
	int dev_numb = 1;
	char indnr[4];
	int exit = 0;

	d_num = 0;

/* Allocate memory for the pointer's array */

	if ((ptra = (char **) calloc(MAXDEV,sizeof(nam))) == NULL)
	{
		back->res_val.arr1_val = NULL;
		return(DbErr_ServerMemoryAllocation);
	}

/* Get device server type */

	tmp = (char *) strchr(dev_na,'/'); 
	diff = (u_int)(tmp++ - dev_na);
	strncpy(dev.ds_class,dev_na,diff);
	dev.ds_class[diff] = '\0';

/* Get device type */

	strcpy(dev.ds_name,tmp);

#ifdef DEBUG
	cout << "Device server class (getdevlist) : " << dev.ds_class << endl;
	cout << "Device server name (getdevlist) : " << dev.ds_name << endl;
#endif /* DEBUG */

/* Try to retrieve the right tuple in NAMES table */

	key.dptr = (char *)malloc(MAX_KEY);
	
	do
	{

		strcpy(key.dptr, dev.ds_class);
		strcat(key.dptr,"|");
		strcat(key.dptr, dev.ds_name);
		strcat(key.dptr,"|");
		sprintf(indnr,"%d",dev_numb);
		strcat(key.dptr, indnr);
		strcat(key.dptr,"|");
		key.dsize = strlen(key.dptr);

		resu = dbm_fetch(dbgen.tid[0], key);                                 

		if (resu.dptr != NULL)
		{
	
/* Unpack the retrieved content */

			tbeg = resu.dptr;
			tend = (char *) strchr(tbeg,'|');
			if(tend == NULL)
			{
				fprintf(stderr, "No separator in the content.\n");
				free(key.dptr);
				return(ERR_DEVNAME);
			}
			diff = (unsigned int)(tend++ - tbeg);
			strncpy(dev1.d_name, tbeg, diff);
			dev1.d_name[diff] = '\0';

/* Allocate memory for device name */

			if (d_num != 0 && (d_num & 0xF) == 0)
			{
				tp = d_num >> 4;
				if ((ptra = (char **)realloc(ptra,sizeof(nam) * ((tp + 1) * MAXDEV))) == NULL)
				{
					for(j=0;j<d_num;j++)
						free(ptra[j]);
					free(key.dptr);
					return(DbErr_MaxNumberOfDevice);
				}
			}
			if ((ptra[d_num] = (char *)malloc(strlen(dev1.d_name) + 1)) == NULL)
			{
				for (j=0;j<d_num;j++)
					free(ptra[j]);
				free(key.dptr);
				return(DbErr_ServerMemoryAllocation);
			}

/* Copy the device name */

			strcpy(ptra[d_num],dev1.d_name);
			dev_numb++;
			d_num++;
			
		}
		else
			exit = 1;

	}
	while(!exit);

/*
	if (resu == 0)
	{
		return(DbErr_DeviceServerNotDefined);
		return(DbErr_DatabaseAccess);
	}
*/
/* Initialize the structure */

	back->res_val.arr1_val = ptra;
	back->res_val.arr1_len = d_num;

	*dev_num = d_num;

	if (resu.dptr == NULL && d_num == 0)
	{
		free(key.dptr);
		return(DbErr_DeviceServerNotDefined);
	}

	free(key.dptr);
	return(0);

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
	int res_num;
	int i,l, ret_res=0;
	int ires;
	static int sent_back;
	char domain[40];
	char family[40];
	char member[40];
	char r_name[40];
	unsigned int diff;
	char *tmp,*temp;
	register putres *tmp_ptr;
	char ind_name[20];
	char numb[20];
	unsigned int ctr;
	DBM *tab;
	datum key;
	datum content;
	reso ret;
	int flags = 0;
	int res_numb = 1;
	int exit = 0;
	char indnr[4];

	res_num = rece->tab_putres_len;

#ifdef DEBUG
	for (i=0;i<res_num;i++)
		cout << "Resource name : " << rece->tab_putres_val[i].res_name << endl;
#endif /* DEBUG */

/* Initialize sent back error code */

	sent_back = 0;

/* Return error code if the server is not connected to the database */

	if (dbgen.connected == False)
	{
		sent_back = DbErr_DatabaseNotConnected;
		return(&sent_back); 
	}
	
/* Allocate memory for key and content pointers */

	if ((key.dptr = (char *)malloc(MAX_KEY)) == NULL)
	{
		sent_back = DbErr_ServerMemoryAllocation;
		return(&sent_back); 
	}
	if ((content.dptr = (char *)malloc(MAX_CONT)) == NULL)
	{
		sent_back = DbErr_ServerMemoryAllocation;
		return(&sent_back); 
	}

/* A loop for every resource */

	for (ires = 0;ires < res_num;ires++)
	{
		tmp_ptr = &(rece->tab_putres_val[ires]);
		res_numb = 1;

/* Extract domain, family, member and resource name from the full resource name */

		temp = (char *) strchr(tmp_ptr->res_name,'/');
		diff = (unsigned int)(temp++ - tmp_ptr->res_name);
		strncpy(domain,tmp_ptr->res_name,diff);
		domain[diff] = 0;

		tmp = (char *) strchr(temp,'/');
		diff = (unsigned int)(tmp++ - temp);
		strncpy(family,temp,diff);
		family[diff] = 0;

		temp = (char *) strchr(tmp,'/');
		diff = (unsigned int)(temp++ - tmp);
		strncpy(member,tmp,diff);
		member[diff] = 0;

		strcpy(r_name,temp);

#ifdef DEBUG
		cout << "Domain name : " << domain << endl;
		cout << "Family name : " << family << endl;
		cout << "Member name : " << member << endl;
		cout << "Resource name : " << r_name << endl;
#endif /* DEBUG */

/* Select the right resource table in CS database */

		for (i = 0;i < dbgen.TblNum;i++)
		{
			if (strcmp(domain,dbgen.TblName[i].c_str()) == 0)
			{
				tab = dbgen.tid[i];
				break;
			}
		}

		if (i == dbgen.TblNum)
		{
			sent_back = DbErr_DomainDefinition;
			free(key.dptr);
			free(content.dptr);
			return(&sent_back); 
		}


/* Try to retrieve this resource from the database */

		strcpy(key.dptr, family);
		strcat(key.dptr,"|");
		strcat(key.dptr, member);
		strcat(key.dptr,"|");
		strcat(key.dptr, r_name);
		strcat(key.dptr,"|");
		sprintf(indnr,"%d",res_numb);
		strcat(key.dptr, indnr);
		strcat(key.dptr,"|");
		key.dsize = strlen(key.dptr);


/* Delete the old information (single or array) if the array already exists */


		while(( dbm_delete(tab, key)) == 0)
		{
			res_numb++;
			ret_res = 1;
			strcpy(key.dptr, family);
			strcat(key.dptr,"|");
			strcat(key.dptr, member);
			strcat(key.dptr,"|");
			strcat(key.dptr, r_name);
			strcat(key.dptr,"|");
			sprintf(indnr,"%d",res_numb);
			strcat(key.dptr, indnr);
			strcat(key.dptr,"|");
			key.dsize = strlen(key.dptr);
		}

/* If the new update is for an array */

		if (tmp_ptr->res_val[0] == INIT_ARRAY)
		{

/* Retrieve the number of element in the array.
   Initialize the loop counter "ctr" to number of element minus one because
   it is not necessary to look for the element separator to extract the last
   element value from the string. */

			tmp = (char *) strchr(tmp_ptr->res_val,SEP_ELT);
			diff = (u_int)(tmp++ - tmp_ptr->res_val) - 1;
			strncpy(numb,&tmp_ptr->res_val[1],diff);
			numb[diff] = 0;
			ctr = (unsigned int)atoi(numb) - 1;
			res_numb = 1;

			for (l = 0;l < ctr;l++)
			{

/* Initialize database information */

				strcpy(key.dptr,family);
				strcat(key.dptr,"|");
				strcat(key.dptr,member);
				strcat(key.dptr,"|");
				strcat(key.dptr,r_name);
				strcat(key.dptr,"|");
				sprintf(indnr,"%d",res_numb);
				strcat(key.dptr, indnr);
				strcat(key.dptr,"|");
				key.dsize = strlen(key.dptr);

/* Add one array element in the database */

				temp = (char*) strchr(tmp,SEP_ELT);
				diff = (u_int)(temp++ - tmp);
				strncpy(content.dptr, tmp, diff);
				content.dptr[diff] = '\0';
				content.dsize = strlen(content.dptr);
				res_numb++;
				tmp = temp;
				flags = DBM_INSERT;

				if (dbm_store(tab, key, content, flags))
				{
					sent_back = DbErr_DatabaseAccess;
					free(key.dptr);
					free(content.dptr);
					return(&sent_back);
				}
			} 

/* For the last element value */

			strcpy(content.dptr,tmp);
			content.dsize = strlen(content.dptr);
			strcpy(key.dptr,family);
			strcat(key.dptr,"|");
			strcat(key.dptr,member);
			strcat(key.dptr,"|");
			strcat(key.dptr,r_name);
			strcat(key.dptr,"|");
			sprintf(indnr,"%d",res_numb);
			strcat(key.dptr, indnr);
			strcat(key.dptr,"|");
			key.dsize = strlen(key.dptr);
			res_numb++;

			flags = DBM_INSERT;
			if (dbm_store(tab, key, content, flags))
			{
				sent_back = DbErr_DatabaseAccess;
				free(key.dptr);
				free(content.dptr);
				return(&sent_back);
			}
		}

		else
		{
			if (ret_res == 1)
			{

/* If the resource is already defined in the database, just update the tuple */

				strcpy(key.dptr, family);
				strcat(key.dptr, "|");
				strcat(key.dptr, member);
				strcat(key.dptr, "|");
				strcat(key.dptr, r_name);
				strcat(key.dptr, "|");
				res_numb=1;
				sprintf(indnr,"%d",res_numb);
				strcat(key.dptr, indnr);
				strcat(key.dptr, "|");
				key.dsize = strlen(key.dptr);

				strcpy(content.dptr, tmp_ptr->res_val);
				content.dsize = strlen(content.dptr);
				flags = DBM_REPLACE;

				if (dbm_store(tab, key, content, flags))
				{ 
					sent_back = DbErr_DatabaseAccess;
					free(key.dptr);
					free(content.dptr);
					return(&sent_back);
				}
			} /* end of updating a single tuple */

			else
			{

/* Insert a new tuple */
				strcpy(key.dptr, family);
				strcat(key.dptr, "|");
				strcat(key.dptr, member);
				strcat(key.dptr, "|");
				strcat(key.dptr, r_name);
				strcat(key.dptr, "|");
				res_numb=1;
				sprintf(indnr,"%d",res_numb);
				strcat(key.dptr, indnr);
				strcat(key.dptr, "|");
				key.dsize = strlen(key.dptr);

				strcpy(content.dptr, tmp_ptr->res_val);
				content.dsize = strlen(content.dptr);
				flags = DBM_INSERT;

				if (dbm_store(tab, key, content, flags))
				{ 
					sent_back = DbErr_DatabaseAccess;
					free(key.dptr);
					free(content.dptr);
					return(&sent_back);
				}
			} /* end of inserting a tuple */
		} /* end of else (no array) */
	} /* end of for for every resource */

/* Leave server */

	free(key.dptr);
	free(content.dptr);
	return(&sent_back);

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


int *db_delres_1_svc(arr1 *rece,struct svc_req *rqstp)
#if 0
int *db_delres_1_svc(arr1 *rece/*,struct svc_req *rqstp*/)
#endif

{
	int i,j;
	static int sent_back;
	u_int num_res,err_db;
	register char *ptrc;
	char **old_res;

	num_res = rece->arr1_len;

#ifdef DEBUG
	for(i=0;i<num_res;i++)
	{
		cout << "Resource to delete : " << rece->arr1_val[i] << endl;
	}
#endif

/* Initialize error code */

	sent_back = 0;

/* Return error code if the server is not connected to the database files */

	if (dbgen.connected == False)
	{
		sent_back = DbErr_DatabaseNotConnected;
		return(&sent_back);
	}

/* Mark the server as not connected. This will prevent dbm_update to
   add/modify resources during this call */

	dbgen.connected = False;

/* Allocate array for pointers to store deleted resources value */

	if ((old_res = (char **)calloc(num_res,sizeof(char *))) == NULL)
	{
		dbgen.connected = True;
		sent_back = DbErr_ServerMemoryAllocation;
		return(&sent_back);
	}

/* A loop on the resource's number to be deleted */

	for(i = 0;i < num_res;i++)
	{
		ptrc = rece->arr1_val[i];

/* Try to delete the resource from database */

		if((err_db = db_del(ptrc,&(old_res[i]))) != 0 )
		{
			if (i != 0)
				db_reinsert(rece,old_res,i);
			for (j = 0;j < i;j++)
			{
				if (old_res[j] != NULL)
					free(old_res[j]);
			}
			free(old_res);
			dbgen.connected = True;
			sent_back = err_db;
			return(&sent_back);
		}
	}

/* Free memory and exit server */

	dbgen.connected = True;
	for (i = 0;i < num_res;i++)
	{
		if (old_res[i] != NULL)
			free(old_res[i]);
	}
	free(old_res);
	return(&sent_back);

}




/****************************************************************************
*                                                                           *
*		Code for db_del function                                    *
*                        ------                                             *
*                                                                           *
*    Function rule : To delete a resource from the database         	    *
*                                                                           *
*    Argin : - The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)	    *
*	     - The address where to store the string to memorize the deleted*
*	       resource value						    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/

int db_del(char *res_name,char **p_oldres)
{
	char t_name[40];
	char family[40];
	char member[40];
	char r_name[40];
	char indnr[10];
	unsigned int diff;
	char *temp,*tmp;
	int i,k,k1;
	int ctr = 0;
	char *tmp_buf;
	datum resu,key;
	DBM *tab;
	int res_numb = 1;
	int err;
	int exit = 0;
	int error = False;

/* Miscellaneous init. */

	t_name[0] = 0;

/* Get table name */

	if ((tmp = strchr(res_name,'/')) == (char *)NULL)
	{
		fprintf(stderr,"db_del : Error in resource name %s\n",res_name);
		return(DbErr_BadResourceType);
	}
	diff = (u_int)(tmp++ - res_name);
	strncpy(t_name,res_name,diff);
	t_name[diff] = 0;

/* Get family name */

	if ((temp = strchr(tmp,'/')) == NULL)
	{
		fprintf(stderr,"db_del : Error in resource name %s\n",res_name);
		return(DbErr_BadResourceType);
	}
	diff = (u_int)(temp++ - tmp);
	strncpy(family,tmp,diff);
	family[diff] = '\0';

/* Get member name */

	if ((tmp = strchr(temp,'/')) == NULL)
	{
		fprintf(stderr,"db_del : Error in resource name %s\n",res_name);
		return(DbErr_BadResourceType);
	}
	diff = (u_int)(tmp++ - temp);
	strncpy(member,temp,diff);
	member[diff] = '\0';

/* Get resource name */

	strcpy(r_name,tmp);

#ifdef DEBUG
	cout << "Family name : " << family << endl;
	cout << "Number name : " << member << endl;
	cout << "Resource name : " << r_name << endl;
#endif

/* Select the right resource table in database */

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (strcmp(t_name,dbgen.TblName[i].c_str()) == 0)
		{
			tab = dbgen.tid[i];
			break;
		}
	}
	if (i == dbgen.TblNum)
		return(DbErr_DomainDefinition);

/* Allocate memory to store the old resource value (to reinsert it in case
   of problem) and for the key */

	if ((tmp_buf = (char *)malloc(SIZE)) == NULL)
		return(DbErr_ServerMemoryAllocation);
	k1 = 1;

	if ((key.dptr = (char *)malloc(MAX_KEY)) == NULL)
	{
		free(tmp_buf);
		return(DbErr_ServerMemoryAllocation);
	}

/* Try to retrieve the right tuple in table and loop for the case of an
   array of resource */

	do
	{
        	strcpy(key.dptr, family);
        	strcat(key.dptr,"|");
       		strcat(key.dptr, member);
        	strcat(key.dptr,"|");
        	strcat(key.dptr, r_name);
        	strcat(key.dptr,"|");
        	sprintf(indnr,"%d",res_numb);
        	strcat(key.dptr, indnr);
        	strcat(key.dptr,"|");
        	key.dsize = strlen(key.dptr);
	
		resu = dbm_fetch(tab,key);
		if (resu.dptr != NULL)
		{
			if (ctr)
			{

/* Copy the new element in the temporary buffer. If it is full, reallocate
   memory for it. */

				k = strlen(tmp_buf);
				if (k > ((k1 * SIZE) - LIM))
				{
					if ((tmp_buf = (char *)realloc(tmp_buf,((k1 + 1) * SIZE))) == NULL)
						return(DbErr_ServerMemoryAllocation);
					k1++;
				}
				tmp_buf[k] = SEP_ELT;
				tmp_buf[k + 1] = 0;
				strncat(tmp_buf,resu.dptr,resu.dsize);
				tmp_buf[k + 1 + resu.dsize] = 0;
			}
			else
			{

/* It is the first element, just copy it in the temporary buffer */

				strncpy(tmp_buf,resu.dptr,resu.dsize);
				tmp_buf[resu.dsize] = 0;
			}

/* Remove the tuple from database */

			dbm_delete(tab,key);
			ctr++;
			res_numb++;
		}
		else
		{

/* Is it an error or simply the data does not exist in the database */

			err = dbm_error(tab);
			if (err != 0)
			{
				dbm_clearerr(tab);
				error = True;
			}
			exit = 1;
		}
	}
	while(!exit);
	
	free(key.dptr);

/* If it is a classical resource, copy the res. value in the real old res value
   buffer */

	if (ctr == 1)
	{
		if ((*p_oldres = (char *)malloc(strlen(tmp_buf) + 1)) == NULL)
		{
			*p_oldres = tmp_buf;
			return(0);
		}
		strcpy(*p_oldres,tmp_buf);
	}

/* For an array of resource, add the number of resources at the beginning
   of the string */

	if (ctr > 1)
	{
		if((*p_oldres = (char *)malloc(strlen(tmp_buf) + 10)) == NULL)
		{
			free(tmp_buf);
			return(DbErr_ServerMemoryAllocation);
		}
		(*p_oldres)[0] = INIT_ARRAY;
		sprintf(&((*p_oldres)[1]),"%d",ctr);
		k = strlen(*p_oldres);
		(*p_oldres)[k] = SEP_ELT;
		(*p_oldres)[k + 1] = 0;
		strcat(*p_oldres,tmp_buf);
	}

/* Return if database error */

	if (error == True)
	{
		free(tmp_buf);
		return(DbErr_DatabaseAccess);
	}

/* Return if the resource is not found */

	if ((error == False) && (ctr == 0))
	{
		free(tmp_buf);
		return(DbErr_ResourceNotDefined);
	}
	
/* Free memory and leave function */

	free(tmp_buf);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for db_reinsert function                               *
*                        -----------                                        *
*                                                                           *
*    Function rule : To reinsert a resource in the datbase                  *
*		     This function is called only if the db_del function    *
*		     returns a error					    *
*                                                                           *
*    Argin : - The array passed to the db_delresource server part	    *
*	     - A array with the resource value				    *
*	     - The number of resource to be reinserted			    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/

int db_reinsert(arr1 *rece,char **res_value,int num_res)
{
	tab_putres tmp;
	int i,j;
	int *pi;
	char *padding = NULL;
	int num = 0;

/* Find out how many devices are really to be reinserted */

	for (i = 0;i < num_res;i++)
	{
		if (res_value[i] != NULL)
			num++;
	}

/* If the error happens after several try to delte only resources which don't
   exist, it  is not necessary to reinsert them ! */

	if (num == 0)
		return(0);
	else
		tmp.tab_putres_len = num;

/* Allocate a array of putres structure (one structure for each res.) */

	if ((tmp.tab_putres_val = (putres *)calloc(num,sizeof(putres))) == NULL)
		return(-1);

/* Initialise the array of putres structure with the resource name and resource
   value */

	j = 0;
	for (i = 0;i < num_res;i++)
	{
		if (res_value[i] != NULL)
		{
			tmp.tab_putres_val[j].res_name = rece->arr1_val[i];
			tmp.tab_putres_val[j].res_val = res_value[i];
			j++;
		}
	}

/* Call the putresource function */

	pi = db_putres_1_svc(&tmp);

/* Leave function */

	free(tmp.tab_putres_val);
	return(0);

}



void leave()
{
int i;

/* Close database */

	for (i = 0;i < dbgen.TblNum;i++) 
		dbm_close(dbgen.tid[i]);

/* Exit now */

	exit(-1);
}
