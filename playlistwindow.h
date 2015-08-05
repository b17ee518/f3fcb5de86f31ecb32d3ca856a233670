#ifndef PLAYLISTWINDOW_H
#define PLAYLISTWINDOW_H

#include <QMainWindow>
#include "ui_playlistwindow.h"

class PlaylistWindow : public QMainWindow
{
	Q_OBJECT

public:
	PlaylistWindow(QWidget *parent = 0);
	~PlaylistWindow();

	void moveWithMainWindow();

protected:
	virtual void showEvent(QShowEvent *e) override;
	virtual void moveEvent(QMoveEvent *e) override;

public slots:
	void slotListItemSelectionChanged();
	void slotListItemDoubleClicked(QListWidgetItem* item);
	void slotSongItemSelectionChanged();
	void slotSongItemDoubleClicked(QListWidgetItem* item);

	void slotOnMusicLoaded(const QString& path);

private:
	Ui::PlaylistWindow ui;
};

#endif // PLAYLISTWINDOW_H
