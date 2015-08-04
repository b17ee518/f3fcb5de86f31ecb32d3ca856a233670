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
#include <QTimer>

MainWindow* MainWindow::s_mainWindow = NULL;

#define ICONPATH_PLAY	":/button/play"
#define ICONPATH_PAUSE	":/button/pause"
#define ICONPATH_STOP	":/button/stop"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	setMainWindow(this);

	Settings::getInstance()->loadIni();

	ui->setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setStyleSheet("background-color: rgba(0, 0, 0, 2);");

	auto scrGeo = QApplication::desktop()->screenGeometry();
	this->setMaximumSize(scrGeo.size());

	_player = new QMediaPlayer(this);
	connect(_player, SIGNAL(positionChanged(qint64)), this, SLOT(updateTimeElapsedSlider(qint64)));
	connect(_player, SIGNAL(durationChanged(qint64)), this, SLOT(setDuration(qint64)));
	connect(_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(slotOnMediaStatusChanged(QMediaPlayer::MediaStatus)));
	/*
	connect(ui->positionSlider, SIGNAL(directJumped(int)), this, SLOT(on_positionHorizontalSlider_sliderMoved(int)));
	connect(ui->volumeSlider, SIGNAL(directJumped(int)), this, SLOT(on_volumeHorizontalSlider_sliderMoved(int)));
	*/
	setVolume(Settings::getInstance()->volume());

	ui->lyricFrame->setMoveHandlingWidget(this);

	resetPlayPauseState();

	Settings::getInstance()->loadWindowPos(this);
	loadMusic(Settings::getInstance()->lastPlayedFileName(), Settings::getInstance()->lastPlayedMS());
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
	_stopped = false;

	_player->play();
	ui->lyricFrame->Play();

	QIcon icon;
	icon.addFile(QStringLiteral(ICONPATH_PAUSE), QSize(), QIcon::Normal, QIcon::Off);
	ui->playPauseButton->setIcon(icon);
	ui->playPauseButton->blockSignals(true);
	ui->playPauseButton->setChecked(true);
	ui->playPauseButton->blockSignals(false);
}

void MainWindow::pause()
{
	_player->pause();
	ui->lyricFrame->Pause();
	
	resetPlayPauseState();
}

void MainWindow::stop()
{
	_stopped = true;

	_player->stop();
	ui->lyricFrame->Stop();

	resetPlayPauseState();
}

void MainWindow::resetPlayPauseState()
{
	ui->playPauseButton->blockSignals(true);
	ui->playPauseButton->setChecked(false);
	ui->playPauseButton->blockSignals(false);

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

void MainWindow::loadMusic(const QString& path, qint64 beginOffset)
{
	_player->setMedia(QUrl::fromLocalFile(path));

	if (beginOffset != 0)
	{
		// slide volume in for 1000 ms

		beginOffset -= slideInMS;
		if (beginOffset < 0)
		{
			beginOffset = 0;
		}
		_player->setVolume(0);

		QTimer* slideInTimer = new QTimer(this);
		qreal subVol = (qreal)(Settings::getInstance()->volume() * 16) / slideInMS;
		_slideVol = 0.0;
		if (subVol > 0)
		{
			connect(slideInTimer, &QTimer::timeout, [this, slideInTimer, subVol]()
			{
				this->_slideVol += subVol;
				int curVol = this->_slideVol;
				if (curVol >= Settings::getInstance()->volume())
				{
					curVol = Settings::getInstance()->volume();
					this->_slideVol = curVol;
					slideInTimer->stop();
					delete slideInTimer;
					this->ui->volumeSlider->setValue(curVol);
				}
				_player->setVolume(curVol);
			});
			slideInTimer->start(16);
		}

		_postOffset = beginOffset;
	}

	// load lyric

	auto settings = Settings::getInstance();
	// if fixed json found then read fixed
	QString fixedJsonPath = settings->makeJsonPath(path, true);
	if (!loadJson(fixedJsonPath))
	{
		// if it's working file, delete working json
		auto workingJsonPath = settings->makeJsonPath(path, false);
		if (settings->isWorkingSong(path))
		{
			QFile::remove(workingJsonPath);
		}
		if (!loadJson(workingJsonPath))
		{
			if (!loadASS(settings->makeASSPath(path)))
			{
				if (!loadLRC(settings->makeLRCPath(path), path))
				{
					// no lrc
				}
			}
		}
	}

	settings->setLastPlayedFileName(path);
	this->setWindowTitle(Settings::getInstance()->getSongName(path));
	/*
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
	*/
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

void MainWindow::showEvent(QShowEvent *e)
{
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	auto settings = Settings::getInstance();
	settings->setVolume(_player->volume());

//	settings->setPlayMode()
	
	settings->setLastPlayedMS(_player->position());

	settings->saveIni();
	settings->saveWindowPos(this);
	QMainWindow::closeEvent(e);
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

void MainWindow::slotOnMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	if (status == QMediaPlayer::MediaStatus::LoadedMedia)
	{
		if (_postOffset > 0)
		{
			_player->setPosition(_postOffset);
			_postOffset = 0;
		}
		if (!_stopped)
		{
			play();
		}
	}
}

void MainWindow::on_positionHorizontalSlider_sliderMoved(int position)
{
	if (!_bUpdatingPositionHorizontalSlider)
	{
		bool bPaused = isPaused();
		qint64 oldPosition = playerPosition();
		setPlayerPosition(position);

		ui->lyricFrame->Jumped(oldPosition);
		if (bPaused)
		{
			QTimer* pauseTimer = new QTimer(this);
			_player->setMuted(true);
			connect(pauseTimer, &QTimer::timeout, [this, pauseTimer]()
			{
				this->play();
				this->pause();
				this->_player->setMuted(false);
				delete pauseTimer;
			});
			pauseTimer->start(250);
		}
	}
}

void MainWindow::on_volumeHorizontalSlider_sliderMoved(int position)
{
	setVolume(position);
}
