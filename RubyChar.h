#pragma once

#include <QObject>
#include <QSize>

class RubyChar
{
public:
	RubyChar();
	~RubyChar();

	void setRuby(const QString& ruby, qint64 beginMS, qint64 endMS);
	void setRenderSize(const QSize& size);

	inline const QString& ruby() { return _ruby; }
	inline qint64 beginMS() { return _beginMS; }
	inline qint64 endMS() { return _endMS; }
	inline const QSize& renderSize() {return _renderSize; }

private:
	QString _ruby;
	qint64 _beginMS = 0;
	qint64 _endMS = 1;
	QSize _renderSize;
};

