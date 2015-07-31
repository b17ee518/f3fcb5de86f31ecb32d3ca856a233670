#include "LyricXML.h"
#include "Settings.h"

#define QLFOREACH_NONCONST(CLASS, IT, LIST) for (QList<CLASS>::iterator IT=LIST.begin(); IT!=LIST.end(); ++IT)


LyricXML::LyricXML()
{
}


LyricXML::~LyricXML()
{
}

bool LyricXML::loadXML(const QString& path)
{
	QDomDocument document;

	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}
	if (!document.setContent(&file))
	{
		return false;
	}
	file.close();

	QDomElement root = document.firstChildElement();
	_song.Clear();
	_song.ReadFromXML(root);
	prepare();

	return true;
}

bool LyricXML::loadLRC(const QString& path)
{
	QFile infile(path);
	if (!infile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	_song.Clear();
	auto settings = Settings::getInstance();
	_song.general.maxline = 2;	// always 2 ?

	while (!infile.atEnd())
	{
		QByteArray lineByte = infile.readLine();
		QString line = QString::fromStdString(lineByte.toStdString()).trimmed();
		if (!line.startsWith("["))
		{
			continue;
		}
		if (!line.contains("]"))
		{
			continue;
		}
		int indexOfClose = line.indexOf("]");
		QString controlPart = line.mid(1, indexOfClose - 1).trimmed();
		if (controlPart.startsWith("ti:"))
		{
			_song.info.title = controlPart.right(controlPart.length() - 3); // no trim
		}
		else if (controlPart.startsWith("au:"))
		{
			_song.info.author = controlPart.right(controlPart.length() - 3); // no trim
		}
		else if (controlPart.startsWith("ar:"))
		{
			_song.info.artist = controlPart.right(controlPart.length() - 3); // no trim
		}
		else if (controlPart.startsWith("al:"))
		{
			_song.info.album = controlPart.right(controlPart.length() - 3); // no trim
		}
		else if (controlPart.startsWith("by:"))
		{
			_song.info.by = controlPart.right(controlPart.length() - 3); // no trim
		}
		else if (controlPart.startsWith("offset:"))
		{
			_song.general.offset = controlPart.right(controlPart.length() - 7).toLongLong();
		}
		else if (controlPart.at(0).isDigit())
		{
			QTime beginTime = QTime::fromString(controlPart + "0", "mm:ss.zzz");
			qint64 begin = beginTime.msecsSinceStartOfDay();

			KXMLSentence sentence;
			KXMLWord word;
			word.birth = begin;
			word.duration = std::numeric_limits<qint64>::max();
			word.text = line.right(line.length() - indexOfClose-1);

			if (word.text.isEmpty())
			{
				int thisIndex = _song.lyric.sentencelist.count();
				if (thisIndex % _song.general.maxline == 0)
				{
					// skip
					continue;
				}
				else
				{
					// move up
					// skip multiple whites
					if (_song.lyric.sentencelist.last().wordlist.last().birth < 0)
					{
						continue;
					}
					else
					{
						word.duration = settings->minimumDuration();
						/*
						if (thisIndex % _song.general.maxline == _song.general.maxline-1
							&& word.birth - _song.lyric.sentencelist[thisIndex - (_song.general.maxline - 1)].wordlist.last().birth < Settings::getInstance()->lyricShortFadeTimeMS)
						{
							word.birth = _song.lyric.sentencelist[thisIndex - (_song.general.maxline - 1)].wordlist.last().birth + Settings::getInstance()->lyricShortFadeTimeMS;
						}
						*/
					}
				}
			}

			if (!_song.lyric.sentencelist.empty())
			{
				KXMLWord& lastword = _song.lyric.sentencelist.last().wordlist.last();
				if (lastword.duration > settings->minimumDuration())
				{
					lastword.duration = begin - lastword.birth;
				}
			}
			sentence.wordlist.append(word);
			_song.lyric.sentencelist.append(sentence);
		}
	}
	lrcWordSeparate();

	QFileInfo info(path);
	QString strXML = info.absolutePath() + "/" + info.completeBaseName() + ".xml";
	exportToXML(strXML);

	loadXML(strXML);
//	prepare();
	return true;
}

void LyricXML::exportToXML(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}
	QDomDocument doc;
	QDomProcessingInstruction instr = doc.createProcessingInstruction(
		"xml", "version='1.0' encoding='UTF-8'");
	doc.appendChild(instr);
	QDomElement songNode = doc.createElement("song");
	_song.Export(songNode);
	doc.appendChild(songNode);
	QTextStream stream(&file);
	stream.setGenerateByteOrderMark(true);
	stream << doc.toString();
	file.close();
}

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
				_song.lyric.sentencelist.push_back(buildEmptyLine(i%maxline));
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
					if (qAbs((int)(k-j)) >= distance)
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
			for (int j = 0; j < maxline-1; j++)
			{
				int index = i*maxline + j;
				_song.lyric.sentencelist[index].birth = firstSeveralBirth;
				Q_ASSERT(firstSeveralBirth <= _song.lyric.sentencelist[index]._birthCalc);
				int lastIndex = (i - 1)*maxline + j;
				_song.lyric.sentencelist[lastIndex].duration = prevFirstSeveralDie - _song.lyric.sentencelist[lastIndex].birth;
				Q_ASSERT(firstSeveralBirth >= _song.lyric.sentencelist[lastIndex]._birthCalc + _song.lyric.sentencelist[lastIndex]._durationCalc);
			}
			_song.lyric.sentencelist[(i + 1)*maxline - 1].birth = lastBirth;
			Q_ASSERT(lastBirth <= _song.lyric.sentencelist[(i + 1)*maxline - 1]._birthCalc);
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

