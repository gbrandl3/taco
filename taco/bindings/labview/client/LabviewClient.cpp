/*+*******************************************************************

 File:		LabviewClient.cpp

 Project:	TACO Client for TACO Labview Device Server

 Description:	Implements several type specific routines for getting and 
					setting values of VI controls and indicators. 
					Uses the generic interface which passes any arguments as string lists. 
					To send and recieve the correct types they have to be transformed into 
					strings when sending and vice versa when at retrieval.

 Author(s);	Hartmut Gilde 

 Original:	November 2003

 Version:	$Revision: 1.1 $

 Date:		$Date: 2004-01-26 08:44:22 $

 Copyright (c) 1990-1997 by FRM2 TU München, 
                            Garching, Germany

********************************************************************-*/


#include <macros.h>
#include <API.h>
//#include <ApiP.h>
#include <Admin.h>
#include <BlcDsNumbers.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <DevErrors.h>
#include <maxe_xdr.h>
#include <db_setup.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "LabviewClient.h"
#include "LabViewGeneric.h"

/* define static member */
/* TO DO: */
/* try to get device name from static database instead of hardwiring it into the code*/
char* LabviewClient::device_name = "test/labviewgeneric/1";


/* default constructor */
LabviewClient::LabviewClient() {
	extern_server = FALSE;
	InitVars();
}

/* constructor with device name as argument */
LabviewClient::LabviewClient(char* devicename) {
	readwrite = 0;
	status = dev_import((char*)devicename, readwrite, lv, &error);
	printf("dev_import(%s) returned %d (error=%d)\n", devicename, status, error);

	if (status != 0) 
	{
		printf("%s\n", dev_error_str(error));
		exit(1);
	}
	extern_server = FALSE;
	InitVars();
}

/* constructor which delivers the device server object as argument */
LabviewClient::LabviewClient(devserver* lv) {
	this->lv = lv;
	extern_server = TRUE;
	InitVars();
}


LabviewClient::~LabviewClient() {
	if (!extern_server) {
		dev_free(*lv,&error);
	}
}

/* test if device has been exported, and if device is ready for communication */
int LabviewClient::InitDevice() {
	readwrite = 0;	
	char **device_names = NULL;
   unsigned int nb_devices = 0;
   long error = 0;
  	char *err_str;
  	DevVarCmdArray cmd_table;	
	status = db_import (&error);
  	if (status != DS_OK)
  	{
   	err_str = dev_error_str(error);
      printf("Cannot import Static Data Base.\n");
      printf(err_str);
      printf("xdevmenu aborted.\n");
      if (err_str != NULL)
         free(err_str);
      return(DS_NOTOK);
   }

   status = db_getdevexp( LabviewClient::device_name, &device_names, &nb_devices, &error);
   if (status != DS_OK) {
		printf("DEVICE NOT EXPORTED !!!\n");
		printf("db_getdevexp(%s) returned %d (error=%d)\n", LabviewClient::device_name, status, error);
   	return(DS_NOTOK);
		//exit(1);
	}
	else {
		/*for (unsigned int i=0; i<nb_devices;i++) {
			printf("device %d:%s", i, device_names[i]);
		}*/
	}

	lv = new devserver();	
	status = dev_import((DevString)LabviewClient::device_name, readwrite, lv, &error);
	printf("dev_import(%s) returned %d (error=%d)\n", LabviewClient::device_name, status, error);
	if (status != DS_OK) {
		printf("DEVICE NOT IMPORTED !!!\n");
		printf("%s\n", dev_error_str(error));
   	return(DS_NOTOK);
		//exit(1);
	}

   
	cmd_table.length = 0;
   cmd_table.sequence = NULL;
   status = dev_cmd_query (*lv, &cmd_table, &error);
   if (status != DS_OK) {
      err_str = dev_error_str(error);
      printf("Cannot get the list of the available commands.\n");
      printf("The call to dev_cmd_query failed.\n\n");
      printf("%s", err_str);
      if (err_str != NULL) {
			free(err_str);
		}
      printf("\n\nDevice not imported.\n");

      status = dev_free(*lv, &error);
      if (status != DS_OK) {
         printf("Error occured when tried to free the device server\n");
      };

      return DS_NOTOK;
   }
	extern_server = FALSE;
	return DS_OK;
}


