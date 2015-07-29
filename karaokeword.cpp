#include "karaokeword.h"

#include <QLabel>
#include <QFont>
#include "Settings.h"
#include "helperkaraokelabel.h"

#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

KaraokeWord::KaraokeWord(QWidget *parent)
	: QWidget(parent)
{
	this->setAttribute(Qt::WA_TransparentForMouseEvents);
	this->setAutoFillBackground(false);
	this->setAttribute(Qt::WA_TranslucentBackground);
}

KaraokeWord::~KaraokeWord()
{
	ClearPixmap();
}

void KaraokeWord::setLyric(const QString& lyric)
{
	if (_lyric.compare(lyric))
	{
		_lyric = lyric;
		BuildWord();
	}
}

void KaraokeWord::setBeginEnd(qint64 beginMS, qint64 endMS)
{
	_beginMS = beginMS;
	_endMS = endMS;
	if (_beginMS >= _endMS)
	{
		_endMS = _beginMS + 1;
	}
}

void KaraokeWord::updateProportion(qreal prop)
{
	if (_proportion != prop)
	{
		_proportion = prop;
		if (_proportion < 0.0)
		{
			_proportion = 0.0;
		}
		if (_proportion > 1.0)
		{
			_proportion = 1.0;
		}
		repaint();
	}
}

void KaraokeWord::act(qint64 curMS)
{
	if (curMS <= _beginMS)
	{
		updateProportion(0);
	}
	else if (curMS >= _endMS)
	{
		updateProportion(1);
	}
	else
	{
		if (_rubyList.empty())
		{
			updateProportion(((qreal)(curMS - _beginMS)) / ((qreal)(_endMS - _beginMS)));
		}
		else
		{
			int section = _rubyList.size();
			qreal prop = 0;
			for (int i = 0; i < section; i++)
			{
				if (curMS >= _rubyList[i].endMS())
				{
					prop = (qreal)i/(qreal)section;
					continue;
				}
				prop += (qreal)(curMS - _rubyList[i].beginMS()) / (qreal)(_rubyList[i].endMS() - _rubyList[i].beginMS()) / (qreal)section;
			}
			updateProportion(prop);
		}
	}
}

void KaraokeWord::addRubyChar(const QString& rubyText, qint64 beginMS, qint64 endMS)
{
	RubyChar ruby;
	ruby.setRuby(rubyText, beginMS, endMS);

	if (_rubyList.empty())
	{
		setBeginEnd(ruby.beginMS(), ruby.endMS());
	}
	else
	{
		setBeginEnd(ruby.beginMS()<_beginMS ? ruby.beginMS() : _beginMS, ruby.endMS()>_endMS ? ruby.endMS() : _endMS);
	}

	_rubyList.append(ruby);
}

void KaraokeWord::rebuild()
{
	BuildWord();
	repaint();
}

QSize KaraokeWord::boundingBoxSize()
{
	if (_pre)
	{
		return _pre->size();
	}
	return QSize();
}

void KaraokeWord::paintEvent(QPaintEvent * e)
{
	if (_pre && _post)
	{
		auto size = boundingBoxSize();

		auto w = size.width();
		auto h = size.height();
		QRect preRect = QRect(0, 0, w*_proportion, h);
		QRect postRect = QRect(w*_proportion, 0, w*(1 - _proportion), h);

		QPainter painter(this);

		if (_proportion > 0)
		{
			painter.drawImage(preRect, *_pre, preRect);
		}
		if (_proportion < 1)
		{
			painter.drawImage(postRect, *_post, postRect);
		}
		
	}
}

void KaraokeWord::showEvent(QShowEvent *e)
{
	BuildWord();
}

void KaraokeWord::BuildWord()
{
	if (!isVisible())
	{
		return;
	}
	if (_locked)
	{
		return;
	}
	_locked = true;

	HelperKaraokeLabel * label = new HelperKaraokeLabel(this);

	QSize size = label->RenderToImage(&_pre, _lyric, _rubyList, Qt::red, Qt::white);
	label->RenderToImage(&_post, _lyric, _rubyList, Qt::white, Qt::black);
	this->setFixedSize(size);
	_locked = false;
}

void KaraokeWord::ClearPixmap()
{
	if (_pre)
	{
		delete _pre;
		_pre = NULL;
	}

	if (_post)
	{
		delete _post;
		_post = NULL;
	}
}
