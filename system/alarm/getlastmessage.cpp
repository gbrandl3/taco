/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * File:
 *
 * Description:
 *
 * Authors:
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 21:42:27 $
 *
 */

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

