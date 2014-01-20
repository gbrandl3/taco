/****************************************************************************
 * 
 * File          :   	test_asc_stat.c
 *
 * Project       :   	ASCII based interface to device server API
 *
 * Description   :   	The code for a C program (main) which test the archived
 *                   	version of the library libascapi.a. To debug the libascapi
 *                   	library one should first build the archive version of that
 *                   	libascapi.a and then use this (test_asc_stat.c) program to
 *                   	test the library.
 *
 * Author        :   	Faranguiss Poncet
 *			$Author: jkrueger1 $
 *
 * Original      :   	November 1996
 *
 * Version	 :	$Revision: 1.1 $
 *
 * Date		 :	$Date: 2003-04-25 12:03:34 $
 *
 * Copyright (c) 1996-2014 by European Synchrotron Radiation Facility,
 *                       Grenoble, France
 *
 *                       All Rights Reserved
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <API.h>
#include <asc_api.h>

int main (int argc, char **argv)
{
	char	device_name[101],
		command_name[101],
		out_str[2001],
		err_str[1001],
		*result, 
		*error_msg;
	long	get_status;

	printf("\n\nLet's first try the dev_get_ascii command.\n\n");
 
	while (1)
	{
		printf("\n\nType in the name of the device : ");
		scanf("%s", device_name);

		if (strcmp("END", device_name) == 0)
			break;

		printf("\n\nType in the name of the command : ");
		scanf("%s", command_name);

		printf("Device = %s  ,  ", device_name);
		printf("Command = %s\n", command_name);


		out_str[0] = '\0';
		err_str[0] = '\0';

		result = (char *) out_str;
		error_msg = (char *) err_str;
		if ( dev_get_ascii(device_name, command_name, &result, &error_msg) == 0 )
		{
			printf("Youpi! dev_get_ascii succeeded!\n");
			printf("out argument = %s\n", out_str);
		}
		else
		{
			printf("Zut! dev_get_ascii failed!\n");
			printf("error message = %s\n", err_str);
		}
	}
	return 0;
}


