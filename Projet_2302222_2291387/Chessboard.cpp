#include "Chessboard.h"
#include <QObject>
#include <memory>
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
	_contour(QPixmap("Images/contour.png")),
	_pos(pos),
	QWidget(parent),
	_piece(nullptr)
{
	setFixedSize(SQUARE_SIZE, SQUARE_SIZE);
}

void Square::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.fillRect(rect(), _color);

	if (_piece != nullptr)
		painter.drawPixmap(rect(), _piece->getImage());
	
	if (_isContour) 
		painter.drawPixmap(rect(), _contour);
}

void Square::tempMovePiece(Square* square) {
	_piece->move(square->_pos);
	square->_piece = std::move(_piece);
}

void Square::movePiece(Square* square, Chessboard& board) {
	if (square->_piece != nullptr && board._currentPlayer == Piece::Color::WHITE)
		board._blackPieces.erase(remove_if(board._blackPieces.begin(), board._blackPieces.end(),
			[&](shared_ptr<Piece> piece) { return piece == square->_piece; }), board._blackPieces.end());

	else if (square->_piece != nullptr && board._currentPlayer == Piece::Color::BLACK)
		board._whitePieces.erase(remove_if(board._whitePieces.begin(), board._whitePieces.end(),
			[&](shared_ptr<Piece> piece) { return piece == square->_piece; }), board._whitePieces.end());

	tempMovePiece(square);
	square->update();
	update();
}