void LabviewClient::InitVars() {
	cntl_array_in = NULL;
	cntl_array_out = NULL;
	num_get_in_args  = 3; 
	num_get_out_args = 1;
	num_set_out_args = 3;
}

void LabviewClient::InitSetOutputBuffer(DevString cntl_name, DevString cntl_type, DevString cntl_value) {
	/* free previously used memory */
	if (cntl_array_in != NULL) {
		delete cntl_array_in;
	}
	/* allocate new memory */
	cntl_array_in = new DevVarStringArray();

	/* construct generic array sequence*/
	cntl_array_in->length = num_set_out_args;
	cntl_array_in->sequence = new DevString[num_set_out_args];

	cntl_array_in->sequence[0] = new char[strlen(cntl_name)+1];
	strcpy(cntl_array_in->sequence[0], cntl_name);

	cntl_array_in->sequence[1] = new char[strlen(cntl_type)+1];
	strcpy(cntl_array_in->sequence[1], cntl_type);

	cntl_array_in->sequence[2] = new char[strlen(cntl_value)+1];
	strcpy(cntl_array_in->sequence[2], cntl_value);
}


void LabviewClient::InitGetOutputBuffer(DevString cntl_name) {
	/* free previously used memory */
	if (cntl_array_in != NULL) {
		delete cntl_array_in;
	}
	/* allocate new memory */
	cntl_array_in = new DevVarStringArray();
	cntl_array_in->length = num_get_out_args;
	cntl_array_in->sequence = new DevString[num_get_out_args];
	cntl_array_in->sequence[0] = new char[strlen(cntl_name)+1];
	strcpy(cntl_array_in->sequence[0], cntl_name);

}

void LabviewClient::InitGetInputBuffer() {
	if (cntl_array_out != NULL) {
		delete cntl_array_out;
	}
	cntl_array_out= new DevVarStringArray();
}


void LabviewClient::TestPutGetError(char* calling_method) {
	printf("\n%s dev_putget() returned %d\n", calling_method, status);
	if (status < 0) {
		dev_printerror_no(SEND,NULL,error);
	}
}


/* get control type */
int LabviewClient::GetControlType(DevString lv_cntl_name, bool indicator) {
	DevVarStringArray* cntl_list = new DevVarStringArray();
	
	status = GetControlInfo(lv_cntl_name, cntl_list, indicator);
	if (status < 0) {
		dev_printerror_no(SEND,NULL,error);
	} 

	int type;
	if (!strcmp("DevDouble", cntl_list->sequence[1])) {
		type = D_DOUBLE_TYPE;
	}
	else if (!strcmp("DevFloat", cntl_list->sequence[1])) {
		type = D_FLOAT_TYPE;
	}
	else if (!strcmp("DevShort", cntl_list->sequence[1])) {
		type = D_SHORT_TYPE;
	}
	else if (!strcmp("DevUShort", cntl_list->sequence[1])) {
		type = D_USHORT_TYPE;
	}
	else if (!strcmp("DevLong", cntl_list->sequence[1])) {
		type = D_LONG_TYPE;
	}
	else if (!strcmp("DevULong", cntl_list->sequence[1])) {
		type = D_ULONG_TYPE;
	}
	else if (!strcmp("DevBoolean", cntl_list->sequence[1])) {
		type = D_BOOLEAN_TYPE;
	}
	else if (!strcmp("DevString", cntl_list->sequence[1])) {
		type = D_STRING_TYPE;
	}
	else {
		type = D_VOID_TYPE;
	}
	
	return type;
}


/* get vi parameters */
int LabviewClient::GetViInfo(DevVarStringArray* lv_cntl_list) {

	status = dev_putget(*lv, DevGetViInfo, NULL, D_VOID_TYPE, lv_cntl_list, D_VAR_STRINGARR, &error);
	
	TestPutGetError("DevGetViInfo");

	return status;
}


