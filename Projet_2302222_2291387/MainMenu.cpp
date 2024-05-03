#include "MainMenu.h"
#include <QWidget>
#include <QHboxLayout>
#include "Chessboard.h"
#include <QPushButton>


MainMenu::MainMenu(QWidget* parent) :
	QWidget(parent), _mainWindow(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignCenter);

	QLabel* chessLabel = new QLabel(this);
	chessLabel->setStyleSheet("font: 80pt 'Times New Roman';");
	chessLabel->setText("Chess!");
	chessLabel->adjustSize();
	mainLayout->addWidget(chessLabel, 0, Qt::AlignCenter);

	QPushButton* standardGameButton = new QPushButton("Start Standard Game", this);
	standardGameButton->setStyleSheet("font: 40pt 'Times New Roman';");
	mainLayout->addWidget(standardGameButton, 0, Qt::AlignCenter);

	connect(standardGameButton, &QPushButton::clicked, this, &MainMenu::startStandardGame);

	QPushButton* endGameButton = new QPushButton("Start Vladimorov vs. Palatnik, 1977 Endgame", this);
	endGameButton->setStyleSheet("font: 40pt 'Times New Roman';");
	mainLayout->addWidget(endGameButton, 0, Qt::AlignCenter);

	connect(endGameButton, &QPushButton::clicked, this, &MainMenu::startEndGame);

	_chessboardWindow.setWindowTitle("Chess");

	_chessboard  = new interface::Chessboard(&_chessboardWindow);

	QHBoxLayout* layout = new QHBoxLayout(&_chessboardWindow);
	layout->addWidget(_chessboard);
	_chessboardWindow.setLayout(layout);

	connect(_chessboard, &interface::Chessboard::gameEnded, this, &MainMenu::gameEnded);
}

void MainMenu::startStandardGame() {
	_mainWindow->close();
	_chessboard->populateStandard();
	_chessboardWindow.show();
}

void MainMenu::startEndGame() {
	_mainWindow->close();
	_chessboard->populateEndgame();
	_chessboardWindow.show();
}

void MainMenu::gameEnded() {
	_chessboardWindow.close();
	_mainWindow->show();
}
