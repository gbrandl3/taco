/*********************************************************************
 *
 * File:        dev_ping.c
 *
 * Project:     Test Clients for Device Servers
 *
 * Description: A simple program for executing dev_ping
 *		indefinitely on a single device.
 *
 *		Usage : dev_ping [device 1]
 *
 * Author:     	A. Goetz
 *		$Author: jkrueger1 $
 *
 * Original:    July 1997
 *
 *********************************************************************/

#include "config.h"
#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <time.h>

#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#error could not find signal.h
#endif

/*
 * global variables
 */

char 		device_name[80];
devserver 	ping_device;
static long 	n_total = 0, 
		n_ok = 0, 
		n_notok = 0;
static double 	t_min = 1e10, 
		t_max = 0, 
		t_total = 0;

void signal_handler(int signal)
{
	long 	error;
	double	t_avg = t_total / (double)n_total;; 
/*
 *
 * ----orion.ctrm.esrf.fr PING Statistics----
 * 4 packets transmitted, 4 packets received, 0% packet loss
 * round-trip (ms)  min/avg/max = 0/0/1
 *
 */
	printf("\n----%s DEV_PING Statistics----\n",device_name);
	printf("%d packets transmitted, %d packets successful, %d errors\n", n_total, n_ok, n_notok);
	printf("round-trip (ms) min/avg/max = %.3f / %.3f / %.3f\n",t_min,t_avg,t_max);

	dev_free(ping_device,&error);
	exit(0);
}

int main(int argc, char **argv)
{
	long 	readwrite = 0, 
		error;
	int 	i, 
		j,
		c, 
		n_devices, 
		n_counts = 1000000,
		status;
	short 	devstatus;
	struct timeval t1, t2;
	double 	elapsed;

/*	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);*/

	while((c = getopt(argc, argv, "c:h")) != -1)
		switch (c)
		{
			case 'c' : 
				n_counts = atoi(optarg);
				break;
			case 'h' :
			case '?' :
				fprintf(stderr, "usage: dev_ping [-c num] <device name>\n");
				exit(1);
		}
				
	if (optind != argc -1)	
	{
		fprintf(stderr, "usage: dev_ping [-c num] <device name>\n");
		exit(1);
	}

	strcpy(device_name,argv[optind]);
	status = dev_import(device_name,readwrite,&ping_device,&error);
	/*printf("dev_import(%s) returned %d\n",device_name,status);*/
	if (status != DS_OK) 
	{
		dev_printerror_no(SEND,NULL,error); 
		exit(1);
	}
	n_devices = 1;

        (void) signal(SIGINT,  signal_handler);
	(void) signal(SIGQUIT, signal_handler);
	(void) signal(SIGTERM, signal_handler);

	for (j = 0; j < n_counts; j++)
	{
		gettimeofday(&t1, NULL);
		status = dev_ping(ping_device,&error);
		gettimeofday(&t2, NULL);
		if (t1.tv_usec > t2.tv_usec)
		{
			t2.tv_usec += 1000000;
			t2.tv_sec--;
		}
		elapsed = (t2.tv_sec-t1.tv_sec) * 1000;
		elapsed += (t2.tv_usec-t1.tv_usec) / 1000.;

		n_total++;
	        if (status < 0) 
		{
			printf("dev_ping to %s: status=NOTOK sequence=%d time=%.3f ms\n",device_name, j, elapsed);
			dev_printerror_no(SEND,NULL,error); 
			n_notok++;
		}
		else
		{
			if (elapsed > t_max) 
				t_max = elapsed;
			if (elapsed < t_min) 
				t_min = elapsed;
			t_total += elapsed;
			printf("dev_ping to %s: status=OK sequence=%d time=%.3f ms %.3f %.3f %.3f\n",device_name, j, elapsed, t_min, t_max, t_total);
			n_ok++;
		}
		sleep(1);
	}
	kill(getpid(), SIGQUIT);
}
