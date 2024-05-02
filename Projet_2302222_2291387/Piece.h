#pragma once

#include <stdexcept>
#include <QPixmap>


namespace interface {
	class Chessboard;
};

namespace piecetype {

	class Pos {
	public:
		Pos(int row, int col);
		Pos(const Pos& other);

		Pos operator+(const Pos& other) const;
		void operator+=(const Pos& other);
		bool operator==(const Pos& other) const;
		bool operator!=(const Pos& other) const;
		bool operator<(const Pos& other) const;
		Pos& operator=(const Pos& other);
		bool isValid() const;

		int getRow() const { return _row; }
		int getCol() const { return _col; }

	private:
		int _row;
		int _col;
	};


	class Piece {
	public:
		enum class Color {
			BLACK,
			WHITE
		};

		Piece(Color color, Pos pos);

		virtual ~Piece() = default;

		virtual void updateValidMoves(interface::Chessboard& board) = 0;
		void move(Pos newPos);

		Pos getPos() const { return _pos; };
		QPixmap getImage() const { return _image; }
		Color getColor() const { return _color;  }

	protected:
		std::vector<Pos> _validMoves;
		Pos _pos;
		QPixmap _image;

	private:
		Color _color;
	};


	class King : public Piece {
	public:
		King(Color color, Pos pos);

		void updateAggroMoves(interface::Chessboard& board);
		void updateValidMoves(interface::Chessboard& board) override;

	private:
		static inline int _kingCount = 0;
	};
	

	class Queen : public Piece {
	public:
		Queen(Color color, Pos pos);

		void updateValidMoves(interface::Chessboard& board) override;
	};


	class Bishop : public Piece {
	public:
		Bishop(Color color, Pos pos);

		void updateValidMoves(interface::Chessboard& board) override;
	};


	class TooManyKingsException : public std::logic_error {
	public:
		using logic_error::logic_error;
	};


}
