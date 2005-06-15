#include "config.h"
#define _dc_h
#include <API.h>
#undef _dc_h
#include <dc.h>

#include <DevErrors.h>
#include <DevCmds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <dcP.h>

void devall(int pseudo);
void kern_sort_1(char **tab, int *tab_num, int n);

/* Some global variables */
DevVarStringArray 	host_dc = {0, NULL};


void usage(const char *cmd)
{
	fprintf(stderr, "usage : %s [options]\n", cmd);
	fprintf(stderr, "  Display all the devices registered in the data collector\n");
	fprintf(stderr, "     options: -p display only 'pseudo' devices\n");
	fprintf(stderr, "              -h display this message\n");
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for dc_devall command                                  *
*                        ---------                                          *
*                                                                           *
*    Command rule : To display all the devices registered in the data       *
*		    collector						    *
*                                                                           *
*    Synopsis : dc_devall [-p]						    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	int			c;
	extern int		optopt,
				optind;
	int 			pseudo = False,
				l;
	long 			error;
	db_resource 		res1[] = {
					{"host",D_VAR_STRINGARR,&host_dc},
		     		};
	int 			res1_size = sizeof(res1) / sizeof(db_resource);

/* Argument test */
	while ((c = getopt(argc,argv,"ph")) != -1)
		switch (c)
		{
			case 'p':
				pseudo = True;
                        	break;
			case 'h':
			case '?':
				usage(argv[0]);
		}
	if (optind != argc)
		usage(argv[0]);

/* Import static database */

	if (db_import(&error)) 
	{
		fprintf(stderr,"dc_devall : Can't import static database\n");
		fprintf(stderr,"dc_devall : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Retrieve on which host the data collector is running */
	if (db_getresource("CLASS/DC/1",res1,res1_size,&error)) 
	{
		fprintf(stderr,"dc_devall : Can't retrieve resources\n");
		fprintf(stderr,"dc_devall : Error code : %d\n",error);
		exit(-1);
	}
	if (host_dc.length == 0) 
	{
		fprintf(stderr,"dc_devall : Resource host not defined\n");
		fprintf(stderr,"dc_devall : Sorry, but I exit !\n");
		exit(-1);
	}
	devall(pseudo);
	return 0;
}



/**
 * To retrieve the name of all the registered devices and print them
 * 
 * @param pseudo The pseudo devices only flag
 * 
 */
void devall(int pseudo)
{
	long 		error;
	int 		i,j,k,l,ind;
	int 		tmp_ptr;
	char 		**dcall[10];
	int 		dev_numb[10];
	int 		nb_dc;
	int 		cptr = 0;
	char 		**devname_ptr;
	int 		*dev_host;
	db_devinf_imp 	*dev_imp_ptr;
	int 		tmp_ctr;

	nb_dc = host_dc.length;

/* Ask the dc_devall request on every host where a dc is running */
	for (i = 0;i < nb_dc;i++) 
	{
		if (dc_devall(host_dc.sequence[i],&(dcall[i]),&(dev_numb[i]),&error)) 
		{
			fprintf(stderr,"dc_devall : bad answer from %s\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_devall : Sorry, but I exit\n");
			exit(-1);
		}
	}

/* If the pseudo-device option has been requested by the user, for every
   device registered in the data collector, interrogate the static database
   to know if this device is a real one or a pseudo device */

	if (pseudo == True) 
	{
		for (i = 0;i < nb_dc;i++) 
		{
			tmp_ctr = dev_numb[i];
			dev_numb[i] = 0;
			l = 0;
			for(k = 0; k < tmp_ctr;k++) 
			{
				if (db_dev_import(&(dcall[i][k]),&dev_imp_ptr,1,&error)) 
				{
					if (error == DbErr_DeviceNotDefined) 
					{
						strcpy(dcall[i][l++],dcall[i][k]);
						dev_numb[i]++;	
					}
					else if (error == DbErr_DeviceNotExported) 
						continue;
					else 
					{
						fprintf(stderr,"dc_devall : Error during static database request\n");
						fprintf(stderr,"dc_devall : Sorry, but I exit\n");
					}
				}
				free(dev_imp_ptr);
			}
		}
	}

/* Compute the total number of devices defined */
	for (i = 0;i < nb_dc;i++)
		cptr += dev_numb[i];

/* Allocate memory for only one array with all the pointers to device name and
   for one array with the host associated with each devices */
	if ((devname_ptr = (char **)calloc(cptr,sizeof(char *))) == NULL) 
	{
		fprintf(stderr,"dc_devall : Can't allocate memory\n");
		fprintf(stderr,"dc_devall : Sorry, but I exit\n");
		exit(-1);
	}
	if ((dev_host = (int *)calloc(cptr,sizeof(int))) == NULL) 
	{
		fprintf(stderr,"dc_devall : Can't allocate memory\n");
		fprintf(stderr,"dc_devall : Sorry, but I exit\n");
		exit(-1);
	}
		
/* Build the array with all device name and the array with the associated host */

	ind = 0;
	for (i = 0;i < nb_dc;i++) 
	{
		for (k = 0;k < dev_numb[i];k++) 
		{
			devname_ptr[ind] = dcall[i][k];
			dev_host[ind++] = i;
		}
	}

/* Sort the arrays */
	kern_sort_1(devname_ptr,dev_host,cptr);

/* Display the result */
	if (cptr == 0) 
	{
		if (pseudo == True)
			fprintf(stderr, "No pseudo devices are registered in the data collector\n");
		else
			fprintf(stderr, "No devices are registered in the data collector\n");
	}
	else 
	{
		if (pseudo == True)
			fprintf(stderr, "The following %d pseudo devices are registered in the data collector :\n",cptr);
		else
			fprintf(stderr, "The following %d devices are registered in the data collector :\n",cptr);
		for (i = 0;i < cptr;i++)
			fprintf(stderr, "  %s on %s\n",devname_ptr[i],host_dc.sequence[dev_host[i]]);
	}
}


/**
 * To sort an array of strings. The sort is done with the 
 * strcmp function. The algorithm come from the famous  Kernighan and Ritchie book (chapter 5)
 * 
 * @param tab		The address of the array of strings pointers
 * @param tab_num	The number of elements in this array
 * @param n		
 */
void kern_sort_1(char **tab, int *tab_num, int n)
{
	int gap,i,j;
	char *temp;
	int tmp;

	for (gap = n/2;gap > 0;gap /= 2) 
	{
		for (i = gap;i < n;i++) 
		{
			for (j = i - gap;j >=0; j -= gap) 
			{
				if (strcmp(tab[j],tab[j + gap]) <= 0) 
					break;
				else 
				{
					temp = tab[j];
					tab[j] = tab[j + gap];
					tab[j + gap] = temp;

					tmp = tab_num[j];
					tab_num[j] = tab_num[j + gap];
					tab_num[j + gap] = tmp;
				}
			}
		}
	}
	return;
}