void LyricXML::lrcWordSeparate()
{
	for (QList<KXMLSentence>::iterator sentenceIt = _song.lyric.sentencelist.begin(); sentenceIt != _song.lyric.sentencelist.end(); ++sentenceIt)
	{
		if (sentenceIt->wordlist.count() != 1)
		{
			continue;
		}
		QString text = sentenceIt->wordlist.first().text;
		if (text.trimmed().isEmpty())
		{
			continue;
		}
		qint64 firstBegin = sentenceIt->wordlist.first().birth;
		qint64 totalDuration = sentenceIt->wordlist.first().duration;

		sentenceIt->wordlist.clear();
		int len = text.length();
		for (int i = 0; i < len; i++)
		{
			KXMLWord word;
			word.text = text.at(i);
			word.birth = firstBegin + i*totalDuration / len;
			word.duration = totalDuration / len;
			sentenceIt->wordlist.append(word);
		}
	}
}

KXMLSentence LyricXML::buildEmptyLine(int lineNum)
{
	KXMLSentence sentence;
	sentence.line = lineNum;
	sentence.color = 0;
	return sentence;
}

QString KXMLBase::subTextForElem(const QDomElement& elem, const QString& name)
{
	auto elements = elem.elementsByTagName(name);
	if (!elements.count())
	{
		return "";
	}
	for (int i = 0; i < elements.count(); i++)
	{
		if (elements.at(i).parentNode() == elem)
		{
			return elements.at(i).toElement().text();
		}
	}
	return "";
}

void KXMLBase::addTextToElem(QDomElement& elem, const QString& name, const QString& text)
{
	QDomDocument doc = elem.ownerDocument();
	QDomElement node = doc.createElement(name);
	QDomText textDom = doc.createTextNode(text);
	node.appendChild(textDom);
	elem.appendChild(node);
}

bool KXMLInfo::ReadFromXML(const QDomElement& elem)
{
	title = subTextForElem(elem, "title");
	author = subTextForElem(elem, "author");
	artist = subTextForElem(elem, "artist");
	album = subTextForElem(elem, "album");
	by = subTextForElem(elem, "by");
	description = subTextForElem(elem, "description");

	return true;
}

void KXMLInfo::Export(QDomElement& elem)
{
	if (!title.isEmpty())
	{
		addTextToElem(elem, "title", title);
	}
	if (!author.isEmpty())
	{
		addTextToElem(elem, "author", author);
	}
	if (!artist.isEmpty())
	{
		addTextToElem(elem, "artist", artist);
	}
	if (!album.isEmpty())
	{
		addTextToElem(elem, "album", album);
	}
	if (!by.isEmpty())
	{
		addTextToElem(elem, "by", by);
	}
	if (!description.isEmpty())
	{
		addTextToElem(elem, "description", description);
	}
}

bool KXMLGeneral::ReadFromXML(const QDomElement& elem)
{
	auto maxlineText = subTextForElem(elem, "maxline");
	if (!maxlineText.isEmpty())
	{
		maxline = maxlineText.toInt();
	}
	auto offsetText = subTextForElem(elem, "offset");
	if (!offsetText.isEmpty())
	{
		offset = offsetText.toLongLong();
	}

	return true;
}

void KXMLGeneral::Export(QDomElement& elem)
{
	addTextToElem(elem, "maxline", QString::number(maxline));
	if (offset != 0)
	{
		addTextToElem(elem, "offset", QString::number(offset));
	}
}

bool KXMLRuby::ReadFromXML(const QDomElement& elem)
{
	text = subTextForElem(elem, "text");
	auto birthText = subTextForElem(elem, "birth");
	if (!birthText.isEmpty())
	{
		birth = birthText.toLongLong();
	}
	auto durationText = subTextForElem(elem, "duration");
	if (!durationText.isEmpty())
	{
		duration = durationText.toLongLong();
	}

	return true;
}

void KXMLRuby::Export(QDomElement& elem)
{
	addTextToElem(elem, "text", text);
	if (birth >= 0)
	{
		addTextToElem(elem, "birth", QString::number(birth));
	}
	if (duration >= 0)
	{
		addTextToElem(elem, "duration", QString::number(duration));
	}
}

