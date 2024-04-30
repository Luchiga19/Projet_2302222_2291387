#include "Chessboard.h"
#include <QWidget>
#include <QGridLayout>
#include "piece.h"
#include <iostream>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>


using namespace std;


interface::Square::Square(QWidget* parent, piecetype::Pos pos) :
	QWidget(parent),
	_piece(make_unique<piecetype::Empty>(piecetype::Empty(pos)))
{
	setFixedSize(SQUARE_SIZE, SQUARE_SIZE);
	_label = new QLabel(this);
	_label->setText(QString::fromStdString(_piece->getName()));
}

void interface::Square::updateSquare() {
	update();
}

void interface::Square::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.fillRect(rect(), _color); // Fill the entire widget area with red
}

interface::Chessboard::Chessboard(QWidget* parent) :
	QWidget(parent)
{
	QGridLayout* layout = new QGridLayout(this);
	layout->setSpacing(0);

	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			Square *square = new Square(this, piecetype::Pos(row, col));
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

interface::Square** interface::Chessboard::operator[] (int i) {
	return _board[i];
}

void interface::Chessboard::populateStandard() {
	using piecetype::King;
	using piecetype::Pos;
	try {
		Square* square = _board[0][3];
		square->_piece = make_unique<King>(King(King::Color::BLACK, Pos(0, 3)));
		square->_label->setText(QString::fromStdString(square->_piece->getName()));
		square = _board[7][3];
		square->_piece = make_unique<King>(King(King::Color::WHITE, Pos(7, 3)));
		square->_label->setText(QString::fromStdString(square->_piece->getName()));
	}

	catch (const piecetype::TooManyKingsException) {
		cout << "Attempting to initialize more kings than permitted" << endl;
		QMessageBox::warning(nullptr, "Warning", "Attempting to initialize more kings than permitted");
	}

	// Continuer l'initialisation des autres pieces.
}
