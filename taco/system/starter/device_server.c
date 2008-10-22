#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#include <macros.h>
#include <db_setup.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <DevSignal.h>
#ifdef HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif

/**
 * @ingroup dsAPIintern
 * Checks wether a device server with the same name is already running.
 *
 * @param error Will contain an appropriate error code if the corresponding
 *              call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long svc_check (DevLong *error)
{
        CLIENT          *clnt;
        enum clnt_stat  clnt_stat;
        char            *host_name;
        char            *svc_name = NULL;
        unsigned int    prog_number;
        unsigned int    vers_number;

        *error = 0;
        if (db_svc_check(config_flags->server_name, &host_name, &prog_number, &vers_number, error) == DS_OK)
	{
/*
 * old server already unmapped ?
 */
        	if (prog_number != 0)
		{
/*
 * force version number to latest API_VERSION [4] , new servers do not
 * support the old DSERVER_VERSION [1] anymore
 */
			vers_number = API_VERSION;

/*
 *  old server still exists ?
 */
			clnt = clnt_create (host_name, prog_number,vers_number, "udp");
			if (clnt != NULL)
			{
				clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &msg_retry_timeout);
				clnt_control (clnt, CLSET_TIMEOUT, (char *) &msg_timeout);
/*
 *  call device server check function
 */
				clnt_stat = clnt_call (clnt, RPC_CHECK, (xdrproc_t)xdr_void, NULL,
                                  	(xdrproc_t)xdr_wrapstring, (caddr_t) &svc_name, TIMEVAL(msg_timeout));
				if (clnt_stat == RPC_SUCCESS)
				{
					if (strcmp (config_flags->server_name, svc_name) == 0)
					{
						*error = DevErr_ServerAlreadyExists;
						clnt_destroy (clnt);
						return (DS_NOTOK);
					}
				}
				else if (clnt_stat != RPC_PROCUNAVAIL)
				{
					/*pmap_unset (prog_number, DEVSERVER_VERS);*/
					pmap_unset (prog_number, API_VERSION);
					pmap_unset (prog_number, ASYNCH_API_VERSION);
				}
				clnt_destroy (clnt);
			}
		}
	}
	else
	{
		char resource_line[256];
		long	nb_devdef,
			nb_resdef,
			error_line,
			dev_err;
		char 	**resdef,
			**devdef;
		char *host = strchr(config_flags->server_name, '/') + 1;
		snprintf(resource_line, sizeof(resource_line), "%s/device: sys/start/%s", config_flags->server_name, host);
		printf("RESOURCE LINE : %s\n", resource_line);
		if (db_analyze_data(Db_Buffer, resource_line, &nb_devdef, &devdef, &nb_resdef, &resdef, &error_line, error) != DS_OK)
			return DS_NOTOK;
                if (db_upddev(nb_devdef, devdef, &dev_err, error) == DS_NOTOK)
                {
                        if (dev_err != 0)
                                fprintf(stderr, "Error in device definition number %ld, error = %d\n", dev_err, *error);
                        else
                                fprintf(stderr, "Device(s) update call failed with error %ld\n", *error);
                        fprintf(stderr, "Error message : %s\n", dev_error_str(*error)); 
                        return DS_NOTOK;
                }
	}
        return (DS_OK);
}

long db_check(DevLong *error)
{
	while (db_import(error) == DS_NOTOK)
	{
		dev_printerror_no (SEND,"db_import failed", *error);
		sleep(1);
	}
	return DS_OK;
}

