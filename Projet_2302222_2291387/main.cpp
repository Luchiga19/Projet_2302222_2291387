#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>
#include "Chessboard.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QWidget window;
	window.setWindowTitle("Chess");

	interface::Chessboard chessboard(&window);

	QHBoxLayout* layout = new QHBoxLayout(&window);
	layout->addWidget(&chessboard);

	window.setLayout(layout);
	chessboard.updateAllValidMoves();

	window.show();
	return app.exec();
}