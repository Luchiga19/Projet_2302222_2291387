#pragma once

#include <QObject>
#include <QMouseEvent>
#include <QWidget>
#include <memory>
#include "Piece.h"
#include <QLabel>
#include <span>
#include <stdexcept>
#include <set>


namespace interface {

	class Square : public QWidget {
		Q_OBJECT

	public:
		friend class Chessboard;

		static constexpr int SQUARE_SIZE = 80;

		Square(QWidget* parent, piecetype::Pos pos);
		void paintEvent(QPaintEvent* event) override;

		bool isEmpty() const;
		bool isSameColorPiece(const piecetype::Piece& other) const;

	signals:
		void squareClicked(Square* square);

	protected:
		void mousePressEvent(QMouseEvent* event) override;

	private:
		piecetype::Pos _pos;
		std::unique_ptr<piecetype::Piece> _piece;
		QColor _color;
	};


	class Chessboard : public QWidget {
		Q_OBJECT

	public:
		static constexpr int BOARD_SIZE = 8;

		class iterator {
		public: 
			iterator(Square* (*ptr)[BOARD_SIZE], int row = 0, int col = 0);

			iterator& operator++();
			Square*& operator*();
			bool operator==(const iterator& it);
			bool operator!=(const iterator& it);



		private:
			Square* _sourceSquare;
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
		Square* operator[](const piecetype::Pos& pos) const;
		void populateStandard();

		bool isPosInAggroMoves(const piecetype::Pos& pos, const piecetype::Piece::Color& color) const;

		void insertAggroMove(const piecetype::Pos& pos, const piecetype::Piece::Color& color);
		void updateAllValidMoves();

	private:
		std::set<piecetype::Pos> _validWhiteAggroMoves;
		std::set<piecetype::Pos> _validBlackAggroMoves;
		Square* _board[BOARD_SIZE][BOARD_SIZE];
	};
}

