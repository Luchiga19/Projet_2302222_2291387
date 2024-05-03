#include "PromotionDialog.h"
#include <QObject>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDialog>
#include "Piece.h"
#include <type_traits>


using namespace std;
using namespace piecetype;

PromotionDialog::PromotionDialog(QWidget* parent, Piece::Color color) :
	QDialog(parent)
{
	QHBoxLayout* layout = new QHBoxLayout(this);

	layout->addWidget(createPieceButton<Queen>((color == Piece::Color::WHITE) ? "Images/white_queen.png" : "Images/black_queen.png"));
	layout->addWidget(createPieceButton<Rook>((color == Piece::Color::WHITE) ? "Images/white_rook.png" : "Images/black_rook.png"));
	layout->addWidget(createPieceButton<Bishop>((color == Piece::Color::WHITE) ? "Images/white_bishop.png" : "Images/black_bishop.png"));
	layout->addWidget(createPieceButton<Knight>((color == Piece::Color::WHITE) ? "Images/white_knight.png" : "Images/black_knight.png"));

	setLayout(layout);
	setWindowTitle("Promotion");
}

template<typename T>
QPushButton* PromotionDialog::createPieceButton(const QString& imagePath) {
	QPushButton* button = new QPushButton;
	button->setIcon(QIcon(imagePath));
	button->setIconSize(QSize(80, 80));
	button->setFixedSize(80, 80);

	connect(button, &QPushButton::clicked, this, [this]() {
			if constexpr (is_same_v<T, Queen>)
				emit queenSelected();
			else if constexpr (is_same_v<T, Rook>)
				emit rookSelected();
			else if constexpr (is_same_v<T, Bishop>)
				emit bishopSelected();
			else if constexpr (is_same_v<T, Knight>)
				emit knightSelected();
			close();
		});

	return button;
}