/* retrieve control list, if the indicator argument is TRUE, then retrieve control list */
int LabviewClient::GetControlList(DevVarStringArray* lv_cntl_list, bool indicator ) {
	long command = indicator? DevGetIndicatorList: DevGetControlList;

	status = dev_putget(*lv, command, NULL, D_VOID_TYPE, lv_cntl_list, D_VAR_STRINGARR, &error);
	
	TestPutGetError("DevGetControlList");

	return status;
}


/* get control type and value */
int LabviewClient::GetControlInfo(DevString lv_cntl_name, DevVarStringArray* lv_cntl_info, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;

	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, lv_cntl_info, D_VAR_STRINGARR, &error);

	/*test errors*/	
	TestPutGetError("DevGetControlInfo");

	return status;
}



/* --------------------------------------------------
The Following get functions follow the same scheme: 
	1) initialize the array sequence which holds the data
	2) send request via dev_putget and the corresponding command name
	3) extract returned array and convert types correspondingly
-----------------------------------------------------------*/


/* -----------------------------------------------------*/
/*              GetDoubleValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetDoubleValue(DevString lv_cntl_name, DevDouble* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;

	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);

	/* convert string to double */
	*lv_cntl_value = DevStringToDouble(cntl_array_out->sequence[2]);

	/* test for errors */
	TestPutGetError("DevGetControlInfo");

	return status;
}
	

/* -----------------------------------------------------*/
/*              GetFloatValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetFloatValue(DevString lv_cntl_name, DevFloat* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;
	
	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);

	/* convert string to float */
	*lv_cntl_value = DevStringToFloat(cntl_array_out->sequence[2]);

	/* test for errors */
	TestPutGetError("DevGetControlInfo");

	return status;
}


/* -----------------------------------------------------*/
/*              GetShortValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetShortValue(DevString lv_cntl_name, DevShort* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;
	
	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);

	/* convert string to short */
	*lv_cntl_value = DevStringToShort(cntl_array_out->sequence[2]);

	/* test for errors */
	TestPutGetError("DevGetControlInfo");

	return status;
}


/* -----------------------------------------------------*/
/*              GetUShortValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetUShortValue(DevString lv_cntl_name, DevUShort* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;
	
	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);

	/* convert string to ushort */
	*lv_cntl_value = DevStringToUShort(cntl_array_out->sequence[2]);

	/* test for errors */
	TestPutGetError("DevGetControlInfo");

	return status;
}


/* -----------------------------------------------------*/
/*              GetLongValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetLongValue(DevString lv_cntl_name, DevLong* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;
	
	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);

	/* convert string to long */
	*lv_cntl_value = DevStringToLong(cntl_array_out->sequence[2]);

	/* test for errors */
	TestPutGetError("DevGetControlInfo");

	return status;
}


/* -----------------------------------------------------*/
/*              GetULongValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetULongValue(DevString lv_cntl_name, DevULong* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;
	
	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);

	/* convert string to ulong */
	*lv_cntl_value = DevStringToULong(cntl_array_out->sequence[2]);

	/* test for errors */
	TestPutGetError("DevGetControlInfo");
	
	return status;
}


/* -----------------------------------------------------*/
/*              GetStringValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetStringValue(DevString lv_cntl_name, DevString* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;
	
	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);
	
	/* nothing to convert, just copy the string */
	strcpy(*lv_cntl_value, cntl_array_out->sequence[2]);

	/* test for errors */
	TestPutGetError("DevGetControlInfo");

	return status;
}


/* -----------------------------------------------------*/
/*              GetBooleanValue                           */
/* -----------------------------------------------------*/
int LabviewClient::GetBooleanValue(DevString lv_cntl_name, DevBoolean* lv_cntl_value, bool indicator) {
	long command = indicator? DevGetIndicatorInfo: DevGetControlInfo;
	
	/* allocate memory for input and output argument array sequence */
	InitGetOutputBuffer(lv_cntl_name);
	InitGetInputBuffer();

	/* send it to server */
	status = dev_putget(*lv, command, cntl_array_in, D_VAR_STRINGARR, cntl_array_out, D_VAR_STRINGARR, &error);

	/* convert string to boolean */
	*lv_cntl_value = DevStringToBoolean(cntl_array_out->sequence[2]);	

	/* test for errors */
	TestPutGetError("DevGetControlInfo");

	return status;
}




