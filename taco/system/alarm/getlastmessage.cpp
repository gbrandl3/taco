#include <string>
#include <vector>
#include <iostream>
#include <fstream>

std::string	getLastMessage(const std::string fileName)
{
	std::ifstream			file(fileName.c_str());
	std::vector<std::string>	list;
	if (file)
	{
		std::string s;
		while(std::getline(file, s))
			if (!s.empty())
				list.push_back(s);
		return  list[list.size() - 2] + "\n" + list[list.size() - 1];

	}
	else
		return fileName;
}