bool KXMLWord::ReadFromXML(const QDomElement& elem)
{
	text = subTextForElem(elem, "text");
	auto colorText = subTextForElem(elem, "color");
	if (!colorText.isEmpty())
	{
		color = colorText.toInt();
	}
	auto birthText = subTextForElem(elem, "birth");
	if (!birthText.isEmpty())
	{
		birth = birthText.toLongLong();
	}
	auto durationText = subTextForElem(elem, "duration");
	if (!durationText.isEmpty())
	{
		duration = durationText.toLongLong();
	}
	auto rubyhiddenText = subTextForElem(elem, "rubyhidden");
	if (!rubyhiddenText.isEmpty())
	{
		rubyhidden = rubyhiddenText.toInt();
	}
	auto rubies = elem.elementsByTagName("ruby");
	for (int i = 0; i < rubies.count(); i++)
	{
		KXMLRuby ruby;
		ruby.ReadFromXML(rubies.at(i).toElement());
		rubylist.append(ruby);
	}

	return true;
}

void KXMLWord::Export(QDomElement& elem)
{
	addTextToElem(elem, "text", text);
	if (color >= 0)
	{
		addTextToElem(elem, "color", QString::number(color));
	}
	if (birth >= 0)
	{
		addTextToElem(elem, "birth", QString::number(birth));
	}
	if (duration >= 0)
	{
		addTextToElem(elem, "duration", QString::number(duration));
	}
	if (rubyhidden > 0)
	{
		addTextToElem(elem, "rubyhidden", QString::number(rubyhidden));
	}

	QDomDocument doc = elem.ownerDocument();
	Q_FOREACH(auto ruby, rubylist)
	{
		QDomElement rubyNode = doc.createElement("ruby");
		ruby.Export(rubyNode);
		elem.appendChild(rubyNode);
	}
}

bool KXMLSentence::ReadFromXML(const QDomElement& elem)
{
	auto lineText = subTextForElem(elem, "line");
	if (!lineText.isEmpty())
	{
		line = lineText.toInt();
	}
	auto colorText = subTextForElem(elem, "color");
	if (!colorText.isEmpty())
	{
		color = colorText.toInt();
	}
	auto birthText = subTextForElem(elem, "birth");
	if (!birthText.isEmpty())
	{
		birth = birthText.toLongLong();
	}
	auto durationText = subTextForElem(elem, "duration");
	if (!durationText.isEmpty())
	{
		duration = durationText.toLongLong();
	}
	auto words = elem.elementsByTagName("word");
	for (int i = 0; i < words.count(); i++)
	{
		KXMLWord word;
		word.ReadFromXML(words.at(i).toElement());
		wordlist.append(word);
	}

	return true;
}

void KXMLSentence::Export(QDomElement& elem)
{
	if (line >= 0)
	{
		addTextToElem(elem, "line", QString::number(line));
	}
	if (color >= 0)
	{
		addTextToElem(elem, "color", QString::number(color));
	}
	if (birth >= 0)
	{
		addTextToElem(elem, "birth", QString::number(birth));
	}
	if (duration >= 0)
	{
		addTextToElem(elem, "duration", QString::number(duration));
	}

	QDomDocument doc = elem.ownerDocument();
	Q_FOREACH(auto word, wordlist)
	{
		QDomElement wordNode = doc.createElement("word");
		word.Export(wordNode);
		elem.appendChild(wordNode);
	}

}

bool KXMLLyric::ReadFromXML(const QDomElement& elem)
{
	auto sentences = elem.elementsByTagName("sentence");
	for (int i = 0; i < sentences.count(); i++)
	{
		KXMLSentence sentence;
		sentence.ReadFromXML(sentences.at(i).toElement());
		sentencelist.append(sentence);
	}

	return true;
}

void KXMLLyric::Export(QDomElement& elem)
{
	QDomDocument doc = elem.ownerDocument();
	Q_FOREACH(auto sentence, sentencelist)
	{
		QDomElement sentenceNode = doc.createElement("sentence");
		sentence.Export(sentenceNode);
		elem.appendChild(sentenceNode);
	}
}

bool KXMLSong::ReadFromXML(const QDomElement& elem)
{
	auto infos = elem.elementsByTagName("info");
	if (infos.count())
	{
		info.ReadFromXML(infos.at(0).toElement());
	}
	auto generals = elem.elementsByTagName("general");
	if (generals.count())
	{
		general.ReadFromXML(generals.at(0).toElement());
	}
	auto lyrics = elem.elementsByTagName("lyric");
	if (lyrics.count())
	{
		return lyric.ReadFromXML(lyrics.at(0).toElement());
	}
	return false;
}

void KXMLSong::Export(QDomElement& elem)
{
	QDomDocument doc = elem.ownerDocument();
	QDomElement infoNode = doc.createElement("info");
	info.Export(infoNode);
	elem.appendChild(infoNode);

	QDomElement generalNode = doc.createElement("general");
	general.Export(generalNode);
	elem.appendChild(generalNode);

	QDomElement lyricNode = doc.createElement("lyric");
	lyric.Export(lyricNode);
	elem.appendChild(lyricNode);
}
