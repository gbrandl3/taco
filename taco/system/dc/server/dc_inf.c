#include "config.h"
#include <API.h>
#include <DevErrors.h>

#include <dc_xdr.h>
#include <dcP.h>

#include <stdlib.h>

/* Variables defined in dc_svc.c */

extern hash_info mem;

extern char *addr_ptr,*addr_alloc,*addr_data;
extern int dat_size,alloc_size,ptr_size;

extern int req_call;



/*
 * To return to the client a structure with : 
 *	- The amount of free memory in the data buffer
 *	- The amount of devices defined in this dc
 *	- The number of devices defined in each domain
 *
 * @param fill
 */
dc_infox_back *dc_info_1(int *fill)
{
	static dc_infox_back ret;
	static domain_info dom_array[MAX_DOM];
	static dom_x dom_x_array[MAX_DOM];
	long error;
	unsigned int free,lfree;
	int nb_area;
	int nb_tot,i,j;
	int resu,cptr;
	register dc_dev_param *array;
	char *temp;
	unsigned int diff;
	char domain[40];
	int nb_dom = 0;

/* Miscellaneous initialization */

	ret.err_code = 0;
	for (i = 0;i < MAX_DOM;i++) 
	{
		dom_array[i].dom_name[0] = 0;
		dom_array[i].dom_nb_dev = 0;
	}
	ret.back.dom_ax.dom_ax_val = dom_x_array;

#ifdef DEBUG
	fprintf(stderr, "Executing dc_info request\n");
#endif /* DEBUG */

/* Init the number of request to 1 for this call */
	req_call  = 1;

/* Compute the amount of elemet in the hash table plus the cellar table */
	nb_tot = mem.hash_table_size + mem.cellar_size;
	array = mem.parray;

/* Ask for data buffer status */
	if (dcmem_info(addr_alloc,alloc_size,&lfree,&free,&nb_area,&error)) 
	{
		ret.err_code = DcErr_DatabaseError;
		return(&ret);
	}
	ret.back.free_mem = free;
	ret.back.mem = dat_size;

/* Count all the register devices */
	cptr = 0;
	for (i = 0;i < nb_tot;i++) 
	{
		if (array[i].device_name[0] == 0)
			continue;

		cptr++;

/* Get domain name from device name */
		temp = (char *)strchr(array[i].device_name,'/');
		diff = (u_int)(temp++ - array[i].device_name);
		strncpy(domain,array[i].device_name,diff);
		domain[diff] = 0;

/* Is it a known domain ? */
		for (j = 0;j < nb_dom;j++) 
		{
			if (strcmp(dom_array[j].dom_name,domain) == 0)
				break;
		}

		if (j == nb_dom) 
		{
			nb_dom++;
			strcpy(dom_array[j].dom_name,domain);
		}
		dom_array[j].dom_nb_dev++;
	}

/* Init. parameters sent back to caller */
	ret.back.dom_ax.dom_ax_len = nb_dom;
	for (i = 0;i < nb_dom;i++) 
	{
		ret.back.dom_ax.dom_ax_val[i].dom_nb_dev = dom_array[i].dom_nb_dev;
		ret.back.dom_ax.dom_ax_val[i].dom_name = dom_array[i].dom_name;
	}
	ret.back.nb_dev = cptr;

/* Leave server */
	return(&ret);
}