/* ------------------------------------------------------------
	The Following set functions follow the same scheme: 
	1) convert the types to an appropriate string
	2) initialize the array sequences which hold the data
	3) send request via dev_putget and the corresponding command name
----------------------------------------------------------------*/


/* -----------------------------------------------------*/
/*              SetDoubleValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetDoubleValue(DevString lv_cntl_name, DevDouble* lv_cntl_value) {

	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevDouble", DevDoubleToString(*lv_cntl_value));	

	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);

	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}


/* -----------------------------------------------------*/
/*              SetFloatValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetFloatValue(DevString lv_cntl_name, DevFloat* lv_cntl_value) {

	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevFloat", DevFloatToString(*lv_cntl_value));	

	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);

	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}


/* -----------------------------------------------------*/
/*              SetShortValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetShortValue(DevString lv_cntl_name, DevShort* lv_cntl_value) {
	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevShort", DevShortToString(*lv_cntl_value));	
	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);
	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}


/* -----------------------------------------------------*/
/*              SetUShortValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetUShortValue(DevString lv_cntl_name, DevUShort* lv_cntl_value) {
	
	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevUShort", DevUShortToString(*lv_cntl_value));	

	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);

	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}


/* -----------------------------------------------------*/
/*              SetLongValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetLongValue(DevString lv_cntl_name, DevLong* lv_cntl_value) {

	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevLong", DevLongToString(*lv_cntl_value));	

	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);

	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}


/* -----------------------------------------------------*/
/*              SetUlongValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetULongValue(DevString lv_cntl_name, DevULong* lv_cntl_value) {

	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevULong", DevULongToString(*lv_cntl_value));	

	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);

	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}


/* -----------------------------------------------------*/
/*              SetStringValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetStringValue(DevString lv_cntl_name, DevString* lv_cntl_value) {
	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevString", *lv_cntl_value);	
	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);
	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}



/* -----------------------------------------------------*/
/*              SetBooleanValue                           */
/* -----------------------------------------------------*/
int LabviewClient::SetBooleanValue(DevString lv_cntl_name, DevBoolean* lv_cntl_value) {

	/* init array sequence */
	InitSetOutputBuffer(lv_cntl_name, (DevString)"DevBoolean", DevBooleanToString(*lv_cntl_value));	

	/* send it to server */	
	status = dev_putget(*lv, DevSetControlValue, cntl_array_in, D_VAR_STRINGARR, NULL, D_VOID_TYPE, &error);

	/* test for errors */
	TestPutGetError("DevSetControlValue");

	return status;
}




/* Convenience Function 
 returns the TACO type as String representation
*/
DevString LabviewClient::stringType(int type) 
{
	static char string_type[100];

	switch (type) {
		case D_DOUBLE_TYPE:
			strcpy(string_type, "DevDouble");
			break;
		case D_FLOAT_TYPE:
			strcpy(string_type, "DevFloat");
			break;
		case D_SHORT_TYPE:
			strcpy(string_type, "DevShort");
			break;
		case D_USHORT_TYPE:
			strcpy(string_type, "DevUShort");
			break;
		case D_LONG_TYPE:
			strcpy(string_type, "DevLong");
			break;
		case D_ULONG_TYPE:
			strcpy(string_type, "DevULong");
			break;
		case D_STRING_TYPE:
			strcpy(string_type, "DevString");
			break;
		case D_BOOLEAN_TYPE:
			strcpy(string_type, "DevBoolean");
			break;
		case D_VOID_TYPE:
			strcpy(string_type, "unknown");
			break;
		default:
			strcpy(string_type, "unknown");
			break;
	}
	return string_type;
}

