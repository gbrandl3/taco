#include "config.h"
#include <API.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netdb.h>

#define	DEFAULT_TIMEOUT		9
#define	DEFAULT_MAX_CALL	1000
#define DEFAULT_READ_SVC_NB	2
#define DEFAULT_WRITE_SVC_NB	2
#define DEFAULT_DEV_NUMBER	100
#define	DEFAULT_CELLAR_NUMBER	50
#define DEFAULT_DATA_SIZE	512 * 1024
#define DEFAULT_PASSWORD	"setup"
#define	DEFAULT_LOGIN		"blissadm"

char *gethostname_wo_dot(int length);

/****************************************************************************
*                                                                           *
*		Code for dc_init command                                    *
*                        -------                                            *
*                                                                           *
*    Command rule : To initialise the data collector and to 		    *
*		    create the shared memories segments used by the dc.     *
*		    The data collector system could be distributed on       *
*		    several hosts. This command will retrieve on which host *
*		    this system is running (with resources) and will run a  *
*		    local command (with remote shell) on each host          *
*                                                                           *
*    Synopsis : dc_init		 		   			    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	DevVarStringArray 	host_dc = {0, NULL};
	int 			ds;
	long 			error;
	char 			*tmp,
				*net;
	DevString		dc_path,
				s_dc_path,
				dc_login;
	unsigned int 		diff;
	char 			hostna[HOST_NAME_LENGTH],
				dev_name[DEV_NAME_LENGTH],
				psd_name[PROC_NAME_LENGTH],
				cmd[160],
				*rsh;
	int 			i,
				resu;
	struct stat 		fista;
	db_resource 		res1[] = {
					{"host", D_VAR_STRINGARR, &host_dc},
				},
				res2[] = {
					{"path", D_STRING_TYPE, &dc_path},
					{"s_path", D_STRING_TYPE, &s_dc_path},
					{"login", D_STRING_TYPE, &dc_login},
		     		};
	int 			res2_size = sizeof(res2) / sizeof(db_resource),
				res1_size = sizeof(res1) / sizeof(db_resource);

/* Arguments number test */
	if(argc != 1)
	{
		fprintf(stderr,"dc_init usage : dc_init\n");
		exit(-1);
	}

/* Import static database */
	if (db_import(&error))
	{
		fprintf(stderr,"dc_init : Can't import static database\n");
		fprintf(stderr,"dc_init : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Create default resource if necessary */
	if (dc_res_management(&error))
	{
		fprintf(stderr,"dc_init: Can't correctly manage dc resources\n");
		fprintf(stderr,"dc_init: Error code = %d\n",error);
		exit(-1);
	}
	
	if (dc_res_serv_nb(&error))
	{
		fprintf(stderr,"dc_init: Can't correctly manage server_nb dc resources\n");
		fprintf(stderr,"dc_init: Error code = %d\n",error);
		exit(-1);
	}

	if (dc_res_request(&error))
	{
		fprintf(stderr,"dc_init: Can't correctly manage server request dc resources\n");
		fprintf(stderr,"dc_init: Error code = %d\n",error);
		exit(-1);
	}
	
	if (dc_class_dc(&error))
	{
		fprintf(stderr,"dc_init: Can't correctly manage class/dc/<host> dc resources\n");
		fprintf(stderr,"dc_init: Error code = %d\n",error);
		exit(-1);
	}
			
/* Retrieve on which host the data collector is running */

	host_dc.length = 0;
	if (db_getresource("CLASS/DC/1",res1,res1_size,&error))
	{
		fprintf(stderr,"dc_init : Can't retrieve resources\n");
		fprintf(stderr,"dc_init : Error code : %d\n",error);
		exit(-1);
	}
	if (host_dc.length == 0)
	{
		fprintf(stderr,"dc_init : Resource host not defined\n");
		fprintf(stderr,"dc_init : Sorry, but I exit !\n");
		exit(-1);
	}

/* Get the NETHOST environment variable (No need to test if NETHOST is
   defined because this has been done during the static database import 
   stuff */

	net = getenv("NETHOST");
	rsh = getenv("TACO_RSH");
	if (rsh == NULL)
	{
		
#if defined(sun) || defined(linux)
		rsh = "rsh";
#else
		rsh = "remsh";
#endif /* sun */
	}

/* Call the dc_inits command on each dc host */

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
			fprintf(stderr,"dc_init : Can't retrieve primary dc tools path for %s\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_init : Error code : %d\n",error);
			exit(-1);
		}

		if (dc_path == NULL || s_dc_path == NULL || dc_login == NULL)
		{
			fprintf(stderr,"dc_init : Path or login for dc_tools on %s not defined\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_init : Sorry, but I exit\n");
			exit(-1);
		}

/* Build the string which is the remsh command */
		snprintf(cmd, sizeof(cmd), 
			"%s %s -l %s \" echo \\\"NETHOST=%s;export NETHOST;%s/%s/dc_inits \\\" | sh \" | tee /usr/tmp/dcinitsta", 
			rsh, host_dc.sequence[i], dc_login, net, dc_path, s_dc_path);
#ifdef DEBUG
		fprintf(stderr, "Command string : %s\n",cmd);
#endif /* DEBUG */

		fprintf(stderr, "Executing dc_inits on %s \n", host_dc.sequence[i]);
		if (system(cmd))
			fprintf(stderr, "failed\n");

/* Any error during local dc_init command ? */

		stat("/usr/tmp/dcinitsta", &fista);
		unlink("/usr/tmp/dcinitsta");
		if (fista.st_size != 0)
			break;
	}
	return 0;
}

