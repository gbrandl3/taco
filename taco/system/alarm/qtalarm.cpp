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


