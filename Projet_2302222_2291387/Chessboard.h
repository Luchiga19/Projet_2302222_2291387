#pragma once

#include <QWidget>
#include <memory>
#include "Piece.h"
#include "QLabel"
#include <span>
#include <stdexcept>


namespace interface {

	class Square : public QWidget {
		Q_OBJECT

	public:
		friend class Chessboard;

		static constexpr int SQUARE_SIZE = 80;

		Square(QWidget* parent, piecetype::Pos pos);
		void paintEvent(QPaintEvent* event) override;
		void updateSquare();

	private:
		piecetype::Pos _pos;
		std::unique_ptr<piecetype::Piece> _piece;
		QColor _color;
		QLabel* _label;
	};


	class Chessboard : public QWidget {
	public:
		static constexpr int BOARD_SIZE = 8;

		Chessboard(QWidget* parent);

		Square** operator[] (int i);
		void populateStandard();

	private:
		Square* _board[BOARD_SIZE][BOARD_SIZE];
	};
}

