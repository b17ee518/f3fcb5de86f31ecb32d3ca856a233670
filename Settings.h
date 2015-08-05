#pragma once

#include <QString>
#include <QObject>
#include <QSettings>
#include <QMainWindow>
#include <QMediaPlaylist>

class Settings : QObject
{
	Q_OBJECT
private:
	Settings();
	Settings(Settings const&);
	Settings& operator=(Settings const&);
	~Settings();

public:
	static Settings* getInstance() {
		static Settings instance;
		return &instance;
	}

public:

	void loadWindowPos(QMainWindow* w);
	void saveWindowPos(QMainWindow* w);
	void loadIni();
	void saveIni();

	void setFontSize(qreal fontSize);
	void setFontSizeBySentenceHight(qreal height);
	void setFont(const QString& fontName);

	void setVolume(int vol);
	void setPlayMode(QMediaPlaylist::PlaybackMode playmode);

	void setLastPlayedFileName(const QString& path);
	void setLastPlayedMS(int ms);

	qreal minimumHeightForLine(int line);

public:
	inline qreal fontSize(){ return _fontSize; }
	inline qreal rubyFontSize(){ return _fontSize*rubyFontScale; }
	qreal rubyVSpace();

	inline const QString& fontName(){ return _fontName; }

	inline qreal sentenceHight(){ return _fontSize + verticalSpace * 2; }

	inline qreal majorScale(){ return _fontSize / standardFontSize; }
	inline qreal standardBeginMarginSpaceForRightAlignedSentence(){ return standardFontSize*beginMarginSpaceChar; }

	inline qreal minimumDuration(){ return lyricShortFadeTimeMS + lyricRestIntervalMS; }
	inline qreal maximumDuration(){ return lyricEndSentenceTimeMS; }

	inline const QString& musicPath(){ return _musicPath; }

	inline int volume(){ return _volume; }
	inline const QString& lastPlayedFileName(){ return _lastPlayedFileName; }
	inline const QString& workingSongName(){ return _workingSongName; }
	const qint64 lastPlayedMS();

	inline qint64 visualOffset(){ return _visualOffset; }

	inline QMediaPlaylist::PlaybackMode playMode(){ return _playMode; }

	// path
	// Path = e:/music
	// Song = e:/music/2015Su/some.mp3
	// FixedJson = e:/music/lyric/some.json
	// workingjson = e:/music/working/some.json

	QString makeJsonPath(const QString& path, bool bFixed);
	QString makeASSPath(const QString& path);
	QString makeLRCPath(const QString& path);
	QString makePlaylistName(const QString& path);

	bool isWorkingSong(const QString& path);
	QString getSongName(const QString& path);

public:
	const qreal rubyFontScale = 0.35;
	const qreal verticalSpace = 10.0;
	const qreal rubyVSpaceScale = 0.08;
	const qreal rubyVSpaceMax = 15.0;
	const qreal standardFontSize = 40.0;
	const int beginMarginSpaceChar = 3;
	const qreal lyricFastIntroProp = 0.3;

	const qint64 lyricLongFadeTimeMS = 2000;
	const qint64 lyricEndSentenceTimeMS = 60000;
	const qint64 lyricShortFadeTimeMS = 500;
	const qint64 lyricRestIntervalMS = 200;
//	const qreal lyricVeryShortIntervalScale = 0.1; omit now

	const int lineMaxCharAspec = 25;

	const char* jsonExtention = ".json";
	
private:
	qreal _fontSize = standardFontSize;
	QString _fontName = "Meiryo";

	int _volume = 10;
	QString _musicPath;
	QString _workingSongName;

	QString _lastPlayedFileName;
	qint64 _lastPlayedMS = 0;
	QMediaPlaylist::PlaybackMode _playMode = QMediaPlaylist::PlaybackMode::Loop;
	qint64 _visualOffset = -250;

signals:
	void sigFontChanged();
};
