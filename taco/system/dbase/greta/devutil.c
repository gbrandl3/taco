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
 * File           : devutil.c
 * 
 * Project        : GRETA application
 * 
 * Description    : The module which contains all the utilities used to display
 * 		  device server information (device list, device resources..)
 *
 * Author         : E. Taurel
 *		  $Author: jkrueger1 $
 *
 * Original       : June 1998
 *
 * Version	: $Revision: 1.6 $
 *
 * Date		: $Date: 2008-04-06 09:07:38 $
 *
 */

/*
 * Standard includes for builtins.
 */
#ifdef HAVE_CONFIG_H
#	include "config.h" 
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#error Could not find signal.h
#endif
#include <time.h>
#include <math.h>

#include <API.h>
#include <DevErrors.h>
#include <DevCmds.h>
#include <DserverTeams.h>
#include <dc.h>
#if HAVE_SIGGEN_H
#include <siggen.h>
#endif


#include <hdb_out.h>

#include <greta.h>

/* include types to display XDR data types */

#include <atte_xdr.h>
#include <bpm_xdr.h>
#include <bpss_xdr.h>
#include <ct_xdr.h>
#include <gpib_xdr.h>
#include <grp_xdr.h>
#include <haz_xdr.h>
#include <icv101_xdr.h>
#include <m4_xdr.h>
#include <maxe_xdr.h>
#include <mclock_xdr.h>
#include <mstat_xdr.h>
#include <pin_xdr.h>
#include <pss_xdr.h>
#include <ram_xdr.h>
#include <rf_xdr.h>
#include <seism_xdr.h>
#include <slit_xdr.h>
#include <thc_xdr.h>
#include <union_xdr.h>
#include <vgc_xdr.h>
#include <vrif_xdr.h>
#include <ws_xdr.h>


/* Function declaration */

static void printtype(int,char *);
static long get_print_daemon(char *,char *);
static long ask_daemon(char *,char *);
static long get_type_str(long ,char *,long *);
static void disp_storage(HdbSigConf *,char *);
static void disp_values(HdbSigOut *,char *);
static long objinfo_sig(long,char *);


void devinfo_str(db_devinfo_call *p_info,char *dev_name,char *info_str)
{
	char tmp[100];
	
	if (p_info->device_type == DB_Device)
	{	
		if (p_info->device_exported == False)
			sprintf(info_str,"The device %s is not actually exported",dev_name);
		else
		{
			sprintf(info_str,"Device %s belongs to class : %s\n",dev_name,p_info->device_class);
			sprintf(tmp,"It is monitored by the server : %s/%s version %d\n",p_info->server_name,p_info->personal_name,p_info->server_version);
			strcat(info_str,tmp);
			if (strcmp(p_info->process_name,"unknown") != 0)
			{
				sprintf(tmp,"The device server process name is : %s",p_info->process_name);
				strcat(info_str,tmp);
				if (p_info->program_num != 0)
				{
					sprintf(tmp," with program number : %d\n",p_info->program_num);
					strcat(info_str,tmp);
				}
				else
					strcat(info_str,"\n");
			}
			sprintf(tmp,"The process is running on the computer : %s",p_info->host_name);
			strcat(info_str,tmp);
			if (p_info->pid != 0)
			{
				sprintf(tmp," with process ID : %d\n",p_info->pid);
				strcat(info_str,tmp);
			}
		}
	}
	else
	{
		sprintf(info_str,"Device %s is a pseudo device\n",dev_name);
		sprintf(tmp,"It has been created by a process with PID : %d running on host : %s",p_info->pid,p_info->host_name);
		strcat(info_str,tmp);
	}
}



