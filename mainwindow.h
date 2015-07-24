#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void doCloseWindow();
	void togglePlayPause();
	void play();
	void pause();
	void stop();

	void jumpToPosition(qint64 msec);
	void nextSong();
	void previousSong();

protected:
	void enterEvent(QEvent *e) override;
	void leaveEvent(QEvent *e) override;
	
private:
    Ui::MainWindow *ui;

	bool _isPlaying = false;
	
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
};

#endif // MAINWINDOW_H
