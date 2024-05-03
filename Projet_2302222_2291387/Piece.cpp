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
}

void Piece::removeValidMove(vector<Pos>& invalidMoves) {
	for (Pos& pos : invalidMoves)
		_validMoves.erase(remove_if(_validMoves.begin(), _validMoves.end(), [&](const Pos& other) { return pos == other; }), _validMoves.end());
}

bool Piece::isInValidMoves(const Pos& pos) {
	return _validMoves.end() != std::find(_validMoves.begin(), _validMoves.end(), pos);
}


int King::_whiteKingCount = 0;
int King::_blackKingCount = 0;

King::King(Color color, Pos pos) : Piece(color, pos) {
	if (color == Color::BLACK && _blackKingCount < 1) {
		_blackKingCount++;
		_image = QPixmap("Images/black_king.png");
	}

	else if (color == Color::WHITE && _whiteKingCount < 1) {
		_whiteKingCount++;
		_image = QPixmap("Images/white_king.png");
	}
	else {
		throw TooManyKingsException("Trying to generate more than a kings per color.");
	}
}

void King::resetKingCounts() {
	_whiteKingCount = 0;
	_blackKingCount = 0;
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

			if (_pos.isValid() && !board.isCheck(*this) && !board[_pos]->isSameColorPiece(*this))
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

void Queen::updateAggroMoves(Chessboard& board) {
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

				if (board[_pos]->isKing() && !board[_pos]->isSameColorPiece(*this)) {
					_pos += addedPos;
					board.insertAggroMove(_pos, getColor());
					break;
				}

				else if (!board[_pos]->isEmpty())
					break;
			}
			_pos = initialPos;
		}
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

				if (!_pos.isValid() || board[_pos]->isSameColorPiece(*this))
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

void Bishop::updateAggroMoves(Chessboard& board) {
	Pos initialPos = _pos;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {

			while (true) {
				Pos addedPos(i, j);
				_pos += addedPos;

				if (!_pos.isValid())
					break;

				board.insertAggroMove(_pos, getColor());

				if (board[_pos]->isKing() && !board[_pos]->isSameColorPiece(*this)) {
					_pos += addedPos;
					board.insertAggroMove(_pos, getColor());
					break;
				}

				else if (!board[_pos]->isEmpty())
					break;
			}
			_pos = initialPos;
		}
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

				if (!_pos.isValid() || board[_pos]->isSameColorPiece(*this))
					break;

				_validMoves.push_back(_pos);

				if (!board[_pos]->isEmpty())
					break;
			}
			_pos = initialPos;
		}
	}
}


const vector<Pos> Knight::_knightMoves = { Pos(1, 2), Pos(2, 1), Pos(2, -1), Pos(1, -2), Pos(-1, -2), Pos(-2, -1), Pos(-2, 1), Pos(-1, 2) };

Knight::Knight(Color color, Pos pos) : Piece(color, pos) {
	if (color == Color::BLACK) {
		_image = QPixmap("Images/black_knight.png");
	}

	else if (color == Color::WHITE) {
		_image = QPixmap("Images/white_knight.png");
	}
}

void Knight::updateAggroMoves(Chessboard& board) {
	Pos initialPos = _pos;

	for (const Pos& addedPos : _knightMoves) {
		_pos += addedPos;

		if (!_pos.isValid()) {
			_pos = initialPos;
			continue;
		}

		board.insertAggroMove(_pos, getColor());
		_pos = initialPos;
	}
}

void Knight::updateValidMoves(Chessboard& board) {
	_validMoves.clear();
	Pos initialPos = _pos;

	for (const Pos& addedPos : _knightMoves) {
		_pos += addedPos;

		if (!_pos.isValid() || board[_pos]->isSameColorPiece(*this)) {
			_pos = initialPos;
			continue;
		}

		_validMoves.push_back(_pos);
		_pos = initialPos;
	}
}


const vector<Pos> Rook::_rookMoves = { Pos(0, 1), Pos(1, 0), Pos(0, -1), Pos(-1, 0) };

Rook::Rook(Color color, Pos pos) : Piece(color, pos) {
	if (color == Color::BLACK) {
		_image = QPixmap("Images/black_rook.png");
	}

	else if (color == Color::WHITE) {
		_image = QPixmap("Images/white_rook.png");
	}
}

void Rook::updateAggroMoves(Chessboard& board) {
	Pos initialPos = _pos;

	for (const Pos& addedPos : _rookMoves) {

		while (true) {
			_pos += addedPos;

			if (!_pos.isValid())
				break;

			board.insertAggroMove(_pos, getColor());

			if (board[_pos]->isKing() && !board[_pos]->isSameColorPiece(*this)) {
				_pos += addedPos;
				board.insertAggroMove(_pos, getColor());
				break;
			}

			else if (!board[_pos]->isEmpty())
				break;
		}
		_pos = initialPos;
	}
}

void Rook::updateValidMoves(Chessboard& board) {
	_validMoves.clear();
	Pos initialPos = _pos;

	for (const Pos& addedPos : _rookMoves) {
		while (true) {
			_pos += addedPos;

			if (!_pos.isValid() || board[_pos]->isSameColorPiece(*this))
				break;

			_validMoves.push_back(_pos);

			if (!board[_pos]->isEmpty())
				break;
		}
		_pos = initialPos;
	}
}


Pawn::Pawn(Color color, Pos pos) : Piece(color, pos) {
	if (color == Color::BLACK) {
		_image = QPixmap("Images/black_pawn.png");
	}

	else if (color == Color::WHITE) {
		_image = QPixmap("Images/white_pawn.png");
	}
}

void Pawn::updateAggroMoves(Chessboard& board) {
	Pos initialPos = _pos;

	int direction = (getColor() == Color::BLACK) ? 1 : -1;

	for (int i = -1; i <= 1; i += 2) {
		_pos += Pos(direction, i);

		if (_pos.isValid()) 
			board.insertAggroMove(_pos, getColor());

		_pos = initialPos;
	}
}

void Pawn::updateValidMoves(Chessboard& board) {
	_validMoves.clear();
	Pos initialPos = _pos;

	int direction = (getColor() == Color::BLACK) ? 1 : -1;

	for (int i = -1; i <= 1; i += 2) {
		_pos += Pos(direction, i);

		if (_pos.isValid() && !board[_pos]->isEmpty() && !board[_pos]->isSameColorPiece(*this))
			_validMoves.push_back(_pos);

		else if (_pos.isValid() && board[_pos]->isEnPassant()) {
			_validMoves.push_back(_pos);
			board[_pos]->enPassant();
		}

		_pos = initialPos;
	}

	_pos += Pos(direction, 0);
	
	if (_pos.isValid() && board[_pos]->isEmpty()) {
		_validMoves.push_back(_pos);

		if ((initialPos.getRow() == 6 && getColor() == Color::WHITE) || (initialPos.getRow() == 1 && getColor() == Color::BLACK)) {
			_pos += Pos(direction, 0);

			if (_pos.isValid() && board[_pos]->isEmpty())
				_validMoves.push_back(_pos);
		}
	}

	_pos = initialPos;
}
