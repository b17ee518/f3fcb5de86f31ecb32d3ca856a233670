#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "lyricframe.h"

#include "helperkaraokelabel.h"
#include "karaokeword.h"

#include "LyricXML.h"
#include "Settings.h"
#include <QGraphicsVideoItem>
#include <QGraphicsView>

MainWindow* MainWindow::s_mainWindow = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setStyleSheet("background-color: rgba(0, 0, 0, 2);");
    setWindowFlags(Qt::FramelessWindowHint);

	ui->lyricFrame->setMoveHandlingWidget(this);

	_player = new QMediaPlayer(this);
	connect(_player, SIGNAL(positionChanged(qint64)), this, SLOT(updateTimeElapsedSlider(qint64)));
	connect(_player, SIGNAL(durationChanged(qint64)), this, SLOT(setDuration(qint64)));
	connect(ui->positionSlider, SIGNAL(directJumped(int)), this, SLOT(on_positionHorizontalSlider_sliderMoved(int)));

	setVolume(ui->volumeSlider->value());
	//
	loadMusic("e:/Karaoke/temp/testLrc.mp3");
//	loadLRC("e:/Karaoke\temp/testLrc.lrc");
//	loadXML("e:/Karaoke/f3fcb5de86f31ecb32d3ca856a233670/resource/test.xml");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMainWindow(MainWindow* w)
{
	s_mainWindow = w;
}

MainWindow* MainWindow::mainWindow()
{
	return s_mainWindow;
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
	_player->play();
	ui->lyricFrame->Play();
}

void MainWindow::pause()
{
	_player->pause();
	ui->lyricFrame->Pause();
}

void MainWindow::stop()
{
	_player->stop();
	ui->lyricFrame->Stop();
}

bool MainWindow::isPaused()
{
	return (_player->state() == QMediaPlayer::State::PausedState);
}

void MainWindow::setVolume(int vol)
{
	_player->setVolume(vol);
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

bool MainWindow::loadXML(const QString& path)
{
	return LyricXML::getInstance()->loadXML(path);
}

bool MainWindow::loadLRC(const QString& path)
{
	return LyricXML::getInstance()->loadLRC(path);
}

void MainWindow::loadMusic(const QString& path)
{
	QFileInfo info(path);
	QString strBase = info.absolutePath() + "/" + info.completeBaseName();
	QString xmlName = strBase + Settings::getInstance()->xmlExtention;
	if (!QFile().exists(xmlName))
	{
		loadLRC(strBase + ".lrc");
	}
	else
	{
		loadXML(xmlName);
	}
	_player->setMedia(QUrl::fromLocalFile(path));
}

qint64 MainWindow::playerPosition()
{
	return _player->position();
}

void MainWindow::setPlayerPosition(qint64 position)
{
	_player->setPosition(position);
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

void MainWindow::updateTimeElapsedSlider(qint64 percent)
{
	ui->positionSlider->setValue(percent);
}

void MainWindow::setDuration(qint64 duration)
{
	ui->positionSlider->setMaximum(duration);
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
	stop();
}

void MainWindow::slotOnPlayStopButtonToggled(bool bChecked)
{
	if (bChecked)
	{
		play();
	}
	else
	{
		pause();
	}
}

void MainWindow::slotOnListButtonToggled(bool bChecked)
{

}

void MainWindow::on_positionHorizontalSlider_sliderMoved(int position)
{
	bool bPaused = isPaused();
	qint64 oldPosition = playerPosition();
	setPlayerPosition(position);

	if (bPaused)
	{
		play();
		pause();
	}
	ui->lyricFrame->Jumped(oldPosition);
}