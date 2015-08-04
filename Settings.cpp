#include "Settings.h"

#define SETTINGFILE_NAME	"settings.ini"
#define SETTINGFILE_SETTINGGROUP	"Settings"

#include <QApplication>
#include <QFileInfo>

Settings::Settings()
{
}


Settings::~Settings()
{
}

void Settings::loadWindowPos(QMainWindow* w)
{
	QSettings settings("h5nc", "Karaoke");
	w->restoreGeometry(settings.value(w->objectName() + "/geometry").toByteArray());
	w->restoreState(settings.value(w->objectName() + "/windowstate").toByteArray());
}

void Settings::saveWindowPos(QMainWindow* w)
{
	QSettings settings("h5nc", "Karaoke");
	settings.setValue(w->objectName() + "/geometry", w->saveGeometry());
	settings.setValue(w->objectName() + "/windowState", w->saveState());
}

void Settings::loadIni()
{
	QSettings settings(QApplication::applicationDirPath() + "/" + SETTINGFILE_NAME, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");

	settings.beginGroup(SETTINGFILE_SETTINGGROUP);
	
	int vol = settings.value("Volume").toInt();
	setVolume(vol);
	int playmode = settings.value("PlayMode").toInt();
	setPlayMode(playmode);

	_visualOffset = settings.value("VisualOffset").toLongLong();

	_musicPath = settings.value("Path").toString();

	// always delete json and regen from ass
	_workingSongName = settings.value("WorkingSongName").toString();

	_lastPlayedFileName = settings.value("LastPlayedFileName").toString();
	_lastPlayedMS = settings.value("LastPlayedMS").toLongLong();
	
	settings.endGroup();
}

void Settings::saveIni()
{
	QSettings settings(QApplication::applicationDirPath() + "/" + SETTINGFILE_NAME, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");

	settings.beginGroup(SETTINGFILE_SETTINGGROUP);

	settings.setValue("Volume", _volume);
	settings.setValue("PlayMode", _playMode);

//	settings.setValue("Path", _musicPath);		// never update

//	settings.setValue("WorkingSongName", _workingSongName) // never update

	settings.setValue("LastPlayedFileName", _lastPlayedFileName);
	settings.setValue("LastPlayedMS", _lastPlayedMS);

	settings.endGroup();
}

void Settings::setFontSize(qreal fontSize)
{
	if (_fontSize != fontSize)
	{
		_fontSize = fontSize;
		emit sigFontChanged();
	}
}

void Settings::setFontSizeBySentenceHight(qreal height)
{
	qreal availableHeight = height - verticalSpace*2.0;

	qreal scaleDown = 1.0 + rubyFontScale + rubyVSpaceScale;
	if (scaleDown < 1.0)
	{
		scaleDown = 1.0;
	}
	setFontSize(availableHeight / scaleDown);
}

void Settings::setFont(const QString& fontName)
{
	if (fontName.compare(_fontName, Qt::CaseInsensitive))
	{
		_fontName = fontName;
		emit sigFontChanged();
	}
}

void Settings::setVolume(int vol)
{
	if (vol < 0)
	{
		vol = 0;
	}
	else if (vol > 100)
	{
		vol = 100;
	}
	_volume = vol;
}

void Settings::setPlayMode(int playmode)
{
	if (playmode < PLAYMODE_MIN || playmode > PLAYMODE_MAX)
	{
		playmode = PLAYMODE_ALLLOOP;
	}
	_playMode = playmode;
}

void Settings::setLastPlayedFileName(const QString& path)
{
	_lastPlayedFileName = path;
}

void Settings::setLastPlayedMS(int ms)
{
	_lastPlayedMS = ms;
}

qreal Settings::minimumHeightForLine(int line)
{
	qreal oneLine = standardFontSize * (1.0 + rubyFontScale + rubyVSpaceScale) + verticalSpace * 2;
	return oneLine*line;
}

qreal Settings::rubyVSpace()
{
	qreal space = rubyVSpaceScale * _fontSize;
	if (space > rubyVSpaceMax)
	{
		space = rubyVSpaceMax;
	}
	return space;
}

const qint64 Settings::lastPlayedMS()
{
	if (!_workingSongName.isEmpty())
	{
		if (_lastPlayedFileName.contains(_workingSongName))
		{
			return 0;
		}
	}
	return _lastPlayedMS;
}

QString Settings::makeJsonPath(const QString& path, bool bFixed)
{
	QString retPath = _musicPath;
	if (bFixed)
	{
		retPath += "/lyric";
	}
	else
	{
		retPath += "/working/";
	}
	retPath += getSongName(path) + jsonExtention;
	return retPath;
}

QString Settings::makeASSPath(const QString& path)
{
	QString retPath = _musicPath + "/working/";
	retPath += getSongName(path) + ".ass";
	return retPath;
}

QString Settings::makeLRCPath(const QString& path)
{
	QString retPath = _musicPath + "/working/";
	retPath += getSongName(path) + ".lrc";
	return retPath;
}

bool Settings::isWorkingSong(const QString& path)
{
	if (_workingSongName == getSongName(path))
	{
		return true;
	}
	return false;
}

QString Settings::getSongName(const QString& path)
{
	QFileInfo info(path);
	return info.completeBaseName();
}
