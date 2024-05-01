#include "Piece.h"

using namespace std;
using namespace piecetype;


Pos::Pos(int row, int col) : _row(row), _col(col) {}

Pos::Pos(const Pos& other) : _row(other._row), _col(other._col) {}

Pos Pos::operator+(Pos other) {
	return Pos(_row + other._row, _col + other._col);
}


Piece::Piece(Color color, Pos pos) :
	_color(color), _pos(pos)
{ }

void Piece::move(Pos newPos) {
	_pos = newPos;
	// add emission of signal to adjuste the view
}

Piece::~Piece() {}


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
}

string King::getName() const {
	return "King";
}