bool Square::isKing() const {
	return dynamic_cast<King*>(_piece.get());
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


TempCheckMove::TempCheckMove(Square* initialSquare, Square* destinationSquare) :
	_initialSquare(initialSquare), 
	_destinationSquare(destinationSquare), 
	_pieceHolder(destinationSquare->_piece)
{
	_initialSquare->tempMovePiece(_destinationSquare);
}

TempCheckMove::~TempCheckMove() {
	_destinationSquare->tempMovePiece(_initialSquare);
	_destinationSquare->_piece = _pieceHolder;
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
	QWidget(parent), _currentPlayer(Piece::Color::WHITE)
{
	QGridLayout* layout = new QGridLayout(this);
	layout->setSpacing(0);

	King::resetKingCounts();

	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			Square* square = new Square(this, Pos(row, col));
			QObject::connect(square, &Square::squareClicked, this, &Chessboard::onSquareClick);
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

Square* Chessboard::operator[](const Pos& pos) { 
	return _board[pos.getRow()][pos.getCol()]; 
}

void Chessboard::populateStandard() {
	Square* currentSquare;

	try {
		currentSquare = _board[0][3];
		_blackKing = make_shared<King>(King(King::Color::BLACK, currentSquare->_pos));
		currentSquare->_piece = _blackKing;
		_blackPieces.push_back(currentSquare->_piece);

		currentSquare = _board[7][3];
		_whiteKing = make_shared<King>(King(King::Color::WHITE, currentSquare->_pos));
		currentSquare->_piece = _whiteKing;
		_whitePieces.push_back(currentSquare->_piece);
	}

	catch (const TooManyKingsException&) {
		cout << "Attempting to initialize more kings than permitted" << endl;
		QMessageBox::warning(nullptr, "Warning", "Attempting to initialize more kings than permitted");
	}

	// Queens
	currentSquare = _board[0][4];
	currentSquare->_piece = make_shared<Queen>(Queen(Queen::Color::BLACK, currentSquare->_pos));
	_blackPieces.push_back(currentSquare->_piece);

	currentSquare = _board[7][4];
	currentSquare->_piece = make_shared<Queen>(Queen(Queen::Color::WHITE, currentSquare->_pos));
	_whitePieces.push_back(currentSquare->_piece);

	// Bishops
	currentSquare = _board[0][2];
	currentSquare->_piece = make_shared<Bishop>(Bishop(Bishop::Color::BLACK, currentSquare->_pos));
	_blackPieces.push_back(currentSquare->_piece);

	currentSquare = _board[0][5];
	currentSquare->_piece = make_shared<Bishop>(Bishop(Bishop::Color::BLACK, currentSquare->_pos));
	_blackPieces.push_back(currentSquare->_piece);

	currentSquare = _board[7][2];
	currentSquare->_piece = make_shared<Bishop>(Bishop(Bishop::Color::WHITE, currentSquare->_pos));
	_whitePieces.push_back(currentSquare->_piece);

	currentSquare = _board[7][5];
	currentSquare->_piece = make_shared<Bishop>(Bishop(Bishop::Color::WHITE, currentSquare->_pos));
	_whitePieces.push_back(currentSquare->_piece);

	// Knights
	currentSquare = _board[0][1];
	currentSquare->_piece = make_shared<Knight>(Knight(Knight::Color::BLACK, currentSquare->_pos));
	_blackPieces.push_back(currentSquare->_piece);

	currentSquare = _board[0][6];
	currentSquare->_piece = make_shared<Knight>(Knight(Knight::Color::BLACK, currentSquare->_pos));
	_blackPieces.push_back(currentSquare->_piece);

	currentSquare = _board[7][1];
	currentSquare->_piece = make_shared<Knight>(Knight(Knight::Color::WHITE, currentSquare->_pos));
	_whitePieces.push_back(currentSquare->_piece);

	currentSquare = _board[7][6];
	currentSquare->_piece = make_shared<Knight>(Knight(Knight::Color::WHITE, currentSquare->_pos));
	_whitePieces.push_back(currentSquare->_piece);

	// Rooks
	currentSquare = _board[0][0];
	currentSquare->_piece = make_shared<Rook>(Rook(Rook::Color::BLACK, currentSquare->_pos));
	_blackPieces.push_back(currentSquare->_piece);

	currentSquare = _board[0][7];
	currentSquare->_piece = make_shared<Rook>(Rook(Rook::Color::BLACK, currentSquare->_pos));
	_blackPieces.push_back(currentSquare->_piece);

	currentSquare = _board[7][0];
	currentSquare->_piece = make_shared<Rook>(Rook(Rook::Color::WHITE, currentSquare->_pos));
	_whitePieces.push_back(currentSquare->_piece);

	currentSquare = _board[7][7];
	currentSquare->_piece = make_shared<Rook>(Rook(Rook::Color::WHITE, currentSquare->_pos));
	_whitePieces.push_back(currentSquare->_piece);

	// Pawn
	for (int i = 0; i < BOARD_SIZE; i++) {
		currentSquare = _board[1][i];
		currentSquare->_piece = make_shared<Pawn>(Pawn(Pawn::Color::BLACK, currentSquare->_pos));
		_blackPieces.push_back(currentSquare->_piece);
	}

	for (int i = 0; i < BOARD_SIZE; i++) {
		currentSquare = _board[6][i];
		currentSquare->_piece = make_shared<Pawn>(Pawn(Pawn::Color::WHITE, currentSquare->_pos));
		_whitePieces.push_back(currentSquare->_piece);
	}
}

bool Chessboard::isCheck(const King& king) const {
	if (king.getColor() == Piece::Color::WHITE)
		return _validBlackAggroMoves.end() != std::find(_validBlackAggroMoves.begin(), _validBlackAggroMoves.end(), king.getPos());

	else if (king.getColor() == Piece::Color::BLACK)
		return _validWhiteAggroMoves.end() != std::find(_validWhiteAggroMoves.begin(), _validWhiteAggroMoves.end(), king.getPos());
}

void Chessboard::insertAggroMove(const Pos& pos, const Piece::Color& color) {
	if (color == Piece::Color::WHITE) {
		_validWhiteAggroMoves.insert(pos);
	}

	else if (color == Piece::Color::BLACK) {
		_validBlackAggroMoves.insert(pos);
	}
}

void Chessboard::resetAggroMoves() {
	_validBlackAggroMoves.clear();
	_validWhiteAggroMoves.clear();
}

void Chessboard::updateTurnMoves() {
	shared_ptr<King> teamKing = (_currentPlayer == Piece::Color::WHITE) ? _whiteKing : _blackKing;

	vector<shared_ptr<Piece>>& teamPieces = (_currentPlayer == Piece::Color::WHITE) ? _whitePieces : _blackPieces;
	vector<shared_ptr<Piece>>& enemyPieces = (_currentPlayer == Piece::Color::WHITE) ? _blackPieces : _whitePieces;

	for (shared_ptr<Piece> piece : enemyPieces)
		piece->updateAggroMoves(*this);

	for (shared_ptr<Piece> piece : teamPieces) {
		piece->updateValidMoves(*this);
		vector<Pos> invalidMoves;
		for (Pos& pos : piece->getValidMoves()) {
			
			TempCheckMove tempMove((*this)[piece->getPos()], (*this)[pos]);
			resetAggroMoves();
			for (shared_ptr<Piece> piece : enemyPieces)
				if (piece->getPos() != pos)
					piece->updateAggroMoves(*this);
			if (isCheck(*teamKing))
				invalidMoves.push_back(pos);
		}
		if (!invalidMoves.empty())
			piece->removeValidMove(invalidMoves);
	}
}

void Chessboard::setHighlightValidMoves(bool set) {
	for (const Pos& pos : _sourceSquare->_piece->getValidMoves()) {
		(*this)[pos]->_isContour = set;
		(*this)[pos]->update();
	}
}

void Chessboard::onSquareClick(Square* square) {
	if (_sourceSquare == nullptr && !square->isEmpty() && square->_piece->getColor() == _currentPlayer) {
		_sourceSquare = square;
		setHighlightValidMoves(true);
	}

	else if (_sourceSquare != nullptr && !square->isEmpty() && square->_piece->getColor() == _currentPlayer) {
		setHighlightValidMoves(false);
		_sourceSquare = square;
		setHighlightValidMoves(true);
	}

	else if (_sourceSquare == square) {
		setHighlightValidMoves(false);
		_sourceSquare = nullptr;
	}

	else if (_sourceSquare != nullptr && _sourceSquare->_piece->isInValidMoves(square->_pos)) {
		setHighlightValidMoves(false);
		_sourceSquare->movePiece(square, *this);
		_sourceSquare = nullptr;
		_currentPlayer = (_currentPlayer == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;
		resetAggroMoves();
		updateTurnMoves();
	}
}
