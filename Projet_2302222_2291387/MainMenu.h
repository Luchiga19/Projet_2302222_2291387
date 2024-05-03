#pragma once

#include "Chessboard.h"
#include <QWidget>


class MainMenu : public QWidget {
	Q_OBJECT

public:
	MainMenu(QWidget* parent);

public slots:
	void startStandardGame();
	void startEndGame();
	void gameEnded();

private:
	QWidget* _mainWindow;
	QWidget _chessboardWindow;
	interface::Chessboard* _chessboard;
};

