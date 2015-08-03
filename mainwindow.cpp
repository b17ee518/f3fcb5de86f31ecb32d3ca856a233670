#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "lyricframe.h"

#include "helperkaraokelabel.h"
#include "karaokeword.h"

#include "LyricJson.h"
#include "Settings.h"
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QDesktopWidget>
#include <QMoveEvent>

#include <QFileInfo>

MainWindow* MainWindow::s_mainWindow = NULL;

#define ICONPATH_PLAY	":/button/play"
#define ICONPATH_PAUSE	":/button/pause"
#define ICONPATH_STOP	":/button/stop"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	setMainWindow(this);

	ui->setupUi(this);
	setWindowFlags(windowFlags()  | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setStyleSheet("background-color: rgba(0, 0, 0, 2);");

	auto scrGeo = QApplication::desktop()->screenGeometry();
	this->setMaximumSize(scrGeo.size());

	_player = new QMediaPlayer(this);
	connect(_player, SIGNAL(positionChanged(qint64)), this, SLOT(updateTimeElapsedSlider(qint64)));
	connect(_player, SIGNAL(durationChanged(qint64)), this, SLOT(setDuration(qint64)));
	connect(ui->positionSlider, SIGNAL(directJumped(int)), this, SLOT(on_positionHorizontalSlider_sliderMoved(int)));
	connect(ui->volumeSlider, SIGNAL(directJumped(int)), this, SLOT(on_volumeHorizontalSlider_sliderMoved(int)));

	setVolume(ui->volumeSlider->value());
	ui->lyricFrame->setMoveHandlingWidget(this);

	resetPlayPauseState();
	//
	loadMusic("e:/Karaoke/temp/testLrc.mp3");
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

	QIcon icon;
	icon.addFile(QStringLiteral(ICONPATH_PAUSE), QSize(), QIcon::Normal, QIcon::Off);
	ui->playPauseButton->setIcon(icon);
}

void MainWindow::pause()
{
	_player->pause();
	ui->lyricFrame->Pause();
	
	resetPlayPauseState();
}

void MainWindow::stop()
{
	_player->stop();
	ui->lyricFrame->Stop();

	resetPlayPauseState();
}

void MainWindow::resetPlayPauseState()
{
	ui->playPauseButton->setChecked(false);

	QIcon icon;
	icon.addFile(QStringLiteral(ICONPATH_PLAY), QSize(), QIcon::Normal, QIcon::Off);
	ui->playPauseButton->setIcon(icon);
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

bool MainWindow::loadJson(const QString& path)
{
	return LyricJson::getInstance()->loadJson(path);
}

bool MainWindow::loadLRC(const QString& path, const QString& musicPath)
{
	return LyricJson::getInstance()->loadLRC(path, musicPath);
}

bool MainWindow::loadASS(const QString& path)
{
	return LyricJson::getInstance()->loadASS(path);
}

void MainWindow::loadMusic(const QString& path)
{
	QFileInfo info(path);
	QString strBase = info.absolutePath() + "/" + info.completeBaseName();
	QString jsonName = strBase + Settings::getInstance()->jsonExtention;
	if (!QFile().exists(jsonName))
	{
		if (QFile().exists(strBase+".ass"))
		{
			loadASS(strBase + ".ass");
		}
		else
		{
			loadLRC(strBase + ".lrc", path);
		}
	}
	else
	{
		loadJson(jsonName);
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

void MainWindow::moveEvent(QMoveEvent *e)
{
	QRect triedRect = QRect(e->pos(), this->rect().size());
	moveIntoScreen(triedRect);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
	QRect triedRect = QRect(this->pos(), e->size());
	moveIntoScreen(triedRect);
}

void MainWindow::moveIntoScreen(QRect triedRect)
{
	auto desktop = QApplication::desktop();
	auto scrGeo = desktop->availableGeometry();
	QPoint ptBottomRight = triedRect.bottomRight();
	qreal moveX = 0;
	qreal moveY = 0;

	// multiple screen probs.
	/*
	if (ptBottomRight.x() > scrGeo.right())
	{
	moveX = -(ptBottomRight.x() - (scrGeo.right()));
	if (e->pos().x() + moveX < scrGeo.x())
	{
	moveX = 0;
	}
	}
	*/
	if (ptBottomRight.y() > scrGeo.bottom())
	{
		moveY = scrGeo.bottom() - ptBottomRight.y();
	}
	if (moveX != 0.0 || moveY != 0.0)
	{
		move(triedRect.left() + moveX, triedRect.top() + moveY);
	}
}


void MainWindow::updateTimeElapsedSlider(qint64 percent)
{
	_bUpdatingPositionHorizontalSlider = true;
	ui->positionSlider->setValue(percent);
	_bUpdatingPositionHorizontalSlider = false;
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
	on_volumeHorizontalSlider_sliderMoved(value);
}

void MainWindow::slotOnLoopSwitchButtonClicked()
{

}

void MainWindow::slotOnPositionSliderValueChanged(int value)
{
	on_positionHorizontalSlider_sliderMoved(value);
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
	if (!_bUpdatingPositionHorizontalSlider)
	{
		ui->lyricFrame->Jumped(oldPosition);
	}
}

void MainWindow::on_volumeHorizontalSlider_sliderMoved(int position)
{
	setVolume(position);
}
