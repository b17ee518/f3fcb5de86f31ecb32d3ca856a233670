#include "sentenceholder.h"
#include "karaokeword.h"

#include <QHBoxLayout>
#include "Settings.h"

SentenceHolder::SentenceHolder(QWidget *parent)
	: QFrame(parent)
{
	_layout = new QHBoxLayout(this);
	_layout->setMargin(0);
	_layout->setSpacing(0);
//	_layout->setContentsMargins(0, 0, 0, 0);
	_layout->setAlignment(Qt::AlignLeft);
	
	_horizontalSpacer = new QSpacerItem(0, 0);
	_layout->addSpacerItem(_horizontalSpacer);

	this->setAutoFillBackground(false);
	this->setAttribute(Qt::WA_TranslucentBackground);
}

SentenceHolder::~SentenceHolder()
{

}

void SentenceHolder::addWord(KaraokeWord* word)
{
	word->setParent(this);
	if (_direction == DIRECTION_LEFT)
	{
		_layout->addWidget(word);
	}
	else
	{
		int index = _layout->count()-1;
		_layout->insertWidget(index, word);
	}
}

void SentenceHolder::clearSentence()
{
	Q_FOREACH(auto item, children())
	{
		KaraokeWord *word = dynamic_cast<KaraokeWord *>(item);
		if (NULL != word)
		{
			delete word;
		}
	}
}

void SentenceHolder::setBeginMarginSpace(qreal space)
{
	_beginMarginSpace = space;
	_horizontalSpacer->changeSize(_beginMarginSpace*Settings::getInstance()->majorScale(), 0);
}

void SentenceHolder::setDirection(int dir)
{
	_direction = dir;
	if (dir == DIRECTION_LEFT)
	{
		_layout->setAlignment(Qt::AlignLeft);
	}
	else
	{
		_layout->setAlignment(Qt::AlignRight);
	}
}

void SentenceHolder::act(qint64 curMS)
{
	Q_FOREACH(auto child, children())
	{
		KaraokeWord *karaoke = dynamic_cast<KaraokeWord *>(child);
		if (NULL != karaoke)
		{
			karaoke->act(curMS);
		}
	}
}

void SentenceHolder::settingsChanged()
{
	Q_FOREACH(auto child, children())
	{
		KaraokeWord *karaoke = dynamic_cast<KaraokeWord *>(child);
		if (NULL != karaoke)
		{
			karaoke->rebuild();
		}
	}
	if (_beginMarginSpace != 0.0)
	{
		_horizontalSpacer->changeSize(_beginMarginSpace*Settings::getInstance()->majorScale(), 0);
	}
}
