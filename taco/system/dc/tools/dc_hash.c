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
 * File         : dc_hash.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author       :
 *                $Author: jkrueger1 $
 *
 * Original     :
 *
 * Version      : $Revision: 1.6 $
 *
 * Date         : $Date: 2008-04-06 09:07:51 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <dcP.h>
#include <string.h>
#include <stdlib.h>

void usage(const char *cmd)
{
	fprintf(stderr, "usage : %s [options] <device name>\n", cmd);
	fprintf(stderr, "  Remove the data collector system from memory.\n");
	fprintf(stderr, "  The data collector system could be distributed on\n");
	fprintf(stderr, "  several hosts. This command will retrieve on which host\n");
	fprintf(stderr, "  this system is running (with resources) and will run a\n");
	fprintf(stderr, "  local command (with remote shell) on each host\n");
	fprintf(stderr, "         options: -h display this message\n");
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for dc_hash command                                    *
*                        -------                                            *
*                                                                           *
*    Command rule : To remove the data collector system from memory  	    *
*		    The data collector system could be distributed on       *
*		    several hosts. This command will retrieve on which host *
*		    this system is running (with resources) and will run a  *
*		    local command (with remote shell) on each host          *
*                                                                           *
*    Synopsis : dc_hash <device name>					    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	DevLong		error;
	static int 	dev_num;
	char 		hostna[HOST_NAME_LENGTH],
			dev_name[DEV_NAME_LENGTH],
			devname[DEV_NAME_LENGTH];
	char 		*tmp;
	int 		i,
			l;
	int 		ind;
	unsigned int 	diff;
	db_resource 	res1[] = {
				{"dev_number",D_LONG_TYPE, &dev_num},
		     	};
	int 		res1_size = sizeof(res1) / sizeof(db_resource);

	int		c;
        extern int      optind,
                        optopt;

/* Argument test */
       while ((c = getopt(argc, argv, "h")) != -1)
                switch(c)
                {
                        case 'h' :
                        case '?' :
                                usage(argv[0]);
                }
        if (optind != argc - 1)
                usage(argv[0]);

/* Check device name validity */
	l = strlen(argv[optind]);
	for (i=0; i < l && i < sizeof(devname) - 1; i++)
		devname[i] = tolower(argv[optind][i]);
	devname[l] = 0;

	i = 0;
	NB_CHAR(i,argv[optind],'/');
	if (i != 2) 
	{
		fprintf(stderr,"%s : Bad device name\n",argv[0]);
		exit(-1);
	}

/* Import static database */
	if (db_import(&error)) 
	{
		fprintf(stderr,"dc_hash : Can't import static database\n");
		fprintf(stderr,"dc_hash : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Build device name. It is a function of the host name */
	gethostname(hostna, sizeof(hostna));
	if ((tmp = strchr(hostna,'.')) != NULL) 
	{
		diff = (u_int)(tmp - hostna);
		hostna[diff] = 0;
	}
	strcpy(dev_name,"CLASS/DC/");
	strcat(dev_name,hostna);

/* Retrieve data collector hashing table size */
	if (db_getresource(dev_name,res1,res1_size,&error)) 
	{
		fprintf(stderr, "dc_hash : Can't retrieve resources\n");
		fprintf(stderr, "dc_hash : Error code : %d\n",error);
		exit(-1);
	}

/* Do the hashing */
	ind = hash(devname,dev_num);
	printf("Hashing function result : %d\n",ind);
	printf("Offset (hexa) in the ptrs area to device record : %x\n",ind * sizeof(dc_dev_param));
	return 0;
}
