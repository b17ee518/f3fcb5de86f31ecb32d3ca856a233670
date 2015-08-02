#include "LyricJson.h"
#include "Settings.h"

#define QLFOREACH_NONCONST(CLASS, IT, LIST) for (QList<CLASS>::iterator IT=LIST.begin(); IT!=LIST.end(); ++IT)

void LyricJson::prepare()
{
	// maxline must greater or equal to 2
	if (_song.general.maxline < 2)
	{
		_song.general.maxline = 2;
	}
	if (_song.lyric.sentencelist.empty())
	{
		return;
	}

	// feed every paragraph
	auto maxline = _song.general.maxline;

	// feed front
	for (int i = 0; i < _song.lyric.sentencelist.first().line; i++)
	{
		_song.lyric.sentencelist.push_front(buildEmptyLine(i%maxline));
	}
	//	for (QList<KJsonSentence>::iterator it = _song.lyric.sentencelist.begin(); it != _song.lyric.sentencelist.end();)
	for (int i = 0; i < _song.lyric.sentencelist.size(); i++)
	{
		//		QList<KJsonSentence>::iterator itNext = std::next(it);

		if (_song.lyric.sentencelist[i].line < 0)
		{
			if (i == 0)
			{
				_song.lyric.sentencelist[i].line = 0;
			}
			else
			{
				_song.lyric.sentencelist[i].line = (_song.lyric.sentencelist[i - 1].line + 1) % maxline;
			}
		}
		// set line color
		if (_song.lyric.sentencelist[i].color < 0)
		{
			_song.lyric.sentencelist[i].color = 0;
		}
		// have next
		//		if (itNext != _song.lyric.sentencelist.end())
		if (i < _song.lyric.sentencelist.size() - 1)
		{
			if (_song.lyric.sentencelist[i + 1].line >= 0)
			{
				int thisModIndex = (_song.lyric.sentencelist[i].line) % maxline;
				int nextModIndex = (_song.lyric.sentencelist[i + 1].line) % maxline + maxline;
				if (thisModIndex + 1 != nextModIndex)
				{
					for (int j = thisModIndex + 1; j < nextModIndex; j++)
					{
						_song.lyric.sentencelist.insert(std::next(_song.lyric.sentencelist.begin(), i + 1), buildEmptyLine(j%maxline));
						i++;
					}
				}
			}
		}
		else
			// this is the last
		{
			// feed last
			// shouldn't be ever called
			for (int j = _song.lyric.sentencelist[i].line + 1; j < maxline; j++)
			{
				_song.lyric.sentencelist.push_back(buildEmptyLine(j%maxline));
				i++;
			}
		}

	}
	// done

	// add word begin time
	QLFOREACH_NONCONST(KJsonSentence, sentenceIt, _song.lyric.sentencelist)
	{
		if (sentenceIt->wordlist.empty())
		{
			continue;
		}
		// first word must set
		Q_ASSERT(sentenceIt->wordlist.first().begin >= 0);
		for (QList<KJsonWord>::iterator it = sentenceIt->wordlist.begin(); it != sentenceIt->wordlist.end(); ++it)
		{
			// every word must have duration
			//			qDebug("%s", it->text.toStdString().c_str());
			Q_ASSERT(it->end >= it->begin);
			if (it != sentenceIt->wordlist.begin())
			{
				QList<KJsonWord>::iterator itPrev = std::prev(it);
				if (it->begin < 0)
				{
					it->begin = itPrev->end;
				}
			}
			// set color
			if (it->color < 0)
			{
				it->color = sentenceIt->color;
			}
		}
	}

	// set ruby begin end
	QLFOREACH_NONCONST(KJsonSentence, sentenceIt, _song.lyric.sentencelist)
	{
		QLFOREACH_NONCONST(KJsonWord, wordIt, sentenceIt->wordlist)
		{
			if (wordIt->rubylist.empty())
			{
				continue;
			}
			// if no begin&end
			bool bNoBeginEnd = true;
			QLFOREACH_NONCONST(KJsonRuby, rubyIt, wordIt->rubylist)
			{
				if (rubyIt->end > 0 || rubyIt->begin > 0)
				{
					bNoBeginEnd = false;
					break;
				}
			}
			// set evenly
			int rubyCount = wordIt->rubylist.count();
			if (bNoBeginEnd)
			{
				int wordDuration = wordIt->end - wordIt->begin;
				for (int i = 0; i < rubyCount; i++)
				{
					wordIt->rubylist[i].begin = wordIt->begin + i*wordDuration / rubyCount;
					wordIt->rubylist[i].end = wordIt->rubylist[i].begin + wordDuration / rubyCount;
				}
			}
			else
			{
				// set first begin
				if (wordIt->rubylist[0].begin < 0)
				{
					wordIt->rubylist[0].begin = wordIt->begin;
				}
				qint64 remainDuration = wordIt->end - wordIt->begin;
//				qDebug("A%lld", wordIt->begin);
//				qDebug("A%lld", remainDuration);
				for (int i = 1; i < rubyCount; i++)
				{
					// set this begin
					qint64 lastDuration = wordIt->rubylist[i - 1].end - wordIt->rubylist[i - 1].begin;
					if (wordIt->rubylist[i].begin < 0)
					{
						wordIt->rubylist[i].begin = wordIt->rubylist[i - 1].begin + lastDuration;
					}
					remainDuration -= lastDuration;
//					qDebug("B%lld", remainDuration);
//					qDebug("C%lld", lastDuration);
					if (remainDuration < 0)
					{
						qDebug("%s", wordIt->text.toStdString().c_str());
					}
				}
				// set last duration
				if (wordIt->rubylist[rubyCount - 1].end < 0)
				{
					wordIt->rubylist[rubyCount - 1].end = wordIt->rubylist[rubyCount - 1].begin + remainDuration;
				}
			}
		}
	}

	// last set sentence begin/end by paragraph
	// by now sentence count should be maxline*N
	Q_ASSERT(_song.lyric.sentencelist.count() % maxline == 0);
	// first calc pseudo begin and end
	QLFOREACH_NONCONST(KJsonSentence, sentenceIt, _song.lyric.sentencelist)
	{
		// TODO: empty sentences
		if (sentenceIt->wordlist.empty())
		{
			// should be set same with the nearest sentence
			continue;
		}
		// ignore if force set
		Q_ASSERT((sentenceIt->begin >= 0) == (sentenceIt->end >= 0));
		if (sentenceIt->begin >= 0 && sentenceIt->end >= 0)
		{
			sentenceIt->_beginCalc = sentenceIt->begin;
			sentenceIt->_endCalc = sentenceIt->end;
			continue;
		}
		qint64 minBegin = std::numeric_limits<qint64>::max();
		qint64 duration = 0;
		QLFOREACH_NONCONST(KJsonWord, wordIt, sentenceIt->wordlist)
		{
			if (wordIt->begin < minBegin)
			{
				minBegin = wordIt->begin;
			}
			duration += wordIt->end - wordIt->begin;
		}
		sentenceIt->_beginCalc = minBegin;
		sentenceIt->_endCalc = duration + sentenceIt->_beginCalc;
	}
	// deal with empty sentences
	int sentenceCount = _song.lyric.sentencelist.count();
	int paragraphCount = sentenceCount / maxline;
	for (int i = 0; i < paragraphCount; i++)
	{
		for (int j = 0; j < maxline; j++)
		{
			int index = i*maxline + j;
			auto sentence = &(_song.lyric.sentencelist[index]);
			if (sentence->_beginCalc < 0 || sentence->_endCalc < 0)
			{
				// find nearest
				int distance = maxline;
				for (int k = 0; k < maxline; k++)
				{
					if (k == j)
					{
						continue;
					}
					if (qAbs((int)(k - j)) >= distance)
					{
						continue;
					}
					int nIndex = i*maxline + k;
					auto nSentence = &(_song.lyric.sentencelist[nIndex]);
					if (nSentence->_beginCalc < 0 || nSentence->_endCalc < 0)
					{
						continue;
					}
					sentence->_beginCalc = nSentence->_beginCalc;
					sentence->_endCalc = nSentence->_endCalc;
				}

				Q_ASSERT(sentence->_beginCalc >= 0 && sentence->_endCalc >= 0);
			}
		}
	}
	// done with pseudo begin and end

	// calc paragraph pseudo begin and end
	QList<ParagraphData> paragraphList;
	for (int i = 0; i < paragraphCount; i++)
	{
		qint64 minBegin = std::numeric_limits<qint64>::max();
		qint64 maxEnd = 0;
		for (int j = 0; j < maxline; j++)
		{
			int index = i*maxline + j;
			auto sentence = &(_song.lyric.sentencelist[index]);
			if (sentence->_beginCalc < minBegin)
			{
				minBegin = sentence->_beginCalc;
			}
			if (sentence->_endCalc > maxEnd)
			{
				maxEnd = sentence->_endCalc;
			}
		}
		ParagraphData pdata;
		pdata.begin = minBegin;
		pdata.end = maxEnd;
		paragraphList.append(pdata);
	}
	auto setting = Settings::getInstance();

	// first paragraph appears all at once
	{
		qint64 adjustedBegin = paragraphList[0].begin - setting->lyricLongFadeTimeMS;
		if (adjustedBegin < 0)
		{
			adjustedBegin = 0;
		}
		for (int i = 0; i < maxline; i++)
		{
			if (_song.lyric.sentencelist[i].begin < 0)
			{
				_song.lyric.sentencelist[i].begin = adjustedBegin;
			}
		}
	}

	for (int i = 0; i < paragraphCount; i++)
	{
		// check every space to set previous's death and this begin!!!

		qint64 previousEndTime = std::numeric_limits<qint64>::min();
		if (i > 0)
		{
			previousEndTime = paragraphList[i - 1].end;
		}
		// process begin
		// if previous is long enough, cut previous all at once and this all at once
		if (previousEndTime + setting->lyricShortFadeTimeMS < paragraphList[i].begin - setting->lyricLongFadeTimeMS)
		{
			// set all this begin at once
			qint64 adjustedBegin = paragraphList[i].begin - setting->lyricLongFadeTimeMS;
			if (adjustedBegin < 0)
			{
				adjustedBegin = 0;
			}
			for (int j = 0; j < maxline; j++)
			{
				int index = i*maxline + j;
				if (_song.lyric.sentencelist[index].begin < 0)
				{
					_song.lyric.sentencelist[index].begin = adjustedBegin;
				}
			}
			// set all previous end at once
			if (i > 0)
			{
				qint64 endTime = paragraphList[i - 1].end + setting->lyricShortFadeTimeMS;
				for (int j = 0; j < maxline; j++)
				{
					int index = (i - 1)*maxline + j;
					if (_song.lyric.sentencelist[index].end < 0)
					{
						_song.lyric.sentencelist[index].end = endTime;
					}
				}
			}

		}
		else
		{
			// flash change: this begin
			// i should > 0

			// problems:
			// this begin earlier than prev end

			// WARNING omit too short cases?

			// first several appear 2 sec front of last previous abs end
			// previous end just before this emerge

			int lastPreviousIndex = (i - 1)*maxline + maxline - 1;
			Q_ASSERT(lastPreviousIndex >= 0);
			qint64 lastPreviousEndTime = _song.lyric.sentencelist[lastPreviousIndex]._endCalc;

			int thisLastIndex = i*maxline + maxline - 1;

			qint64 firstSeveralBegin = lastPreviousEndTime - setting->lyricShortFadeTimeMS;
			qint64 lastBegin = lastPreviousEndTime + setting->lyricShortFadeTimeMS;		// cause problems
			qint64 prevFirstSeveralEnd = firstSeveralBegin - setting->lyricRestIntervalMS; // cause problems
			qint64 prevLastEnd = lastBegin - setting->lyricRestIntervalMS;
			for (int j = 0; j < maxline - 1; j++)
			{
				int index = i*maxline + j;
				_song.lyric.sentencelist[index].begin = firstSeveralBegin;
				Q_ASSERT(firstSeveralBegin <= _song.lyric.sentencelist[index]._beginCalc);
				int lastIndex = (i - 1)*maxline + j;
				_song.lyric.sentencelist[lastIndex].end = prevFirstSeveralEnd;
				Q_ASSERT(firstSeveralBegin >= _song.lyric.sentencelist[lastIndex]._endCalc);
			}
			_song.lyric.sentencelist[(i + 1)*maxline - 1].begin = lastBegin;

			if (lastBegin > _song.lyric.sentencelist[(i + 1)*maxline - 1]._beginCalc)
			{
				if (_song.lyric.sentencelist[(i + 1)*maxline - 1].wordlist.empty())
				{
					// ok
				}
				else
				{
					Q_ASSERT(false);
				}
			}
			_song.lyric.sentencelist[i*maxline - 1].end = prevLastEnd;

		}
	}

	// last paragraph disappears all at once
	{
		qint64 endTime = paragraphList[paragraphCount - 1].end + setting->lyricShortFadeTimeMS;
		for (int j = 0; j < maxline; j++)
		{
			int index = (paragraphCount - 1)*maxline + j;
			if (_song.lyric.sentencelist[index].end < 0)
			{
				_song.lyric.sentencelist[index].end = endTime;
			}
		}
	}
}