#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
	
	QFont font;
	font.setFamily("Meiryo UI");
	a.setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}
