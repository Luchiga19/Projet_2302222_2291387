#pragma once

#include <QObject>
#include <QMouseEvent>
#include <QWidget>
#include <memory>
#include "Piece.h"
#include <QLabel>
#include <span>
#include <QPixmap>
#include <stdexcept>
#include <set>


namespace interface {

	class Square : public QWidget {
		Q_OBJECT

	public:
		friend class Chessboard;
		friend class TempCheckMove;

		static constexpr int SQUARE_SIZE = 80;

		Square(QWidget* parent, piecetype::Pos pos);
		void paintEvent(QPaintEvent* event) override;

		void tempMovePiece(Square* square);
		void movePiece(Square* square, Chessboard& board);

		bool isKing() const;
		bool isEmpty() const;
		bool isSameColorPiece(const piecetype::Piece& other) const;

	signals:
		void squareClicked(Square* square);

	protected:
		void mousePressEvent(QMouseEvent* event) override;

	private:
		bool _isContour = false;

		QPixmap _contour;
		piecetype::Pos _pos;
		std::shared_ptr<piecetype::Piece> _piece;
		QColor _color;
	};


	class TempCheckMove {
	public:
		TempCheckMove(Square* initialSquare, Square* destinationSquare);
		~TempCheckMove();

	private:
		Square* _initialSquare;
		Square* _destinationSquare;
		std::shared_ptr<piecetype::Piece> _pieceHolder;
	};


	class Chessboard : public QWidget {
		Q_OBJECT

	public:
		friend class Square;
		static constexpr int BOARD_SIZE = 8;

		class iterator {
		public: 
			iterator(Square* (*ptr)[BOARD_SIZE], int row = 0, int col = 0);

			iterator& operator++();
			Square*& operator*();
			bool operator==(const iterator& it);
			bool operator!=(const iterator& it);

		private:
			Square* (*_ptr)[BOARD_SIZE];
			int _row, _col;
		};

		iterator begin() {
			return iterator(_board);
		}

		iterator end() {
			return iterator(_board, BOARD_SIZE, 0);
		}

		Chessboard(QWidget* parent);

		Square** operator[](int i);
		Square* operator[](const piecetype::Pos& pos);

		template<typename T>
		void addPiece(Square* square, piecetype::Piece::Color color);

		void clearBoard();
		void populateStandard();
		void populateEndgame();

		bool isCheck(const piecetype::King& king) const;
		void checkIfGameEnded(std::vector<std::shared_ptr<piecetype::Piece>>& teamPieces, std::shared_ptr<piecetype::King> teamKing);

		void insertAggroMove(const piecetype::Pos& pos, const piecetype::Piece::Color& color);
		void resetAggroMoves();
		void updateTurnMoves();

		void setHighlightValidMoves(bool set);

	signals:
		void gameEnded();

		void queenPromotion();
		void rookPromotion();
		void bishopPromotion();
		void knightPromotion();

	public slots:
		void onSquareClick(Square* square);

	private:
		piecetype::Piece::Color _currentPlayer;
		Square* _sourceSquare = nullptr;

		std::shared_ptr<piecetype::King> _whiteKing;
		std::shared_ptr<piecetype::King> _blackKing;

		std::vector<std::shared_ptr<piecetype::Piece>> _whitePieces;
		std::vector<std::shared_ptr<piecetype::Piece>> _blackPieces;

		std::set<piecetype::Pos> _validWhiteAggroMoves;
		std::set<piecetype::Pos> _validBlackAggroMoves;
		Square* _board[BOARD_SIZE][BOARD_SIZE];
	};
}