/* Convenience Function 
 returns the TACO type as String representation
*/
DevString LabviewClient::stringDType(int type) 
{
	static char string_type[100];

	switch (type) {
		case D_DOUBLE_TYPE:
			strcpy(string_type, "D_DOUBLE_TYPE");
			break;
		case D_FLOAT_TYPE:
			strcpy(string_type, "D_FLOAT_TYPE");
			break;
		case D_SHORT_TYPE:
			strcpy(string_type, "D_SHORT_TYPE");
			break;
		case D_USHORT_TYPE:
			strcpy(string_type, "D_USHORT_TYPE");
			break;
		case D_LONG_TYPE:
			strcpy(string_type, "D_LONG_TYPE");
			break;
		case D_ULONG_TYPE:
			strcpy(string_type, "D_ULONG_TYPE");
			break;
		case D_STRING_TYPE:
			strcpy(string_type, "D_STRING_TYPE");
			break;
		case D_BOOLEAN_TYPE:
			strcpy(string_type, "D_BOOLEAN_TYPE");
			break;
		case D_VOID_TYPE:
			strcpy(string_type, "unknown");
			break;
		default:
			strcpy(string_type, "unknown");
			break;
	}
	return string_type;
}



/* Auxiliary Conversion Functions */

DevDouble LabviewClient::DevStringToDouble(DevString string) {
	DevDouble d_value;
	//DevFloat f_value;

	char* dot = strchr(string, ',');
	if (dot != NULL) {
		*dot = '.';
	}

	//sscanf(string, "%f", &f_value);
	//d_value = (DevDouble)f_value;						

	char* indicate = new char[strlen(string)+1]; 
	d_value = (DevDouble)strtod(string, &indicate);		
	if ((d_value == 0) && (!strcmp(string, indicate))) {
		printf("Conversion of '%s' to DevDouble failed !!: String contains no valid numbers", string); 
		return 0;
	}
	if (errno == ERANGE ) {
		printf("Conversion of '%s' to DevDouble failed !!  Range Overflow/Underflow", string); 
		return 0;
	}
	return d_value;
}

DevFloat LabviewClient::DevStringToFloat (DevString string) {
	DevFloat fl_value;

	char* dot = strchr(string, ',');
	if (dot != NULL) {
		*dot = '.';
	}

	//sscanf(string, "%f", &fl_value);

	char* indicate = new char[strlen(string)+1]; 
	fl_value = (DevFloat)strtod(string, &indicate);		
	if ((fl_value == 0) && (!strcmp(string, indicate))) {
		printf("Conversion of '%s' to DevFloat failed !!: String contains no valid numbers", string); 
		return 0;
	}
	if (errno == ERANGE ) {
		printf("Conversion of '%s' to DevFloat failed !!  Range Overflow/Underflow", string); 
		return 0;
	}
	return fl_value;
}

DevShort LabviewClient::DevStringToShort (DevString string) {
	DevShort s_value;
	//sscanf(string, "%ld", &s_value);
	char* indicate = new char[strlen(string)+1]; 
	s_value = (DevShort)strtol(string, &indicate, 10);		
	if ((s_value == 0) && (!strcmp(string, indicate))) {
		printf("Conversion of '%s' to DevShort failed !!: String contains no valid numbers", string); 
		return 0;
	}
	if ((errno == ERANGE) /*|| (s_value > (2^((sizeof(DevUShort)*8)-1)))*/) {
		printf("Conversion of '%s' to DevShort failed !!  Range Overflow/Underflow", string); 
		return 0;
	}
	return s_value;
}

DevUShort  LabviewClient::DevStringToUShort (DevString string) {
	DevShort s_value;
	//sscanf(string, "%d", &us_value);
	char* indicate = new char[strlen(string)+1]; 
	s_value = (DevShort)strtol(string, &indicate, 10);		
	if ((s_value == 0) && (!strcmp(string, indicate))) {
		printf("Conversion of '%s' to DevUShort failed !!: String contains no valid numbers", string); 
		return 0;
	}
	if ((errno == ERANGE) /*|| (s_value > (2^(sizeof(DevUShort)*8)))*/) {
		printf("Conversion of '%s' to DevUShort failed !!  Range Overflow/Underflow", string); 
		return 0;
	}
	if (s_value < 0) {
		s_value = abs(s_value);
		return 0;
	}
	return (DevUShort)s_value;
}

