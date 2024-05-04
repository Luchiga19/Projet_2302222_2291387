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
#include "PromotionDialog.h"
#include <vector>
#include <algorithm>


using namespace std;
using namespace interface;
using namespace piecetype;


Square::Square(QWidget* parent, Pos pos) :
	_contour(QPixmap("Images/contour.png")),
	_check(QPixmap("Images/check.png")),
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

	if (_isCheck)
		painter.drawPixmap(rect(), _check);
}

void Square::tempMovePiece(Square* square) {
	_piece->move(square->_pos);
	square->_piece = std::move(_piece);
}

void Square::movePiece(Square* square, Chessboard& board) {
	if (square->_piece != nullptr && board._currentPlayer == Piece::Color::WHITE)
		square->erasePiece(board._blackPieces);

	else if (square->_piece != nullptr && board._currentPlayer == Piece::Color::BLACK)
		square->erasePiece(board._whitePieces);

	else if (square->isEnPassant() && dynamic_cast<Pawn*>(_piece.get())) {
		if (_piece->getColor() == Piece::Color::WHITE)
			board[square->_pos + Pos(1, 0)]->erasePiece(board._blackPieces);
		else if(_piece->getColor() == Piece::Color::BLACK)
			board[square->_pos + Pos(-1, 0)]->erasePiece(board._whitePieces);
	}

	tempMovePiece(square);
	square->update();
	update();
}

void Square::erasePiece(vector<shared_ptr<Piece>>& pieceList) {
	pieceList.erase(remove_if(pieceList.begin(), pieceList.end(), [&](shared_ptr<Piece> other) { return other == _piece; }), pieceList.end());
	_piece.reset();
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
			connect(square, &Square::squareClicked, this, &Chessboard::onSquareClick);
			square->_color = ((row + col) % 2 == 0 ? Qt::white : QColor(255, 209, 181));
			layout->addWidget(square, row, col);
			_board[row][col] = square;
		}
	}

	layout->setSizeConstraint(QLayout::SetFixedSize);

	for (int col = 0; col < BOARD_SIZE; col++) {
		QString labelText = QString("%1 ").arg(QChar('a' + col));
		QLabel* label = new QLabel(labelText, this);
		
		label->setFont(QFont("Arial", 11, QFont::Bold));
		label->setStyleSheet((1 + col) % 2 == 0 ? "color: rgb(255, 209, 181);" : "color: white;");

		layout->addWidget(label, 7, col, Qt::AlignBottom | Qt::AlignRight);
	}

	for (int row = 0; row < BOARD_SIZE; row++) {
		QString labelText = QString(" %1").arg(8 - row);
		QLabel* label = new QLabel(labelText, this);

		label->setFont(QFont("Arial", 11, QFont::Bold));
		label->setStyleSheet((row) % 2 == 0 ? "color: rgb(255, 209, 181);" : "color: white;");

		layout->addWidget(label, row, 0, Qt::AlignTop | Qt::AlignLeft);
	}

	setLayout(layout);
};

Square** Chessboard::operator[] (int i) {
	return _board[i];
}

Square* Chessboard::operator[](const Pos& pos) { 
	return _board[pos.getRow()][pos.getCol()]; 
}

template<typename T>
void Chessboard::addPiece(Square* square, Piece::Color color) {
	shared_ptr<Piece> piece = make_shared<T>(T(color, square->_pos));
	square->_piece = piece;
	
	if (color == Piece::Color::WHITE) {
		_whitePieces.push_back(piece);
		if (square->isKing())
			_whiteKing = dynamic_pointer_cast<King>(piece);
	}

	else if (color == Piece::Color::BLACK) {
		_blackPieces.push_back(piece);
		if (square->isKing())
			_blackKing = dynamic_pointer_cast<King>(piece);
	}
}

void Chessboard::clearBoard() {
	for (Square* square : *this)
		square->_piece.reset();

	_currentPlayer = Piece::Color::WHITE;
	_sourceSquare = nullptr;

	if (_enPassantCheckWhite != nullptr)
		*_enPassantCheckWhite = false;
	_enPassantCheckWhite = nullptr;

	if (_enPassantCheckBlack != nullptr)
		*_enPassantCheckBlack = false;
	_enPassantCheckBlack = nullptr;

	_checkPos.reset();

	_whiteKing.reset();
	_blackKing.reset();

	_whitePieces.clear();
	_blackPieces.clear();

	_validBlackAggroMoves.clear();
	_validWhiteAggroMoves.clear();

	King::resetKingCounts();
}

