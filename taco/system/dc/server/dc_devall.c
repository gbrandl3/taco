#include "config.h"
#include <API.h>
#include <DevErrors.h>
#define DC_SERVER 1
#include <dc_xdr.h>
#include <dcP.h>

#include <stdlib.h>

/* Some global variables */
dc_devallx_back ret_devall;

/* Variables defined in dc_svc.c */
extern hash_info mem;
extern int req_call;



/**
 * To return to the client a list with all the names of
 * devices defined in the local dc 
 * 
 * @param fill
 */
dc_devallx_back *dc_devall_1(int *fill)
{
	char **str_ptr;
	int cptr,i,resu;
	int nb_16 = 1;
	int nb_tot;
	register dc_dev_param *array;

/* Miscellaneous initialization */
	ret_devall.err_code = 0;

#ifdef DEBUG
	fprintf(stderr, "Executing dc_devall request\n");
#endif /* DEBUG */

/* Init the number of request to 1 for this call */
	req_call  = 1;

/* Compute the amount of element in the hash table plus the cellar table */
	nb_tot = mem.hash_table_size + mem.cellar_size;
	array = mem.parray;

/* Allocate an array for the first 16 string pointers */
	if ((str_ptr = (char **)calloc(16,sizeof(char *))) == NULL)  
	{
		ret_devall.err_code = DcErr_ServerMemoryAllocation;
		ret_devall.dev_name.name_arr_len = 0;
		return(&ret_devall);
	}

/* Retrieve in the deviec_info part of the pointers area, all the registered devices */
	cptr = 0;
	for (i = 0;i < nb_tot;i++) 
	{
		if (array[i].device_name[0] == 0)
			continue;

/* Allocate memory for the full device name and build it */
		if ((str_ptr[cptr] = (char *)malloc(60)) == NULL) 
		{
			for (i = 0;i < cptr;i++)
				free(str_ptr[i]);
			free(str_ptr);
			ret_devall.err_code = DcErr_ServerMemoryAllocation;
			ret_devall.dev_name.name_arr_len = 0;
			return(&ret_devall);
		}

		strcpy(str_ptr[cptr],array[i].device_name);
		cptr++;

/* Reallocate memory for the array of pointers if it is full */
		if ((cptr & 0xF) == 0) 
		{
			nb_16++;
			if ((str_ptr = (char **)realloc(str_ptr,(nb_16 << 4) * sizeof(char *))) == NULL) 
			{
				for (i = 0;i < cptr;i++)
					free(str_ptr[i]);
				free(str_ptr);
				ret_devall.err_code = DcErr_ServerMemoryAllocation;
				ret_devall.dev_name.name_arr_len = 0;
				return(&ret_devall);
			}
		}
	}

/* Initialise sent back parameters */
	ret_devall.dev_name.name_arr_len = cptr;
	ret_devall.dev_name.name_arr_val = str_ptr;

/* Leave server */
	return(&ret_devall);
}