DevLong    LabviewClient::DevStringToLong   (DevString string) {
	DevLong l_value;
	//sscanf(string, "%ld", &l_value);
	char* indicate = new char[strlen(string)+1]; 
	l_value = (DevLong)strtol(string, &indicate, 10);		
	if ((l_value == 0) && (!strcmp(string, indicate))) {
		printf("Conversion of '%s' to DevLong failed !!: String contains no valid numbers", string); 
		return 0;
	}
	if (errno == ERANGE ) {
		printf("Conversion of '%s' to DevLong failed !!  Range Overflow/Underflow", string); 
		return 0;
	}
	return l_value;
}

DevULong   LabviewClient::DevStringToULong  (DevString string) {
	DevLong l_value;
	//sscanf(string, "%ld", &ul_value);
	char* indicate = new char[strlen(string)+1]; 
	l_value = (DevLong)strtol(string, &indicate, 10);		
	if ((l_value == 0) && (!strcmp(string, indicate))) {
		printf("Conversion of '%s' to DevULong failed !!: String contains no valid numbers", string); 
		return 0;
	}
	if (errno == ERANGE ) {
		printf("Conversion of '%s' to DevULong failed !!  Range Overflow/Underflow", string); 
		return 0;
	}
	if (l_value < 0) {
		l_value = labs(l_value);
	}
	return (DevULong)l_value;
}


DevBoolean LabviewClient::DevStringToBoolean(DevString string) {
	for (int i=0; i<strlen(string); i++) {
		string[i] = toupper(string[i]);
	}
	DevBoolean bl_value;
	strcmp(string, "TRUE")?(bl_value = FALSE):(bl_value = TRUE);
	return bl_value;
}



DevString LabviewClient::DevDoubleToString (DevDouble  value) {
	char* s_value = new char[25];
	sprintf(s_value, "%f", value);	
	char *comma = strchr(s_value, '.');
	*comma = ',';
	return s_value;
}

DevString LabviewClient::DevFloatToString  (DevFloat   value) {
	char* s_value = new char[25];
	sprintf(s_value, "%f", value);	
	char *comma = strchr(s_value, '.');
	*comma = ',';
	return s_value;
}

DevString LabviewClient::DevShortToString  (DevShort   value) {
	char* s_value = new char[25];
	sprintf(s_value, "%d", (short)(value));	
	return s_value;
}

DevString LabviewClient::DevUShortToString (DevUShort  value) {
	char* s_value = new char[25];
	sprintf(s_value, "%d", (unsigned short)(value));	
	return s_value;
}

DevString LabviewClient::DevLongToString   (DevLong    value) {
	char* s_value = new char[25];
	sprintf(s_value, "%ld", (long)(value));	
	return s_value;
}

DevString LabviewClient::DevULongToString  (DevULong   value) {
	char* s_value = new char[25];
	sprintf(s_value, "%ld", (unsigned long)(value));	
	return s_value;
}

DevString LabviewClient::DevBooleanToString(DevBoolean value) {
	char* s_value;
	if (value) {
		s_value = new char[strlen("TRUE")+1]; 
		strcpy(s_value, "TRUE");
	}
	else {
		s_value = new char[strlen("FALSE")+1]; 
		strcpy(s_value, "FALSE");

	}
	return (DevString)s_value;
}


