#include <API.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


/****************************************************************************
*                                                                           *
*		Code for dc_mfree command                                   *
*                        --------                                           *
*                                                                           *
*    Command rule : To display information about the data collector data    *
*		    buffer.						    *
*		    The data collector system could be distributed on       *
*		    several hosts. This command will retrieve on which host *
*		    this system is running (with resources) and will run a  *
*		    local command (with remote shell) on ecah host          *
*                                                                           *
*    Synopsis : dc_mfree		    				    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	long 			error;
	char 			*tmp,
				*rsh,
				*net;
	unsigned int 		diff;
	char 			hostna[32],
				dev_name[40],
				psd_name[80],
				cmd[160];
	int 			i;
	DevVarStringArray 	host_dc = {0, NULL};
	DevString 		dc_path,
				s_dc_path,
				dc_login;
	db_resource 		res1[] = {
					{"host",D_VAR_STRINGARR,&host_dc},
			     	},
				res2[] = {
					{"path",D_STRING_TYPE,&dc_path},
					{"s_path",D_STRING_TYPE,&s_dc_path},
					{"login",D_STRING_TYPE,&dc_login},
				};
	int 			res1_size = sizeof(res1) / sizeof(db_resource);
	int 			res2_size = sizeof(res2) / sizeof(db_resource);

/* Argument test */
	if (argc != 1) 
	{
		fprintf(stderr,"dc_mfree usage : dc_mfree\n");
		exit(-1);
	}

/* Import static database */
	if (db_import(&error)) 
	{
		fprintf(stderr,"dc_mfree : Can't import static database\n");
		fprintf(stderr,"dc_mfree : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Retrieve on which host the data collector is running */
	if (db_getresource("CLASS/DC/1",res1,res1_size,&error)) 
	{
		fprintf(stderr,"dc_mfree : Can't retrieve resources\n");
		fprintf(stderr,"dc_mfree : Error code : %d\n",error);
		exit(-1);
	}
	if (host_dc.length == 0) 
	{
		fprintf(stderr,"dc_mfree : Resource host not defined\n");
		fprintf(stderr,"dc_mfree : Sorry, but I exit !\n");
		exit(-1);
	}

/* Get the NETHOST environment variable (No need to test if NETHOST is
   defined because this has been done during the static database import
   stuff */

	net = getenv("NETHOST");
	rsh = getenv("TACO_RSH");
	if (rsh == NULL)
#if defined(sun) || defined(linux)
		rsh = "rsh";
#else
		rsh = "remsh";
#endif /* sun || linux */

/* Call the dc_dels command on each dc host */
	for (i = 0;i < host_dc.length;i++) 
	{
/* Retrieve from database the primary path for dc tools */
		psd_name[0] = 0;
		strcpy(psd_name,"class/dc/");
		strcat(psd_name,host_dc.sequence[i]);

		dc_path = NULL;
		s_dc_path = NULL;
		dc_login = NULL;

		if(db_getresource(psd_name,res2,res2_size,&error)) 
		{
			fprintf(stderr,"dc_mfree : Can't retrieve primary dc tools path for %s\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_mfree : Error code : %d\n",error);
			exit(-1);
		}

		if (dc_path == NULL || s_dc_path == NULL || dc_login == NULL) 
		{
			fprintf(stderr,"dc_mfree : Path or login for dc_tools on %s not defined\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_mfree : Sorry, but I exit\n");
			exit(-1);
		}

/* Build the string which is the remsh command */
		snprintf(cmd, sizeof(cmd), 
			"%s %s -l %s \"echo \\\"NETHOST=%s;export NETHOST; %s/%s/dc_mfrees \\\" | sh \"", 
			rsh,host_dc.sequence[i], dc_login, net, dc_path, s_dc_path);

#ifdef DEBUG
		printf("Command string : %s\n",cmd);
#endif /* DEBUG */

		printf("Data collector data buffer status on %s \n\n", host_dc.sequence[i]);
		if(system(cmd))
			printf("failed\n");
	}
	return 0;
}
