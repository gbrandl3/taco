#include <API.h>

#include <DevErrors.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <db_xdr.h>
#include <fcntl.h>

/* Some C++ include files */

#include <iostream>
#include <string>
#include <NdbmClass.h>

/* Some local functions declaration */

int stringOK(char *,char *);
int memb_fil(device *);
int fam_fil(device *);

/* Variables defined elsewhere */

extern NdbmInfo dbgen;
extern db_res dev_back;
extern u_short udp_port;

extern int errno;


/* Global variables */

char domain[DOMAIN_NAME_LENGTH];	// 20
char family[FAMILY_NAME_LENGTH];	// 20
char member[MEMBER_NAME_LENGTH];	// 20 
char domain_tup[DOMAIN_NAME_LENGTH];	// 20
char family_tup[FAMILY_NAME_LENGTH];	// 20
char member_tup[MEMBER_NAME_LENGTH];	// 20
int dev_num;
u_short prot;
char **ptra;



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
	int i,err,ret;
	register char *tmp,*temp;
	unsigned int diff;
	device dev;
	int k1 = 1;
	struct sockaddr_in so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
	socklen_t  so_size;   /* from POSIX draft - already used by GLIBC */
#else
	int so_size;
#endif
	datum key, key_sto;
	datum content;
	char *tbeg, *tend;
	char ret_host_name[20];
	char ret_pn[20];
	char prgnr[12];
	int exit = 0;
	int flags=O_RDWR;

/* If the server is not connected to the database, return an error */

	if (dbgen.connected == False)
	{
		dev_back.db_err = DbErr_DatabaseNotConnected;
		dev_back.res_val.arr1_len = 0;
		return(&dev_back);
	}

#ifdef sun
	if (rqstp->rq_xprt->xp_port == udp_port)
		prot = IPPROTO_UDP;
	else
		prot = IPPROTO_TCP;
#else
/* Retrieve the protocol used to send this request to the server */

	so_size = sizeof(so);
	if (getsockname(rqstp->rq_xprt->xp_sock,(struct sockaddr *)&so,&so_size) == -1)
	{
		dev_back.db_err = DbErr_MaxDeviceForUDP;
		dev_back.res_val.arr1_len = 0;
		return(&dev_back);
	}

	if (so.sin_port == udp_port)
		prot = IPPROTO_UDP;
	else
		prot = IPPROTO_TCP;
#endif

/* Extract from filter string each part of the filter (domain, family and 
   member). If two / characters can be retrieved in the filter string, this
   means that the domain, family and member part of the filter are initialized.
   If only one / can be retrieved, only the domain and family part are
   initialized and if there is no / in the filter string, just the domain
   is initialized. */

	i = 0;
	tmp = *fil_name;
	NB_CHAR(i,tmp,'/');

	switch(i)
	{
		case 2 : tmp = strchr(*fil_name,'/');
			 diff = (u_int)(tmp++ - *fil_name);
			 strncpy(domain,*fil_name,diff);	// potential error
			 domain[diff] = 0;

			 temp = strchr(tmp,'/');
			 diff = (u_int)(temp++ - tmp);
			 strncpy(family,tmp,diff);		// potential error
			 family[diff] = 0;

			 strcpy(member,temp);
			 break;

		case 1 : tmp = strchr(*fil_name,'/');
			 diff = (u_int)(tmp++ - *fil_name);
			 strncpy(domain,*fil_name,diff);	// potential error
			 domain[diff] = 0;

			 strcpy(family,tmp);
			 strcpy(member,"*");
			 break;

		case 0 : strcpy(domain,*fil_name);		// potential error
			 strcpy(family,"*");
			 strcpy(member,"*");
			 break;
	}

#ifdef DEBUG
	cout << "filter domain : " << domain << endl;
	cout << "filter family : " << family << endl;
	cout << "filter member : " << member << endl;
#endif /* DEBUG */

/* Allocate memory for the pointer's array */

	dev_num = 0;
	if ((ptra = (char **)calloc(MAXDEV,sizeof(nam))) == NULL)
	{
		dev_back.db_err = DbErr_ServerMemoryAllocation;
		dev_back.res_val.arr1_len = 0;
		return(&dev_back);
	}

