#include "config.h"
#define _dc_h
#include <API.h>
#undef _dc_h
#include <dc.h>

#include <DevErrors.h>
#include <DevCmds.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef sun
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#else
#include <sys/shm.h>
#endif

#include <string.h>
#include <errno.h>

#include <dcP.h>

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

void printtype(int type);
void get_print_daemon(char *devname);
void ask_daemon(char *devname);

/****************************************************************************
*                                                                           *
*		Code for dc_devinfo command                                 *
*                        ----------                                         *
*                                                                           *
*    Command rule : To display the command registered in the data collector *
*		    for a device					    *
*                                                                           *
*    Synopsis : dc_devinfo <device name> [-v]				    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	int 		c, 
			i,
			j,
			l,
			k;
	int 		resu,
			nb_cmd,
			ind;
	extern int	optind,
			optopt;
	char 		devname[80];
	long 		error;
	dc_devinf 	dcdev;
	int 		verbose = False;
	unsigned int 	cmd_team;
	unsigned int 	cmd_dsid;
	int 		cmd_num;
	char 		*cmd_str;
	char 		cmd_team_str[40];
	char 		cmd_dsid_str[40];
	char 		cmd_num_str[40];
	char 		dev_cmd_name[40];
	db_resource 	res_cmd;

/* Argument test */
	while ((c = getopt(argc, argv, "vh")) != -1)
		switch(c)
		{
			case 'v' :
				verbose = True;
				break;
			case 'h' :
			case '?' :
				fprintf(stderr,"dc_devinfo usage : dc_devinfo [-v] <device name>\n");
				exit(-1);
		}
	if (optind != argc - 1)
	{
		fprintf(stderr,"dc_devinfo usage : dc_devinfo [-v] <device name>\n");
		exit(-1);
	}


	l = strlen(argv[optind]);
	for (i = 0; i < l; i++)
		devname[i] = tolower(argv[optind][i]);
	devname[l] = 0;

/* Ask dc server for device information */
	resu = 0;
	resu = dc_dinfo(devname,&dcdev,&error);
	if (resu == -1)
	{
		if (error != DcErr_DeviceNotDefined)
		{
			fprintf(stderr,"dc_devinfo : dc_dinfo failed with error %d\n",error);
			fprintf(stderr,"dc_devinfo : Sorry, but I exit\n");
			exit(-1);
		}
		else 
		{
			printf("The device %s is not registered in the data collector\n",argv[1]);
			exit(-1);
		}
	}

/* Display device information */
	fprintf(stderr, "The device %s is registered in the dc with %d command(s)\n",argv[1],dcdev.devinf_nbcmd);

/* Retrieve command name from resources */

	for (j = 0;j < dcdev.devinf_nbcmd;j++)
	{
		if (dcdev.devcmd[j].devinf_cmd == 0)
		{
			fprintf(stderr, "Unknown command code !!!\n");
			continue;
		}

		cmd_team = (unsigned int)(dcdev.devcmd[j].devinf_cmd & TEAM_MASK) >> TEAM_SHIFT;
		cmd_dsid = (unsigned int)(dcdev.devcmd[j].devinf_cmd & DSID_MASK) >> DSID_SHIFT;
		cmd_num = dcdev.devcmd[j].devinf_cmd & CMD_MASK;

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
			fprintf(stderr,"dc_devinfo : Can't retrieve command name from resource \n");
			fprintf(stderr,"dc_devinfo : Error code : %d\n",error);
			exit(-1);
		}
		if (cmd_str == NULL)
			fprintf(stderr,"dc_devinfo : Can't retrieve command name from resource\n");
		else
		{
			printf("Command name : %s",cmd_str);
			printtype(dcdev.devcmd[j].devinf_argout);
			if (dcdev.devcmd[j].devinf_time != 0)
				printf("The last %s command was executed in %d mS\n",cmd_str,dcdev.devcmd[j].devinf_time);
		}
	}

/* Print polling intervall */
	printf("\nThe polling interval is (in 1/10 s) : %d\n",dcdev.devinf_poll);

