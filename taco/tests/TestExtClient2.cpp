#include <iostream>

#include "TACOClient.h"

#include <unistd.h>

void usage(std::string process)
{
	std::cerr << "usage: " << process << " options device_name_1 device_name_2" << std::endl;
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

        if (optind != argc - 2)
		usage(argv[0]);

	std::string device_name_1 = argv[optind];	
	std::string device_name_2 = argv[optind + 1];	

	TACO::Client 	*clnt1,
			*clnt2;

	try
	{
		std::cerr << device_name_1 << " : ";
		clnt1 = new TACO::Client(device_name_1);
		std::cerr << "created." << std::endl;
		std::cerr << clnt1->deviceStatus() << std::endl;
		std::cerr << device_name_2 << " : ";
		clnt2 = new TACO::Client(device_name_2);
		std::cerr << "created." << std::endl;
		std::cerr << clnt2->deviceStatus() << std::endl;
		delete clnt1;
		delete clnt2;
	}
	catch (::TACO::Exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 2;	
	}
	return 0;
}

