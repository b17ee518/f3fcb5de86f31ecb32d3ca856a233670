#include "RubyChar.h"


RubyChar::RubyChar()
{
}


RubyChar::~RubyChar()
{
}

void RubyChar::setRuby(const QString& ruby, qint64 beginMS, qint64 endMS)
{
	_ruby = ruby;
	_beginMS = beginMS;
	_endMS = endMS;
	if (_endMS <= _beginMS)
	{
		_endMS = _beginMS + 1;
	}
}

void RubyChar::setRenderSize(const QSizeF& size)
{
	_renderSize = size;
}
