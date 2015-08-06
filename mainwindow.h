#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "playlistwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	static void setMainWindow(MainWindow* w);
	static MainWindow* mainWindow();

	void doCloseWindow();
	void togglePlayPause();
	void play();
	void pause();
	void stop(bool byUser=false);

	void resetPlayPauseState();

	bool isPaused();

	void setVolume(int vol);

	void jumpToPosition(qint64 msec);
	void nextSong();
	void previousSong();

	bool loadJson(const QString& path);
	bool loadLRC(const QString& path, const QString& musicPath);
	bool loadASS(const QString& path);

	void loadMusic(const QString& path, qint64 beginOffset);

	void setPlayMode(QMediaPlaylist::PlaybackMode playmode);

	qint64 playerPosition();
	void setPlayerPosition(qint64 position);

	void reloadPlayList();
	void clearPlayList();

	void loadLyric(const QString& path);
	
	QString currentMusicPath();

public:
	inline const QList<QMediaPlaylist*>& playLists(){ return _playLists; }

protected:
	void enterEvent(QEvent *e) override;
	void leaveEvent(QEvent *e) override;
	void moveEvent(QMoveEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

	void showEvent(QShowEvent *e) override;
	void closeEvent(QCloseEvent *e) override;

	void wheelEvent(QWheelEvent *e) override;
	
protected slots:
	void slotOnPlayerPositionChanged(qint64 position);
	void slotOnPlayerDurationChanged(qint64 duration);
	
private:
	void moveIntoScreen(QRect triedRect);
	void addShortcuts();

private:
    Ui::MainWindow *ui;

	QMediaPlayer* _player = NULL;
	bool _isPlaying = false;

	bool _bUpdatingPositionHorizontalSlider = false;

	static MainWindow* s_mainWindow;
	
protected slots:

	void slotOnCloseButtonClicked();
	void slotOnVolumeSliderValueChanged(int value);
	void slotOnLoopSwitchButtonClicked();
	void slotOnPositionSliderValueChanged(int value);
	void slotOnNextButtonClicked();
	void slotOnPreviousButtonClicked();
	void slotOnStopButtonClicked();
	void slotOnPlayStopButtonToggled();
	void slotOnPlayStopButtonToggled(bool bChecked);
	void slotOnListButtonToggled();
	void slotOnListButtonToggled(bool bChecked);

	void slotOnMediaStatusChanged(QMediaPlayer::MediaStatus status);

	void on_positionHorizontalSlider_sliderMoved(int position);
	void on_volumeHorizontalSlider_sliderMoved(int position);

	void slotOnReloadLyric();

signals:
	void sigMusicLoaded(const QString& path);

private:
	const qint64 slideInMS = 1000;
	qreal _slideVol = 0.0;

	qint64 _postOffset = 0;
	qint64 _duration = 0;

	bool _stoppedByUser = false;

	QMediaPlaylist::PlaybackMode _playmode = QMediaPlaylist::PlaybackMode::Loop;

	QList<QMediaPlaylist*> _playLists;
	QMediaPlaylist* _curPlayList = NULL;

	PlaylistWindow * _playlistWindow = NULL;
};

#endif // MAINWINDOW_H
