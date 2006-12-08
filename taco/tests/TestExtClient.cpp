#include <iostream>

#include "TACOClient.h"

#include <unistd.h>

void usage(std::string process)
{
	std::cerr << "usage: " << process << " options <device name>" << std::endl;
	std::cerr << "      -c num - number of repeats" << std::endl;
	exit(1);
}

int main(int argc, char **argv)
{
	int c;
	int n_counts;
	while((c = getopt(argc, argv, "c:h")) != -1)
		switch (c)
		{
			case 'c' :
                                n_counts = atoi(optarg);
                                break;
                        case 'h' :
			case '?' :
				usage(argv[0]);
                }

        if (optind != argc -1)
		usage(argv[0]);

	std::string device_name = argv[optind];	

	TACO::Client *clnt;

	try
	{
		clnt = new TACO::Client(device_name);
		delete clnt;
	}
	catch (::TACO::Exception)
	{
		return 2;	
	}
	return 0;
}

