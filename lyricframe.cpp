#include "lyricframe.h"
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QResizeEvent>
#include <QVBoxLayout>

#include "Settings.h"

#include "karaokeword.h"
#include "LyricXML.h"

#define SIZEGRIP_SIZE	10
#define UPDATE_INTERVAL	16

LyricFrame::LyricFrame(QWidget *parent)
	: QFrame(parent)
{
	_pMovingUpdateTimer = new QTimer(this);
	connect(_pMovingUpdateTimer, SIGNAL(timeout()), this, SLOT(slotMovingUpdateTimer()));
	
	_layout = new QVBoxLayout(this);
	_layout->setSpacing(0);
	_layout->setMargin(0);
	
	setMaxSentences(2);

	_pSizeGripLB = new QSizeGrip(this);
	_pSizeGripRB = new QSizeGrip(this);

	_updateTimer = new QTimer(this);
	_elapsedTimer = new QElapsedTimer();
	_elapsedTimer->start();
	connect(_updateTimer, SIGNAL(timeout()), this, SLOT(slotOnUpdateTimer()));
	_updateTimer->start(UPDATE_INTERVAL);

}

LyricFrame::~LyricFrame()
{
}

void LyricFrame::setMoveHandlingWidget(QWidget *pw)
{
	_pHandlingWidget = pw;

}

void LyricFrame::stopMoving()
{
	_bMoving = false;
}

void LyricFrame::setMaxSentences(int count)
{
	if (_sentences.size() != count)
	{
		_sentences.clear();
		for (int i = 0; i < count; i++)
		{
			auto sentence = new SentenceHolder(this);
			_sentences.append(sentence);
			_layout->addWidget(sentence);
			if (i == count - 1)
			{
				// last sentence always right align
				sentence->setDirection(DIRECTION_RIGHT);
			}
			else
			{
				if (i%2 == 0)
				{
					// line 1 3 5 ~
					sentence->setDirection(DIRECTION_LEFT);
				}
				else
				{
					// line 2 4 6 ~
					sentence->setDirection(DIRECTION_RIGHT);
					sentence->setBeginMarginSpace(Settings::getInstance()->standardBeginMarginSpaceForRightAlignedSentence());
				}
			}
		}
		this->setMinimumHeight(Settings::getInstance()->minimumHeightForLine(count));
	}
}

void LyricFrame::BuildByXML()
{
	auto song = LyricXML::getInstance()->song();
	setMaxSentences(song.general.maxline);
	Q_FOREACH(auto sentence, song.lyric.sentencelist)
	{
		Q_FOREACH(auto word, sentence.wordlist)
		{
			auto kw = new KaraokeWord(NULL);
			kw->setLyric(word.text);
			kw->setBeginEnd(word.birth, word.birth + word.duration);
			kw->setRubyHidden(word.rubyhidden);
			if (!word.rubylist.empty())
			{
				Q_FOREACH(auto ruby, word.rubylist)
				{
					kw->addRubyChar(ruby.text, ruby.birth, ruby.birth + ruby.duration);
				}
			}
			//kw->setTextColor()
			_sentences[sentence.line]->addWord(kw);
		}
	}
}

void LyricFrame::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		_bMoving = true;
		_lastMovingPos = QCursor::pos();
		_pMovingUpdateTimer->start(16);
	}
	QFrame::mousePressEvent(event);

}

void LyricFrame::mouseReleaseEvent(QMouseEvent *event)
{
	if (_bMoving && event->button() == Qt::LeftButton)
	{
		_bMoving = false;
	}
	QFrame::mouseReleaseEvent(event);
}

void LyricFrame::resizeEvent(QResizeEvent *e)
{
	if (_pSizeGripLB)
	{
		_pSizeGripLB->setGeometry(0, e->size().height() - SIZEGRIP_SIZE, SIZEGRIP_SIZE, SIZEGRIP_SIZE);
	}
	if (_pSizeGripRB)
	{
		_pSizeGripRB->setGeometry(e->size().width() - SIZEGRIP_SIZE, e->size().height() - SIZEGRIP_SIZE, SIZEGRIP_SIZE, SIZEGRIP_SIZE);
	}

	if (_sentences.size())
	{
		Settings::getInstance()->setFontSizeBySentenceHight(e->size().height() / _sentences.size());

		auto sentenceHeight = Settings::getInstance()->sentenceHight();
		for (int i = 0; i < _sentences.size(); i++)
		{
			auto item = _sentences[i];
			item->setGeometry(0, i*sentenceHeight, e->size().width(), sentenceHeight);
			item->settingsChanged();
		}
	}
}

void LyricFrame::showEvent(QShowEvent *e)
{
	static bool bInited = false;
	if (!bInited)
	{
		BuildByXML();
		/*
//		_sentences[1]->setBeginMarginSpace(40);
		for (int j = 0; j < 3;j++)
		{
			for (int i = 0; i < 2; i++)
			{
				auto word = new KaraokeWord(NULL);
				word->setLyric(QString::fromLocal8Bit("Šó"));
				//				word->setRubyHidden(true);
				int wordlength = 6000;
				word->addRubyChar(QString::fromLocal8Bit("‚«"), i * wordlength, i * wordlength + 1000);
				word->addRubyChar(QString::fromLocal8Bit("‚Ú"), i * wordlength + 1000, i * wordlength + 2000);
				word->addRubyChar(QString::fromLocal8Bit("‚¤"), i * wordlength + 2000, (i + 1) * wordlength);
				//			word->setBeginEnd(i * 1000, (i + 1) * 1000);
				_sentences[j]->addWord(word);
			}
			for (int i = 0; i < 2; i++)
			{
				auto word = new KaraokeWord(NULL);
				word->setLyric(QString::fromLocal8Bit("Šó–]"));
				//				word->setRubyHidden(true);
				int wordlength = 6000;
				word->addRubyChar(QString::fromLocal8Bit("‚«"), i * wordlength, i * wordlength + 1000);
				word->addRubyChar(QString::fromLocal8Bit("‚Ú"), i * wordlength + 1000, i * wordlength + 2000);
				word->addRubyChar(QString::fromLocal8Bit("‚¤"), i * wordlength + 2000, (i + 1) * wordlength);
				//			word->setBeginEnd(i * 1000, (i + 1) * 1000);
				_sentences[j]->addWord(word);
			}
		}
		*/
	}
}

void LyricFrame::slotMovingUpdateTimer()
{
	if (!QApplication::mouseButtons().testFlag(Qt::LeftButton))
	{
		_bMoving = false;
	}
	if (!_bMoving)
	{
		_pMovingUpdateTimer->stop();
		return;
	}
	QPoint newpos = QCursor::pos();
	_pHandlingWidget->move(_pHandlingWidget->pos() + newpos - _lastMovingPos);
	_lastMovingPos = newpos;
	QCursor::pos();

}

void LyricFrame::slotOnUpdateTimer()
{
	//
	if (_sentences.size())
	{
		qint64 curMS = _elapsedTimer->elapsed();
		for (int i = 0; i < _sentences.size(); i++)
		{
			_sentences[i]->act(curMS);
		}
	}
}
