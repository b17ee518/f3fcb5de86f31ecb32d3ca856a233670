#include "playlistwindow.h"

#include "mainwindow.h"
#include "Settings.h"

#include <QDesktopWidget>

PlaylistWindow::PlaylistWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setStyleSheet("background-color: rgba(128, 128, 128, 192);");

	connect(ui.playlistWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotListItemSelectionChanged()));
	connect(ui.playlistWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotListItemDoubleClicked(QListWidgetItem*)));
	connect(ui.songlistWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotSongItemSelectionChanged()));
	connect(ui.songlistWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotSongItemDoubleClicked(QListWidgetItem*)));
}

PlaylistWindow::~PlaylistWindow()
{

}

void PlaylistWindow::moveWithMainWindow()
{
	auto mw = MainWindow::mainWindow();

	auto geo = geometry();
	auto mwgeo = mw->geometry();
	move(mwgeo.left(), mwgeo.top() - geo.height());
}

void PlaylistWindow::showEvent(QShowEvent *e)
{
	auto mw = MainWindow::mainWindow();
	moveWithMainWindow();
	auto path = mw->currentMusicPath();
	auto settings = Settings::getInstance();

	QString curPlName = settings->makePlaylistName(path);
	QString curSongName = settings->getSongName(path);
	auto playlists = mw->playLists();

	ui.playlistWidget->clear();
//	ui.songlistWidget->clear();

	for (int i = 0; i < playlists.size(); i++)
	{
		auto pl = playlists[i];
		QString plname = pl->objectName();
		ui.playlistWidget->addItem(plname);

		if (!plname.compare(curPlName, Qt::CaseInsensitive))
		{
			// select this
			ui.playlistWidget->clearSelection();
			ui.playlistWidget->item(i)->setSelected(true);
			ui.playlistWidget->scrollToItem(ui.playlistWidget->item(i));
			/*
			// add song
			for (int j = 0; j < pl->mediaCount(); j++)
			{
				QString songName = settings->getSongName(pl->media(j).canonicalUrl().toLocalFile());

				ui.songlistWidget->addItem(songName);
				if (!songName.compare(curSongName, Qt::CaseInsensitive))
				{
					// select this
					ui.songlistWidget->clearSelection();
					ui.songlistWidget->item(j)->setSelected(true);
					ui.songlistWidget->scrollToItem(ui.songlistWidget->item(j));
				}
			}
			*/
		}
	}
}

void PlaylistWindow::moveEvent(QMoveEvent *e)
{
	if (geometry().top() < 0)
	{
		auto mw = MainWindow::mainWindow();
		auto mwgeo = mw->geometry();
		move(geometry().left(), mwgeo.bottom());
	}
	else
	{
		auto mw = MainWindow::mainWindow();
		auto mwgeo = mw->geometry();
		auto desktop = QApplication::desktop();
		auto scrGeo = desktop->availableGeometry();
		if (geometry().bottom() > scrGeo.bottom())
		{
			move(geometry().left(), mwgeo.top() - geometry().height());
		}
	}
}

void PlaylistWindow::slotListItemSelectionChanged()
{
	auto items = ui.playlistWidget->selectedItems();
	if (items.count())
	{
		auto item = items.first();
		auto plname = item->text();

		auto mw = MainWindow::mainWindow();

		auto path = mw->currentMusicPath();
		auto settings = Settings::getInstance();

		QString curPlName = settings->makePlaylistName(path);
		QString curSongName = settings->getSongName(path);
		bool bToCurList = false;
		if (!curPlName.compare(plname, Qt::CaseInsensitive))
		{
			bToCurList = true;
		}

		ui.songlistWidget->clear();
		// add song
		auto playlists = mw->playLists();
		for (int i = 0; i < playlists.count(); i++)
		{
			if (!playlists[i]->objectName().compare(plname, Qt::CaseInsensitive))
			{
				auto pl = playlists[i];
				for (int j = 0; j < pl->mediaCount(); j++)
				{
					QString songName = settings->getSongName(pl->media(j).canonicalUrl().toLocalFile());

					ui.songlistWidget->addItem(songName);
					if (bToCurList)
					{
						if (!songName.compare(curSongName, Qt::CaseInsensitive))
						{
							// select this
							ui.songlistWidget->clearSelection();
							ui.songlistWidget->item(j)->setSelected(true);
							ui.songlistWidget->scrollToItem(ui.songlistWidget->item(j));
						}
					}
				}
				break;
			}
		}
	}
}

void PlaylistWindow::slotListItemDoubleClicked(QListWidgetItem* item)
{
	ui.playlistWidget->clearSelection();
	item->setSelected(true);

	auto mw = MainWindow::mainWindow();

	auto path = mw->currentMusicPath();
	auto settings = Settings::getInstance();

	QString curPlName = settings->makePlaylistName(path);

	if (!item->text().compare(curPlName, Qt::CaseInsensitive))
	{
		// do nothing if double clicked current
		return;
	}

	// play the first
	auto playlists = mw->playLists();
	Q_FOREACH(auto pl, playlists)
	{
		if (!pl->objectName().compare(item->text(), Qt::CaseInsensitive))
		{
			mw->loadMusic(pl->media(0).canonicalUrl().toLocalFile(), 0);
			break;
		}
	}
}

void PlaylistWindow::slotSongItemSelectionChanged()
{
	// nothing to do
}

void PlaylistWindow::slotSongItemDoubleClicked(QListWidgetItem* item)
{
	ui.songlistWidget->clearSelection();
	item->setSelected(true);

	// play this

	auto plitems = ui.playlistWidget->selectedItems();
	QString plname = "";
	if (plitems.count())
	{
		plname = plitems.first()->text();
	}
	auto mw = MainWindow::mainWindow();
	auto playlists = mw->playLists();
	Q_FOREACH(auto pl, playlists)
	{
		if (!pl->objectName().compare(plname, Qt::CaseInsensitive))
		{
			for (int i = 0; i < pl->mediaCount(); i++)
			{
				QString path = pl->media(i).canonicalUrl().toLocalFile();
				QString songName = Settings::getInstance()->getSongName(path);
				if (!songName.compare(item->text(), Qt::CaseInsensitive))
				{
					mw->loadMusic(path, 0);
					break;
				}
			}
			break;
		}
	}
}

void PlaylistWindow::slotOnMusicLoaded(const QString& path)
{
	if (isHidden())
	{
		return;
	}
	auto mw = MainWindow::mainWindow();
	auto settings = Settings::getInstance();

	QString toPlName = settings->makePlaylistName(path);

	auto plitems = ui.playlistWidget->selectedItems();
	QString plname = "";
	if (plitems.count())
	{
		plname = plitems.first()->text();
	}

	QString songName = settings->getSongName(path);
	if (!plname.compare(toPlName, Qt::CaseInsensitive))
	{
		// change song selection
		for (int i = 0; i < ui.songlistWidget->count(); i++)
		{
			if (!ui.songlistWidget->item(i)->text().compare(songName, Qt::CaseInsensitive))
			{
				ui.songlistWidget->clearSelection();
				ui.songlistWidget->item(i)->setSelected(true);
				ui.songlistWidget->scrollToItem(ui.songlistWidget->item(i));
				break;
			}
		}
	}
}
