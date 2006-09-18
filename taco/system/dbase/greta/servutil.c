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
 * File         : servutil.c
 *
 * Project      : GRETA application
 *
 * Description  :The module which contains all the utilities used to display
 * 		 device server information (device list, device resources..)
 *
 * Author       : E. Taurel
 *                $Author: jkrueger1 $
 *
 * Original     : June 1998
 *
 * Version      : $Revision: 1.3 $
 *
 * Date         : $Date: 2006-09-18 21:59:06 $
 *
 */

/*
 * Standard includes for builtins.
 */
 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
 
#include <greta.h>

long db_getserverdeviceres(char *ds_name,char *pers_name,db_svcinfo_call *p_info,
			   char **p_str,long dev_nb,long all_ds,long *error)
{
	long i,j,ind;
	long res_nb;
	char **res_list = NULL;
	char **dev_list;
	char *tmp = NULL;

/* Build the list of devices for which we must retrieve resources */

	if ((dev_list = (char **)calloc(dev_nb,sizeof(char *))) == NULL)
	{
		*error = DbErr_ClientMemoryAllocation;
		return(-1);
	}

	ind = 0;	
	for (i = 0;i < p_info->embedded_server_nb;i++)
	{
		if (all_ds == True)
		{
			for (j = 0;j < p_info->server[i].device_nb;j++)
			{
				dev_list[ind] = p_info->server[i].device[j].dev_name;
				ind++;
			}
		}
		else
		{
			if (strcmp(ds_name,p_info->server[i].server_name) == 0)
			{
				for (j = 0;j < p_info->server[i].device_nb;j++)
				{
					dev_list[ind] = p_info->server[i].device[j].dev_name;
					ind++;
				}
			}
		}
	}

/* Get resources for all devices */

	if (db_deviceres(dev_nb,dev_list,&res_nb,&res_list,error))
	{
		free(dev_list);
		return(-1);
	}
			
	if (res_nb != 0)
	{
		if (xsCreateStringForTextServ(res_list,res_nb,&tmp))
		{
			free(dev_list);
			for (i = 0;i < res_nb;i++)
				free(res_list[i]);
			free(res_list);
			*error = DbErr_ClientMemoryAllocation;
			return(-1);
		}
	}

/* Free memory and leave function */

	free(dev_list);
	for (i = 0;i < res_nb;i++)
		free(res_list[i]);
	if (res_nb != 0)
		free(res_list);	
	*p_str = tmp;
	return(0);
	
}


long db_builddevicelist(char *ds_name,char *pers_name,db_svcinfo_call *p_info,
			char **p_str,long all_ds,long *error)
{
	long i,j;
	long size = 0;
	long nb_dev,nb_line;
	char *tmp;
	
/* Compute resulting string size (don't forget spaces at beginning of each
   lines) */

	for (i = 0;i < p_info->embedded_server_nb;i++)
	{
		if (all_ds == True)
		{
			for (j = 0;j < p_info->server[i].device_nb;j++)
			{
				size = size + strlen(p_info->server[i].device[j].dev_name);
			}
			size = size + strlen(p_info->server[i].server_name) + strlen(pers_name) + 15 + p_info->server[i].device_nb;
			nb_line = p_info->server[i].device_nb / 4;
			size = size + (nb_line * 35);
		}
		else
		{
			if (strcmp(ds_name,p_info->server[i].server_name) == 0)
			{
				for (j = 0;j < p_info->server[i].device_nb;j++)
				{
					size = size + strlen(p_info->server[i].device[j].dev_name);
				}
				size = size + strlen(p_info->server[i].server_name) + strlen(pers_name) + 15 + p_info->server[i].device_nb;
				nb_line = p_info->server[i].device_nb / 4;
				size = size + (nb_line * 35);
			}
		}
	}
	
/* Allocate memory */

	if ((tmp = (char *)malloc(size)) == NULL)
	{
		*error = DbErr_ClientMemoryAllocation;
		return(-1);
	}
	
/* Build resulting string */

	nb_dev = 0;	
	for (i = 0;i < p_info->embedded_server_nb;i++)
	{
		if (all_ds == True)
		{
			if (i == 0)
				strcpy(tmp,p_info->server[i].server_name);
			else
				strcat(tmp,p_info->server[i].server_name);
			strcat(tmp,"/");
			strcat(tmp,pers_name);
			strcat(tmp,"/device : ");
			nb_dev = 0;
			for (j = 0;j < p_info->server[i].device_nb;j++)
			{
				nb_dev++;
				strcat(tmp,p_info->server[i].device[j].dev_name);
				if (nb_dev != p_info->server[i].device_nb)
					strcat(tmp,",");
				if ((nb_dev != 0) && ((nb_dev % 4) == 0))
				{
					strcat(tmp,"\\\n                              ");
				}
			}
			strcat(tmp,"\n");
		}
		else
		{
			if (strcmp(p_info->server[i].server_name,ds_name) == 0)
			{
				if (i == 0)
				{
					nb_dev = 0;
					strcpy(tmp,ds_name);
					strcat(tmp,"/");
					strcat(tmp,pers_name);
					strcat(tmp,"/device : ");
				}
				for (j = 0;j < p_info->server[i].device_nb;j++)
				{
					if (nb_dev > 0)
						strcat(tmp,",");
					strcat(tmp,p_info->server[i].device[j].dev_name);
					nb_dev++;
					if (((nb_dev % 4) == 0) && (j != p_info->server[i].device_nb - 1))
					{
						strcat(tmp,"\\\n                              ");
					}
				}
				if (i == p_info->embedded_server_nb - 1)
					strcat(tmp,"\n");
			}
		}
	}
		
	*p_str = tmp;	
	return(0);
	
}


