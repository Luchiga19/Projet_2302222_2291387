#include "Chessboard.h"
#include <QObject>
#include <QMouseEvent>
#include <QWidget>
#include <QGridLayout>
#include "piece.h"
#include <iostream>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>
#include <vector>
#include <algorithm>


using namespace std;
using namespace interface;
using namespace piecetype;


Square::Square(QWidget* parent, Pos pos) :
	_pos(pos),
	QWidget(parent),
	_piece(nullptr)
{
	setFixedSize(SQUARE_SIZE, SQUARE_SIZE);
}

void Square::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.fillRect(rect(), _color); // Fill the entire widget area with red

	if (_piece != nullptr)
		painter.drawPixmap(rect(), _piece->getImage());
}

bool Square::isEmpty() const {
	return _piece == nullptr;
}

bool Square::isSameColorPiece(const Piece& other) const {
	if (!isEmpty())
		return _piece->getColor() == other.getColor();
	return false;
}

void Square::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)
		emit squareClicked(this);
}


Chessboard::iterator::iterator(Square* (*ptr)[BOARD_SIZE], int row, int col)
	: _row(row), _col(col) 
{
	_ptr = ptr;
}

Chessboard::iterator& Chessboard::iterator::operator++() {
	_col++;
	if (_col >= BOARD_SIZE) {
		_col = 0;
		_row++;
	}
	return *this;
}

Square*& Chessboard::iterator::operator*() {
	return _ptr[_row][_col];
}

bool Chessboard::iterator::operator==(const iterator& it) {
	return _ptr == it._ptr && _row == it._row && _col == it._col;
}

bool Chessboard::iterator::operator!=(const iterator& it) {
	return !(*this == it);
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
		}
	}

	layout->setSizeConstraint(QLayout::SetFixedSize);

	populateStandard();
	
	setLayout(layout);
};

Square** Chessboard::operator[] (int i) {
	return _board[i];
}

Square* Chessboard::operator[](const Pos& pos) const { 
	return _board[pos.getRow()][pos.getCol()]; 
}

void Chessboard::populateStandard() {
	try {
		Square* currentSquare = _board[0][3];
		currentSquare->_piece = make_unique<King>(King(King::Color::BLACK, currentSquare->_pos));
		currentSquare = _board[7][3];
		currentSquare->_piece = make_unique<King>(King(King::Color::WHITE, currentSquare->_pos));
		currentSquare = _board[0][4];
		currentSquare->_piece = make_unique<Queen>(Queen(Queen::Color::BLACK, currentSquare->_pos));
		currentSquare = _board[7][4];
		currentSquare->_piece = make_unique<Queen>(Queen(Queen::Color::WHITE, currentSquare->_pos));
	}

	catch (const TooManyKingsException) {
		cout << "Attempting to initialize more kings than permitted" << endl;
		QMessageBox::warning(nullptr, "Warning", "Attempting to initialize more kings than permitted");
	}

	// Continuer l'initialisation des autres pieces.
}

bool Chessboard::isPosInAggroMoves(const piecetype::Pos& pos, const piecetype::Piece::Color& color) const {
	if (color == Piece::Color::WHITE) {
		set<Pos>::iterator it = std::find(_validBlackAggroMoves.begin(), _validBlackAggroMoves.end(), pos);
		return !(it == _validBlackAggroMoves.end());
	}
	else if (color == Piece::Color::BLACK) {
		set<Pos>::iterator it = std::find(_validWhiteAggroMoves.begin(), _validWhiteAggroMoves.end(), pos);
		return !(it == _validWhiteAggroMoves.end());
	}
}

void Chessboard::insertAggroMove(const Pos& pos, const Piece::Color& color) {
	if (color == Piece::Color::WHITE) {
		_validWhiteAggroMoves.insert(pos);
	}

	else if (color == Piece::Color::BLACK) {
		_validBlackAggroMoves.insert(pos);
	}
}

void Chessboard::updateAllValidMoves() {
	vector<King*> kings;
	for (Square* currentSquare : *this) {
		if (dynamic_cast<King*>(currentSquare->_piece.get())) {
			King* king = dynamic_cast<King*>(currentSquare->_piece.get());
			king->updateAggroMoves(*this);
			kings.push_back(king);
		}
		else if (currentSquare->_piece != nullptr)
			currentSquare->_piece->updateValidMoves(*this);
	}

	kings[0]->updateValidMoves(*this);
	kings[1]->updateValidMoves(*this);
}
