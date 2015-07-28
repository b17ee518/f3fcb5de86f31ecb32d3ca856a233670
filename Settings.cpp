#include "Settings.h"


Settings::Settings()
{
}


Settings::~Settings()
{
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
	setFontSize(height - verticalSpace * 2);
}

void Settings::setFont(const QString& fontName)
{
	if (fontName.compare(_fontName, Qt::CaseInsensitive))
	{
		_fontName = fontName;
		emit sigFontChanged();
	}
}
