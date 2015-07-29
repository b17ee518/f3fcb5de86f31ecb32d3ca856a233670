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