long db_buildservinfo(char *ds_name,char *pers_name,db_svcinfo_call *p_info,
		      char **p_str,long all_ds,long *error)
{
	char tmp[100];
	long i,j,nb_alloc;
	char *out;

/* Allocate memory for the resulting string by 1000 bytes block */

	if ((out = (char *)malloc(1000 * sizeof(char))) == NULL)
	{
		*error = DbErr_ClientMemoryAllocation;
		return(-1);
	}
	nb_alloc = 1;
	
/* Copy process info into resulting string */
	
	if (p_info->pid != 0)
	{
		if (strcmp(p_info->process_name,"unknown") != 0)
		{
			sprintf(out,"Device server %s/%s is part of the process %s\n",ds_name,pers_name,p_info->process_name);
			sprintf(tmp,"The process is running on host %s with PID %d (Program number = %d)\n",p_info->host_name,p_info->pid,p_info->program_num);
			strcat(out,tmp);
		}
		else
		{
			sprintf(out,"The DS process is running on host %s with PID %d (Program number = %d)\n",p_info->host_name,p_info->pid,p_info->program_num);
			
		}
	}
	else
	{
		if (strcmp(p_info->host_name,"not_exp") != 0)
			sprintf(out,"The DS process is running on host %s (Program number = %d)\n",p_info->host_name,p_info->program_num);
		else
			sprintf(out,"The device server does not have any exported device(s)\n");
	}

/* Copy each device info into resulting string and reallocate memory if needed */

	for (i = 0;i < p_info->embedded_server_nb;i++)
	{
		if (all_ds == True)
		{
			if (p_info->embedded_server_nb != 1)
			{
				sprintf(tmp,"Device server class : %s\n",p_info->server[i].server_name);
				strcat(out,tmp);
			}
			for (j = 0;j < p_info->server[i].device_nb;j++)
			{
				if (strlen(out) > ((nb_alloc * 1000) - 100))
				{
					nb_alloc++;
					if ((out = (char *)realloc(out,1000 * nb_alloc)) == NULL)
					{
						*error = DbErr_ClientMemoryAllocation;
						return(-1);
					}
				}
			
				if (p_info->server[i].device[j].exported_flag == True)
				{
					sprintf(tmp,"Device number %d : %s exported from host %s\n",j + 1,p_info->server[i].device[j].dev_name,p_info->host_name);
					strcat(out,tmp);
				}
				else
				{
					sprintf(tmp,"The device %s is defined for this server but is not exported\n",p_info->server[i].device[j].dev_name);
					strcat(out,tmp);
				}
			}
		}
		else
		{
			if (strcmp(ds_name,p_info->server[i].server_name) == 0)
			{
				for (j = 0;j < p_info->server[i].device_nb;j++)
				{
					if (strlen(out) > ((nb_alloc * 1000) - 100))
					{
						nb_alloc++;
						if ((out = (char *)realloc(out,1000 * nb_alloc)) == NULL)
						{
							*error = DbErr_ClientMemoryAllocation;
							return(-1);
						}
					}
			
					if (p_info->server[i].device[j].exported_flag == True)
					{
						sprintf(tmp,"Device number %d : %s exported from host %s\n",j + 1,p_info->server[i].device[j].dev_name,p_info->host_name);
						strcat(out,tmp);
					}
					else
					{
						sprintf(tmp,"The device %s is defined for this server but is not exported\n",p_info->server[i].device[j].dev_name);
						strcat(out,tmp);
					}
				}
			}
		}
	}

/* Add embedded device classes if only one class is requested and several ones
   are defined in the server */
   	
	if ((all_ds == False) && (p_info->embedded_server_nb != 1))
	{
		sprintf(tmp,"Folowing %d device server classes are also embedded into this process :\n",p_info->embedded_server_nb - 1);
		strcat(out,tmp);
		for (i = 0;i < p_info->embedded_server_nb;i++)
		{
			if (strcmp(ds_name,p_info->server[i].server_name) != 0)
			{
				if (strlen(out) > ((nb_alloc * 1000) - 100))
				{
					nb_alloc++;
					if ((out = (char *)realloc(out,1000 * nb_alloc)) == NULL)
					{
						*error = DbErr_ClientMemoryAllocation;
						return(-1);
					}
				}
			
				strcat(out,p_info->server[i].server_name);
				strcat(out,"\n");
			}
		}
	}		
	
	*p_str = out;
	return(0);
	
}
