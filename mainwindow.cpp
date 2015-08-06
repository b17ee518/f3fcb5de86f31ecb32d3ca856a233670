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
#include <QDirIterator>
#include <QTime>

#include <QShortcut>
#include <QWheelEvent>

MainWindow* MainWindow::s_mainWindow = NULL;

#define ICONPATH_PLAY	":/button/play"
#define ICONPATH_PAUSE	":/button/pause"
#define ICONPATH_STOP	":/button/stop"

#define ICONPATH_ALLLOOP			":/button/allloop"
#define ICONPATH_SINGLELOOP	":/button/singleloop"
#define ICONPATH_SHUFFLE			":/button/shuffle"

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
	connect(_player, SIGNAL(positionChanged(qint64)), this, SLOT(slotOnPlayerPositionChanged(qint64)));
	connect(_player, SIGNAL(durationChanged(qint64)), this, SLOT(slotOnPlayerDurationChanged(qint64)));
	connect(_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(slotOnMediaStatusChanged(QMediaPlayer::MediaStatus)));

	addShortcuts();

	setVolume(Settings::getInstance()->volume());

	ui->lyricFrame->setMoveHandlingWidget(this);

	resetPlayPauseState();

	Settings::getInstance()->loadWindowPos(this);
	
	setPlayMode(Settings::getInstance()->playMode());
	reloadPlayList();
	loadMusic(Settings::getInstance()->lastPlayedFileName(), Settings::getInstance()->lastPlayedMS());

	_playlistWindow = new PlaylistWindow(this);
//	_playlistWindow->show();
	_playlistWindow->hide();

	connect(this, SIGNAL(sigMusicLoaded(const QString&)), _playlistWindow, SLOT(slotOnMusicLoaded(const QString&)));
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
	_stoppedByUser = false;

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

void MainWindow::stop(bool byUser/*=false*/)
{
	if (byUser)
	{
		_stoppedByUser = true;
	}

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
	ui->volumeSlider->blockSignals(true);
	ui->volumeSlider->setValue(vol);
	ui->volumeSlider->blockSignals(false);
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
	QUrl url = QUrl::fromLocalFile(path);
	int index = 0;
	
	QMediaPlaylist* asPl = NULL;
	Q_FOREACH(auto pl, _playLists)
	{
		for (int i = 0; i < pl->mediaCount(); i++)
		{
			if (pl->media(i).canonicalUrl().matches(url, QUrl::None))
			{
				asPl = pl;
				index = i;
				break;
			}
		}
		if (asPl)
		{
			break;
		}
	}

	if (!asPl)
	{
		if (_playLists.size())
		{
			asPl = _playLists[0];
		}
	}

	if (!asPl)
	{
		return;
	}

	if (_curPlayList != asPl)
	{
		if (_curPlayList)
		{
			stop();
			delete _curPlayList;
		}
		_curPlayList = new QMediaPlaylist(this);
		for (int i = 0; i < asPl->mediaCount(); i++)
		{
			_curPlayList->addMedia(asPl->media(i));
		}
	}
	
	_player->setPlaylist(_curPlayList);
	_curPlayList->setPlaybackMode(_playmode);
	_curPlayList->setCurrentIndex(index);
//	_player->setMedia(QUrl::fromLocalFile(path));

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
}


void MainWindow::setPlayMode(QMediaPlaylist::PlaybackMode playmode)
{
	if (playmode == QMediaPlaylist::PlaybackMode::Loop)
	{
		QIcon icon;
		icon.addFile(QStringLiteral(ICONPATH_ALLLOOP), QSize(), QIcon::Normal, QIcon::Off);
		ui->loopSwitchButton->setIcon(icon);
	}
	else if (playmode == QMediaPlaylist::PlaybackMode::CurrentItemInLoop)
	{
		QIcon icon;
		icon.addFile(QStringLiteral(ICONPATH_SINGLELOOP), QSize(), QIcon::Normal, QIcon::Off);
		ui->loopSwitchButton->setIcon(icon);
	}
	else if (playmode == QMediaPlaylist::PlaybackMode::Random)
	{
		QIcon icon;
		icon.addFile(QStringLiteral(ICONPATH_SHUFFLE), QSize(), QIcon::Normal, QIcon::Off);
		ui->loopSwitchButton->setIcon(icon);
	}
	else
	{
		setPlayMode(QMediaPlaylist::PlaybackMode::Loop);
		return;
	}

	_playmode = playmode;
	if (_curPlayList)
	{
		_curPlayList->setPlaybackMode(_playmode);
	}
	Settings::getInstance()->setPlayMode(_playmode);
}

