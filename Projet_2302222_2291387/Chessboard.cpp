#include "Chessboard.h"
#include <QWidget>
#include <QGridLayout>
#include "piece.h"
#include <iostream>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>


using namespace std;
using namespace interface;
using namespace piecetype;


Square::Square(QWidget* parent, Pos pos) :
	_pos(pos),
	QWidget(parent),
	_piece(nullptr)
{
	setFixedSize(SQUARE_SIZE, SQUARE_SIZE);
	_label = new QLabel(this);
	if (_piece != nullptr)
		_label->setText(QString::fromStdString(_piece->getName()));
}

void Square::updateSquare() {
	update();
}

void Square::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(rect(), _color); // Fill the entire widget area with red

	if (_piece != nullptr)
		painter.drawPixmap(rect(), _piece->getImage());
}

Chessboard::Chessboard(QWidget* parent) :
	QWidget(parent)
{
	QGridLayout* layout = new QGridLayout(this);
	layout->setSpacing(0);

	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			Square *square = new Square(this, Pos(row, col));
			square->_color = ((row + col) % 2 == 0 ? Qt::white : QColor(255, 209, 181));
			layout->addWidget(square, row, col);
			_board[row][col] = square;
			square->updateSquare();
		}
	}

	layout->setSizeConstraint(QLayout::SetFixedSize);

	populateStandard();
	
	setLayout(layout);
};

Square** Chessboard::operator[] (int i) {
	return _board[i];
}

void Chessboard::populateStandard() {
	try {
		Square* square = _board[0][3];
		square->_piece = make_unique<King>(King(King::Color::BLACK, Pos(0, 3)));
		square->_label->setText(QString::fromStdString(square->_piece->getName()));
		square = _board[7][3];
		square->_piece = make_unique<King>(King(King::Color::WHITE, Pos(7, 3)));
		square->_label->setText(QString::fromStdString(square->_piece->getName()));
	}

	catch (const TooManyKingsException) {
		cout << "Attempting to initialize more kings than permitted" << endl;
		QMessageBox::warning(nullptr, "Warning", "Attempting to initialize more kings than permitted");
	}

	// Continuer l'initialisation des autres pieces.
}
