/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>

long Db_getdsonhost(char *, long *, db_svc **, long *);

typedef	struct _Device
{
	std::string	domain;
	std::string	family;
	std::string	member;
	bool		exported;
} Tdevice;

typedef struct _server
{
	std::string		name;
	std::string		personal_name;
	pid_t			pid;
	unsigned long		program_number;
	std::vector<Tdevice>	devices;
}Tserver;

typedef struct _host
{
	std::string		name;
	std::vector<Tserver>	servers;
}Thost;

typedef std::vector<Thost>	Tnethost;

/****************************************************************************
*                                                                           
*    Code for db_listservers
*                                                                           
*    To get a list of device servers from a special nethost.
*                                                                           
*    db_listservers NETHOST
*                                                                           
****************************************************************************/
int main(int argc, char **argv)
{
	Tnethost	nethost;
	long 		error;
//
// Argument test and device name structure
//
	if (argc != 1)
	{
		std::cerr << "usage : " << *argv << std::endl;
		exit(-1);
	}
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << *argv << " : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Ask database server to delete resource
// Display error message if the call fails
//
	long n_host;
	char **host_list; 
	if (db_gethostlist(&n_host, &host_list, &error) == DS_NOTOK)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
	}
	for (int i = 0; i < n_host; i++)
	{
		Thost	host;
		host.name = host_list[i];
		nethost.push_back(host);
	}
	db_freedevexp(host_list);

	long	n_server;
	char	**server_names;
	if (db_getdsserverlist(&n_server, &server_names, &error) == DS_OK)
	{
		for (int i = 0; i < n_server; ++i)
		{
			long	n_pers;
			char	**pers_names;
			if (db_getdspersnamelist(server_names[i], &n_pers, &pers_names, &error) == DS_OK)
			{
				for (int j = 0; j < n_pers; ++j)
				{
					db_svcinfo_call	info;
					if (db_servinfo(server_names[i], pers_names[j], &info, &error) == DS_OK)
					{
						Tserver	server;
						server.name = server_names[i];
						server.personal_name = pers_names[j];
						server.pid = info.pid;
						server.program_number = info.program_num;

						Tnethost::iterator it;
						for (it = nethost.begin(); it != nethost.end(); ++it)
							if (it->name == std::string(info.host_name))
								break;
						if (it == nethost.end())
						{
							Thost host;
							host.name = info.host_name;
							it = nethost.insert(it, host);
						}

						for (int k = 0; k < info.embedded_server_nb; ++k)
						{
							for (int l = 0; l < info.server[k].device_nb; ++l)
							{	
								Tdevice	device;
								device.exported = info.server[k].device[l].exported_flag == 1;
								std::string	dev_name(info.server[k].device[l].dev_name);
								std::string::size_type	pos = dev_name.find('/');

								device.domain = dev_name.substr(0, pos);
								dev_name.erase(0, pos + 1);
								pos = dev_name.find('/');
								device.family = dev_name.substr(0, pos);
								device.member = dev_name.substr(pos + 1);
								server.devices.push_back(device);
							}
						}
						it->servers.push_back(server);
					}
				}
				db_freedevexp(pers_names);
			}
		}
		db_freedevexp(server_names);
	}
	for (Tnethost::iterator it = nethost.begin(); it != nethost.end(); ++it)
	{
		std::cout << it->name << std::endl;
		for (std::vector<Tserver>::iterator s = it->servers.begin(); s != it->servers.end(); ++s)
		{
			std::cout << "\t" << s->name + "/" + s->personal_name << " : " << s->pid << std::endl;
			for (std::vector<Tdevice>::iterator d = s->devices.begin(); d != s->devices.end(); ++d)
				std::cout << "\t\t" << d->domain + "/" + d->family + "/" + d->member << 
					(d->exported ? " : exported" : " : not exported") << std::endl;
		}
	}
#if 0		
		long	n_ds;
		db_svc	*ds;
		if (Db_getdsonhost(host_list[i], &n_ds, &ds,&error) == DS_OK)
		{
			for (int j = 0; j < n_ds; ++j)
			{
				std::string server_name(ds[j].server_name);
				server_name += '/';
				server_name += ds[j].personal_name;
				std::cout << "    " << server_name << std::endl;
				u_int n_dev;
				char **device_names;
				if (db_getdevlist(const_cast<char *>(server_name.c_str()), &device_names, &n_dev, &error) == DS_OK)
				{
					for (int k = 0; k < n_dev; ++k)
						std::cout << "              " << device_names[k] << std::endl;
				}
			}
			free(ds);
		}
	}
#endif
	return 0;
}


long Db_getdsonhost(char *host, long *n_ds, db_svc **ds, long *perror)
{
	long 	n_server;
	char	**server_names;

	*n_ds = 0;
	*ds = NULL;
	if (db_getdsserverlist(&n_server, &server_names, perror) == DS_OK)
	{
		for (int i = 0; i < n_server; ++i)
		{
			long	n_pers;
			char 	**pers_names;
			if (db_getdspersnamelist(server_names[i], &n_pers, &pers_names, perror) == DS_OK)
			{
				for (int j = 0; j < n_pers; ++j)
				{
					db_svcinfo_call	info;
					if (db_servinfo(server_names[i], pers_names[j], &info, perror) == DS_OK)
					{
						if (!strcmp(host, info.host_name))
						{
							int n = *n_ds;
							db_svc	help;
							if (!*ds)
								*ds = (db_svc *)malloc(sizeof(db_svc));
							else
								*ds = (db_svc *)realloc(*ds, sizeof(db_svc) * (1 + n));
							help.pid = info.pid;
							help.program_num = info.program_num;
							strncpy(help.server_name, server_names[i], sizeof(help.server_name) - 1);
							help.server_name[sizeof(help.server_name) - 1] = '\0'; 
							strncpy(help.personal_name, pers_names[j], sizeof(help.personal_name) - 1);
							help.personal_name[sizeof(help.personal_name) - 1] = '\0'; 
							strncpy(help.host_name, host, sizeof(help.host_name) - 1);
							help.host_name[sizeof(help.host_name) - 1] = '\0'; 
							memcpy(*ds + n, &help, sizeof(help));
							(*n_ds)++;	
						}
					}
				}
				db_freedevexp(pers_names);
			}
		}	
		db_freedevexp(server_names);
		return DS_OK;
	}
	return DS_NOTOK;
}