/* Try to retrieve all tuples in the database NAMES table with the PN column
   different than "not_exp" */

	key_sto.dptr = (char *) malloc(MAX_KEY);

	key = dbm_firstkey(dbgen.tid[0]);
	if (key.dptr == NULL)
	{
		free(key_sto.dptr);
		free(ptra);
		dev_back.db_err = DbErr_DatabaseAccess;
		dev_back.res_val.arr1_len = 0;
		return(&dev_back);
	}

	do
	{

/* Store the key if it is needed later */

		strncpy(key_sto.dptr, key.dptr, key.dsize);
		key_sto.dptr[key.dsize] = '\0';
		key_sto.dsize = key.dsize;
		content = dbm_fetch(dbgen.tid[0], key);

		if (content.dptr != NULL)
		{

			tbeg = content.dptr;
		
			tend = strchr(tbeg, '|');
			diff = (u_int)(tend++ - tbeg);
			strncpy(dev.d_name, tbeg, diff);
			dev.d_name[diff] = '\0';
			tbeg = tend;

			tend = strchr(tbeg, '|');
			diff = (u_int)(tend++ - tbeg);
			strncpy(ret_host_name, tbeg, diff);
			ret_host_name[diff] = '\0';
		
			tbeg = tend;
			tend = strchr(tbeg, '|');
			diff = (u_int)(tend++ - tbeg);
			strncpy(ret_pn, tbeg, diff);
			ret_pn[diff] = '\0';

			if (strcmp(ret_pn, "0") != 0)
			{

/* Move the the rest of the content and key values to the structure of dev */

				strcpy(dev.h_name, ret_host_name);

				dev.pn = atoi(ret_pn);

				tbeg = tend;

				tend = strchr(tbeg, '|');
				diff = (u_int)(tend++ - tbeg);
				strncpy(prgnr, tbeg, diff);
				prgnr[diff] = 0;
				dev.vn = atoi(prgnr);

				tbeg = tend;

				tend = strchr(tbeg, '|');
				diff = (u_int)(tend++ - tbeg);
				strncpy(dev.d_type, tbeg, diff);
				dev.d_type[diff] = '\0';
		
				tbeg = tend;

				tend = strchr(tbeg, '|');
				diff = (u_int)(tend++ - tbeg);
				strncpy(dev.d_class, tbeg, diff);
				dev.d_class[diff] = '\0';

				tbeg = tend;

				tend = strchr(tbeg, '|');
				diff = (u_int)(tend++ - tbeg);
				strncpy(prgnr,tbeg,diff);
				prgnr[diff] = 0;
				dev.pid = atoi(prgnr);

/* Extract device server class and name from the key */
		
				tbeg = key_sto.dptr;
				tend = strchr(tbeg, '|');
				if (tend == NULL)
				{
					fprintf(stderr,
					"No separator in db tuple\n");
					dev_back.db_err = DbErr_DatabaseAccess;
					dev_back.res_val.arr1_len = 0;
					free(key_sto.dptr);
					return(&dev_back);
				}
				diff = (u_int)(tend++ - tbeg);
				strncpy(dev.ds_class, tbeg, diff);
				dev.ds_class[diff] = '\0';

				tbeg = tend;

				tend = strchr(tbeg, '|');
				diff = (u_int)(tend++ - tbeg);
				strncpy(dev.ds_name, tbeg, diff);
				dev.ds_name[diff] = '\0';


/* Extract the domain part of the device name from the retrieved tuple */

				tmp = strchr(dev.d_name,'/');
				diff = (u_int)(tmp++ - dev.d_name);
				strncpy(domain_tup, dev.d_name, diff);
				domain_tup[diff] = 0;

#ifdef DEBUG
				cout << "Domain part from DB: " << domain_tup << endl;
#endif /* DEBUG */

/* Call the stringOK function to verify that the retrieved devices 
   device name is OK */

				ret = stringOK(domain,domain_tup);

/* If the domain part of the filter is *, directly call the fam_fil
   function */

				if (ret == 0)
				{
					if ((err = fam_fil(&dev)) != 0)
					{
						if (dev_num != 0)
						{
							dev_back.res_val.arr1_val = ptra;
							for (i=0;i<dev_num;i++)
							{
								free(dev_back.res_val.arr1_val[i]);
							}
							free(dev_back.res_val.arr1_val);
						}
						dev_back.db_err = err;
						dev_back.res_val.arr1_len = 0;
						free(key_sto.dptr);
						return(&dev_back);
					}
				} /* end of if */
			} /* end of FOUND */
		} /* end of NOT NULL */

		if (exit == 0)
		{
			key = dbm_nextkey(dbgen.tid[0]);
			if (key.dptr == NULL)
				exit = 1;
		}
	} /* end of do */
	while (!exit);

/* If a problem occurs during database function */

	if (content.dptr == 0)
	{
		if (dev_num != 0)
		{
			dev_back.res_val.arr1_val = ptra;
			for (i=0;i<dev_num;i++)
			{
				free(dev_back.res_val.arr1_val[i]);
			}
			free(dev_back.res_val.arr1_val);
		}
		dev_back.db_err = DbErr_DatabaseAccess;
		dev_back.res_val.arr1_len = 0;
		free(key_sto.dptr);
		return(&dev_back);
	}

/* Initialize the structure sended back to client and leave the server */

	dev_back.res_val.arr1_len = dev_num;
	dev_back.res_val.arr1_val = ptra;
	dev_back.db_err = 0;
	free(key_sto.dptr);
	return(&dev_back);

}



/****************************************************************************
*                                                                           *
*		Code for fam_fil function                                   *
*                        -------                                            *
*                                                                           *
*    Function rule : To check if the family part of the filter is the same  *
*		     than the family part of the device name retrieved from *
*		     the database					    *
*                                                                           *
*    Argin : - A pointer to the structure with the contents of one          *
*	       database tuple.						    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int fam_fil(device *dev1)
{
	register char *tmp,*temp;
	unsigned int diff;
	int err,ret;

/* If the family part of the filter is *, directly call the memb_fil
   function */