int dc_res_management(long *error_ptr)
{
	DevLong 		m_call = 0,
				to = 1000;
	DevVarStringArray 	host_dc = {0, NULL};
	int 			nb_put_class_res = 0;
	char			hostna[HOST_NAME_LENGTH],
				hostna_wo_dot[HOST_NAME_LENGTH],
				tmp_host_name[HOST_NAME_LENGTH],
	 			*net_def = NULL,
	 			res_name[32],
				*def,
				*tmp;
	db_resource 		put_class_res[4],
	 			net_res;
	unsigned char 		ho = 0,
				net = 0;
	struct hostent 		*host;
	db_resource 		res_class_def[] = {
					{"host", D_VAR_STRINGARR, &host_dc},
					{"max_call", D_LONG_TYPE, &m_call},
					{"timeout", D_LONG_TYPE, &to},
				     };
	int 			res_class_def_size = sizeof(res_class_def) / sizeof(db_resource);
	unsigned int diff;
		
/* First, check class resource definition */
	if (db_getresource("class/dc/1",res_class_def,res_class_def_size,error_ptr) == -1)
	{
		fprintf(stderr,"dc_init: Can't retrieve class/dc/1 resources\n");
		return -1;
	}

	taco_gethostname(hostna, sizeof(hostna));
	if ((host = gethostbyname(hostna)) == (struct hostent *)NULL)
	{
		fprintf(stderr,"dc_init: Can't retrieve host network address\n");
		return -1;
	}
	
	ho = (unsigned char)(host->h_addr_list[0][3]);
	net = (unsigned char)(host->h_addr_list[0][2]);
	
	snprintf(res_name, sizeof(res_name), "%u_default", net);
	net_res.resource_name = res_name;
	net_res.resource_type = D_STRING_TYPE;
	net_res.resource_adr = &net_def;
	
	if (db_getresource("class/dc/1",&net_res,(unsigned int)1,error_ptr) == -1)
	{
		fprintf(stderr,"dc_init: Can't retrieve class/dc/1 resources\n");
		return -1;
	}	

	tmp = strchr(hostna, '.');
	if (tmp != NULL)
	{
		diff = (unsigned int)(tmp - hostna);
		strncpy(hostna_wo_dot,hostna,diff);
		hostna_wo_dot[diff] = '\0';
	}
	else
		strcpy(hostna_wo_dot,hostna);

/* Check for each variable if it is necessary to insert one in db */
	if (host_dc.length == 0)
	{
	
/* Set a default value to one host which is the host where this command
   is running. Remove domain name from its name */
		host_dc.length = 1;
		host_dc.sequence = malloc(sizeof(char *));
		tmp = &hostna_wo_dot[0];
		host_dc.sequence[0] = tmp;
		put_class_res[nb_put_class_res].resource_name = "host";
		put_class_res[nb_put_class_res].resource_type = D_VAR_STRINGARR;
		put_class_res[nb_put_class_res].resource_adr = &host_dc;		
		nb_put_class_res++;
	}

	if (m_call == 0)
	{
/* Set a default value for the max_call resource */
		m_call = DEFAULT_MAX_CALL;
		put_class_res[nb_put_class_res].resource_name = "max_call";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &m_call;
		nb_put_class_res++;
	}

	if (to == 1000)
	{
/* Set a default value for timeout */
		to = DEFAULT_TIMEOUT;
		put_class_res[nb_put_class_res].resource_name = "timeout";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &to;
		nb_put_class_res++;
	}
	
	if (net_def == NULL)
	{
/* Set a default value for the default dc host based on the net number of the
   IP address of the computer on which this command is executed */
		net_def = &hostna_wo_dot[0];
		put_class_res[nb_put_class_res].resource_name = res_name;
		put_class_res[nb_put_class_res].resource_type = D_STRING_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &net_def;	
		nb_put_class_res++;
	}
	fprintf(stderr, "%d %s\n", nb_put_class_res, hostna_wo_dot);
	
/* Update db if necessary */
	if (nb_put_class_res != 0)
	{
		if (db_putresource("class/dc/1", put_class_res, nb_put_class_res, error_ptr))
		{
			fprintf(stderr,"dc_init: Error while trying to put default class resource\n");
			return -1;
		}
	}
	return 0;
}