long getdc_info(char *devname, char *dc_str)
{
	int i,j,l,k;
	int resu,nb_cmd,ind;
	DevLong error;
	dc_devinf dcdev;
	unsigned int cmd_team;
	unsigned int cmd_dsid;
	int cmd_num;
	char *cmd_str;
	char cmd_team_str[40];
	char cmd_dsid_str[40];
	char cmd_num_str[40];
	char dev_cmd_name[40];
	char tmp[200];
	db_resource res_cmd;
	db_poller po;

/* Ask dc server for device information */

	resu = 0;
	resu = dc_dinfo(devname,&dcdev,&error);
	if (resu == -1)
	{
		if (error != DcErr_DeviceNotDefined)
		{
			return(-1);
		}
		else 
		{
			sprintf(dc_str,"The device %s is not registered in the data collector\n",devname);
			return(0);
		}
	}

/* Display device information */

	sprintf(dc_str,"The device %s is registered in the dc with %d command(s)\n",devname,dcdev.devinf_nbcmd);

/* Retrieve command name from resources */

	for (j = 0;j < dcdev.devinf_nbcmd;j++)
	{
		if (dcdev.devcmd[j].devinf_cmd == 0)
		{
			strcat(dc_str,"Unknown command code !!!\n");
			continue;
		}

		cmd_team = (unsigned int)(dcdev.devcmd[j].devinf_cmd & DS_LSHIFT(DS_TEAM_MASK,DS_TEAM_SHIFT)) >> DS_TEAM_SHIFT;
		cmd_dsid = (unsigned int)(dcdev.devcmd[j].devinf_cmd & DS_LSHIFT(DS_IDENT_MASK,DS_IDENT_SHIFT)) >> DS_IDENT_SHIFT;
		cmd_num = dcdev.devcmd[j].devinf_cmd & 0x0003FFFF;

		dev_cmd_name[0] = 0;
		strcpy(dev_cmd_name,"CMDS/");
		sprintf(cmd_team_str,"%d",cmd_team);
		strcat(dev_cmd_name,cmd_team_str);
		strcat(dev_cmd_name,"/");
		sprintf(cmd_dsid_str,"%d",cmd_dsid);
		strcat(dev_cmd_name,cmd_dsid_str);
		sprintf(cmd_num_str,"%d",cmd_num);

		cmd_str = NULL;
		res_cmd.resource_name = cmd_num_str;
		res_cmd.resource_type = D_STRING_TYPE;
		res_cmd.resource_adr = &cmd_str;
		
		if (db_getresource(dev_cmd_name,&res_cmd,1,&error))
		{
			return(-1);
		}
		if (cmd_str == NULL)
		{
			sprintf(&(dc_str[strlen(dc_str)]),"Command name is not defined in DB");
		}
		else
		{
			sprintf(&(dc_str[strlen(dc_str)]),"Command name : %s",cmd_str);
			printtype(dcdev.devcmd[j].devinf_argout,dc_str);
			if (dcdev.devcmd[j].devinf_time != 0)
				sprintf(&(dc_str[strlen(dc_str)]),"The last %s command was executed in %d mS\n",cmd_str,dcdev.devcmd[j].devinf_time);
			free(cmd_str);
		}
	}

/* Print polling intervall */

	sprintf(&(dc_str[strlen(dc_str)]),"\nThe polling interval is (in 1/10 s) : %d\n",dcdev.devinf_poll);

/* Are some data available ? */

	if (dcdev.devinf_data_base == 0)
		sprintf(&(dc_str[strlen(dc_str)]),"No data available yet\n");
	else
	{

/* Print interval between the last five records */

		sprintf(&(dc_str[strlen(dc_str)]),"\nDelta (in 1/10s) between the last six records :\n");
		sprintf(&(dc_str[strlen(dc_str)]),"%d, %d, %d, %d, %d\n",dcdev.devinf_delta[0],
					      dcdev.devinf_delta[1],
					      dcdev.devinf_delta[2],
					      dcdev.devinf_delta[3],
					      dcdev.devinf_delta[4]);
		sprintf(&(dc_str[strlen(dc_str)]),"Data not updated since %d tenth(s) of a second\n",dcdev.devinf_diff_time);

/* Get daemon info. */

		if (db_getpoller(devname,&po,&error))
		{
			sprintf(&(dc_str[strlen(dc_str)]),"Can't get by which poller the device is polled !!(error = %d)\n",error);
		}
		else
		{
			sprintf(&(dc_str[strlen(dc_str)]),"\nDevice polled by a %s running on : %s with PID %d\n",po.server_name,po.host_name,po.pid);
			sprintf(&(dc_str[strlen(dc_str)]),"Daemon started with the personal name : %s\n",po.personal_name);
		}

	}
	
/* Leave function */

	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for printtype  function                                *
*                        ---------                                          *
*                                                                           *
*    Function rule : To print on the sreen a message according to the       *
*		     command output argument type			    *
*                                                                           *
*    Argin : - type : The argout type					    *
*                                                                           *
*    Argout : - str : The buffer where the command output type should be    *
*		      written						    * 
*                                                                           *
****************************************************************************/

static void printtype(int type,char *str)
{
	switch(type) {
		case D_VOID_TYPE : strcat(str,"     with D_VOID_TYPE\n");
				   break;

		case D_BOOLEAN_TYPE : strcat(str,"     with D_BOOLEAN_TYPE\n");
				   break;

		case D_SHORT_TYPE : strcat(str,"     with D_SHORT_TYPE\n");
				   break;

		case D_LONG_TYPE : strcat(str,"     with D_LONG_TYPE\n");
				   break;

		case D_FLOAT_TYPE : strcat(str,"     with D_FLOAT_TYPE\n");
				   break;

		case D_DOUBLE_TYPE : strcat(str,"     with D_DOUBLE_TYPE\n");
				   break;

		case D_STRING_TYPE : strcat(str,"     with D_STRING_TYPE\n");
				   break;

		case D_FLOAT_READPOINT : strcat(str,"     with D_FLOAT_READPOINT\n");
				   break;

		case D_STATE_FLOAT_READPOINT : strcat(str,"     with D_STATE_FLOAT_READPOINT\n");
				   break;

		case D_VAR_CHARARR : strcat(str,"     with D_VAR_CHARARR\n");
				   break;

		case D_VAR_SHORTARR : strcat(str,"     with D_VAR_SHORTARR\n");
				   break;

		case D_VAR_LONGARR : strcat(str,"     with D_VAR_LONGARR\n");
				   break;

		case D_VAR_FLOATARR : strcat(str,"     with D_VAR_FLOATARR\n");
				   break;

		case D_BPM_POSFIELD : strcat(str,"     with BPM_POSFIELD\n");
				   break;

		case D_BPM_ELECFIELD : strcat(str,"     with D_BPM_ELECFIELD\n");
				   break;

		case D_WS_BEAMFITPARM : strcat(str,"     with D_WS_BEAMFITPARM\n");
				   break;

		case D_VGC_STATUS : strcat(str,"     with D_VGC_STATUS\n");
				   break;

		case D_VGC_GAUGE : strcat(str,"     with D_VGC_GAUGE\n");
				   break;

		case D_VGC_CONTROLLER : strcat(str,"     with D_VGC_CONTROLLER\n");
				   break;

		case D_UNION_TYPE : strcat(str,"     with D_UNION_TYPE\n");
				   break;

		case D_NEG_STATUS : strcat(str,"     with D_NEG_STATUS\n");
				   break;

		case D_RAD_DOSE_VALUE : strcat(str,"     with D_RAD_DOSE_VALUE\n");
				   break;

		case D_LONG_READPOINT : strcat(str,"     with D_LONG_READPOINT\n");
				   break;

		case D_DOUBLE_READPOINT : strcat(str,"     with D_DOUBLE_READPOINT\n");
				   break;

		case D_VAR_STRINGARR : strcat(str,"     with D_VAR_STRINGARR\n");
				   break;

		case D_VAR_FRPARR : strcat(str,"     with D_VAR_FRPARR\n");
				   break;

		case D_VAR_THARR : strcat(str,"     with D_VAR_THARR\n");
				   break;

		case D_INT_FLOAT_TYPE : strcat(str,"     with D_INT_FLOAT_TYPE\n");
				   break;

		case D_HAZ_STATUS : strcat(str,"     with D_HAZ_STATUS\n");
				   break;

		case D_VRIF_WDOG : strcat(str,"     with D_VRIF_WDOG\n");
				   break;

		case D_VRIF_STATUS : strcat(str,"     with D_VRIF_STATUS\n");
				   break;

		case D_VRIF_POWERSTATUS : strcat(str,"     with D_VRIF_POWERSTATUS\n");
				   break;

		case D_GPIB_WRITE : strcat(str,"     with D_GPIB_WRITE\n");
				   break;

		case D_GPIB_MUL_WRITE : strcat(str,"     with D_GPIB_MUL_WRITE\n");
				   break;

		case D_STATE_INDIC : strcat(str,"     with D_STATE_INDIC\n");
				   break;

		case D_BPSS_STATE : strcat(str,"     with D_BPSS_STATE\n");
				   break;

		case D_BPSS_READPOINT : strcat(str,"     with D_BPSS_READPOINT\n");
				   break;

		case D_GPIB_RES : strcat(str,"     with D_GPIB_RES\n");
				   break;

		case D_GPIB_LOC : strcat(str,"     with D_GPIB_LOC\n");
				   break;

		case D_PSS_STATUS : strcat(str,"     with D_PSS_STATUS\n");
				   break;

		case D_RF_SIGCONFIG : strcat(str,"     with D_RF_SIGCONFIG\n");
				   break;

		case D_BPSS_LINE : strcat(str,"     with D_BPSS_LINE\n");
				   break;

		case D_CT_LIFETIME : strcat(str,"     with D_CT_LIFETIME\n");
				   break;

		case D_VAR_LRPARR : strcat(str,"     with D_VAR_LRPARR\n");
				   break;

		case D_MULMOVE_TYPE : strcat(str,"     with D_MULMOVE_TYPE\n");
				   break;

		case D_OPAQUE_TYPE : strcat(str,"     with D_OPAQUE_TYPE\n");
				   break;

		case D_SEISM_EVENT : strcat(str,"     with D_SEISM_EVENT\n");
				   break;

		case D_SEISM_STAT : strcat(str,"     with D_SEISM_STAT\n");
				   break;

		default : strcat(str,"     with an unknown type\n");
			  break;
			}
}


/************************************************************************
*									*
*		gethdb_info function					*
*		-----------						*
*									*
*   Function rule : To give caller a summary on the HDB status for a    *
*		   device						*
*		   This function mainly returns global information on   *
*		   device, the filling status and modes plus the last 9 *
*		   data recorded					*
*									*
*     Argins : - dev_name : The device name				*
*									*
*     Argouts : - buff : Pointer to the buffer where the result    	*
*			 must be stored					*
*									*
************************************************************************/


long gethdb_info(char *dev_name,char *buff)
{
#if HAVE_SIGGEN_H
	long sig_nb;
	long error;
	char **sig_list;
	long sig_dev = 0;
	long sigind_array[100];
	char tmp_dev_name[HDB_DEV_NAME_LENGTH];
	char dev_name_nethost[80];
	unsigned int diff;
	char *tmp;
	long found = False;
	register long i;
	long sigid;
	char *nethost;

/* Build device name with nethost */

	if ((nethost = getenv("NETHOST")) == NULL)
	{
		strcpy(buff,"No NETHOST environment variable defined\n");
		return(0);
	}
	strcpy(dev_name_nethost,"//");
	strcat(dev_name_nethost,nethost);
	strcat(dev_name_nethost,"/");
	strcat(dev_name_nethost,dev_name);
	
/* First get a list of all signals defined within HDB */

	if (hdb_get_sig_list(&sig_list,&sig_nb,&error) == -1)
	{
		if (error == HDB_CANT_CONNECT)
		{
			strcpy(buff,"Impossible to connect to database\n");
			return(0);
		}
		else
		{
			return(-1);
		}
	}
	
/* Find signal for the requested device */

	for (i = 0;i < sig_nb;i++)
	{
		strcpy(tmp_dev_name,sig_list[i]);
		tmp = strrchr(tmp_dev_name,'/');
		diff = (unsigned int)(tmp - tmp_dev_name);
		tmp_dev_name[diff] = '\0';
		
		if (strcmp(tmp_dev_name,dev_name_nethost) == 0)
		{
			sigind_array[sig_dev] = i;
			sig_dev++;
			found = True;
		}
		else
		{
			if (found == True)
				break;
		}
	}

	sprintf(buff,"%d signal(s) is(are) defined in HDB for device %s\n\n",sig_dev,dev_name);
		
/* A loop on each device's signal */

	for (i = 0;i < sig_dev;i++)
	{
		if (hdb_get_sigid_byname(sig_list[sigind_array[i]],&sigid,&error) == -1)
		{
			return(-1);
		}
		
		if (objinfo_sig(sigid,buff))
			return(-1);
		
		sprintf(&(buff[strlen(buff)]),"\n");
		
	}
	
/* Leave function */

	return(0);
	
#endif /* HAVE_SIGGEN_H */
	
}	



/************************************************************************
*									*
*		objinfo_sig function code				*
*		-----------						*
*									*
*     Function role : To translate the signal type from a number to   	*
*		      a string (much mode understandable)		*
*									*
*     Argins : - sigid : The signal identifier				*
*									*
*     Argouts : - buff : Pointer to the buffer where the result    	*
*			 must be stored					*
*									*
*    This function returns 0 if successful. Otherwise, retuns -1	*
*									*
*************************************************************************/

#if HAVE_SIGGEN_H

static long objinfo_sig(long sigid,char *buff)
{
	char dev_name[HDB_DEV_NAME_LENGTH];
	char sig_name[HDB_SIG_NAME_LENGTH];
	char cmd[80];
	char sig_type_str[80];
	long l,error,cmd_str;
	HdbSigInfo siginfo;
	HdbSigConf conf;
	HdbDateInt when;
	HdbSigOut sig_data;
	long devname_length;
	register int i,j;
	time_t now,now_8hours,now_1day,now_8days;

	   	
/* Get object general information */

	if (hdb_get_sig_info(sigid,&siginfo,&error) != 0)
	{
		return(-1);
	}

/* Translate signal type to a string */

	if (get_type_str(siginfo.sig_type,sig_type_str,&error) == -1)
	{
		return(-1);
	}

/* Extract device name and signal name from full signal name */

	for (i = 2;i < strlen(siginfo.sig_name);i++)
	{
		if (siginfo.sig_name[i] == '/')
			break;
	}
	for (j = strlen(siginfo.sig_name); j > 0;j--)
	{
		if (siginfo.sig_name[j] == '/')
			break;
	}

	devname_length = j;
	strncpy(dev_name,siginfo.sig_name,devname_length);
	dev_name[devname_length] = '\0';
		
	strcpy(sig_name,&(siginfo.sig_name[j + 1]));
		
/* Get the command used to retrieve signal value as a string */

	cmd_str = False;

/* Display these info to the user */


	sprintf(&(buff[strlen(buff)]),"Signal %s is defined in HDB\n",sig_name);
	sprintf(&(buff[strlen(buff)]),"The signal type is %s",sig_type_str);
	if ((siginfo.sig_type == D_VAR_CHARARR) ||
	    (siginfo.sig_type == D_VAR_SHORTARR) ||
	    (siginfo.sig_type == D_VAR_LONGARR) ||
	    (siginfo.sig_type == D_VAR_FLOATARR) ||
	    (siginfo.sig_type == D_VAR_DOUBLEARR))
	{
		sprintf(&(buff[strlen(buff)])," with %d element(s)\n",siginfo.sig_array_length);
	}
	else
		strcat(&(buff[strlen(buff)]),"\n");

	if (siginfo.grp_member.length != 0)
	{
		sprintf(&(buff[strlen(buff)]),"Signal used as group member in %d group(s)\n",siginfo.grp_member.length);
		for (i = 0;i < siginfo.grp_member.length;i++)
			sprintf(&(buff[strlen(buff)]),"Group name : %s\n",siginfo.grp_member.sequence[i].group_name);
	}
		
/* Get signal storage mode */

	if (hdb_get_sig_conf(sigid,"Now",&conf,&error) == -1)
	{
		if (error != HDB_NO_CONF_DATE)
			return(-1);
	}

/* Display storage mode data */

	if (error != HDB_NO_CONF_DATE)
	{
		if (conf.filling == HDB_INACTIVE_MODE)
			strcat(&(buff[strlen(buff)]),"Signal filling is actually OFF\n");
		else
			strcat(&(buff[strlen(buff)]),"Signal filling is ENABLED\n");


/* Display storage mode */

		strcat(&(buff[strlen(buff)]),"Signal storage is :\n");
		disp_storage(&conf,buff);
	}

/* Get the last recorded value during the last eight hours. If there is not
   enought data recorded, get then during the last 24 hours. If there is
   still not enought data, get them during the last 8 days. */

	now = time((time_t *)0);
	now_8hours = now - (8 * 60 * 60);
	hdb_date_ux_to_hdb(now_8hours,when.start,&error);
	strcpy(when.stop,"Now");
		
	if (hdb_get_sig_data(sigid,HDB_ALL_MODES,&when,
			     &sig_data,&error) == -1)
	{
		if (error != HDB_NO_DATA_FOUND)
			return(-1);
	}
	if ((sig_data.rec_nb < 9) || (error == HDB_NO_DATA_FOUND))
	{
		hdb_free_sig_data(&sig_data,&error);
			
		now_1day = now - (24 * 60 * 60);
		hdb_date_ux_to_hdb(now_1day,when.start,&error);
			
		if (hdb_get_sig_data(sigid,HDB_ALL_MODES,
				     &when,&sig_data,&error) == -1)
		{
			if (error != HDB_NO_DATA_FOUND)
				return(-1);
		}
			
		if ((sig_data.rec_nb < 9) || (error == HDB_NO_DATA_FOUND))
		{
			hdb_free_sig_data(&sig_data,&error);
							
			now_8days = now - (8 * 24 * 60 * 60);
			hdb_date_ux_to_hdb(now_8days,when.start,&error);
			
			if (hdb_get_sig_data(sigid,HDB_ALL_MODES,
					     &when,&sig_data,&error) == -1)
			{
				if (error != HDB_NO_DATA_FOUND)
					return(-1);
				else
				{
					strcat(&(buff[strlen(buff)]),"No data recorded during the last 8 days\n");
					return(0);
				}
			}
		}
	}			

/* Display signal values */

	disp_values(&sig_data,buff);
		
/* Free the memory allocated by the hdb_sig_data function */

	hdb_free_sig_data(&sig_data,&error);
	
/* Leave function */

	return(0);
	
}
		




/************************************************************************
*									*
*		get_type_str function code				*
*		------------						*
*									*
*     Function role : To translate the signal type from a number to   	*
*		      a string (much mode understandable)		*
*									*
*     Argins : - type : The object type					*
*	       - p_error : Pointer to store error code in case of	*
*									*
*     Argouts : - p_data : Pointer to the buffer where the result    	*
*			   must be stored				*
*									*
*    This function returns 0 if successful. Otherwise, retuns -1	*
*									*
*************************************************************************/

static long get_type_str(long type,char *p_data,long *p_error)
{
	long ret = 0;

	switch(type)
	{
		case D_BOOLEAN_TYPE : 
			strcpy(p_data,"D_BOOLEAN_TYPE");
			break;
				
		case D_SHORT_TYPE :
			strcpy(p_data,"D_SHORT_TYPE");
			break;
				
		case D_LONG_TYPE :
			strcpy(p_data,"D_LONG_TYPE");
			break;

		case D_FLOAT_TYPE :
			strcpy(p_data,"D_FLOAT_TYPE");
			break;
				
		case D_DOUBLE_TYPE :
			strcpy(p_data,"D_DOUBLE_TYPE");
			break;
				
		case D_STRING_TYPE :
			strcpy(p_data,"D_STRING_TYPE");
			break;

		case D_VAR_CHARARR :
			strcpy(p_data,"D_VAR_CHARARR");
			break;

		case D_VAR_SHORTARR :
			strcpy(p_data,"D_VAR_SHORTARR");
			break;

		case D_VAR_LONGARR :
			strcpy(p_data,"D_VAR_LONGARR");
			break;

		case D_VAR_FLOATARR :
			strcpy(p_data,"D_VAR_FLOATARR");
			break;

		case D_VAR_DOUBLEARR :
			strcpy(p_data,"D_VAR_DOUBLEARR");
			break;

		default : 
			ret = -1;
			break;
	}

	return(ret);

}



/****************************************************************************
*                                                                           *
*               disp_storage function code  		                    *
*               ------------                            		    *
*                                                                           *
*    Function rule : To display a storage mode summary before these data are*
*		     inserted into HDB tables				    *
*                                                                           *
*    Argins : - p_mode : The address of the signal storage mode structure   *
*                                                                           *
*    Argout : - buff : Buffer where the result should be stored 	    *
*                                                                           *
*    This function does not return anything				    *
*									    *
*****************************************************************************/

static void disp_storage(HdbSigConf *p_mode,char *buff)
{

/* Display mode 1 parameters if enabled */

	if (p_mode->mode1.flag == HDB_MODE_ON)
		sprintf(&(buff[strlen(buff)]),"Mode 1, Sample number : %d\n",p_mode->mode1.sample_nb);

/* Display mode 2 parameters if enabled */

	if (p_mode->mode2.flag == HDB_MODE_ON)
		sprintf(&(buff[strlen(buff)]),"Mode 2, UDT : %d seconds\n",p_mode->mode2.udt);

/* Display mode 3 parameters if enabled */

	if (p_mode->mode3.flag != HDB_MODE_OFF)
	{
		switch(p_mode->mode3.sub_mode)
		{
			case HDB_MODE3_AVERAGE :
				sprintf(&(buff[strlen(buff)]),"Mode 3 with AVERAGE on %d data collector samples, UDT : %d seconds\n",p_mode->mode3.n,p_mode->mode3.udt);
				break;

			case HDB_MODE3_MIN :
				sprintf(&(buff[strlen(buff)]),"Mode 3 with MINUMUN research on %d data collector samples, UDT : %d seconds\n",p_mode->mode3.n,p_mode->mode3.udt);
				break;

			case HDB_MODE3_MAX :
				sprintf(&(buff[strlen(buff)]),"Mode 3 with MAXIMUN research on %d data collector samples, UDT : %d seconds\n",p_mode->mode3.n,p_mode->mode3.udt);
				break;

			case HDB_MODE3_RMS :
				sprintf(&(buff[strlen(buff)]),"Mode 3 with RMS computation on %d data collector samples, UDT : %d seconds\n",p_mode->mode3.n,p_mode->mode3.udt);
				break;
		}
	}

/* Display mode 4 parameters if enabled */

	if (p_mode->mode4.flag != HDB_MODE_OFF)
	{
		if (p_mode->mode4.sub_mode == HDB_MODE4_STRICT)
			sprintf(&(buff[strlen(buff)]),"Mode 4, UDT : %d seconds, No delta authorized\n",p_mode->mode4.udt);
		else
		{
			if (p_mode->mode4.sub_mode == HDB_MODE4_WIN_REL)
				sprintf(&(buff[strlen(buff)]),"Mode 4, UDT : %d seconds, Positive delta (X) : %d%%, Negative delta (Y) : %d%%\n",p_mode->mode4.udt,p_mode->mode4.x.rel,p_mode->mode4.y.rel);
			else 
			{
				if ((p_mode->sig_type == D_FLOAT_TYPE) ||
				    (p_mode->sig_type == D_DOUBLE_TYPE) ||
				    (p_mode->sig_type == D_VAR_FLOATARR) ||
				    (p_mode->sig_type == D_VAR_DOUBLEARR))
					sprintf(&(buff[strlen(buff)]),"Mode 4, UDT : %d sec, Pos delta (X) : %.3e, Neg delta (Y) : %.3e\n",p_mode->mode4.udt,p_mode->mode4.x.abs_db,p_mode->mode4.y.abs_db);
				else
					sprintf(&(buff[strlen(buff)]),"Mode 4, UDT : %d sec, Pos delta (X) : %d, Neg delta (Y) : %d\n",p_mode->mode4.udt,p_mode->mode4.x.abs,p_mode->mode4.y.abs);
			}
		}
	}

/* Display mode 5 parameters if enabled */

	if (p_mode->mode5.flag != HDB_MODE_OFF)
	{
		sprintf(&(buff[strlen(buff)]),"Mode 5, UDT : %d seconds\n",p_mode->mode5.udt);
		switch(p_mode->sig_type)
		{
			case D_BOOLEAN_TYPE :
				if (p_mode->mode5.ref.ch == True)
					sprintf(&(buff[strlen(buff)]),"Recording when signal is True(1)\n");
				else
					sprintf(&(buff[strlen(buff)]),"Recording when signal is False(0)\n");
				break;

			case D_STRING_TYPE :
				sprintf(&(buff[strlen(buff)]),"Recording when signal <> than the ref. string\n");
				sprintf(&(buff[strlen(buff)]),"Ref. string : %s\n",p_mode->mode5.ref.str);
				break;

			default :
				switch(p_mode->mode5.sub_mode)
				{
				case HDB_MODE5_STRICT : 
				sprintf(&(buff[strlen(buff)]),"Recording when signal <> than the reference\n");
				break;

				case HDB_MODE5_GREATER : 
				sprintf(&(buff[strlen(buff)]),"Recording when signal > than the reference\n");
				break;

				case HDB_MODE5_LOWER : 
				sprintf(&(buff[strlen(buff)]),"Recording when signal < than the reference\n");
				break;

				case HDB_MODE5_GREATER_EQUAL : 
				sprintf(&(buff[strlen(buff)]),"Recording when signal >= than the reference\n");
				break;

				case HDB_MODE5_LOWER_EQUAL : 
				sprintf(&(buff[strlen(buff)]),"Recording when signal <= than the reference\n");
				break;

				case HDB_MODE5_WINDOW : 
				sprintf(&(buff[strlen(buff)]),"Recording when signal outside a window\n");
				break;
				}
				switch(p_mode->sig_type)
				{
					case D_VAR_CHARARR : 
					if (p_mode->mode5.sub_mode == HDB_MODE5_WINDOW)
						sprintf(&(buff[strlen(buff)]),"Mode 5, Upper limit : %d      Lower limit : %d\n",p_mode->mode5.upp_win.ch,p_mode->mode5.low_win.ch);
					else
						sprintf(&(buff[strlen(buff)]),"Mode 5, Reference : %d\n",p_mode->mode5.ref.ch);
					break;

					case D_VAR_SHORTARR :
					case D_SHORT_TYPE :
					if (p_mode->mode5.sub_mode == HDB_MODE5_WINDOW)
						sprintf(&(buff[strlen(buff)]),"Mode 5, Upper limit : %d      Lower limit : %d\n",p_mode->mode5.upp_win.sh,p_mode->mode5.low_win.sh);
					else
						sprintf(&(buff[strlen(buff)]),"Mode 5, Reference : %d\n",p_mode->mode5.ref.sh);
					break;

					case D_VAR_LONGARR :
					case D_LONG_TYPE :
					if (p_mode->mode5.sub_mode == HDB_MODE5_WINDOW)
						sprintf(&(buff[strlen(buff)]),"Mode 5, Upper limit : %d      Lower limit : %d\n",p_mode->mode5.upp_win.lo,p_mode->mode5.low_win.lo);
					else
						sprintf(&(buff[strlen(buff)]),"Mode 5, Reference : %d\n",p_mode->mode5.ref.lo);
					break;

					case D_VAR_FLOATARR :
					case D_FLOAT_TYPE :
					if (p_mode->mode5.sub_mode == HDB_MODE5_WINDOW)
						sprintf(&(buff[strlen(buff)]),"Mode 5, Upper limit : %e      Lower limit : %e\n",p_mode->mode5.upp_win.fl,p_mode->mode5.low_win.fl);
					else
						sprintf(&(buff[strlen(buff)]),"Mode 5, Reference : %e\n",p_mode->mode5.ref.fl);
			     		break;

					case D_VAR_DOUBLEARR :
					case D_DOUBLE_TYPE :
					if (p_mode->mode5.sub_mode == HDB_MODE5_WINDOW)
						sprintf(&(buff[strlen(buff)]),"Mode 5, Upper limit : %e      Lower limit : %e\n",p_mode->mode5.upp_win.db,p_mode->mode5.low_win.db);
					else
						sprintf(&(buff[strlen(buff)]),"Mode 5, Reference : %e\n",p_mode->mode5.ref.db);
			     		break;
				}
			break;
			
		}
	}

/* Display mode 7 parameters if enabled */

	if (p_mode->mode7.flag != HDB_MODE_OFF)
		sprintf(&(buff[strlen(buff)]),"Mode 7 enabled\n");

}
 


/****************************************************************************
*                                                                           *
*               disp_values function code  		                    *
*               -----------                            			    *
*                                                                           *
*    Function rule : To display the last signal values		    	    *
*                                                                           *
*    Argins : - p_mode : The address of the signal data structure  	    *
*                                                                           *
*    Argout : - buff : Pointer to the buffer where results should be stored *
*                                                                           *
*    This function does not return anything				    *
*									    *
*****************************************************************************/

static void disp_values(HdbSigOut *p_data,char *buff)
{
	register long i,j,k,nb;
	long array_len;

	if (p_data->rec_nb > 9)
		nb = 9;
	else
		nb = p_data->rec_nb;

	sprintf(&(buff[strlen(buff)]),"\nLast value recorded\n");


	i = p_data->rec_nb - 1;
	for (k = 0;k < nb;k++)
	{
		switch(p_data->sig_type)
		{
		case D_BOOLEAN_TYPE :
			if (((short *)p_data->sig_data.null_ptr)[i] == 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : %d recorded at %s\n",((char *)p_data->sig_data.data_ptr)[i],p_data->sig_data.date_str[i]);
			else
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_SHORT_TYPE :
			if (((short *)p_data->sig_data.null_ptr)[i] == 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : %d recorded at %s\n",((short *)p_data->sig_data.data_ptr)[i],p_data->sig_data.date_str[i]);
			else
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_LONG_TYPE :
			if (((short *)p_data->sig_data.null_ptr)[i] == 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : %d recorded at %s\n",((long *)p_data->sig_data.data_ptr)[i],p_data->sig_data.date_str[i]);
			else
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_FLOAT_TYPE :
			if (((short *)p_data->sig_data.null_ptr)[i] == 0)
			{
				if (fabs((double)((float *)p_data->sig_data.data_ptr)[i]) < (double)0.001)
					sprintf(&(buff[strlen(buff)]),"Signal value : %e recorded at %s\n",((float *)p_data->sig_data.data_ptr)[i],p_data->sig_data.date_str[i]);
				else
					sprintf(&(buff[strlen(buff)]),"Signal value : %f recorded at %s\n",((float *)p_data->sig_data.data_ptr)[i],p_data->sig_data.date_str[i]);
			}
			else
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_DOUBLE_TYPE :
			if (((short *)p_data->sig_data.null_ptr)[i] == 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : %e recorded at %s\n",((double *)p_data->sig_data.data_ptr)[i],p_data->sig_data.date_str[i]);
			else
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_STRING_TYPE :
			if (((short *)p_data->sig_data.null_ptr)[i] == 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : %s recorded at %s\n",((char **)p_data->sig_data.data_ptr)[i],p_data->sig_data.date_str[i]);
			else
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_VAR_CHARARR :
			array_len = p_data->array_length;
			if (((short *)p_data->sig_data.null_ptr)[i] != 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL\n");
			else
			{
				for (j = 0;j < array_len;j++)
					sprintf(&(buff[strlen(buff)]),"Signal element %d : %d\n",j,((char **)p_data->sig_data.data_ptr)[j][i]);
			}
			sprintf(&(buff[strlen(buff)]),"Signal recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_VAR_SHORTARR :
			array_len = p_data->array_length;
			if (((short *)p_data->sig_data.null_ptr)[i] != 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL\n");
			else
			{
				for (j = 0;j < array_len;j++)
					sprintf(&(buff[strlen(buff)]),"Signal element %d : %d\n",j,((short **)p_data->sig_data.data_ptr)[j][i]);
			}
			sprintf(&(buff[strlen(buff)]),"Signal recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_VAR_LONGARR :
			array_len = p_data->array_length;
			if (((short *)p_data->sig_data.null_ptr)[i] != 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL\n");
			else
			{
				for (j = 0;j < array_len;j++)
					sprintf(&(buff[strlen(buff)]),"Signal element %d : %d\n",j,((long **)p_data->sig_data.data_ptr)[j][i]);
			}
			sprintf(&(buff[strlen(buff)]),"Signal recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_VAR_FLOATARR :
			array_len = p_data->array_length;
			if (((short *)p_data->sig_data.null_ptr)[i] != 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL\n");
			else
			{
				for (j = 0;j < array_len;j++)
					sprintf(&(buff[strlen(buff)]),"Signal element %d : %e\n",j,((float **)p_data->sig_data.data_ptr)[j][i]);
					
			}
			sprintf(&(buff[strlen(buff)]),"Signal recorded at %s\n",p_data->sig_data.date_str[i]);
			break;

		case D_VAR_DOUBLEARR :
			array_len = p_data->array_length;
			if (((short *)p_data->sig_data.null_ptr)[i] != 0)
				sprintf(&(buff[strlen(buff)]),"Signal value : NULL\n");
			else
			{
				for (j = 0;j < array_len;j++)
					sprintf(&(buff[strlen(buff)]),"Signal element %d : %e\n",j,((double **)p_data->sig_data.data_ptr)[j][i]);
			}
			sprintf(&(buff[strlen(buff)]),"Signal recorded at %s\n",p_data->sig_data.date_str[i]);
			break;
		}
		i--;
	}
}

#endif /* HAVE_SIGGEN_H*/

