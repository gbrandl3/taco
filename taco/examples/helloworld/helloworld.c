/*+*******************************************************************

File : 		helloworld.c

Project : 	Device Server 

Description: 	A simple test client to test using the synchronous
		device server API.

Author(s) : 	Andy Goetz 
		$Author: jkrueger1 $

Original : 	November 1997 

Version :	$Revision: 1.2 $ $Date: 2008-04-06 09:06:55 $

History :
		$Log: not supported by cvs2svn $
		Revision 1.1.2.2  2008/03/18 13:23:44  jkrueger1
		make TACO 64 bit ready
		
		Revision 1.1.2.1  2007/06/29 10:48:27  jkrueger1
		add examples
		

		Revision 1.5 1997/11/13 14:13:31 goetz 
		totally reworked doc; added "Hello World" examples; 
		asynchronous call; xdr_types

*-*******************************************************************/ 
/*
#include <Admin.h> 
*/
#include <API.h>
#include <DevSec.h>

int main(int argc, char **argv) 
{
	devserver hw; 
	long access = WRITE_ACCESS, status; 
	DevLong error; 
	char *ch_ptr,helloworld[256], dev_name[256];

	switch (argc)
	{ 
		case 1:
			printf("enter device name [""exp/hello/world""]? "); 
			if(!fgets(dev_name,sizeof(dev_name),stdin) || !dev_name[0])
				strcpy(dev_name,"exp/hello/world"); 
			break; 
		case 2:
			strcpy(dev_name,argv[1]); 
			break; 
		default:
			printf("usage: helloworld [device name]\n"); 
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

	status = dev_putget(hw,DevHello,
				&helloworld,D_STRING_TYPE,
				&ch_ptr,D_STRING_TYPE,
				&error); 
	printf("\nDevHello dev_putget() returned %d\n",status);

	if (status == 0) 
	{
		printf("device answered : %s\n",ch_ptr); 
		dev_xdrfree(D_STRING_TYPE, &ch_ptr, &error); 
	}
	else 
	{
		dev_printerror_no(SEND,NULL,error);
	}
	dev_free(hw,&error); 
	exit(0); 
}

