#include "lyricframe.h"
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QDesktopWidget>

#include "Settings.h"

#include "karaokeword.h"
#include "LyricJson.h"

#include "mainwindow.h"

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

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_pSizeGripLB = new QSizeGrip(this);
	_pSizeGripRB = new QSizeGrip(this);

	_updateTimer = new QTimer(this);
	_elapsedTimer = new QElapsedTimer();
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
		clearSentences();
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

	/*
	auto scrGeo = QApplication::desktop()->screenGeometry();
	MainWindow::mainWindow()->setMaximumHeight(scrGeo.width()*count / Settings::getInstance()->lineMaxCharAspec);
	*/
}

void LyricFrame::clearSentences()
{
	for (int i = 0; i < _sentences.size(); i++)
	{
		delete _sentences[i];
	}
	_sentences.clear();
}

void LyricFrame::BuildByJson()
{
	auto song = LyricJson::getInstance()->song();
	setMaxSentences(song.general.maxline);
}

void LyricFrame::Play()
{
}

void LyricFrame::Pause()
{
}

void LyricFrame::Stop()
{
	_previousMSec = std::numeric_limits<qint64>::min();
}

void LyricFrame::Jumped(qint64 oldPosition)
{
	_previousMSec = std::numeric_limits<qint64>::min();
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
		BuildByJson();
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
	static bool s_firstSentenceDoneFlag[2] = {false, false};

	auto song = LyricJson::getInstance()->song();
//	qint64 curMS = _elapsedTimer->elapsed() - song.general.offset;
	qint64 curMS = MainWindow::mainWindow()->playerPosition();

	// die old
	if (_sentences.size())
	{
		Q_FOREACH(auto sentence, _sentences)
		{
			if (curMS > sentence->clearTime())
			{
				sentence->clearSentence();
			}
		}
	}
	// birth new
	Q_FOREACH(auto sentence, song.lyric.sentencelist)
	{
		if (curMS >= sentence.begin && sentence.begin > _previousMSec && curMS < sentence.end)
		{
			_sentences[sentence.line]->clearSentence();	// not needed
			_sentences[sentence.line]->setClearTime(sentence.end);

			Q_FOREACH(auto word, sentence.wordlist)
			{
				auto kw = new KaraokeWord(NULL);
				
				auto wordText = word.text;
				if (wordText.startsWith("[") && wordText.endsWith("]"))
				{
					auto testText = wordText.mid(1, wordText.length() - 2);
					if (testText == "title")
					{
						wordText = song.info.title;
					}
					else if (testText == "artist")
					{
						wordText = song.info.artist;
					}
					else if (testText == "album")
					{
						wordText = song.info.album;
					}
					else if (testText == "by")
					{
						wordText = song.info.by;
					}
					else if (testText == "author")
					{
						wordText = song.info.author;
					}
					else if (testText == "description")
					{
						wordText = song.info.description;
					}
					kw->setIsControlText(true);
				}
				kw->setLyric(wordText);

				kw->setBeginEnd(word.begin, word.end);
				kw->setRubyHidden(word.rubyhidden);
				if (!word.rubylist.empty())
				{
					Q_FOREACH(auto ruby, word.rubylist)
					{
						kw->addRubyChar(ruby.text, ruby.begin, ruby.end);
					}
				}
				//kw->setTextColor()
				_sentences[sentence.line]->addWord(kw);
				if (sentence.line < 2)
				{
					if (!s_firstSentenceDoneFlag[sentence.line] && wordText.length())
					{
						// reset minimum
						_sentences[sentence.line]->settingsChanged();
					}
				}
			}
		}
	}

	if (_sentences.size())
	{
		Q_FOREACH(auto sentence, _sentences)
		{
			sentence->act(curMS);
		}
	}
	_previousMSec = curMS;
}