DevVoid*  LabviewClient::DevStringToType(DevString string, int type) {
	char* indicate = new char[strlen(string)+1]; 
	switch (type) {
		case D_DOUBLE_TYPE:
			{
				DevDouble* d_value;

				char *dot = strchr(string, ',');
				*dot = '.';
				
				*d_value = (DevDouble)strtod(string, &indicate);		
				if ((*d_value == 0) && (!strcmp(string, indicate))) {
					printf("Conversion of '%s' to DevDouble failed !!: String contains no valid numbers", string); 
					return 0;
				}
				if (errno == ERANGE ) {
					printf("Conversion of '%s' to DevDouble failed !!  Range Overflow/Underflow", string); 
					return 0;
				}

				return (DevVoid*)d_value;
			}
			break;
		case D_FLOAT_TYPE:
			{
				DevFloat* fl_value;

				char *dot = strchr(string, ',');
				*dot = '.';

				*fl_value = (DevFloat)strtof(string, &indicate);		
				if ((*fl_value == 0) && (!strcmp(string, indicate))) {
					printf("Conversion of '%s' to DevFloat failed !!: String contains no valid numbers", string); 
					return 0;
				}
				if (errno == ERANGE ) {
					printf("Conversion of '%s' to DevFloat failed !!  Range Overflow/Underflow", string); 
					return 0;
				}

				return (DevVoid*)fl_value;
			}
			break;
		case D_SHORT_TYPE:
			{
				DevShort* s_value;
				*s_value = (DevShort)strtol(string, &indicate, 10);		
				if ((*s_value == 0) && (!strcmp(string, indicate))) {
					printf("Conversion of '%s' to DevShort failed !!: String contains no valid numbers", string); 
					return 0;
				}
				printf("size: %d",(2^((sizeof(DevUShort)*8)-1)));
				if ((errno == ERANGE) /*|| (*s_value > (2^((sizeof(DevUShort)*8)-1)))*/) {
					printf("Conversion of '%s' to DevShort failed !!  Range Overflow/Underflow", string); 
					return 0;
				}
				return (DevVoid*)s_value;
			}
			break;
		case D_USHORT_TYPE:
			{
				DevShort* s_value;
				*s_value = (DevShort)strtol(string, &indicate, 10);		
				if ((*s_value == 0) && (!strcmp(string, indicate))) {
					printf("Conversion of '%s' to DevUShort failed !!: String contains no valid numbers", string); 
					return 0;
				}
				if ((errno == ERANGE) || (*s_value > (2^(sizeof(DevUShort)*8)))) {
					printf("Conversion of '%s' to DevUShort failed !!  Range Overflow/Underflow", string); 
					return 0;
				}
				if (*s_value < 0) {
					*s_value = abs(*s_value);
					return 0;
				}
				
				return (DevVoid*)s_value;
			}
			break;
		case D_LONG_TYPE:
			{
				DevLong* l_value;
				*l_value = (DevLong)strtol(string, &indicate, 10);		
				if ((*l_value == 0) && (!strcmp(string, indicate))) {
					printf("Conversion of '%s' to DevLong failed !!: String contains no valid numbers", string); 
					return 0;
				}
				if (errno == ERANGE ) {
					printf("Conversion of '%s' to DevLong failed !!  Range Overflow/Underflow", string); 
					return 0;
				}
				return (DevVoid*)l_value;
			}
			break;
		case D_ULONG_TYPE:
			{
				DevLong* l_value;
				*l_value = (DevLong)strtol(string, &indicate, 10);		
				if ((*l_value == 0) && (!strcmp(string, indicate))) {
					printf("Conversion of '%s' to DevULong failed !!: String contains no valid numbers", string); 
					return 0;
				}
				if (errno == ERANGE ) {
					printf("Conversion of '%s' to DevULong failed !!  Range Overflow/Underflow", string); 
					return 0;
				}
				if (*l_value < 0) {
					*l_value = labs(*l_value);
				}
				return (DevVoid*)l_value;
			}
			break;
		case D_BOOLEAN_TYPE:
			{
				for (int i=0; i<strlen(string); i++) {
					string[i] = toupper(string[i]);
				}
				DevBoolean* bl_value;

				strcmp(string, "TRUE")?(*bl_value = FALSE):(*bl_value = TRUE);
				return (DevVoid*)bl_value;
			}
			break;
		case D_VOID_TYPE:
			return (DevVoid*)NULL;
			break;
		default:
			return (DevVoid*)NULL;
			break;
	}
	return NULL;	
}




