/*+*******************************************************************

File : 		helloworld.asyn.c 

Project : 	Asynchronous Device Server's 

Description: 	A simple test client to test using the asynchronous
		device server API using callbacks.

Author(s) : 	Andy Goetz 
		$Author: jkrueger1 $

Original : 	January 1997 

Version:	$Revision: 1.2 $ 

Data:		$Date: 2008-04-06 09:06:55 $

History:	
		$Log: not supported by cvs2svn $
		Revision 1.1.2.2  2008/03/18 13:23:44  jkrueger1
		make TACO 64 bit ready
		
		Revision 1.1.2.1  2007/06/29 10:48:27  jkrueger1
		add examples
		

		Revision 2.1 1997/11/13 14:16:40 goetz 
		first release of DSAPI V6

		Revision 1.5 1997/11/13 14:13:31 goetz 
		totally reworked doc; added "Hello World" examples; 
		asynchronous call; xdr_types

*-*******************************************************************/ 
#include <API.h> 
#include <DevStates.h>
#include <DevSec.h>

/*+**********************************************************************

Function : 	void hello_callback()

Description: 	callback function to be called asynchronously after executing
		the DevHello commands

***********************************************************************-*/ 
void hello_callback(devserver ds, void *user_data, DevCallbackData cb_data)
{ 
	DevLong error;

	printf("hello_callback(%s): called with asynch_id=%d, status=%d (error=%d) user data = %s\n", 
		ds->device_name,cb_data.asynch_id, cb_data.status, cb_data.error, 
		(char*)user_data); 
	printf("hello_callback(%s): time executed by server = {%d s,%d us}\n",
		ds->device_name,cb_data.time.tv_sec,cb_data.time.tv_usec);

	if (cb_data.status == DS_OK) 
	{
		printf("hello_callback(%s): device answered=%s\n", ds->device_name,
			*(DevString*)cb_data.argout); 
		dev_xdrfree(D_STRING_TYPE, &cb_data.argout, &error); 
	}
	else 
	{
		dev_printerror_no(SEND,NULL,cb_data.error); 
	}

	return; 
}

/*+**********************************************************************

Function : 	main()

Description: 	main function to test asynchronous DSAPI. 

***********************************************************************-*/

int main(int argc, char **argv) 
{
	devserver hw; 
	long access = WRITE_ACCESS, status; 
	DevLong	error; 
	char *ch_ptr, helloworld[256], dev_name[256]; 
	struct timeval timeout_25s = {25,0}; 
	long asynch_id; 
	char *user_data="my data";

	switch (argc) 
	{
		case 1:
			printf("enter device name [\"exp/hello/world\"]? "); 
			if(!fgets(dev_name,sizeof(dev_name),stdin) || !dev_name[0])
				strcpy(dev_name,"exp/hello/world"); 
			break; 
		case 2:
			strcpy(dev_name,argv[1]); 
			break;
		default:
			printf("usage: helloworld_async [device name]\n"); 
			exit(1); 
	}

	status = dev_import(dev_name,access,&hw,&error); 
	printf("dev_import(%s) returned %d\n",dev_name,status); 
	if (status != 0) 
	{
		char *error_str = dev_error_str(error); 
		printf("%s", error_str); 
		free(error_str);
		exit(1); 
	}

	sprintf(helloworld, "Hello World"); 
	ch_ptr = NULL;

	status = dev_putget_asyn(hw,DevHello,
				&helloworld,D_STRING_TYPE, 
				&ch_ptr,D_STRING_TYPE, 
				(DevCallbackFunction*)hello_callback, (void*)user_data, 
				&asynch_id, &error); 
	printf("\nDevHello dev_putget_asynch(%d) returned %d\n",asynch_id, status); 
	if (status != 0) dev_printerror_no(SEND,NULL,error);

/*
 * wait for answer from client (waits for a max of 25 s)
 */
	status = dev_synch(&timeout_25s, &error);

	dev_free(hw,&error); 
	exit(0);
}