#ifdef DEBUG
	cout << "Arrived in fam_fil function" << endl;
#endif /* DEBUG */

/* Extract the family part of the device name in the retrieved tuple */

	tmp = strchr(dev1->d_name,'/');
	tmp++;
	temp = strchr(tmp,'/');
	diff = (u_int)(temp++ - tmp);
	strncpy(family_tup,tmp,diff);		// potential error
	family_tup[diff] = 0;

#ifdef DEBUG
	cout << "Family part from DB : " << family_tup << endl;
#endif /* DEBUG */

/* Special case for the data collector pseudo devices which are not real 
   devices. It is impossible to execute command on them. So, don't return
   them to the caller. */

	if (strncmp("dc_rd", family_tup, 5) == 0)
		return(0);
	if (strncmp("dc_wr", family_tup, 5) == 0)
		return(0);

/* Call the stringOK to verify that the family name is OK */


	ret = stringOK(family,family_tup);

/* Is this family part the same than in the filter, call the memb_fil
   function */

	if (ret == 0)
	{
		if ((err = memb_fil(dev1)) != 0)
			return(err);
	}

	return(0);
}



/****************************************************************************
*                                                                           *
*		Code for memb_fil function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To check if the member part of the filter is the same  *
*		     than the member part of the device name retrieved from *
*		     the database. If yes, this device name must be         *
*		     returned to the caller				    *
*                                                                           *
*    Argin : - A pointer to the structure with the contents of one          *
*	       database tuple.						    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int memb_fil(device *dev2)
{
	register char *tmp,*temp;
	int tp,ret;

/* If the member part of the filter is *, the device name must be sent back to
   the user. This means copy the device name in the special array (allocate
   memory for the string) and increment the device name counter */

#ifdef DEBUG
	cout << "Arrived in memb_fil function" << endl;
#endif /* DEBUG */

/* Extract the member part of the device name in the retrieved tuple */

	tmp = strchr(dev2->d_name,'/');
	tmp++;
	temp = strchr(tmp,'/');
	temp++;
	strcpy(member_tup,temp);		// potential error

#ifdef DEBUG
	cout << "member part from DB : " << member_tup << endl;
#endif /* DEBUG */

/* Call the stringOK function to verify that the member part of the
   retrieved device is OK */

	ret = stringOK(member,member_tup);

/* Is this member part the same than in the filter ? If yes, the device name
   must be sent back to the user: copy the device name in the special array
   (allocate memory for the string) and increment the device name counter. */

	if (ret == 0)
	{
		if (dev_num != 0 && (dev_num & 0xF) == 0)
		{
			tp = dev_num >> 4;
			if ((ptra = (char **)realloc(ptra,sizeof(nam) * ((tp + 1) * MAXDEV))) == NULL)
				return(DbErr_ServerMemoryAllocation);
		}
		if ((ptra[dev_num] = (char *)malloc(strlen(dev2->d_name) + 1)) == NULL)
			return(DbErr_ServerMemoryAllocation);

		strcpy(ptra[dev_num],dev2->d_name);
		dev_num++;
		if (prot == IPPROTO_UDP)
		{
			if (dev_num == MAXDEV_UDP)
				return(DbErr_MaxDeviceForUDP);
		}
#ifdef DEBUG
		cout << "One more device name" << endl;
#endif /* DEBUG */
	}

	return(0);
}



/****************************************************************************
*                                                                           *
*		Code for stringOK function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To check if a string is the same than a wanted string  *
*		     This function allow the caller to have ONE wildcard '*'*
*		     in the wanted string.				    *
*                                                                           *
*    Argin : - A pointer to the wanted string				    *
*   	     - A pointer to the string to be compared			    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if the string in the same than the wanted one  *
*    Otherwise, this function returns 1					    *
*                                                                           *
****************************************************************************/
int stringOK(char *wanted,char *retrieved)
{
	register char *tmp;
	unsigned int before,after;
	int i,k,l;

/* Get strings length */

	l = strlen(retrieved);
	k = strlen(wanted);

/* If the wanted string is only the wild card, return 0 */

	if (k == 1 && wanted[0] == '*')
		return(0);

/* If the wild card is in the wanted string, compute the number of
   characters before and after it */

	if ((tmp = strchr(wanted,'*')) != NULL)
	{
		before = (unsigned int)(tmp - wanted);
		if (before >= l)
			return(1);
		after = k - before - 1;
		if (after >= l)
			return(1);
	}

/* Test to see if the string is stricly the same than the wanted one */

	else
		return strcmp(wanted,retrieved); 

/* Test characters before the wild card */

	for (i = 0;i < before;i++)
		if (retrieved[i] != wanted[i])
			return(1);
/* Test characters after the wild card */

	for (i = 0;i < after;i++)
		if (retrieved[l - (i + 1)] != wanted[k - (i + 1)])
			return(1);
/* Leave function */

	return(0);

}
