#include <rpc/rpc.h>

int main (int argc, char **argv)
{
	int  i;
	long nu = 0;

	for (i=1; i<argc; i++)
	   {
	   sscanf (argv[i],"%d",&nu);
	   pmap_unset (nu,1);
	   pmap_unset (nu,2);
	   pmap_unset (nu,3);
	   pmap_unset (nu,4);
	   }
	return 0;
}
