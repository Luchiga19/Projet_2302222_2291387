#include "Piece.h"
#include "Chessboard.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>


using namespace std;
using namespace piecetype;
using namespace interface;


Pos::Pos(int row, int col) : _row(row), _col(col) {}

Pos::Pos(const Pos& other) : _row(other._row), _col(other._col) {}

Pos Pos::operator+(const Pos& other) const {
	return Pos(_row + other._row, _col + other._col);
}

void Pos::operator+=(const Pos & other) {
	_row += other._row;
	_col += other._col;
}

bool Pos::operator==(const Pos& other) const {
	return _row == other._row && _col == other._col;
}

bool Pos::operator!=(const Pos& other) const {
	return !(*this == other);
}

bool Pos::operator<(const Pos& other) const {
	return _row < other._row || (_row == other._row && _col < other._col);
}

Pos& Pos::operator=(const Pos& other) {
	if (*this != other) {
		_row = other._row;
		_col = other._col;
	}
	return *this;
}

bool Pos::isValid() const {
	return _row >= 0 && _row < 8 && _col >= 0 && _col < 8;
}


Piece::Piece(Color color, Pos pos) :
	_color(color), _pos(pos)
{ }

void Piece::move(Pos newPos) {
	_pos = newPos;
	// add emission of signal to adjuste the view
}


King::King(Color color, Pos pos) : Piece(color, pos) {
	if (_kingCount < 2) {
		_kingCount++;
	}
	else {
		throw TooManyKingsException("Trying to generate more than 2 kings.");
	}

	if (color == Color::BLACK) {
		_image = QPixmap("Images/black_king.png");
	}

	else if (color == Color::WHITE) {
		_image = QPixmap("Images/white_king.png");
	}
}

void King::updateAggroMoves(interface::Chessboard& board) {
	Pos initialPos = _pos;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {

			if (i == 0 && j == 0)
				continue;

			Pos addedPos(i, j);
			_pos += addedPos;

			if (_pos.isValid())
				board.insertAggroMove(_pos, getColor());

			_pos = initialPos;
		}
	}
}

void King::updateValidMoves(interface::Chessboard& board) {
	_validMoves.clear();
	Pos initialPos = _pos;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {

			if (i == 0 && j == 0)
				continue;

			Pos addedPos(i, j);
			_pos += addedPos;

			if (_pos.isValid() && !board.isPosInAggroMoves(_pos, getColor()) && !board[_pos]->isSameColorPiece(*this))
				_validMoves.push_back(_pos);

			_pos = initialPos;
		}
	}
}


Queen::Queen(Color color, Pos pos) : Piece(color, pos) {
	if (color == Color::BLACK) {
		_image = QPixmap("Images/black_queen.png");
	}

	else if (color == Color::WHITE) {
		_image = QPixmap("Images/white_queen.png");
	}
}

void Queen::updateValidMoves(Chessboard& board) {
	_validMoves.clear();
	Pos initialPos = _pos;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {

			if (i == 0 && j == 0)
				continue;

			while (true) {
				Pos addedPos(i, j);
				_pos += addedPos;

				if (!_pos.isValid())
					break;

				board.insertAggroMove(_pos, getColor());

				if (board[_pos]->isSameColorPiece(*this))
					break;

				_validMoves.push_back(_pos);

				if (!board[_pos]->isEmpty())
					break;
			}
			_pos = initialPos;
		}
	}
}


Bishop::Bishop(Color color, Pos pos) : Piece(color, pos) {
	if (color == Color::BLACK) {
		_image = QPixmap("Images/black_bishop.png");
	}

	else if (color == Color::WHITE) {
		_image = QPixmap("Images/white_bishop.png");
	}
}

void Bishop::updateValidMoves(Chessboard& board) {
	_validMoves.clear();
	Pos initialPos = _pos;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {

			while (true) {
				Pos addedPos(i, j);
				_pos += addedPos;

				if (!_pos.isValid())
					break;

				board.insertAggroMove(_pos, getColor());

				if (board[_pos]->isSameColorPiece(*this))
					break;

				_validMoves.push_back(_pos);

				if (!board[_pos]->isEmpty())
					break;
			}
			_pos = initialPos;
		}
	}
}
