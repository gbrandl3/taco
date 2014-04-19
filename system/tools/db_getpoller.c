/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File         : db_getpoller.c
 *
 * Project      : TACO tools
 *
 * Description  :
 *
 *
 * Author       : 
 *                $Author: jkrueger1 $
 *
 * Original     :
 *
 * Version      : $Revision: 1.5 $
 *
 * Date         : $Date: 2008-10-24 16:08:39 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <db_setup.h>

#include <taco_utils.h>

void usage(const char *cmd)
{
        fprintf(stderr, "usage : %s [options]\n", cmd);
        fprintf(stderr, " displays polling informations\n");
        fprintf(stderr, "         options: -h display this message\n");
        fprintf(stderr, "                  -n nethost\n");
        fprintf(stderr, "                  -v display the current version\n");
        exit(1);
}

void version(const char *cmd)
{
        fprintf(stderr, "%s version %s\n", cmd,  VERSION);
        exit(0);
}

int main(int argc,char *argv[])
{
	DevLong 	error;
	long 		status;
	db_poller 	poll;
	extern int      optopt;
	extern int      optind;
	extern char     *optarg;
	int             c;
/*
 * Argument test and device name structure
 */
        while ((c = getopt(argc,argv,"hvn:")) != -1)
        {
                switch (c)
                {
                        case 'n':
                                taco_setenv("NETHOST", optarg, 1);
                                break;
                        case 'v':
                                version(argv[0]);
                                break;
                        case 'h':
                        case '?':
                                usage(argv[0]);
                }
        }
        if (optind != argc)
                usage(argv[0]);

	if (db_import(&error) < 0)
	{
		printf("** db_import : %s **\n", dev_error_str(error)+25);
	}
	status = db_getpoller(argv[optind],&poll,&error);
	if (status == -1)
		printf("%s\n", dev_error_str(error));
	else
		printf("%s is polled by %s/%s on %s PID(%d)\n",argv[optind],poll.server_name,poll.personal_name,poll.host_name,poll.pid);
}
