#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "lyricframe.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setStyleSheet("background-color: rgba(0, 0, 0, 2);");
    setWindowFlags(Qt::FramelessWindowHint);

	ui->lyricFrame->setMoveHandlingWidget(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doCloseWindow()
{
	this->close();
}

void MainWindow::togglePlayPause()
{

}

void MainWindow::play()
{

}

void MainWindow::pause()
{

}

void MainWindow::stop()
{

}

void MainWindow::jumpToPosition(qint64 msec)
{

}

void MainWindow::nextSong()
{

}

void MainWindow::previousSong()
{

}

void MainWindow::enterEvent(QEvent *e)
{
	this->setStyleSheet("background-color: rgba(128, 128, 128, 64);");
//	setAttribute(Qt::WA_TranslucentBackground, false);
}

void MainWindow::leaveEvent(QEvent *e)
{
	this->setStyleSheet("background-color: rgba(0, 0, 0, 2);");
//	setAttribute(Qt::WA_TranslucentBackground);
}

void MainWindow::slotOnCloseButtonClicked()
{
	doCloseWindow();
}

void MainWindow::slotOnVolumeSliderValueChanged(int value)
{

}

void MainWindow::slotOnLoopSwitchButtonClicked()
{

}

void MainWindow::slotOnPositionSliderValueChanged(int value)
{

}

void MainWindow::slotOnNextButtonClicked()
{

}

void MainWindow::slotOnPreviousButtonClicked()
{

}

void MainWindow::slotOnStopButtonClicked()
{

}

void MainWindow::slotOnPlayStopButtonToggled(bool bChecked)
{

}

void MainWindow::slotOnListButtonToggled(bool bChecked)
{

}
