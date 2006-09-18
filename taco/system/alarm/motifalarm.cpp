/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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

#include <Xm/MessageB.h>
#include <string>

std::string	getLastMessage(const std::string);

void	my_exit(Widget, XtPointer, XtPointer)
{
	exit(0);
}

int main(int argc, char **argv)
{
	XtAppContext	a;
	XtSetLanguageProc(NULL, NULL, NULL);

	Widget 	toplevel = XtVaAppInitialize(&a, "Test", NULL, 0, &argc, argv, NULL, NULL);

	if (argc != 5)
		return -1;

	XmString	caption,
			text;
	
	std::string	help(argv[2]);

	help += std::string(" on ") + std::string(argv[3]);

	Widget	dialog = XmCreateErrorDialog(toplevel, "dialog", NULL, 0);
	caption = XmStringCreateLocalized(const_cast<char *>(help.c_str()));
	XtVaSetValues(dialog, XmNdialogTitle, caption, NULL);
	XmStringFree(caption);

	text = XmStringCreateLocalized(const_cast<char *>((help + "\n" + getLastMessage(std::string(argv[4]))).c_str()));
	XtVaSetValues(dialog, XmNmessageString, text, NULL);
	XmStringFree(text);

	XtAddCallback(dialog, XmNokCallback, my_exit, NULL);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	XtManageChild(dialog);
	XtPopup(XtParent(dialog), XtGrabNone);

	XtAppMainLoop(a);
	return 0;
}
