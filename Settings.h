#pragma once

#include <QString>
#include <QObject>

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
	void setFontSize(qreal fontSize);
	void setFontSizeBySentenceHight(qreal height);
	void setFont(const QString& fontName);

	qreal minimumHeightForLine(int line);

public:
	inline qreal fontSize(){ return _fontSize; }
	inline qreal rubyFontSize(){ return _fontSize*rubyFontScale; }
	qreal rubyVSpace();

	inline const QString& fontName(){ return _fontName; }

	inline qreal sentenceHight(){ return _fontSize + verticalSpace * 2; }

	inline qreal majorScale(){ return _fontSize / standardFontSize; }
	inline qreal standardBeginMarginSpaceForRightAlignedSentence(){ return standardFontSize*beginMarginSpaceChar; }

public:
	const qreal rubyFontScale = 0.35;
	const qreal verticalSpace = 10.0;
	const qreal rubyVSpaceScale = 0.08;
	const qreal rubyVSpaceMax = 15.0;
	const qreal standardFontSize = 40.0;
	const int beginMarginSpaceChar = 3;

	const qint64 lyricLongFadeTimeMS = 4000;
	const qint64 lyricShortFadeTimeMS = 2000;
	const qint64 lyricRestIntervalMS = 200;
//	const qreal lyricVeryShortIntervalScale = 0.1; omit now

private:
	qreal _fontSize = standardFontSize;
	QString _fontName = "MS PGothic";


signals:
	void sigFontChanged();
};