int dc_res_serv_nb(long *error_ptr)
{
	DevLong 	rd_nb = 1000,
			wr_nb = 1000;
	int 		nb_put_class_res = 0;
	db_resource 	put_class_res[2],
			net_res[2];
	char 		hostna[HOST_NAME_LENGTH],
			hostna_wo_dot[HOST_NAME_LENGTH];
	unsigned int 	diff;
	char 		*tmp,
			res_name_rd[64],
			res_name_wr[64];
		
/* First, check if resources are defined */

//	rd_nb = wr_nb = 1000;
	
	gethostname(hostna,sizeof(hostna));
	tmp = strchr(hostna,'.');
	if (tmp != NULL)
	{
		diff = (unsigned int)(tmp - hostna);
		strncpy(hostna_wo_dot,hostna,diff);
		hostna_wo_dot[diff] = '\0';
	}
	else
		strcpy(hostna_wo_dot,hostna);

	strcpy(res_name_rd, hostna_wo_dot);
	strcat(res_name_rd, "_rd");
	
	strcpy(res_name_wr, hostna_wo_dot);
	strcat(res_name_wr, "_wr");
				
	net_res[0].resource_name = res_name_rd;
	net_res[0].resource_type = D_LONG_TYPE;
	net_res[0].resource_adr = &rd_nb;

	net_res[1].resource_name = res_name_wr;	
	net_res[1].resource_type = D_LONG_TYPE;
	net_res[1].resource_adr = &wr_nb;
	
	if (db_getresource("class/dc/server_nb",net_res,2,error_ptr) == -1)
	{
		fprintf(stderr,"dc_init: Can't retrieve class/dc/server_nb resources\n");
		return -1;
	}
	
	if (rd_nb == 1000)
	{
/* Set a default value for read server numbers */

		rd_nb = DEFAULT_READ_SVC_NB;
		put_class_res[nb_put_class_res].resource_name = res_name_rd;
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &rd_nb;
		nb_put_class_res++;
	}
	
	if (wr_nb == 1000)
	{
	
/* Set a default value for read server numbers */

		wr_nb = DEFAULT_WRITE_SVC_NB;
		put_class_res[nb_put_class_res].resource_name = res_name_wr;
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &wr_nb;
		nb_put_class_res++;
	}
	
/* Update db if necessary */

	if (nb_put_class_res != 0)
	{
		if (db_putresource("class/dc/server_nb",put_class_res,nb_put_class_res,error_ptr))
		{
			fprintf(stderr,"dc_init: Error while trying to put default server_nb resource\n");
			return -1;
		}
	}
	
	return 0;
}

