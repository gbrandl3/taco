
/*********************************************************************

 File:          dev_state.c

 Project:       Test Clients for Device Servers

 Description:   A simple program for executing dev_ping
		indefinitely on a single device.

		Usage : dev_ping [device 1]

 Author:     	A. Goetz

 Original:      July 1997


 *********************************************************************/

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <time.h>
#include <signal.h>

/*
 * global variables
 */

static char device_name[80];
static devserver ping_device;
static long n_total, n_ok, n_notok;
static long t_min, t_max, t_total;

void signal_handler(int signal)
{
	long t_avg, error;
/*
 *
 * ----orion.ctrm.esrf.fr PING Statistics----
 * 4 packets transmitted, 4 packets received, 0% packet loss
 * round-trip (ms)  min/avg/max = 0/0/1
 *
 */

	t_avg = t_total / n_total;

	printf("\n----%s DEV_PING Statistics----\n",device_name);
	printf("%d packets transmitted, %d packets successful, %d errors\n",
                n_total, n_ok, n_notok);
	printf("round-trip (ms) min/avg/max = %d/%d/%d\n",t_min,t_avg,t_max);

	dev_free(ping_device,&error);

	exit(0);
}

main(argc,argv)
unsigned int argc;
char **argv;
{

	long readwrite = 0, error;
	int i, j, n_devices, status;
	short devstatus;
	struct timeval t1, t2;
	long elapsed;

	/*debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);*/


	switch (argc)	{
		case 2:
			strcpy(device_name,argv[1]);
			status = dev_import(device_name,readwrite,&ping_device,&error);
			/*printf("dev_import(%s) returned %d\n",device_name,status);*/
	              	if (status < 0) 
			{
				dev_printerror_no(SEND,NULL,error); 
				exit(1);
			}
			n_devices = 1;

			break;
		default:
			printf("usage: %s [device name]\n",argv[0]);
			exit(1);
	}
        (void) signal(SIGINT,  signal_handler);
	(void) signal(SIGQUIT, signal_handler);
	(void) signal(SIGTERM, signal_handler);

	n_total = n_ok = n_notok = 0;
	t_max = 0;
	t_min = 0;
	sleep(1);
	for (j=0;j<1000000;j++)
	{
		{
			gettimeofday(&t1, NULL);
			status = dev_ping(ping_device,&error);
			gettimeofday(&t2, NULL);
			elapsed = (t2.tv_sec-t1.tv_sec)*1000;
			elapsed = (t2.tv_usec-t1.tv_usec)/1000 + elapsed;

			n_total++;
	              	if (status < 0) 
			{
				printf("dev_ping to %s: status=NOTOK sequence=%d time=%d ms\n",device_name,j,elapsed);
				dev_printerror_no(SEND,NULL,error); 
				n_notok++;
			}
			else
			{
				if (t_max == 0) t_max = elapsed;
				if (elapsed > t_max) t_max = elapsed;
				if (t_min == 0) t_min = elapsed;
				if (elapsed < t_min) t_min = elapsed;
				t_total = t_total + elapsed;
				printf("dev_ping to %s: status=OK sequence=%d time=%d ms\n",device_name,j,elapsed);
				n_ok++;
			}
			sleep(1);
		}

	}
	status = dev_free(ping_device,&error);
}
