/****************************************************************************
 *
 * File         :   	test_asc_dyn.c
 *
 * Project      :   	ASCII based interface to device server API
 *
 * Description  :   	The code for a C program (main) which test the shared
 *                  	library libascapi.sl
 *
 * Author       :   	Faranguiss Poncet
 *			$Author: jkrueger1 $
 *
 * Original     :  	November 1996
 *
 * Version 	:	$Revision: 1.2 $
 *
 * Date		:	$Date: 2003-06-06 09:35:57 $  
 *
 * Copyright (c) 1996 by European Synchrotron Radiation Facility,
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

#ifdef __hpux
#include <dl.h>
#else
#include <dlfcn.h>
#endif /* __hpux */


#ifdef __hpux
shl_t ASC_shl;
#else
void *ASC_shl;
#endif /* __hpux */

int main (int argc, char **argv)
{
	long		err;
	char		*dev_name;

	long            (*p_get_func)(),
			(*p_put_func)(),
			(*p_putget_func)();

	char		func_name[101],
			device_name[101],
			command_name[101],
			in_arg[501],
			out_str[3001],
			err_str[1001],
			*out_param, 
			*err_param;
	long            get_status;
	DevString	lib_path = NULL;
	db_resource	res = {"path", D_STRING_TYPE, &lib_path};

/*
 * Try to find the path for the type conversion library in Data Base 
 */
	if (db_import(&err))
	{
		fprintf(stderr, "test_asc_dev : Cannot import static database to find libpath.\n");
		fprintf(stderr, "test_asc_dev : program aborted.\n");
		exit(-1);
	}
#ifdef __hpux
	dev_name = "sys/libasc/s700";
#else
	dev_name = "sys/libasc/solaris";
#endif /* __hpux */
	if (db_getresource(dev_name,&res,1,&err))
	{
		fprintf(stderr, "test_asc_dev : Cannot get libpath resource from the  static database.\n");
		fprintf(stderr, "test_asc_dev : program aborted.\n");
		exit(-1);
	}

/*** Did not find the libpath in static database so set it to default ***/
	if (lib_path == (char *)NULL)
#ifdef __hpux
      		lib_path = ASC_HPUX_DEF_PATH;
#else
      		lib_path = ASC_SOLARIS_DEF_PATH;
#endif /* __hpux */


/*
 * Load the ascii dev api  shared library 
 */

#ifdef __hpux
	ASC_shl = shl_load(lib_path,BIND_IMMEDIATE,0);
	if (ASC_shl == NULL)
	{
		fprintf(stderr, "test_asc_dev : Cannot load the ascii dev api shared library.\n");
		fprintf(stderr, "test_asc_dev : errno = %d\n", errno);
		fprintf(stderr, "test_asc_dev : lib_path = %s\n", lib_path);
		fprintf(stderr, "test_asc_dev : program aborted.\n");
		exit(-1);
	}
#else
	ASC_shl = dlopen(lib_path,1);
	if (ASC_shl == NULL)
	{
		fprintf(stderr, "test_asc_dev : Cannot load the ascii dev api shared library.\n");
		fprintf(stderr, "test_asc_dev : lib_path = %s\n", lib_path);
		fprintf(stderr, "test_asc_dev : program aborted.\n");
		exit(-1);
	}
#endif /* __hpux */

/*
 * Find the dev_get function in the ascii dev api dynamic lib 
 */
	strcpy (func_name, ASC_GET_FUNCTION);
#ifdef __hpux
	if (shl_findsym(&ASC_shl, func_name, TYPE_PROCEDURE, &p_get_func) == -1)
#else
	p_get_func = (long (*)()) dlsym(ASC_shl, func_name);
	if (p_get_func == NULL)
#endif /* __hpux */
	{
		fprintf(stderr, "test_asc_dev : cannot resolve symbol = %s\n", func_name);
		p_get_func = NULL;
	}

/* 
 * Find the dev_put function in the ascii dev api dynamic lib 
 */
	strcpy (func_name, ASC_PUT_FUNCTION);
#ifdef __hpux
	if (shl_findsym(&ASC_shl, func_name, TYPE_PROCEDURE, &p_put_func) == -1)
#else
	p_put_func = (long (*)()) dlsym(ASC_shl, func_name);
	if (p_put_func == NULL)
#endif /* __hpux */
	{
		fprintf(stderr, "test_asc_dev : cannot resolve symbol = %s\n", func_name);
		p_put_func = NULL;
	}

/*
 * Find the dev_putget function in the ascii dev api dynamic lib 
 */
	strcpy (func_name, ASC_PUTGET_FUNCTION);
#ifdef __hpux
	if (shl_findsym(&ASC_shl, func_name, TYPE_PROCEDURE, &p_putget_func) == -1)
#else
	p_putget_func = (long (*)()) dlsym(ASC_shl, func_name);
	if (p_putget_func == NULL)
#endif /* __hpux */
	{
		fprintf(stderr, "test_asc_dev : cannot resolve symbol = %s\n", func_name);
		p_putget_func = NULL;
	}

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

		if (p_get_func == NULL)
			break;

		out_str[0] = '\0';
		err_str[0] = '\0';
		out_param = (char *) out_str;
		err_param = (char *) err_str;

		if ( (*p_get_func)(device_name, command_name, &out_param, &err_param) == 0 )
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

	printf("\n\nNow let's try the dev_put_ascii command.\n\n");
 
	while (1)
	{
		printf("\n\nType in the name of the device : ");
		scanf("%s", device_name);
	
		if (strcmp("END", device_name) == 0)
			break;

		printf("\n\nType in the name of the command : ");
		scanf("%s", command_name);

		printf("\n\nType in the input argument respecting its format : ");
/*************************************************************************
		scanf("%s", in_arg);  replaced by the 6 following lines.
*************************************************************************/
		gets(in_arg);
		if (gets(in_arg) == NULL)
		{
			printf("test_asc_dyn : gets failed.\n");
			continue;
		}

		printf("Device = %s  ,  ", device_name);
		printf("Command = %s  ,  ", command_name);
		printf("Input Argument = %s\n", in_arg);

		if (p_put_func == NULL)
			break;

		out_str[0] = '\0';
		err_str[0] = '\0';
		out_param = (char *) out_str;
		err_param = (char *) err_str;

		if ( (*p_put_func)(device_name, command_name, in_arg, &err_param) == 0 )
		{
			printf("Youpi! dev_put_ascii succeeded!\n");
			printf("error message = %s\n", err_str);
		}
		else
		{
			printf("Zut! dev_put_ascii failed!\n");
			printf("error message = %s\n", err_str);
		}
	}


	printf("\n\nFinally let's try the dev_putget_ascii command.\n\n");
	while (1)
	{
		printf("\n\nType in the name of the device : ");
		scanf("%s", device_name);

		if (strcmp("END", device_name) == 0)
			break;

		printf("\n\nType in the name of the command : ");
		scanf("%s", command_name);

		printf("\n\nType in the input argument respecting its format : ");
/*************************************************************************
		scanf("%s", in_arg);  replaced by the 6 following lines.
*************************************************************************/
		gets(in_arg);
		if (gets(in_arg) == NULL)
		{
			printf("test_asc_dyn : gets failed.\n");
			continue;
		}

		printf("Device = %s  ,  ", device_name);
		printf("Command = %s  ,  ", command_name);
		printf("Input Argument = %s\n", in_arg);

		if (p_putget_func == NULL)
			break;

		out_str[0] = '\0';
		err_str[0] = '\0';
		out_param = (char *) out_str;
		err_param = (char *) err_str;

		if ( (*p_putget_func)(device_name, command_name, in_arg, &out_param, &err_param) == 0 )
		{
			printf("Youpi! dev_putget_ascii succeeded!\n");
			printf("out argument = %s\n", out_str);
			printf("error message = %s\n", err_str);
		}
		else
		{
			printf("Zut! dev_putget_ascii failed!\n");
			printf("error message = %s\n", err_str);
		}
	}
	return 0;
}


