/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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

#include <qmessagebox.h>
#include <qapplication.h>

#include <string>

std::string getLastMessage(const std::string);

int main(int argc, char **argv)
{
	QApplication	a(argc, argv);

	if (argc != 5)
		return -1;

	QString	caption;

	caption.sprintf("%s on %s", argv[2], argv[3]);
	QString	text(caption + "\n" + getLastMessage(argv[4]).c_str());
	QMessageBox::critical(NULL,  caption, text, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	a.quit();
}


