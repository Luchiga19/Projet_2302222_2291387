#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>
#include "Chessboard.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QWidget chessboardWindow;
	chessboardWindow.setWindowTitle("Chess");

	interface::Chessboard chessboard(&chessboardWindow);

	QHBoxLayout* layout = new QHBoxLayout(&chessboardWindow);
	layout->addWidget(&chessboard);
	chessboardWindow.setLayout(layout);

	chessboardWindow.show();
	return app.exec();
}