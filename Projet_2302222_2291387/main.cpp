#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include "MainMenu.h"
#include "Chessboard.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QWidget mainWindow;

	QVBoxLayout* mainLayout = new QVBoxLayout(&mainWindow);

	MainMenu menu(&mainWindow);
	mainLayout->addWidget(&menu);

	mainWindow.show();

	return app.exec();
}