/* Are some data available ? */

	if (dcdev.devinf_data_base == 0)
		fprintf(stderr, "No data available yet\n");
	else
	{
		if (verbose == True)
		{

/* Print the offset to the last pointer for this device */

			printf("Offset (in the ptr area) to the last write data (hex) : %x\n",dcdev.devinf_ptr_off);

/* Print offset to data buffer */

			printf("Offset (in the data buffer) to the last write data (hex) : %x\n",dcdev.devinf_data_off);
			printf("Base address of the data buffer : %x\n",dcdev.devinf_data_base);
		}

/* Print interval between the last five records */

		printf("\nDelta (in 1/10s) between the last six records :\n");
		printf("%d, %d, %d, %d, %d\n",dcdev.devinf_delta[0],
					      dcdev.devinf_delta[1],
					      dcdev.devinf_delta[2],
					      dcdev.devinf_delta[3],
					      dcdev.devinf_delta[4]);
		printf("Data not updated since %d tenth(s) of a second\n",dcdev.devinf_diff_time);

/* If the verbose mode is required, print daemon info. */
		if (verbose == True)
			get_print_daemon(devname);

	}
	return 0;
}



/**
 * To print on the sreen a message according to the command output argument type scratch. 
 * 
 * @param type The argout type
 *
 */
void printtype(int type)
{
	switch(type) 
	{
		case D_VOID_TYPE : 
			printf("     with D_VOID_TYPE\n");
			break;

		case D_BOOLEAN_TYPE : 
			printf("     with D_BOOLEAN_TYPE\n");
			break;

		case D_SHORT_TYPE : 
			printf("     with D_SHORT_TYPE\n");
			break;

		case D_LONG_TYPE : 
			printf("     with D_LONG_TYPE\n");
			break;

		case D_FLOAT_TYPE : 
			printf("     with D_FLOAT_TYPE\n");
			break;

		case D_DOUBLE_TYPE : 
			printf("     with D_DOUBLE_TYPE\n");
			break;

		case D_STRING_TYPE : 
			printf("     with D_STRING_TYPE\n");
			break;

		case D_FLOAT_READPOINT : 
			printf("     with D_FLOAT_READPOINT\n");
			break;

		case D_STATE_FLOAT_READPOINT : 
			printf("     with D_STATE_FLOAT_READPOINT\n");
			break;

		case D_VAR_CHARARR : 
			printf("     with D_VAR_CHARARR\n");
			break;

		case D_VAR_SHORTARR : 
			printf("     with D_VAR_SHORTARR\n");
			break;

		case D_VAR_LONGARR : 
			printf("     with D_VAR_LONGARR\n");
			break;

		case D_VAR_FLOATARR : 
			printf("     with D_VAR_FLOATARR\n");
			break;

		case D_BPM_POSFIELD : 
			printf("     with BPM_POSFIELD\n");
			break;

		case D_BPM_ELECFIELD : 
			printf("     with D_BPM_ELECFIELD\n");
			break;

		case D_WS_BEAMFITPARM : 
			printf("     with D_WS_BEAMFITPARM\n");
			break;

		case D_VGC_STATUS : 
			printf("     with D_VGC_STATUS\n");
			break;

		case D_VGC_GAUGE : 
			printf("     with D_VGC_GAUGE\n");
			break;

		case D_VGC_CONTROLLER : 
			printf("     with D_VGC_CONTROLLER\n");
			break;

		case D_UNION_TYPE : 
			printf("     with D_UNION_TYPE\n");
			break;

		case D_NEG_STATUS : 
			printf("     with D_NEG_STATUS\n");
			break;

		case D_RAD_DOSE_VALUE : 
			printf("     with D_RAD_DOSE_VALUE\n");
			break;

		case D_LONG_READPOINT : 
			printf("     with D_LONG_READPOINT\n");
			break;

		case D_DOUBLE_READPOINT : 
			printf("     with D_DOUBLE_READPOINT\n");
			break;

		case D_VAR_STRINGARR : 
			printf("     with D_VAR_STRINGARR\n");
			break;

		case D_VAR_FRPARR : 
			printf("     with D_VAR_FRPARR\n");
			break;

		case D_VAR_THARR : 
			printf("     with D_VAR_THARR\n");
			break;

		case D_INT_FLOAT_TYPE : 
			printf("     with D_INT_FLOAT_TYPE\n");
			break;

		case D_HAZ_STATUS : 
			printf("     with D_HAZ_STATUS\n");
			break;

		case D_VRIF_WDOG : 
			printf("     with D_VRIF_WDOG\n");
			break;

		case D_VRIF_STATUS : 
			printf("     with D_VRIF_STATUS\n");
			break;

		case D_VRIF_POWERSTATUS : 
			printf("     with D_VRIF_POWERSTATUS\n");
			break;

		case D_GPIB_WRITE : 
			printf("     with D_GPIB_WRITE\n");
			break;

		case D_GPIB_MUL_WRITE : 
			printf("     with D_GPIB_MUL_WRITE\n");
			break;

		case D_STATE_INDIC : 
			printf("     with D_STATE_INDIC\n");
			break;

		case D_BPSS_STATE : 
			printf("     with D_BPSS_STATE\n");
			break;

		case D_BPSS_READPOINT : 
			printf("     with D_BPSS_READPOINT\n");
			break;

		case D_GPIB_RES : 
			printf("     with D_GPIB_RES\n");
			break;

		case D_GPIB_LOC : 
			printf("     with D_GPIB_LOC\n");
			break;

		case D_PSS_STATUS : 
			printf("     with D_PSS_STATUS\n");
			break;

		case D_RF_SIGCONFIG : 
			printf("     with D_RF_SIGCONFIG\n");
			break;

		case D_BPSS_LINE : 
			printf("     with D_BPSS_LINE\n");
			break;

		case D_CT_LIFETIME : 
			printf("     with D_CT_LIFETIME\n");
			break;

		case D_VAR_LRPARR : 
			printf("     with D_VAR_LRPARR\n");
			break;

		case D_MULMOVE_TYPE : 
			printf("     with D_MULMOVE_TYPE\n");
			break;

		case D_OPAQUE_TYPE : 
			printf("     with D_OPAQUE_TYPE\n");
			break;

		case D_SEISM_EVENT : 
			printf("     with D_SEISM_EVENT\n");
			break;

		case D_SEISM_STAT : 
			printf("     with D_SEISM_STAT\n");
			break;

		default : 
			printf("     with an unknown type\n");
			break;
	}
	return;
}



