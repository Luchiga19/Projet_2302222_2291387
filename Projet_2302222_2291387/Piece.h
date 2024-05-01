#pragma once

#include <stdexcept>
#include <QPixmap>

namespace piecetype {

	class Pos {
	public:
		Pos(int row, int col);
		Pos(const Pos& other);

		Pos operator+(Pos other);

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

		void move(Pos newPos);

		virtual ~Piece() = 0;

		virtual std::string getName() const = 0;
		Pos getPos() const { return _pos; };
		QPixmap getImage() const { return _image; }

	protected:
		// array validMoves[]; add to ensure new move pos is in the array
		Pos _pos;
		QPixmap _image;

	private:
		Color _color;
	};


	class King : public Piece {
	public:
		King(Color color, Pos pos);

		std::string getName() const override;

	private:
		static inline int _kingCount = 0;
	};


	class TooManyKingsException : public std::logic_error {
	public:
		using logic_error::logic_error;
	};
}
