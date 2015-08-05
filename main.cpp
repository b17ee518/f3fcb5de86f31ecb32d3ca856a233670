#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
//	QApplication::setStyle(QStyleFactory::create("Fusion"));

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
	
	QFont font;
	font.setFamily("Meiryo UI");
	a.setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}
