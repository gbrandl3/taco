/****************************************************************************
 * 
 * File:	test_tc.c
 *
 * Project:	Type conversion library
 *
 * Description:	The code for a C program (main) which test the shared
 *              library libtcapi.sl
 *
 * Author:	Faranguiss Poncet
 *		$Author: jkrueger1 $
 *
 * Original:	November 1996
 *
 * Version: 	$Revision: 1.4 $
 *
 * Date: 	$Date: 2004-11-03 17:44:50 $
 *
 * Copyright (c) 1996 by European Synchrotron Radiation Facility,
 *                       Grenoble, France
 *
 *                       All Rights Reserved
 *
 ****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <API.h>
#include <tc_api.h>

#ifdef __hpux
#include <dl.h>
#else
#include <dlfcn.h>
#endif /* __hpux */


#ifdef __hpux
shl_t TC_shl;
#else
void *TC_shl;
#endif /* __hpux */

int call_type_to_str (const char *type_name, const void *data_to_convert, char *res_function)
{
	char  func_name[101];
	long  (*p_conv_func)();

	snprintf(func_name, sizeof(func_name), "tc_%s_to_str", type_name);
	printf("call_type_to_str : entered.\n");

#ifdef __hpux
	if (shl_findsym(&TC_shl, func_name, TYPE_PROCEDURE, &p_conv_func) == -1)
#else
	p_conv_func = (long (*)()) dlsym(TC_shl, func_name);
	if (p_conv_func == NULL)
#endif /* __hpux */
	{
		fprintf(stderr, "call_type_to_str : cannot resolve symbol = %s\n", func_name);
		return(DS_NOTOK);
	}

	if ( (*p_conv_func)(data_to_convert, res_function) != 0 )
	{
		fprintf(stderr, "call_type_to_str : error returned by conversion function.\n");
		return(DS_NOTOK);
	}
	printf("call_type_to_str : normally finished.\n");
	return(0);
}


int call_str_to_type (const char *type_name, const char *str_in, void  *res_function )
{
	char  func_name[101];
	long  (*p_conv_func)();

	snprintf(func_name, sizeof(func_name), "tc_str_to_%s", type_name);
	printf("call_str_to_type : entered.\n");

#ifdef __hpux
	if (shl_findsym(&TC_shl, func_name, TYPE_PROCEDURE, &p_conv_func) == -1)
#else
	p_conv_func = (long (*)()) dlsym(TC_shl, func_name);
	if (p_conv_func == NULL)
#endif /* __hpux */
	{
		fprintf(stderr, "call_str_to_type : cannot resolve symbol = %s\n", func_name);
		return(DS_NOTOK);
	}

	if ( (*p_conv_func)(str_in, res_function) != 0 )
	{
		fprintf(stderr, "call_str_to_type : error returned by conversion function.\n");
		return(DS_NOTOK);
	}
	printf("call_str_to_type : normally finished.\n");
	return(0);
}

