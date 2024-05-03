#pragma once

#include <QObject>
#include <QPushButton>
#include <QDialog>
#include "Piece.h"


class PromotionDialog : public QDialog
{
	Q_OBJECT
public:
	PromotionDialog(QWidget* parent, piecetype::Piece::Color color);
	~PromotionDialog() = default;

signals:
	void queenSelected();
	void rookSelected();
	void bishopSelected();
	void knightSelected();

private:
	template<typename T>
	QPushButton* createPieceButton(const QString& imagePath);
};
