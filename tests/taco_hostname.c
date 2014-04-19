#if HAVE_CONFIG_H
#	include "config.h"
#endif
#include <stdio.h>
#include <private/ApiP.h>

int main(int argc, char **argv)
{
	char hostname[255];

	if (!taco_gethostname(hostname, sizeof(hostname)))
		printf("%s\n", hostname);
	return 0;
}	