int main (int argc, char **argv)
{
	long			err;
	char			*dev_name,
				str_local[301],
				type_name[101],
				my_str[301];
	void			*conv_data,
				*res_data;
	int			i, 
				conv_status,
				ind_seq;

	DevVoid			*void_data;
	DevShort		short_data;
	DevUShort		ushort_data;
	DevLong			long_data;
	DevULong		ulong_data;
	DevFloat		float_data;
	DevDouble		double_data;
	DevString		string_data;

	DevIntFloat		intfl_data;
	DevFloatReadPoint	flrp_data;
	DevStateFloatReadPoint	stflrp_data;
	DevLongReadPoint	lgrp_data;
	DevDoubleReadPoint	dblrp_data;
	DevVarCharArray		char_arr_data;
	DevVarStringArray	str_arr_data;
	DevVarUShortArray	ushort_arr_data;
	DevVarShortArray	short_arr_data;
	DevVarULongArray	ulng_arr_data;
	DevVarLongArray		lng_arr_data;
	DevVarFloatArray	float_arr_data;
	DevVarDoubleArray	dbl_arr_data;
	DevMulMove		mulmove_data;
	DevMotorLong		motorl_data;
	DevMotorFloat		motorf_data;
	DevString		lib_path = NULL;
   	db_resource		res = {"path", D_STRING_TYPE, &lib_path};

/*** Try to find the path for the type conversion library in Data Base ***/
	if (db_import(&err))
	{
		fprintf(stderr, "test_tc : Cannot import static database to find libpath.\n");
		fprintf(stderr, "test_tc : program aborted.\n");
		exit(-1);
	}
#ifdef __hpux
	dev_name = "sys/libtc/hpux10.2";
#else
	dev_name = "sys/libtc/solaris";
#endif /* __hpux */
	if (db_getresource(dev_name,&res,1,&err))
	{
		fprintf(stderr, "test_tc : Cannot get libpath resource from the static database.\n");
		fprintf(stderr, "test_tc : program aborted.\n");
		exit(-1);
	}

/*** Did not find the libpath in static database so set it to default ***/
	if (lib_path == (char *)NULL)
#ifdef __hpux
	lib_path = "../lib/hpux10.2/libtcapi.sl";
#else
	lib_path = "../lib/solaris/libtcapi.so";
#endif /* __hpux */


/*** Load the type conversion shared library ***/
#ifdef __hpux
	TC_shl = shl_load(lib_path,BIND_IMMEDIATE,0);
#else
	TC_shl = dlopen(lib_path,1);
#endif /* __hpux */
	if (TC_shl == NULL)
	{
		fprintf(stderr, "test_tc : Cannot load the type conversion shared library.\n");
		fprintf(stderr, "test_tc : lib_path = %s\n", lib_path);
		fprintf(stderr, "test_tc : program aborted.\n");
		exit(-1);
	}

	string_data = (DevString) malloc(501);
	printf("\n\nLet's first try the conversion from types to string.\n\n");
	do
	{
		printf("\n\nType in the name of the type : ");
		scanf("%s", type_name);
		printf("Type = %s\n", type_name);

		if (strcmp("DevVoid", type_name) == 0)
		{
			conv_data = (void *) void_data;
			conv_status = call_type_to_str ("DevVoid", conv_data, (char *) str_local);
	
			if (conv_status == 0)
				printf("Youpi! Conversion result for DevVoid is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevVoid.\n");
		}
		else if (strcmp("DevShort", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%hd", &short_data);
			conv_data = (void *) &short_data;
			conv_status = call_type_to_str ("DevShort", conv_data, (char *) str_local);
	
			if (conv_status == 0)
				printf("Youpi! Conversion result for DevShort is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevShort.\n");
		}
		else if (strcmp("DevUShort", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%hu", &ushort_data);
			conv_data = (void *) &ushort_data;
			conv_status = call_type_to_str ("DevUShort", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevUShort is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevUShort.\n");
		}
		else if (strcmp("DevLong", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%ld", &long_data);
			conv_data = (void *) &long_data;
			conv_status = call_type_to_str ("DevLong", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevLong is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevLong.\n");
		}
		else if (strcmp("DevULong", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%lu", &ulong_data);
			conv_data = (void *) &ulong_data;
			conv_status = call_type_to_str ("DevULong", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevULong is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevULong.\n");
		}
		else if (strcmp("DevFloat", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%g", &float_data);
			conv_data = (void *) &float_data;
			conv_status = call_type_to_str ("DevFloat", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevFloat is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevFloat.\n");
		}
		else if (strcmp("DevDouble", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%lg", &double_data);
			conv_data = (void *) &double_data;
			conv_status = call_type_to_str ("DevDouble", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevDouble is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevDouble.\n");
		}
		else if (strcmp("DevString", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets((char *) string_data, 500, stdin);  /** for return **/
			if (fgets((char *) string_data, 500, stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			conv_data = (void *) &string_data;
			conv_status = call_type_to_str ("DevString", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevString is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevString.\n");
		}
		else if (strcmp("DevIntFloat", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%ld%g", &(intfl_data.state), &(intfl_data.value));
			conv_data = (void *) &intfl_data;
			conv_status = call_type_to_str ("DevIntFloat", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevIntFloat is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevIntFloat.\n");
		}
		else if (strcmp("DevFloatReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%g%g", &(flrp_data.set), &(flrp_data.read));
			conv_data = (void *) &flrp_data;
			conv_status = call_type_to_str ("DevFloatReadPoint", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevFloatReadPoint is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevFloatReadPoint.\n");
		}
		else if (strcmp("DevStateFloatReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%hd%g%g", &(stflrp_data.state), &(stflrp_data.set), &(stflrp_data.read));
			conv_data = (void *) &stflrp_data;
			conv_status = call_type_to_str ("DevStateFloatReadPoint", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevStateFloatReadPoint is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevStateFloatReadPoint.\n");
		}
		else if (strcmp("DevLongReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%ld%ld", &(lgrp_data.set), &(lgrp_data.read));
			conv_data = (void *) &lgrp_data;
			conv_status = call_type_to_str ("DevLongReadPoint", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevLongReadPoint is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevLongReadPoint.\n");
		}
		else if (strcmp("DevDoubleReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%lg%lg", &(dblrp_data.set), &(dblrp_data.read));
			conv_data = (void *) &dblrp_data;
			conv_status = call_type_to_str ("DevDoubleReadPoint", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevDoubleReadPoint is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevDoubleReadPoint.\n");
		}
		else if (strcmp("DevVarCharArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			};
			char_arr_data.length = strlen(str_local);
			char_arr_data.sequence = (char *) malloc(char_arr_data.length+1);
			strcpy (char_arr_data.sequence , str_local);
			conv_data = (void *) &char_arr_data;
			conv_status = call_type_to_str ("DevVarCharArray", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevVarCharArray is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevVarCharArray.\n");
		}
		else if (strcmp("DevVarStringArray", type_name) == 0)
		{
			str_arr_data.length = 3;
			str_arr_data.sequence = (DevString *) malloc(3 * sizeof(DevString));
			for (i=0; i<3; i++)
				str_arr_data.sequence[i] = (char *) malloc(101);

			fgets(str_local, sizeof(str_local), stdin);  /** for return **/

			printf("Type in the first string : ");
			if (fgets((char *) str_arr_data.sequence[0], 100, stdin) == NULL)
			{
				printf("test_tc : first gets failed.\n");
				continue;
			};

			printf("Type in the second string : ");
			if (fgets( (char *) str_arr_data.sequence[1], 100, stdin) == NULL)
			{
				printf("test_tc : second gets failed.\n");
				continue;
			};

			printf("Type in the third string : ");
			if (fgets( (char *) str_arr_data.sequence[2], 100, stdin) == NULL)
			{
				printf("test_tc : third gets failed.\n");
				continue;
			};

			conv_data = (void *) &str_arr_data;
			conv_status = call_type_to_str ("DevVarStringArray", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevVarStringArray is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevVarStringArray.\n");
		}
		else if (strcmp("DevVarLongArray", type_name) == 0)
		{
			lng_arr_data.length = 4;
			lng_arr_data.sequence = (DevLong *) malloc(4 * sizeof(DevLong));
			printf("Type in 4 decimal values : ");
			scanf("%ld%ld%ld%ld", &(lng_arr_data.sequence[0]), &(lng_arr_data.sequence[1]),
        	                       &(lng_arr_data.sequence[2]), &(lng_arr_data.sequence[3]) );
			conv_data = (void *) &lng_arr_data;
			conv_status = call_type_to_str ("DevVarLongArray", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevVarLongArray is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevVarLongArray.\n");
		}
		else if (strcmp("DevVarDoubleArray", type_name) == 0)
		{
			dbl_arr_data.length = 4;
			dbl_arr_data.sequence = (DevDouble *) malloc(4 * sizeof(DevDouble));
			printf("Type in 4 double float values : ");
			scanf("%lg%lg%lg%lg", &(dbl_arr_data.sequence[0]), &(dbl_arr_data.sequence[1]),
                               &(dbl_arr_data.sequence[2]), &(dbl_arr_data.sequence[3]) );
			conv_data = (void *) &dbl_arr_data;
			conv_status = call_type_to_str ("DevVarDoubleArray", conv_data, (char *) str_local);

			if (conv_status == 0)
				printf("Youpi! Conversion result for DevVarDoubleArray is = %s\n", str_local);
			else
				printf("test_tc : Type conversion failed for DevVarDoubleArray.\n");
		}
	}while(strcmp("END", type_name));


	printf("\n\nNow let's try the conversion from string to types.\n\n");
	do 
	{
		printf("\n\nType in the name of the type : ");
		scanf("%s", type_name);
		printf("Type = %s\n", type_name);

		if (strcmp("DevShort", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%s", str_local);
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &short_data;
			conv_status = call_str_to_type ("DevShort", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevShort read from string = %hd .\n", short_data);
			else
				printf("test_tc : Cannot convert string to DevShort.\n");
		}
		else if (strcmp("DevUShort", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%s", str_local);
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &ushort_data;
			conv_status = call_str_to_type ("DevUShort", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevUShort read from string = %hu .\n", ushort_data);
			else
				printf("test_tc : Cannot convert string to DevUShort.\n");
		}
		else if (strcmp("DevLong", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%s", str_local);
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &long_data;
			conv_status = call_str_to_type ("DevLong", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevLong read from string = %ld .\n", long_data);
			else
				printf("test_tc : Cannot convert string to DevLong.\n");
		}
		else if (strcmp("DevULong", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%s", str_local);
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &ulong_data;
			conv_status = call_str_to_type ("DevULong", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevULong read from string = %lu .\n", ulong_data);
			else
				printf("test_tc : Cannot convert string to DevULong.\n");
		}
		else if (strcmp("DevFloat", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%s", str_local);
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &float_data;
			conv_status = call_str_to_type ("DevFloat", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevFloat read from string = %g .\n", float_data);
			else
				printf("test_tc : Cannot convert string to DevFloat.\n");
		}
		else if (strcmp("DevDouble", type_name) == 0)
		{
			printf("Type in the value : ");
			scanf("%s", str_local);
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &double_data;
			conv_status = call_str_to_type ("DevDouble", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevDouble read from string = %lg .\n", double_data);
			else
				printf("test_tc : Cannot convert string to DevDouble.\n");
		}
		if (strcmp("DevString", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &string_data;
			conv_status = call_str_to_type ("DevString", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevString read from string = %s .\n", string_data);
			else
				printf("test_tc : Cannot convert string to DevString.\n");
		}
		else if (strcmp("DevIntFloat", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &intfl_data;
			conv_status = call_str_to_type ("DevIntFloat", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevIntFloat read from string = %ld ; %g .\n",
					intfl_data.state, intfl_data.value);
			else
				printf("test_tc : Cannot convert string to DevIntFloat.\n");
		}
		else if (strcmp("DevFloatReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &flrp_data;
			conv_status = call_str_to_type ("DevFloatReadPoint", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevFloatReadPoint read from string = %g ; %g .\n",
					flrp_data.set, flrp_data.read );
			else
				printf("test_tc : Cannot convert string to DevFloatReadPoint.\n");
		}
		else if (strcmp("DevStateFloatReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &stflrp_data;
			conv_status = call_str_to_type ("DevStateFloatReadPoint", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevStateFloatReadPoint read from string = %hd ; %g; %g .\n", 
					stflrp_data.state, stflrp_data.set, stflrp_data.read);
			else
				printf("test_tc : Cannot convert string to DevStateFloatReadPoint.\n");
		}
		else if (strcmp("DevLongReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &lgrp_data;
			conv_status = call_str_to_type ("DevLongReadPoint", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevLongReadPoint read from string = %ld ; %ld .\n",
					lgrp_data.set, lgrp_data.read);
			else
				printf("test_tc : Cannot convert string to DevLongReadPoint.\n");
			continue;
		}
		if (strcmp("DevDoubleReadPoint", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			};
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &dblrp_data;
			conv_status = call_str_to_type ("DevDoubleReadPoint", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevDoubleReadPoint read from string = %lg ; %lg .\n",
					dblrp_data.set, dblrp_data.read);
			else
				printf("test_tc : Cannot convert string to DevDoubleReadPoint.\n");
		}
		else if (strcmp("DevVarCharArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			char_arr_data.length = 0;
			char_arr_data.sequence = NULL;
			res_data = (void *) &char_arr_data;
			conv_status = call_str_to_type ("DevVarCharArray", (char *) str_local, res_data);
			strncpy(my_str, char_arr_data.sequence, char_arr_data.length);
			my_str[char_arr_data.length] = '\0';
			if (conv_status == 0)
				printf("Youpi! DevVarCharArray read from string = %s\n", my_str);
			else
				printf("test_tc : Cannot convert string to DevVarCharArray.\n");
		}
		else if (strcmp("DevVarStringArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			str_arr_data.length = 0;
			str_arr_data.sequence = NULL;
			res_data = (void *) &str_arr_data;
			conv_status = call_str_to_type ("DevVarStringArray", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevVarStringArray read from string \n");
				for (ind_seq=0; ind_seq < str_arr_data.length; ind_seq++)
					printf("str_arr_data.sequence[%d] = %s\n", ind_seq, str_arr_data.sequence[ind_seq] );
				for (ind_seq=0; ind_seq < str_arr_data.length; ind_seq++)
				{
					free(str_arr_data.sequence[ind_seq]);
					str_arr_data.sequence[ind_seq]=NULL;
				};
				free(str_arr_data.sequence);
				str_arr_data.sequence = NULL;
				str_arr_data.length = 0;
			}
			else
				printf("test_tc : Cannot convert string to DevVarStringArray.\n");
		}
		else if (strcmp("DevVarUShortArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
			continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			ushort_arr_data.length = 0;
			ushort_arr_data.sequence = NULL;
			res_data = (void *) &ushort_arr_data;
			conv_status = call_str_to_type ("DevVarUShortArray", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevVarUShortArray read from string \n");
		
				for (ind_seq=0; ind_seq < ushort_arr_data.length; ind_seq++)
					printf("ushort_arr_data.sequence[%d] = %hu\n", ind_seq, ushort_arr_data.sequence[ind_seq]);
		
				free(ushort_arr_data.sequence);
				ushort_arr_data.sequence = NULL;
				ushort_arr_data.length = 0;
			}
			else
				printf("test_tc : Cannot convert string to DevVarUShortArray.\n");
		}
		else if (strcmp("DevVarShortArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			short_arr_data.length = 0;
			short_arr_data.sequence = NULL;
			res_data = (void *) &short_arr_data;
			conv_status = call_str_to_type ("DevVarShortArray", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevVarShortArray read from string \n");
		
				for (ind_seq=0; ind_seq < short_arr_data.length; ind_seq++)
					printf("short_arr_data.sequence[%d] = %hd\n", ind_seq, short_arr_data.sequence[ind_seq]);
		
				free(short_arr_data.sequence);
				short_arr_data.sequence = NULL;
				short_arr_data.length = 0;
			}
			else
				printf("test_tc : Cannot convert string to DevVarShortArray.\n");
				continue;
		}
		else if (strcmp("DevVarULongArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			ulng_arr_data.length = 0;
			ulng_arr_data.sequence = NULL;
			res_data = (void *) &ulng_arr_data;
			conv_status = call_str_to_type ("DevVarULongArray", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevVarULongArray read from string \n");
		
				for (ind_seq=0; ind_seq < ulng_arr_data.length; ind_seq++)
					printf("ulng_arr_data.sequence[%d] = %lu\n", ind_seq, ulng_arr_data.sequence[ind_seq]);
		
				free(ulng_arr_data.sequence);
				ulng_arr_data.sequence = NULL;
				ulng_arr_data.length = 0;
			}
			else
				printf("test_tc : Cannot convert string to DevVarULongArray.\n");
		}
		else if (strcmp("DevVarLongArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			lng_arr_data.length = 0;
			lng_arr_data.sequence = NULL;
			res_data = (void *) &lng_arr_data;
			conv_status = call_str_to_type ("DevVarLongArray", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevVarLongArray read from string \n");
		
				for (ind_seq=0; ind_seq < lng_arr_data.length; ind_seq++)
					printf("lng_arr_data.sequence[%d] = %ld\n", ind_seq, lng_arr_data.sequence[ind_seq]);
		
				free(lng_arr_data.sequence);
				lng_arr_data.sequence = NULL;
				lng_arr_data.length = 0;
			}
			else
				printf("test_tc : Cannot convert string to DevVarLongArray.\n");
		}
		if (strcmp("DevVarFloatArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			float_arr_data.length = 0;
			float_arr_data.sequence = NULL;
			res_data = (void *) &float_arr_data;
			conv_status = call_str_to_type ("DevVarFloatArray", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevVarFloatArray read from string \n");
		
				for (ind_seq=0; ind_seq < float_arr_data.length; ind_seq++)
					printf("float_arr_data.sequence[%d] = %g\n", ind_seq, float_arr_data.sequence[ind_seq]);
		
				free(float_arr_data.sequence);
				float_arr_data.sequence = NULL;
				float_arr_data.length = 0;
			}
			else
				printf("test_tc : Cannot convert string to DevVarFloatArray.\n");
		}
		else if (strcmp("DevVarDoubleArray", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			dbl_arr_data.length = 0;
			dbl_arr_data.sequence = NULL;
			res_data = (void *) &dbl_arr_data;
			conv_status = call_str_to_type ("DevVarDoubleArray", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevVarDoubleArray read from string \n");
		
				for (ind_seq=0; ind_seq < dbl_arr_data.length; ind_seq++)
					printf("dbl_arr_data.sequence[%d] = %g\n", ind_seq, dbl_arr_data.sequence[ind_seq]);
		
				free(dbl_arr_data.sequence);
				dbl_arr_data.sequence = NULL;
				dbl_arr_data.length = 0;
			}
			else
				printf("test_tc : Cannot convert string to DevVarDoubleArray.\n");
		}
		if (strcmp("DevMulMove", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			res_data = (void *) &mulmove_data;
			conv_status = call_str_to_type ("DevMulMove", (char *) str_local, res_data);
			if (conv_status == 0)
			{
				printf("Youpi! DevMulMove read from string  : \n");
				printf("      action[2]=%d, delay[6]=%d, position[1]=%g",
					mulmove_data.action[2], mulmove_data.delay[6], mulmove_data.position[1] ); 
			}
			else
				printf("test_tc : Cannot convert string to DevMulMove.\n");
		}
		if (strcmp("DevMotorLong", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			motorl_data.axisnum = -1;
			motorl_data.value = -1;
			res_data = (void *) &motorl_data;
			conv_status = call_str_to_type ("DevMotorLong", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevMotorLong read from string = %ld ; %ld .\n",
					motorl_data.axisnum, motorl_data.value);
			else
				printf("test_tc : Cannot convert string to DevMotorLong.\n");
		}
		else if (strcmp("DevMotorFloat", type_name) == 0)
		{
			printf("Type in the value : ");
			fgets(str_local, sizeof(str_local), stdin);  /** for return **/
			if (fgets(str_local, sizeof(str_local), stdin) == NULL)
			{
				printf("test_tc : gets failed.\n");
				continue;
			}
			printf("Voici la chaine = %s\n", str_local);
			motorf_data.axisnum = -1;
			motorf_data.value = -1.5;
			res_data = (void *) &motorf_data;
			conv_status = call_str_to_type ("DevMotorFloat", (char *) str_local, res_data);
			if (conv_status == 0)
				printf("Youpi! DevMotorFloat read from string = %ld ; %g .\n",
					motorf_data.axisnum, motorf_data.value);
			else
				printf("test_tc : Cannot convert string to DevMotorFloat.\n");
		}
	}while(strcmp("END", type_name));
	return 0; 
}

