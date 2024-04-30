#include "Piece.h"

using namespace std;


piecetype::Pos::Pos(int row, int col) : _row(row), _col(col) {}

piecetype::Pos::Pos(const Pos& other) : _row(other._row), _col(other._col) {}

piecetype::Pos piecetype::Pos::operator+(Pos other) {
	return Pos(_row + other._row, _col + other._col);
}


piecetype::Piece::Piece(Color color, Pos pos) :
	_color(color), _pos(pos)
{
	
}

void piecetype::Piece::move(Pos newPos) {
	_pos = newPos;
	// add emission of signal to adjuste the view
}

piecetype::Piece::~Piece() {}


piecetype::King::King(Color color, Pos pos) : Piece(color, pos) {
	if (_kingCount < 2) {
		_kingCount++;
	}
	else {
		throw TooManyKingsException("Trying to generate more than 2 kings.");
	}
}

string piecetype::King::getName() {
	return "King";
}


piecetype::Empty::Empty(Pos pos) : Piece(Color::NO_COLOR, pos) {}

string piecetype::Empty::getName() {
	return "Empty";
}