qint64 MainWindow::playerPosition()
{
	return _player->position();
}

void MainWindow::setPlayerPosition(qint64 position)
{
	_player->setPosition(position);
}

void MainWindow::reloadPlayList()
{
	// do not change cur
	clearPlayList();
	QString basePath = Settings::getInstance()->musicPath();
	QDirIterator it(basePath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		it.next();
		QString subDir = it.filePath();

		if (subDir.endsWith("/working") || subDir.endsWith("/lyric"))
		{
			continue;
		}

		QMediaPlaylist* pl = new QMediaPlaylist(this);
		pl->setPlaybackMode(_playmode);

		QDirIterator itFile(subDir, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		while (itFile.hasNext())
		{
			itFile.next();
			QString file = itFile.filePath();
			pl->addMedia(QUrl::fromLocalFile(file));
			pl->setCurrentIndex(0);
			// currentIndexChanged(int position)
		}
		if (pl->mediaCount())
		{
			pl->setObjectName(Settings::getInstance()->makePlaylistName(subDir));
			_playLists.append(pl);
		}
	}
}

void MainWindow::clearPlayList()
{
	Q_FOREACH(auto pl, _playLists)
	{
		delete pl;
	}
	_playLists.clear();
}

void MainWindow::loadLyric(const QString& path)
{
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

	ui->lyricFrame->setMaxSentences(LyricJson::getInstance()->song().general.maxline);
	ui->lyricFrame->Jumped(std::numeric_limits<qint64>::min());
	settings->setLastPlayedFileName(path);
	this->setWindowTitle(Settings::getInstance()->getSongName(path));
}

QString MainWindow::currentMusicPath()
{
	return _player->currentMedia().canonicalUrl().toLocalFile();
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

	if (!_playlistWindow->isHidden())
	{
		_playlistWindow->moveWithMainWindow();
	}
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

void MainWindow::wheelEvent(QWheelEvent *e)
{
	if (e->orientation() == Qt::Vertical)
	{
		int delta = e->delta();
		int vol = _player->volume();
		setVolume(vol + delta/24);
	}
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
	if (triedRect.top() < 0)
	{
		moveY = scrGeo.top() - triedRect.top();
	}
	if (moveX != 0.0 || moveY != 0.0)
	{
		move(triedRect.left() + moveX, triedRect.top() + moveY);
	}
}


void MainWindow::addShortcuts()
{
	QShortcut *listShortcut = new QShortcut(QKeySequence("Ctrl+L"), this);
	listShortcut->setContext(Qt::ApplicationShortcut);
	connect(listShortcut, SIGNAL(activated()), this, SLOT(slotOnListButtonToggled()));

	QShortcut *playPauseShortcut = new QShortcut(QKeySequence("Space"), this);
	playPauseShortcut->setContext(Qt::ApplicationShortcut);
	connect(playPauseShortcut, SIGNAL(activated()), this, SLOT(slotOnPlayStopButtonToggled()));

	QShortcut *stopShortcut = new QShortcut(QKeySequence("Ctrl+P"), this);
	stopShortcut->setContext(Qt::ApplicationShortcut);
	connect(stopShortcut, SIGNAL(activated()), this, SLOT(slotOnStopButtonClicked()));

	QShortcut *nextShortcut = new QShortcut(QKeySequence("Ctrl+Right"), this);
	nextShortcut->setContext(Qt::ApplicationShortcut);
	connect(nextShortcut, SIGNAL(activated()), this, SLOT(slotOnNextButtonClicked()));

	QShortcut *prevShortcut = new QShortcut(QKeySequence("Ctrl+Left"), this);
	prevShortcut->setContext(Qt::ApplicationShortcut);
	connect(prevShortcut, SIGNAL(activated()), this, SLOT(slotOnPreviousButtonClicked()));

	QShortcut *closeShortcut = new QShortcut(QKeySequence("Escape"), this);
	closeShortcut->setContext(Qt::ApplicationShortcut);
	connect(closeShortcut, SIGNAL(activated()), this, SLOT(close()));

	QShortcut *reloadLyricShortcut = new QShortcut(QKeySequence("Ctrl+\\"), this);
	reloadLyricShortcut->setContext(Qt::ApplicationShortcut);
	connect(reloadLyricShortcut, SIGNAL(activated()), this, SLOT(slotOnReloadLyric()));
}

void MainWindow::slotOnPlayerPositionChanged(qint64 position)
{
	_bUpdatingPositionHorizontalSlider = true;

	// update time str
	QTime curTime = QTime::fromMSecsSinceStartOfDay(position);
	QTime durTime = QTime::fromMSecsSinceStartOfDay(_duration);
	QString timeStr = curTime.toString("mm:ss") + " / " + durTime.toString("mm:ss");
	ui->timeLabel->setText(timeStr);
	ui->positionSlider->setValue(position);

	_bUpdatingPositionHorizontalSlider = false;
}

void MainWindow::slotOnPlayerDurationChanged(qint64 duration)
{
	ui->positionSlider->setMaximum(duration);
	_duration = duration;
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
	auto toPlayMode = _playmode;
	if (_playmode == QMediaPlaylist::PlaybackMode::Loop)
	{
		toPlayMode = QMediaPlaylist::CurrentItemInLoop;
	}
	else if (_playmode == QMediaPlaylist::PlaybackMode::CurrentItemInLoop)
	{
		toPlayMode = QMediaPlaylist::Random;
	}
	else
	{
		toPlayMode = QMediaPlaylist::Loop;
	}
	setPlayMode(toPlayMode);
}

void MainWindow::slotOnPositionSliderValueChanged(int value)
{
	on_positionHorizontalSlider_sliderMoved(value);
}

void MainWindow::slotOnNextButtonClicked()
{
	if (_curPlayList)
	{
		_curPlayList->next();
	}
}

void MainWindow::slotOnPreviousButtonClicked()
{
	if (_curPlayList)
	{
		_curPlayList->previous();
	}
}

void MainWindow::slotOnStopButtonClicked()
{
	stop(true);
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

void MainWindow::slotOnPlayStopButtonToggled()
{
	ui->playPauseButton->toggle();
//	slotOnPlayStopButtonToggled(!ui->playPauseButton->isChecked());
}

void MainWindow::slotOnListButtonToggled(bool bChecked)
{
	if (_playlistWindow->isHidden())
	{
		// reload
		reloadPlayList();
		_playlistWindow->show();
		_playlistWindow->raise();
	}
	else
	{
		_playlistWindow->hide();
	}
}

void MainWindow::slotOnListButtonToggled()
{
	ui->listButton->toggle();
//	slotOnListButtonToggled(!ui->listButton->isChecked());
}

void MainWindow::slotOnMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	if (status == QMediaPlayer::MediaStatus::LoadingMedia)
	{
		stop();
		LyricJson::getInstance()->song().Clear();
	}
	else if (status == QMediaPlayer::MediaStatus::LoadedMedia)
	{
		if (_postOffset > 0)
		{
			_player->setPosition(_postOffset);
			_postOffset = 0;
		}

		auto path = _player->currentMedia().canonicalUrl().toLocalFile();
		loadLyric(path);

		emit sigMusicLoaded(path);
		if (!_stoppedByUser)
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

void MainWindow::slotOnReloadLyric()
{
	auto path = _player->currentMedia().canonicalUrl().toLocalFile();
	loadLyric(path);
}
