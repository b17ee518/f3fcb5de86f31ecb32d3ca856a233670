#include "LyricXML.h"
#include "Settings.h"

#define QLFOREACH_NONCONST(CLASS, IT, LIST) for (QList<CLASS>::iterator IT=LIST.begin(); IT!=LIST.end(); ++IT)

void LyricXML::prepare()
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
	//	for (QList<KXMLSentence>::iterator it = _song.lyric.sentencelist.begin(); it != _song.lyric.sentencelist.end();)
	for (int i = 0; i < _song.lyric.sentencelist.size(); i++)
	{
		//		QList<KXMLSentence>::iterator itNext = std::next(it);

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

	// add word birth time
	QLFOREACH_NONCONST(KXMLSentence, sentenceIt, _song.lyric.sentencelist)
	{
		if (sentenceIt->wordlist.empty())
		{
			continue;
		}
		// first word must set
		Q_ASSERT(sentenceIt->wordlist.first().birth >= 0);
		for (QList<KXMLWord>::iterator it = sentenceIt->wordlist.begin(); it != sentenceIt->wordlist.end(); ++it)
		{
			// every word must have duration
			//			qDebug("%s", it->text.toStdString().c_str());
			Q_ASSERT(it->duration > 0);
			if (it != sentenceIt->wordlist.begin())
			{
				QList<KXMLWord>::iterator itPrev = std::prev(it);
				if (it->birth < 0)
				{
					it->birth = itPrev->birth + itPrev->duration;
				}
			}
			// set color
			if (it->color < 0)
			{
				it->color = sentenceIt->color;
			}
		}
	}

	// set ruby birth duration
	QLFOREACH_NONCONST(KXMLSentence, sentenceIt, _song.lyric.sentencelist)
	{
		QLFOREACH_NONCONST(KXMLWord, wordIt, sentenceIt->wordlist)
		{
			if (wordIt->rubylist.empty())
			{
				continue;
			}
			// if no duration&birth
			bool bNoDurationBirth = true;
			QLFOREACH_NONCONST(KXMLRuby, rubyIt, wordIt->rubylist)
			{
				if (rubyIt->duration > 0 || rubyIt->birth > 0)
				{
					bNoDurationBirth = false;
					break;
				}
			}
			// set evenly
			int rubyCount = wordIt->rubylist.count();
			if (bNoDurationBirth)
			{
				for (int i = 0; i < rubyCount; i++)
				{
					wordIt->rubylist[i].birth = wordIt->birth + (i*wordIt->duration) / rubyCount;
					wordIt->rubylist[i].duration = wordIt->duration / rubyCount;
				}
			}
			else
			{
				// set first birth
				if (wordIt->rubylist[0].birth < 0)
				{
					wordIt->rubylist[0].birth = wordIt->birth;
				}
				qint64 remainDuration = wordIt->duration;
				qDebug("A%lld", wordIt->birth);
				qDebug("A%lld", remainDuration);
				for (int i = 1; i < rubyCount; i++)
				{
					// set this birth
					qint64 lastDuration = wordIt->rubylist[i - 1].duration;
					if (wordIt->rubylist[i].birth < 0)
					{
						wordIt->rubylist[i].birth = wordIt->rubylist[i - 1].birth + lastDuration;
					}
					remainDuration -= lastDuration;
					qDebug("B%lld", remainDuration);
					qDebug("C%lld", lastDuration);
					if (remainDuration < 0)
					{
						qDebug("%s", wordIt->text.toStdString().c_str());
					}
				}
				// set last duration
				if (wordIt->rubylist[rubyCount - 1].duration < 0)
				{
					wordIt->rubylist[rubyCount - 1].duration = remainDuration;
				}
			}
		}
	}

	// last set sentence birth/duration by paragraph
	// by now sentence count should be maxline*N
	Q_ASSERT(_song.lyric.sentencelist.count() % maxline == 0);
	// first calc pseudo birth and duration
	QLFOREACH_NONCONST(KXMLSentence, sentenceIt, _song.lyric.sentencelist)
	{
		// TODO: empty sentences
		if (sentenceIt->wordlist.empty())
		{
			// should be set same with the nearest sentence
			continue;
		}
		// ignore if force set
		Q_ASSERT((sentenceIt->birth >= 0) == (sentenceIt->duration >= 0));
		if (sentenceIt->birth >= 0 && sentenceIt->duration >= 0)
		{
			sentenceIt->_birthCalc = sentenceIt->birth;
			sentenceIt->_durationCalc = sentenceIt->duration;
			continue;
		}
		qint64 minBirth = std::numeric_limits<qint64>::max();
		qint64 duration = 0;
		QLFOREACH_NONCONST(KXMLWord, wordIt, sentenceIt->wordlist)
		{
			if (wordIt->birth < minBirth)
			{
				minBirth = wordIt->birth;
			}
			duration += wordIt->duration;
		}
		sentenceIt->_birthCalc = minBirth;
		sentenceIt->_durationCalc = duration;
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
			if (sentence->_birthCalc < 0 || sentence->_durationCalc < 0)
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
					if (nSentence->_birthCalc < 0 || nSentence->_durationCalc < 0)
					{
						continue;
					}
					sentence->_birthCalc = nSentence->_birthCalc;
					sentence->_durationCalc = nSentence->_durationCalc;
				}

				Q_ASSERT(sentence->_birthCalc >= 0 && sentence->_durationCalc >= 0);
			}
		}
	}
	// done with pseudo birth and duration

	// calc paragraph pseudo birth and duration
	QList<ParagraphData> paragraphList;
	for (int i = 0; i < paragraphCount; i++)
	{
		qint64 minBirth = std::numeric_limits<qint64>::max();
		qint64 maxDie = 0;
		for (int j = 0; j < maxline; j++)
		{
			int index = i*maxline + j;
			auto sentence = &(_song.lyric.sentencelist[index]);
			if (sentence->_birthCalc < minBirth)
			{
				minBirth = sentence->_birthCalc;
			}
			if (sentence->_birthCalc + sentence->_durationCalc > maxDie)
			{
				maxDie = sentence->_birthCalc + sentence->_durationCalc;
			}
		}
		ParagraphData pdata;
		pdata.birth = minBirth;
		pdata.duration = maxDie - minBirth;
		paragraphList.append(pdata);
	}
	auto setting = Settings::getInstance();

	// first paragraph appears all at once
	{
		qint64 adjustedBirth = paragraphList[0].birth - setting->lyricLongFadeTimeMS;
		if (adjustedBirth < 0)
		{
			adjustedBirth = 0;
		}
		for (int i = 0; i < maxline; i++)
		{
			if (_song.lyric.sentencelist[i].birth < 0)
			{
				_song.lyric.sentencelist[i].birth = adjustedBirth;
			}
		}
	}

	for (int i = 0; i < paragraphCount; i++)
	{
		// check every space to set previous's death and this birth!!!

		qint64 previousDieTime = std::numeric_limits<qint64>::min();
		if (i > 0)
		{
			previousDieTime = paragraphList[i - 1].birth + paragraphList[i - 1].duration;
		}
		// process birth
		// if previous is long enough, cut previous all at once and this all at once
		if (previousDieTime + setting->lyricShortFadeTimeMS < paragraphList[i].birth - setting->lyricLongFadeTimeMS)
		{
			// set all this birth at once
			qint64 adjustedBirth = paragraphList[i].birth - setting->lyricLongFadeTimeMS;
			if (adjustedBirth < 0)
			{
				adjustedBirth = 0;
			}
			for (int j = 0; j < maxline; j++)
			{
				int index = i*maxline + j;
				if (_song.lyric.sentencelist[index].birth < 0)
				{
					_song.lyric.sentencelist[index].birth = adjustedBirth;
				}
			}
			// set all previous die at once
			if (i > 0)
			{
				qint64 dieTime = paragraphList[i - 1].birth + paragraphList[i - 1].duration + setting->lyricShortFadeTimeMS;
				for (int j = 0; j < maxline; j++)
				{
					int index = (i - 1)*maxline + j;
					if (_song.lyric.sentencelist[index].duration < 0)
					{
						_song.lyric.sentencelist[index].duration = dieTime - _song.lyric.sentencelist[index].birth;
					}
				}
			}

		}
		else
		{
			// flash change: this birth
			// i should > 0

			// problems:
			// this birth earlier than prev die

			// WARNING omit too short cases?

			// first several appear 2 sec front of last previous abs die
			// previous die just before this emerge

			int lastPreviousIndex = (i - 1)*maxline + maxline - 1;
			Q_ASSERT(lastPreviousIndex >= 0);
			qint64 lastPreviousDieTime = _song.lyric.sentencelist[lastPreviousIndex]._birthCalc + _song.lyric.sentencelist[lastPreviousIndex]._durationCalc;

			int thisLastIndex = i*maxline + maxline - 1;

			qint64 firstSeveralBirth = lastPreviousDieTime - setting->lyricShortFadeTimeMS;
			qint64 lastBirth = lastPreviousDieTime + setting->lyricShortFadeTimeMS;		// cause problems
			qint64 prevFirstSeveralDie = firstSeveralBirth - setting->lyricRestIntervalMS; // cause problems
			qint64 prevLastDie = lastBirth - setting->lyricRestIntervalMS;
			for (int j = 0; j < maxline - 1; j++)
			{
				int index = i*maxline + j;
				_song.lyric.sentencelist[index].birth = firstSeveralBirth;
				Q_ASSERT(firstSeveralBirth <= _song.lyric.sentencelist[index]._birthCalc);
				int lastIndex = (i - 1)*maxline + j;
				_song.lyric.sentencelist[lastIndex].duration = prevFirstSeveralDie - _song.lyric.sentencelist[lastIndex].birth;
				Q_ASSERT(firstSeveralBirth >= _song.lyric.sentencelist[lastIndex]._birthCalc + _song.lyric.sentencelist[lastIndex]._durationCalc);
			}
			_song.lyric.sentencelist[(i + 1)*maxline - 1].birth = lastBirth;

			if (lastBirth > _song.lyric.sentencelist[(i + 1)*maxline - 1]._birthCalc)
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
			_song.lyric.sentencelist[i*maxline - 1].duration = prevLastDie - _song.lyric.sentencelist[i*maxline - 1].birth;

		}
	}

	// last paragraph disappears all at once
	{
		qint64 dieTime = paragraphList[paragraphCount - 1].birth + paragraphList[paragraphCount - 1].duration + setting->lyricShortFadeTimeMS;
		for (int j = 0; j < maxline; j++)
		{
			int index = (paragraphCount - 1)*maxline + j;
			if (_song.lyric.sentencelist[index].duration < 0)
			{
				_song.lyric.sentencelist[index].duration = dieTime - _song.lyric.sentencelist[index].birth;
			}
		}
	}
}