/*
  Toolkit for building distributed control systems or any other distributed system.
 
  Copyright (c) 2003-2014 Jens Krüger <jkrueger1@users.sf.net>
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>
#include "TACOClient.h"

#include <unistd.h>
#include <vector>

void usage(std::string process)
{
	std::cerr << "usage: " << process << " options <device name>" << std::endl;
	std::cerr << "      -c num - number of repeats" << std::endl;
	exit(1);
}

class TestClient : public TACO::Client
{
public:
	TestClient(const std::string &name)
		: TACO::Client(name)
	{
	}

	std::vector<DevLong> read(void)
	{
		return execute<std::vector<DevLong> >(539574274L);
	}
};

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

	TestClient *clnt;

	try
	{
		clnt = new TestClient(device_name);
		std::vector<DevLong> res = clnt->read();
		delete clnt;
		std::cout << "Len : " << res.size() << std::endl;
	}
	catch (::TACO::Exception)
	{
		return 2;	
	}
	return 0;
}

