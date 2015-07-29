#include "sentenceholder.h"
#include "karaokeword.h"

#include <QHBoxLayout>

SentenceHolder::SentenceHolder(QWidget *parent)
	: QFrame(parent)
{
	_layout = new QHBoxLayout(this);
	_layout->setMargin(0);
	_layout->setSpacing(0);
//	_layout->setContentsMargins(0, 0, 0, 0);
	_layout->setAlignment(Qt::AlignLeft);

	this->setAutoFillBackground(false);
	this->setAttribute(Qt::WA_TranslucentBackground);
}

SentenceHolder::~SentenceHolder()
{

}

void SentenceHolder::addWord(KaraokeWord* word)
{
	word->setParent(this);
	_layout->addWidget(word);
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
}
