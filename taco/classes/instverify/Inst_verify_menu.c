
/*********************************************************************
 *
 * File:   Inst_verify_menu.c
 *
 * Project:   Device Server Distribution 
 *
 * Description:   An interactive main routine for test all the commands
 *      of the Device Server Inst_verify.
 *
 * Author(s):   J.Meyer 
 *
 * Original:   10.5.95   
 *
 * Copyleft (c) 1992 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
/*
long			readwrite = 0;
char			cmd_string[80];
devserver		ds;
long			error;
int			cmd;
short			ret;

DevShort	state;
DevString	status;
DevStateFloatReadPoint 	readvalue_argout;
DevFloat 	setvalue_argin;
*/
long ReadCharValue(char lv, char uv,char *par);
long ReadShortValue(short lv, short uv, short *par);
long ReadIntValue(int lv, int uv, int *par);
long ReadFloatValue(float lv, float uv, float *par);

int main(int argc, char **argv)
{
	long		readwrite = 0;
	char		cmd_string[80];
	devserver	ds;
	long		error;
	int		cmd;
	long		ret;

	DevShort	state;
	DevString	status;
	DevStateFloatReadPoint 	readvalue_argout;
	DevFloat 	setvalue_argin;


	if (argc < 2)
	{
		fprintf(stderr, "usage: %s device-name\n",argv[0]);
		exit(1);
	}

	if(dev_import(argv[1],readwrite,&ds,&error) != DS_OK)
	{
		fprintf(stderr, "can't import %s: %s\n",argv[1],dev_error_str(error));
		exit(1);
	}

	while (1)
	{
		status = 0;
		ret = dev_putget(ds, DevStatus, NULL, D_VOID_TYPE, &status, D_STRING_TYPE, &error);
		printf  ("\n");
		printf("test_menu for %s (%s)\n",argv[1],ds->device_class);
		printf("____________________________________\n");
		printf("1.State \t");
		printf("2.Status\n");
	   	printf("3.ReadValue  \t");
	   	printf("4.SetValue\n");
		printf  ("\n");
		printf("Select one commands (0 to quit) : ?");
/*
 * to get around the strange effects of scanf() wait for something read 
 */
		for( ; gets(cmd_string) == (char *)0 ; );

		ret = sscanf(cmd_string,"%d",&cmd);
		printf  ("\n");

		switch (cmd) 
		{
			case (0) : 
				exit(1);
			case (1) :
				ret = dev_putget(ds, DevState, NULL, D_VOID_TYPE, &state, D_SHORT_TYPE, &error);
				if(ret != DS_OK)
					fprintf(stderr, "can't read state: %s\n", dev_error_str(error));
				else 
					printf("The state is %s (%d)\n",DEVSTATES[state], state);
				break;
			case (2) :
				status = NULL;
				ret = dev_putget(ds, DevStatus, NULL, D_VOID_TYPE, &status, D_STRING_TYPE, &error);
				if(ret != DS_OK)
					fprintf(stderr, "can't read status: %s\n", dev_error_str(error));
				else 
					printf("Status :  %s\n", status);
				if ( dev_xdrfree (D_STRING_TYPE, &status, &error) == DS_NOTOK )
					fprintf(stderr, "can't free arguments: %s\n", dev_error_str(error));
				break;
		
			case (3) :
/* Insert here your code to read the input argument(s), see the 
 * routines below. Exemple:
 * ReadIntValue(0, 100, &setvalue);
 */
				ret = dev_putget(ds, DevReadValue, NULL, D_VOID_TYPE, &readvalue_argout, D_STATE_FLOAT_READPOINT, &error);
				if(ret != DS_OK)
					fprintf(stderr, "Can't execute DevReadValue: %s\n", dev_error_str(error)); 
/* Insert here your code for the command DevReadValue */
				else
				{
					printf ("Set  value = %3.1f\n", readvalue_argout.set);
					printf ("Read value = %3.1f\n", readvalue_argout.read);
					printf ("State      = %d\n", readvalue_argout.state);
				}
				break;
		case (4) :
/* Insert here your code to read the input argument(s), see the 
 * routines below. Exemple:
 * ReadIntValue(0, 100, &setvalue);
 */
				printf ("Choose setpoint between 0 and 100 :\n");
				ReadFloatValue (0,100,&setvalue_argin);
				ret = dev_putget(ds, DevSetValue, &setvalue_argin, D_FLOAT_TYPE, NULL, D_VOID_TYPE, &error);
				if(ret != DS_OK)
					fprintf(stderr, "Can't execute DevSetValue: %s\n", dev_error_str(error)); 
				break;
			default : 
				break;
		}
	}
}


/**
 * Reads user input that should be integer
 *
 * @param lv 	the lower limit
 * @param uv 	the upper limit 
 * @param par 	the value of the parameter
 *
 * @return	DS_OK/DS_NOK
 */
long ReadIntValue(int lv, int uv, int *par)
{
	char cmd_string[80];
  
	fgets(cmd_string, 80, stdin);
	rewind(stdin);
	if (((cmd_string[0]<'0') || (cmd_string[0]>'9')) && (cmd_string[0] !='-'))
		return(DS_NOTOK);
    
/* Convert string value into numeric value */
	sscanf(cmd_string,"%d",par);
	if ((*par<lv) || (*par>uv)) 
		return(DS_NOTOK); 
	return(DS_OK);
}

/**
 * Reads user input that should be a lower character 
 *
 * @param lv 	the lower limit
 * @param uv 	the upper limit 
 * @param par 	the value of the parameter
 *
 * @return	DS_OK/DS_NOK
 */ 
long ReadCharValue(char lv, char uv,char *par)
{
	char cmd_string[80];
  
	fgets(cmd_string, 80, stdin);
	rewind(stdin);
    
	if ((cmd_string[0]<'a') || (cmd_string[0]>'z')) 
		return(DS_NOTOK);
/* Convert string value into numeric value */
	*par = cmd_string[0];
	if ((*par<lv) || (*par>uv)) 
		return(DS_NOTOK); 
	return(DS_OK);
}

/**
 * Reads user input that should be short
 *
 * @param lv 	the lower limit
 * @param uv 	the upper limit 
 * @param par 	the value of the parameter
 *
 * @return	DS_OK/DS_NOK
 */ 
long ReadShortValue(short lv, short uv, short *par)
{
	char	cmd_string[80];

	fgets(cmd_string, 80, stdin);
	rewind(stdin);
	if (((cmd_string[0]<'0') || (cmd_string[0]>'9')) && (cmd_string[0] !='-') )
		return(DS_NOTOK);
/* Convert string value into numeric value */
	sscanf(cmd_string,"%d", par);
	if ((*par<lv) || (*par>uv)) 
		return(DS_NOTOK); 
	return(DS_OK);
}

/**
 * Reads user input that should be a float
 *
 * @param lv 	the lower limit
 * @param uv 	the upper limit 
 * @param par 	the value of the parameter
 *
 * @return	DS_OK/DS_NOK
 */ 
long ReadFloatValue(float lv, float uv, float *par)
{
	char cmd_string[80];
  
	fgets(cmd_string, 80, stdin);
	rewind(stdin);
	if (((cmd_string[0]<'0') || (cmd_string[0]>'9')) && (cmd_string[0] !='-'))
		return(DS_NOTOK);
    
    /* Convert string value into numeric value */
	sscanf(cmd_string,"%f",par);
	if ((*par<lv) || (*par>uv)) 
		return(DS_NOTOK); 
	return(DS_OK);
}

