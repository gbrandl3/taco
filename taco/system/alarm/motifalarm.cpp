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
