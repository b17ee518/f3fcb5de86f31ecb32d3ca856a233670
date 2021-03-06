#include "LyricJson.h"
#include "Settings.h"

#include <QFileInfo>
#include <QTime>

bool LyricJson::loadLRC(const QString& path, const QString& musicPath)
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

			KJsonSentence sentence;
			KJsonWord word;
			word.begin = begin;
			word.end = begin + settings->maximumDuration(); //std::numeric_limits<qint64>::max();
			word.text = line.right(line.length() - indexOfClose - 1);

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
					if (_song.lyric.sentencelist.last().wordlist.last().begin < 0)
					{
						continue;
					}
					else
					{
						word.end = word.begin + settings->minimumDuration();
					}
				}
			}

			if (!_song.lyric.sentencelist.empty())
			{
				KJsonWord& lastword = _song.lyric.sentencelist.last().wordlist.last();
				if (lastword.end - lastword.begin> settings->minimumDuration())
				{
					lastword.end = begin;// - lastword.begin;
				}
			}
			sentence.wordlist.append(word);
			_song.lyric.sentencelist.append(sentence);
		}
	}

	KJsonSong songCopy = _song;

	// first generate weak version
	treatSentecesAsControl();
	QString jsonPath = Settings::getInstance()->makeJsonPath(path, false);
	exportToJson(jsonPath);

	_song = songCopy;

	lrcWordSeparate();

	exportToASS(Settings::getInstance()->makeASSPath(path), musicPath);

	return loadJson(jsonPath);
}

void LyricJson::lrcWordSeparate()
{
	for (QList<KJsonSentence>::iterator sentenceIt = _song.lyric.sentencelist.begin(); sentenceIt != _song.lyric.sentencelist.end(); ++sentenceIt)
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
		qint64 firstBegin = sentenceIt->wordlist.first().begin;
		qint64 totalDuration = sentenceIt->wordlist.first().end - firstBegin;

		// text format
		// [text|ruby]
		// [AB|abc]  count as three words

		QString iterText = text;

		//		QRegExp reg("\\[[^\\[]\\|.*\\]");	prob of []
		QRegExp reg("\\[.*\\|.*\\]");
		int wordRubyCount = 0;

		QString normalText;
		QString rubiedText;

		sentenceIt->wordlist.clear();

		bool bControlWord = isControlOrInfoText(text);
		if (bControlWord)
		{
			// build normal word
			KJsonWord word;
			word.text = text;
			sentenceIt->wordlist.append(word);

			wordRubyCount = 1;

			normalText = text;
			rubiedText = text;
		}
		else
		{
			while (true)
			{
				int searchedIndex = iterText.indexOf(reg);
				if (searchedIndex >= 0)
				{
					for (int i = 0; i < searchedIndex; i++)
					{
						// build normal word
						KJsonWord word;
						word.text = iterText.at(i);
						sentenceIt->wordlist.append(word);

						normalText += word.text;
						rubiedText += word.text;

						wordRubyCount++;
					}
					iterText = iterText.right(iterText.length() - searchedIndex - 1);
					// [ already cut
					int separatorIndex = iterText.indexOf("|");
					int closeIndex = iterText.indexOf("]", separatorIndex);
					QString kanjiText = iterText.left(separatorIndex);
					QString rubyText = iterText.mid(separatorIndex + 1, closeIndex - separatorIndex - 1);

					normalText += kanjiText;
					rubiedText += rubyText;

					KJsonWord word;
					word.text = kanjiText;
					for (int i = 0; i < rubyText.length(); i++)
					{
						KJsonRuby ruby;
						ruby.text = rubyText[i];
						word.rubylist.append(ruby);
					}
					wordRubyCount += rubyText.length();
					sentenceIt->wordlist.append(word);

					iterText = iterText.right(iterText.length() - closeIndex - 1);
				}
				else
				{
					for (int i = 0; i < iterText.length(); i++)
					{
						// white space no adding count
						// build normal word
						KJsonWord word;
						word.text = iterText.at(i);
						sentenceIt->wordlist.append(word);

						normalText += word.text;
						rubiedText += word.text;

						if (!word.text.trimmed().isEmpty())
						{
							wordRubyCount++;
						}
					}
					break;
				}
			}
		}

		sentenceIt->normaltext = normalText;
		sentenceIt->rubiedtext = rubiedText;

		int curCount = 0;
		for (int i = 0; i < sentenceIt->wordlist.size(); i++)
		{
			sentenceIt->wordlist[i].begin = firstBegin + curCount*totalDuration / wordRubyCount;
			int rubyCount = sentenceIt->wordlist[i].rubylist.size();
			if (rubyCount)
			{
				sentenceIt->wordlist[i].end = sentenceIt->wordlist[i].begin + rubyCount*totalDuration / wordRubyCount;
				for (int j = 0; j < rubyCount; j++)
				{
					sentenceIt->wordlist[i].rubylist[j].begin = firstBegin + curCount*totalDuration / wordRubyCount;
					sentenceIt->wordlist[i].rubylist[j].end = sentenceIt->wordlist[i].rubylist[j].begin + totalDuration / wordRubyCount;
					curCount++;
				}
				continue;
			}
			else
			{
				if (!sentenceIt->wordlist[i].text.trimmed().isEmpty())
				{
					sentenceIt->wordlist[i].end = sentenceIt->wordlist[i].begin + totalDuration / wordRubyCount;
				}
				else
				{
					sentenceIt->wordlist[i].end = sentenceIt->wordlist[i].begin;
				}
			}
			curCount++;
		}
	}
}