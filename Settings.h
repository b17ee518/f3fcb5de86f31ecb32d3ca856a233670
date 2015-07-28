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
		static Settings settings;
		return &settings;
	}

public:
	void setFontSize(qreal fontSize);
	void setFontSizeBySentenceHight(qreal height);
	void setFont(const QString& fontName);

public:
	inline qreal fontSize(){ return _fontSize; }
	inline qreal rubyFontSize(){ return _fontSize*rubyFontScale; }

	inline const QString& fontName(){ return _fontName; }

	inline qreal sentenceHight(){ return _fontSize + verticalSpace * 2; }

public:
	const qreal rubyFontScale = 0.35;
	const qreal verticalSpace = 10.0;

private:
	qreal _fontSize = 40.0;
	QString _fontName = "MS PGothic";


signals:
	void sigFontChanged();
};