DevVoid*  LabviewClient::DevScanToType(DevString string, int type) {
	switch (type) {
		case D_DOUBLE_TYPE:
			{
				DevDouble* d_value;
				DevFloat f_value;

				char *dot = strchr(string, ',');
				*dot = '.';

				sscanf(string, "%f", &f_value);
				*d_value = (DevDouble)f_value;						
				
				return (DevVoid*)d_value;
			}
			break;
		case D_FLOAT_TYPE:
			{
				DevFloat* fl_value;

				char *dot = strchr(string, ',');
				*dot = '.';

				sscanf(string, "%f", fl_value);

				return (DevVoid*)fl_value;
			}
			break;
		case D_SHORT_TYPE:
			{
				DevShort* s_value;
				sscanf(string, "%d", s_value);
				return (DevVoid*)s_value;
			}
			break;
		case D_USHORT_TYPE:
			{
				DevUShort* us_value;
				sscanf(string, "%d", us_value);
				return (DevVoid*)us_value;
			}
			break;
		case D_LONG_TYPE:
			{
				DevLong* l_value;
				sscanf(string, "%ld", l_value);
				return (DevVoid*)l_value;
			}
			break;
		case D_ULONG_TYPE:
			{
				DevULong* ul_value;
				sscanf(string, "%ld", ul_value);
				return (DevVoid*)ul_value;
			}
			break;
		case D_BOOLEAN_TYPE:
			{
				for (int i=0; i<strlen(string); i++) {
					string[i] = toupper(string[i]);
				}
				DevBoolean* bl_value;

				strcmp(string, "TRUE")?(*bl_value = FALSE):(*bl_value = TRUE);
				return (DevVoid*)bl_value;
			}
			break;
		case D_VOID_TYPE:
			return (DevVoid*)NULL;
			break;
		default:
			return (DevVoid*)NULL;
			break;
	}
	return NULL;	
}


DevString LabviewClient::DevTypeToString(DevVoid* value, int type) {
	
	char* s_value = new char[25];
	switch (type) {
		case D_DOUBLE_TYPE:	
			{
				sprintf(s_value, "%f", *((DevDouble*)value));	
				char *comma = strchr(s_value, '.');
				*comma = ',';
			}
			break;
		case D_FLOAT_TYPE:
			{
				sprintf(s_value, "%f", *((DevFloat*)value));	
				char *comma = strchr(s_value, '.');
				*comma = ',';
			}
			break;
		case D_SHORT_TYPE:
			{
				sprintf(s_value, "%d", (short) *((DevShort*)value));	
			}
			break;
		case D_USHORT_TYPE:
			{
				sprintf(s_value, "%d", (unsigned short) *((DevUShort*)value));	
			}
			break;
		case D_LONG_TYPE:
			{
				sprintf(s_value, "%ld", (long) *((DevLong*)value));	
			}
			break;
		case D_ULONG_TYPE:
			{
				sprintf(s_value, "%ld", (unsigned long) *((DevULong*)value));	
			}
			break;
		case D_BOOLEAN_TYPE:
			{
				if (*((DevBoolean*)value)) {
					s_value = new char[strlen("TRUE")+1]; 
					strcpy(s_value, "TRUE");
				}
				else {
					s_value = new char[strlen("FALSE")+1]; 
					strcpy(s_value, "FALSE");
				}
			}
			break;
		case D_VOID_TYPE:
			break;
		default:
			break;
	}
	return s_value;
}


int LabviewClient::typeFromString(DevString type_string) 
{
	int type;
	
	if (!strcmp(type_string, "D_DOUBLE_TYPE")) {
		type = D_DOUBLE_TYPE;
	}
	else if (!strcmp(type_string, "D_FLOAT_TYPE")) {
		type = D_FLOAT_TYPE;
	}
	else if (!strcmp(type_string, "D_SHORT_TYPE")) {
		type = D_SHORT_TYPE;
	}
	else if (!strcmp(type_string, "D_USHORT_TYPE")) {
		type = D_USHORT_TYPE;
	}
	else if (!strcmp(type_string, "D_LONG_TYPE")) {
		type = D_LONG_TYPE;
	}
	else if (!strcmp(type_string, "D_ULONG_TYPE")) {
		type = D_ULONG_TYPE;
	}
	else if (!strcmp(type_string, "D_BOOLEAN_TYPE")) {
		type = D_BOOLEAN_TYPE;
	}
	else if (!strcmp(type_string, "D_STRING_TYPE")) {
		type = D_STRING_TYPE;
	}
	else if (!strcmp(type_string, "D_VOID_TYPE")) {
		type = D_VOID_TYPE;
	}
	else {
		type = D_VOID_TYPE;
	}
	
	return type;
}