int dc_res_request(long *error_ptr)
{
	DevLong		req1 = 1234,
			req2 = 1234,
			req3 = 1234,
			req4 = 1234;
	int 		nb_put_class_res = 0;
	db_resource 	put_class_res[4],
			net_res[4];
	char 		hostna[HOST_NAME_LENGTH],
			dev_name[DEV_NAME_LENGTH];
	unsigned char 	ho = 0,
			net = 0;
	struct hostent *host;
		
/* First, check if resources are defined */
	gethostname(hostna,sizeof(hostna));
	if ((host = gethostbyname(hostna)) == (struct hostent *)NULL)
	{
		fprintf(stderr,"dc_init: Can't retrieve host network address\n");
		return -1;
	}
	
	ho = (unsigned char)(host->h_addr_list[0][3]);
	net = (unsigned char)(host->h_addr_list[0][2]);

	snprintf(dev_name, sizeof(dev_name), "sys/dc_wr_%u/request", ho);
				
	net_res[0].resource_name = "1";
	net_res[0].resource_type = D_LONG_TYPE;
	net_res[0].resource_adr = &req1;

	net_res[1].resource_name = "2";	
	net_res[1].resource_type = D_LONG_TYPE;
	net_res[1].resource_adr = &req2;

	net_res[2].resource_name = "3";	
	net_res[2].resource_type = D_LONG_TYPE;
	net_res[2].resource_adr = &req3;
	
	net_res[3].resource_name = "4";	
	net_res[3].resource_type = D_LONG_TYPE;
	net_res[3].resource_adr = &req4;
		
	if (db_getresource(dev_name, net_res, 4, error_ptr) == -1)
	{
		fprintf(stderr,"dc_init: Can't retrieve wr server request nb resources\n");
		return -1;
	}
	
	if (req1 == 1234)
	{
/* Set a default value for read server numbers */
		req1 = 0;
		put_class_res[nb_put_class_res].resource_name = "1";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req1;
		nb_put_class_res++;
	}
	
	if (req2 == 1234)
	{
/* Set a default value for read server numbers */
		req2 = 0;
		put_class_res[nb_put_class_res].resource_name = "2";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req2;
		nb_put_class_res++;
	}

	if (req3 == 1234)
	{
/* Set a default value for read server numbers */
		req3 = 0;
		put_class_res[nb_put_class_res].resource_name = "3";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req3;
		nb_put_class_res++;
	}
	
	if (req4 == 1234)
	{
/* Set a default value for read server numbers */
		req4 = 0;
		put_class_res[nb_put_class_res].resource_name = "4";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req4;
		nb_put_class_res++;
	}
		
/* Update db if necessary */
	if (nb_put_class_res != 0)
	{
		if (db_putresource(dev_name,put_class_res,nb_put_class_res,error_ptr))
		{
			fprintf(stderr,"dc_init: Error while trying to put default wr server request resource\n");
			return -1;
		}
	}

/* Now, do the same for rd servers */
	nb_put_class_res = 0;
	req1 = req2 = req3 = req4 = 1234;	
	snprintf(dev_name, sizeof(dev_name), "sys/dc_rd_%u/request", ho);
				
	net_res[0].resource_name = "1";
	net_res[0].resource_type = D_LONG_TYPE;
	net_res[0].resource_adr = &req1;

	net_res[1].resource_name = "2";	
	net_res[1].resource_type = D_LONG_TYPE;
	net_res[1].resource_adr = &req2;

	net_res[2].resource_name = "3";	
	net_res[2].resource_type = D_LONG_TYPE;
	net_res[2].resource_adr = &req3;
	
	net_res[3].resource_name = "3";	
	net_res[3].resource_type = D_LONG_TYPE;
	net_res[3].resource_adr = &req4;
		
	if (db_getresource(dev_name,net_res,4,error_ptr) == -1)
	{
		fprintf(stderr,"dc_init: Can't retrieve wr server request nb resources\n");
		return -1;
	}
	
	if (req1 == 1234)
	{
/* Set a default value for read server numbers */
		req1 = 0;
		put_class_res[nb_put_class_res].resource_name = "1";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req1;
		nb_put_class_res++;
	}
	
	if (req2 == 1234)
	{
/* Set a default value for read server numbers */
		req2 = 0;
		put_class_res[nb_put_class_res].resource_name = "2";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req2;
		nb_put_class_res++;
	}

	if (req3 == 1234)
	{
/* Set a default value for read server numbers */
		req3 = 0;
		put_class_res[nb_put_class_res].resource_name = "3";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req3;
		nb_put_class_res++;
	}
	
	if (req4 == 1234)
	{
/* Set a default value for read server numbers */
		req4 = 0;
		put_class_res[nb_put_class_res].resource_name = "4";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &req4;
		nb_put_class_res++;
	}
		
/* Update db if necessary */
	if (nb_put_class_res != 0)
	{
		if (db_putresource(dev_name,put_class_res,nb_put_class_res,error_ptr))
		{
			fprintf(stderr,"dc_init: Error while trying to put default wr server request resource\n");
			return -1;
		}
	}
	
	return 0;
}

