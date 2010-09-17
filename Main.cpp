#include <QApplication>
#include <QTextCodec>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QTextCodec::setCodecForTr    (QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	app.addLibraryPath(".");

	MainWindow window;
	window.showMaximized();

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}
