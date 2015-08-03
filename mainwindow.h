#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>

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
	void stop();

	void resetPlayPauseState();

	bool isPaused();

	void setVolume(int vol);

	void jumpToPosition(qint64 msec);
	void nextSong();
	void previousSong();

	bool loadJson(const QString& path);
	bool loadLRC(const QString& path, const QString& musicPath);
	bool loadASS(const QString& path);

	void loadMusic(const QString& path);

	qint64 playerPosition();
	void setPlayerPosition(qint64 position);

protected:
	void enterEvent(QEvent *e) override;
	void leaveEvent(QEvent *e) override;
	void moveEvent(QMoveEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;
	
protected slots:
	void updateTimeElapsedSlider(qint64 percent);
	void setDuration(qint64 duration);
	
private:
	void moveIntoScreen(QRect triedRect);

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
	void slotOnPlayStopButtonToggled(bool bChecked);
	void slotOnListButtonToggled(bool bChecked);

	void on_positionHorizontalSlider_sliderMoved(int position);
	void on_volumeHorizontalSlider_sliderMoved(int position);

};

#endif // MAINWINDOW_H