int dc_class_dc(long *error_ptr)
{
	DevLong 	dev_number = 1234,
			cellar_number = 1234,
			data_size = 1234;
	db_resource 	put_class_res[7];
	int 		nb_put_class_res = 0;
	char 		hostna[HOST_NAME_LENGTH],
			hostna_wo_dot[HOST_NAME_LENGTH];
	unsigned int 	diff;
	char 		*tmp,
			*taco_path;
	DevString	pass = NULL,
			path = NULL,
			s_path = NULL,
			login = NULL;
	char 		tmp_path[128],
			dev_name[64],
			tmp_spath[128];
	DevString	pa,
			spa;
	db_resource 	res_class_host[] = {
					{"dev_number",D_LONG_TYPE, &dev_number},
					{"cellar_number",D_LONG_TYPE, &cellar_number},
					{"data_size",D_LONG_TYPE, &data_size},
					{"password",D_STRING_TYPE, &pass},
					{"path",D_STRING_TYPE, &path},
					{"s_path",D_STRING_TYPE, &s_path},
					{"login",D_STRING_TYPE, &login}
		     	};
	int 		res_class_host_size = sizeof(res_class_host) / sizeof(db_resource);

/* Get host name without domain name */
	
	gethostname(hostna, sizeof(hostna));
	tmp = strchr(hostna, '.');
	if (tmp != NULL)
	{
		diff = (unsigned int)(tmp - hostna);
		strncpy(hostna_wo_dot, hostna,diff);
		hostna_wo_dot[diff] = '\0';
	}
	else
		strcpy(hostna_wo_dot,hostna);
		
/* Try to get resource */
	
	strcpy(dev_name, "class/dc/");
	strcat(dev_name, hostna_wo_dot);
	
	if (db_getresource(dev_name,res_class_host,res_class_host_size,error_ptr) == -1)
	{
		fprintf(stderr,"dc_init: Can't retrieve class/dc/<host> resources\n");
		return -1;
	}

	if (dev_number == 1234)
	{
/* Set a default value for dev_number */
		dev_number = DEFAULT_DEV_NUMBER;
		put_class_res[nb_put_class_res].resource_name = "dev_number";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &dev_number;
		nb_put_class_res++;
	}

	if (cellar_number == 1234)
	{
/* Set a default value for dev_number */
		cellar_number = DEFAULT_CELLAR_NUMBER;
		put_class_res[nb_put_class_res].resource_name = "cellar_number";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &cellar_number;
		nb_put_class_res++;
	}
	
	if (data_size == 1234)
	{
/* Set a default value for dev_number */
		data_size = DEFAULT_DATA_SIZE;
		put_class_res[nb_put_class_res].resource_name = "data_size";
		put_class_res[nb_put_class_res].resource_type = D_LONG_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &data_size;
		nb_put_class_res++;
	}
	
	if (pass == NULL)
	{
/* Set a default value for dev_number */
		pass = DEFAULT_PASSWORD;
		put_class_res[nb_put_class_res].resource_name = "password";
		put_class_res[nb_put_class_res].resource_type = D_STRING_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &pass;
		nb_put_class_res++;
	}
	
	if (login == NULL)
	{
/* Set a default value for dev_number */
		login = DEFAULT_LOGIN;
		put_class_res[nb_put_class_res].resource_name = "login";
		put_class_res[nb_put_class_res].resource_type = D_STRING_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &login;
		nb_put_class_res++;
	}
	
	if (path == NULL)
	{
		taco_path = getenv("TACO_PATH");
		if (taco_path == NULL)
		{
			fprintf(stderr,"dc_init: Environment variable TACO_PATH not set !\n");
			fprintf(stderr,"dc_init: Exiting\n");
			exit(-1);
		}
		tmp = strrchr(taco_path,'/');
		diff = (unsigned int)(tmp - taco_path);
		strncpy(tmp_path,taco_path,diff);
		tmp_path[diff] = '\0';

		tmp++;
		diff = strlen(taco_path) - strlen(tmp_path);		
		strncpy(tmp_spath,tmp,diff);
		tmp_spath[diff] = '\0';

		pa = tmp_path;		
		put_class_res[nb_put_class_res].resource_name = "path";
		put_class_res[nb_put_class_res].resource_type = D_STRING_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &pa;
		nb_put_class_res++;

		spa = tmp_spath;		
		put_class_res[nb_put_class_res].resource_name = "s_path";
		put_class_res[nb_put_class_res].resource_type = D_STRING_TYPE;
		put_class_res[nb_put_class_res].resource_adr = &spa;
		nb_put_class_res++;
	}
	
/* Update db if necessary */
	if (nb_put_class_res != 0)
	{
		if (db_putresource(dev_name, put_class_res, nb_put_class_res, error_ptr))
		{
			fprintf(stderr,"dc_init: Error while trying to put default class/dc/<host> resource\n");
			return -1;
		}
	}
	
	return 0;	
}