void Chessboard::populateStandard() {
	clearBoard();

	try {
		addPiece<King>(_board[0][3], Piece::Color::BLACK);

		addPiece<King>(_board[7][3], Piece::Color::WHITE);
	}

	catch (const TooManyKingsException&) {
		cout << "Attempting to initialize more kings than permitted" << endl;
		QMessageBox::warning(nullptr, "Warning", "Attempting to initialize more kings than permitted");
	}

	// Queens
	addPiece<Queen>(_board[0][4], Piece::Color::BLACK);

	addPiece<Queen>(_board[7][4], Piece::Color::WHITE);

	// Bishops
	addPiece<Bishop>(_board[0][2], Piece::Color::BLACK);

	addPiece<Bishop>(_board[0][5], Piece::Color::BLACK);

	addPiece<Bishop>(_board[7][2], Piece::Color::WHITE);

	addPiece<Bishop>(_board[7][5], Piece::Color::WHITE);

	// Knights
	addPiece<Knight>(_board[0][1], Piece::Color::BLACK);

	addPiece<Knight>(_board[0][6], Piece::Color::BLACK);

	addPiece<Knight>(_board[7][1], Piece::Color::WHITE);

	addPiece<Knight>(_board[7][6], Piece::Color::WHITE);

	// Rooks
	addPiece<Rook>(_board[0][0], Piece::Color::BLACK);

	addPiece<Rook>(_board[0][7], Piece::Color::BLACK);

	addPiece<Rook>(_board[7][0], Piece::Color::WHITE);

	addPiece<Rook>(_board[7][7], Piece::Color::WHITE);

	// Pawn
	for (int i = 0; i < BOARD_SIZE; i++) {
		addPiece<Pawn>(_board[1][i], Piece::Color::BLACK);
	}

	for (int i = 0; i < BOARD_SIZE; i++) {
		addPiece<Pawn>(_board[6][i], Piece::Color::WHITE);
	}

	updateTurnMoves();
}

void Chessboard::populateEndgame() {
	clearBoard();

	addPiece<King>(_board[4][2], Piece::Color::BLACK);

	addPiece<King>(_board[5][6], Piece::Color::WHITE);

	addPiece<Rook>(_board[4][1], Piece::Color::BLACK);

	addPiece<Rook>(_board[0][4], Piece::Color::WHITE);

	addPiece<Bishop>(_board[1][4], Piece::Color::BLACK);

	addPiece<Bishop>(_board[1][1], Piece::Color::BLACK);

	addPiece<Bishop>(_board[4][5], Piece::Color::WHITE);

	updateTurnMoves();
}

bool Chessboard::isCheck(const King& king) const {
	if (king.getColor() == Piece::Color::WHITE)
		return _validBlackAggroMoves.end() != std::find(_validBlackAggroMoves.begin(), _validBlackAggroMoves.end(), king.getPos());

	else if (king.getColor() == Piece::Color::BLACK)
		return _validWhiteAggroMoves.end() != std::find(_validWhiteAggroMoves.begin(), _validWhiteAggroMoves.end(), king.getPos());
}

void Chessboard::checkIfIsPromotion() {
	if (dynamic_cast<Pawn*>(_sourceSquare->_piece.get()) && ((_sourceSquare->_piece->getColor() == Piece::Color::WHITE && _sourceSquare->_pos.getRow() == 0) ||
		(_sourceSquare->_piece->getColor() == Piece::Color::BLACK && _sourceSquare->_pos.getRow() == 7))) {
		PromotionDialog* promoter = new PromotionDialog(this, _sourceSquare->_piece->getColor());

		connect(promoter, &PromotionDialog::queenSelected, this, &Chessboard::queenPromotion);
		connect(promoter, &PromotionDialog::rookSelected, this, &Chessboard::rookPromotion);
		connect(promoter, &PromotionDialog::bishopSelected, this, &Chessboard::bishopPromotion);
		connect(promoter, &PromotionDialog::knightSelected, this, &Chessboard::knightPromotion);

		promoter->exec();

		promoter->deleteLater();
	}
	else
		_sourceSquare = nullptr;
}