/**
 * To retrieve and print information on the daemon used to poll the device
 * 
 * @param devname The device name
 * 
 */
void get_print_daemon(char *devname)
{
	long i,j,k,l;
	long error;
	long found;
	unsigned int 		dev_num;
	char 			filter[] ="sys/daemon/*";
	char 			**dev_tab;
	DevVarStringArray 	str_res = {0, NULL};
	db_resource 		res = {"ud_poll_list",D_VAR_STRINGARR, &str_res};
	char 			dev_name[80];

	l = strlen(devname);
	for (i = 0;i < l && i < sizeof(dev_name) - 1; i++)
		dev_name[i] = toupper(devname[i]);
	dev_name[i] = '\0';

/* First ask db for all the sys/daemon/* devices */
	if (db_getdevexp(filter, &dev_tab, &dev_num, &error) == DS_NOTOK)
	{
		fprintf(stderr,"dc_devinfo : Error %d during db_getdevexp\n",error);
		exit(-1);
	}

/* Get the ud_poll_list resources for each daemon devices */
//	str_res.sequence = NULL;
//	res.resource_adr = &str_res;
	found = False;
	for (i = 0;i < dev_num && found == False;i++)
	{
		if (db_getresource(dev_tab[i],&res,1,&error) == -1)
		{
			fprintf(stderr,"dc_devinfo : Error %d during db_getresource for dev %s\n",error,dev_tab[i]);
			exit(-1);
		}

/* Display device polled */
		for (j = 0;j < str_res.length;j++)
		{
			l = strlen(str_res.sequence[j]);
			for (k = 0;k < l;k++)
				str_res.sequence[j][k] = toupper(str_res.sequence[j][k]);

			if (strcmp(str_res.sequence[j], dev_name) == 0)
			{
				found = True;
				ask_daemon(dev_tab[i]);
				break;
			}
		}

/* Free mem allocated by db_getresource */
		for (j = 0;j < str_res.length;j++)
			free(str_res.sequence[j]);
		free(str_res.sequence);

		str_res.sequence = NULL;
	}
	return;
}



/**
 * To ask the daemon for information about itself
 * 
 * @param devname The daemon device name
 */
void ask_daemon(char *devname)
{
	devserver 	ds;
	long 		error;
	DevInfo 	*dev_info = NULL;
	char 		pers_name[40];
	char 		*tmp;

/* First import the device */
	if (dev_import(devname,0,&ds,&error) != DS_OK)
	{
		fprintf(stderr,"dc_devinfo : Error %d during dev_import\n",error);
		exit(-1);
	}

/* Ask server for info on device */
	if (dev_inform(&ds,1,&dev_info,&error) != DS_OK)
	{
		fprintf(stderr,"dc_devinfo : Error %d during dev_inform\n",error);
		exit(-1);
	}

/* Display information */

	tmp = (char *)strchr(dev_info->server_name,'/');
	tmp++;
	strcpy(pers_name,tmp);
	printf("\nDevice polled by a ud_daemon running on : %s\n",dev_info->server_host);
	printf("Daemon started with the personal name : %s\n",pers_name);

/* Free the device */
	dev_free(ds,&error);
	return;
}
