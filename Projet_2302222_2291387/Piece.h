#pragma once

#include <stdexcept>

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
			WHITE,
			NO_COLOR
		};

		Piece(Color color, Pos pos);

		void move(Pos newPos);

		virtual ~Piece() = 0;

		virtual std::string getName() = 0;
		Pos getPos() { return _pos; };

	protected:
		// array validMoves[]; add to ensure new move pos is in the array
		Pos _pos;

	private:
		Color _color;
	};


	class King : public Piece {
	public:
		King(Color color, Pos pos);

		std::string getName() override;

	private:
		static inline int _kingCount = 0;
	};


	class Empty : public Piece {
	public:
		Empty(Pos pos);
		std::string getName() override;
	};


	class TooManyKingsException : public std::logic_error {
	public:
		using logic_error::logic_error;
	};
}