void Chessboard::checkIfEnPassant(Square* square) {
	if (dynamic_cast<Pawn*>(_sourceSquare->_piece.get())) {
		int direction = (_sourceSquare->_piece->getColor() == Piece::Color::WHITE) ? -2 : 2;
		bool*& enPassantCheck = (_sourceSquare->_piece->getColor() == Piece::Color::WHITE) ? _enPassantCheckWhite : _enPassantCheckBlack;
		if (_sourceSquare->_pos + Pos(direction, 0) == square->_pos) {
			enPassantCheck = &(*this)[_sourceSquare->_pos + Pos(direction / 2, 0)]->_isEnPassant;
			*enPassantCheck = true;
		}
	}
}

void Chessboard::removeEnPassantCheck() {
	bool*& enPassantCheck = (_currentPlayer == Piece::Color::WHITE) ? _enPassantCheckWhite : _enPassantCheckBlack;
	if (enPassantCheck != nullptr) {
		*enPassantCheck = false;
		enPassantCheck = nullptr;
	}
}

void Chessboard::checkIfGameEnded(vector<shared_ptr<Piece>>& teamPieces, shared_ptr<King> teamKing) {
	for (shared_ptr<Piece> piece : teamPieces) {
		if (!piece->getValidMoves().empty())
			return;
	}

	if (isCheck(*teamKing) && _currentPlayer == Piece::Color::WHITE)
		QMessageBox::information(this, "Checkmate!", "Black pieces win the game!");

	else if (isCheck(*teamKing) && _currentPlayer == Piece::Color::BLACK)
		QMessageBox::information(this, "Checkmate!", "White pieces win the game!");

	else
		QMessageBox::information(this, "Stalemate!", "It's a draw!");

	emit gameEnded();
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
			for (shared_ptr<Piece> enemyPiece : enemyPieces)
				if (enemyPiece->getPos() != pos)
					enemyPiece->updateAggroMoves(*this);
			if (isCheck(*teamKing))
				invalidMoves.push_back(pos);
		}
		if (!invalidMoves.empty())
			piece->removeValidMove(invalidMoves);
	}

	if (isCheck(*teamKing))
		setHighlightCheck(*teamKing);

	checkIfGameEnded(teamPieces, teamKing);
}

void Chessboard::setHighlightCheck(const piecetype::King& king) {
	_checkPos = make_shared<Pos>(Pos(king.getPos()));
	Square* square = (*this)[*_checkPos];
	square->_isCheck = true;
	square->update();
}

void Chessboard::removeHighlightCheck() {
	if (_checkPos != nullptr) {
		Square* square = (*this)[*_checkPos];
		square->_isCheck = false;
		square->update();
		_checkPos.reset();
	}
}

void Chessboard::setHighlightValidMoves(bool set) {
	for (const Pos& pos : _sourceSquare->_piece->getValidMoves()) {
		(*this)[pos]->_isContour = set;
		(*this)[pos]->update();
	}
}

template<typename T>
void Chessboard::piecePromotion() {
	vector<shared_ptr<Piece>>& teamPieces = (_currentPlayer == Piece::Color::WHITE) ? _whitePieces : _blackPieces;
	_sourceSquare->erasePiece(teamPieces);
	_sourceSquare->_piece = make_shared<T>(T(_currentPlayer, _sourceSquare->_pos));
	teamPieces.push_back(_sourceSquare->_piece);
	_sourceSquare = nullptr;
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
		removeEnPassantCheck();
		checkIfEnPassant(square);
		_sourceSquare->movePiece(square, *this);
		removeHighlightCheck();
		_sourceSquare = square;
		checkIfIsPromotion();
		_currentPlayer = (_currentPlayer == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;
		resetAggroMoves();
		updateTurnMoves();
	}
}

void Chessboard::queenPromotion() {
	piecePromotion<Queen>();
}

void Chessboard::rookPromotion() {
	piecePromotion<Rook>();
}

void Chessboard::bishopPromotion() {
	piecePromotion<Bishop>();
}

void Chessboard::knightPromotion() {
	piecePromotion<Knight>();
}
