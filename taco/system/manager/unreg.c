#include <rpc/rpc.h>

int main (int argc, char **argv)
{
	int  i;
	unsigned long nu = 0;

	for (i=1; i<argc; i++)
	{
		sscanf (argv[i],"%d", &nu); /* Flawfinder: ignore */
		if (nu > 0)
		{
			pmap_unset(nu, 1L);
			pmap_unset(nu, 2L);
			pmap_unset(nu, 3L);
			pmap_unset(nu, 4L);
			pmap_unset(nu, 5L);
		}
	}
	return 0;
}
