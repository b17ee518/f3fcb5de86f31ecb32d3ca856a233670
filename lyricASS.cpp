#include "lyricJson.h"
#include <QTime>
#include <QFileInfo>
#include "Settings.h"

QString LyricJson::MStoTimeStr(qint64 msec)
{
	QString str;
	if (msec >= 0)
	{
		str = QTime::fromMSecsSinceStartOfDay(msec).toString("H:mm:ss.zzz");
	}
	else
	{
		str = "9:59:59.990";
	}
	str = str.left(str.length() - 1);
	return str;
}


void LyricJson::writeAssLineToStream(QTextStream& stream, int rubyCount, int sentenceCode, qint64 begin, qint64 end, int line, int color, bool rubyHidden, const QString& text)
{
	QString beginStr = MStoTimeStr(begin);
	QString endStr = MStoTimeStr(end);

	int layer = rubyCount + sentenceCode * operationCodeMask;
	int colorRubyHidden = color + (rubyHidden ? operationCodeMask : 0);

	stream << "Dialogue: "
		<< layer << ","
		<< beginStr << ","
		<< endStr << ","
		<< line << ","
		<< colorRubyHidden << ","
		<< "0,0,0,,"
		<< text
		<< "\n";
}


qint64 LyricJson::TimeStrToMS(const QString& timestr)
{
	QString str = timestr + "0";
	if (!str.compare("9:59:59.990"))
	{
		return -1;
	}
	return QTime::fromString(str, "H:mm:ss.zzz").msecsSinceStartOfDay();
}


bool LyricJson::loadASS(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	KJsonSentence* pSentence = NULL;
	KJsonWord* pWord = NULL;
	int rubyCountdown = 0;

	_song.Clear();
	while (!file.atEnd())
	{
		auto lineStr = file.readLine();

		// info
		if (lineStr.startsWith("title:"))
		{
			_song.info.title = lineStr.right(lineStr.length() - 6).trimmed();
		}
		if (lineStr.startsWith("author:"))
		{
			_song.info.author = lineStr.right(lineStr.length() - 7).trimmed();
		}
		if (lineStr.startsWith("artist:"))
		{
			_song.info.artist = lineStr.right(lineStr.length() - 7).trimmed();
		}
		if (lineStr.startsWith("album:"))
		{
			_song.info.album = lineStr.right(lineStr.length() - 6).trimmed();
		}
		if (lineStr.startsWith("by:"))
		{
			_song.info.by = lineStr.right(lineStr.length() - 7).trimmed();
		}
		if (lineStr.startsWith("description:"))
		{
			_song.info.description = lineStr.right(lineStr.length() - 12).trimmed();
		}

		// general
		if (lineStr.startsWith("maxline:"))
		{
			_song.general.maxline = lineStr.right(lineStr.length() - 8).trimmed().toInt();
		}
		if (lineStr.startsWith("offset:"))
		{
			_song.general.offset = lineStr.right(lineStr.length() - 7).trimmed().toInt();
		}

		// lyric
		if (lineStr.startsWith("Dialogue:"))
		{
			QString eventStr = lineStr.right(lineStr.length() - 9).trimmed();
			QStringList splited = eventStr.split(",");
			Q_ASSERT(splited.size() >= 10);

			int layer = splited.at(0).toInt();
			QString beginStr = splited.at(1);
			QString endStr = splited.at(2);
			int line = splited.at(3).toInt();
			int colorRubyHidden = splited.at(4).toInt();
			QString text = splited.at(9);
			for (int i = 10; i < splited.size() - 1; i++)
			{
				text += "," + splited.at(i);
			}

			qint64 begin = TimeStrToMS(beginStr);
			qint64 end = TimeStrToMS(endStr);

			bool rubyHidden = false;
			int color = colorRubyHidden;
			if (color > operationCodeMask/2)
			{
				color -= operationCodeMask;
				rubyHidden = true;
			}

			if (layer >= operationCodeMask)
			{
				// it's a sentence
				int op = layer / operationCodeMask;
				if (op == 1)
				{
					// begin of a sentence
					KJsonSentence sentence;
					sentence.begin = begin;
					sentence.end = end;
					sentence.color = color;
					sentence.line = line;
					sentence.normaltext = text;
					_song.lyric.sentencelist.append(sentence);
					pSentence = &(_song.lyric.sentencelist[_song.lyric.sentencelist.size() - 1]);
				}
				else
				{
					// end of a sentence
					Q_ASSERT(pSentence);
					pSentence->rubiedtext = text;
					pSentence = NULL;
				}
			}
			else
			{
				// it's a word or ruby
				if (rubyCountdown > 0)
				{
					// still in a ruby
					Q_ASSERT(pWord);

					KJsonRuby ruby;
					ruby.begin = begin;
					ruby.end = end;
					ruby.text = text;
					pWord->rubylist.append(ruby);

					rubyCountdown--;
					if (!rubyCountdown)
					{
						pWord->begin = pWord->rubylist.first().begin;
						pWord->end = pWord->rubylist.last().end;
						pWord = NULL;
					}
				}
				else
				{
					// it's a word
					int textcount = text.count();
					bool isControlText = false;
					if (text.startsWith("[") && text.endsWith("]"))
					{
						QString testText = text.mid(1, textcount - 2);
						if (testText == "title"
							|| testText == "artist"
							|| testText == "author"
							|| testText == "album"
							|| testText == "by"
							|| testText == "description"
							|| testText.startsWith("control:"))
						{
							isControlText = true;
							// treat as single
						}
					}
					if (layer == 0 && textcount > 1 && (!isControlText))
					{
						// separate to several
						for (int i = 0; i < textcount; i++)
						{
							QString subtext = text[i];
							KJsonWord word;
							if (end < 0 || begin < 0)
							{
								word.begin = begin;
							}
							else
							{
								int duration = end - begin;
								word.begin = begin + i*duration / textcount;
								word.end = word.begin + duration / textcount;
							}
							word.color = color;
							word.rubyhidden = rubyHidden;
							word.text = subtext;

							Q_ASSERT(pSentence);
							pSentence->wordlist.append(word);
						}
					}
					else
					{
						// singles
						KJsonWord word;
						word.begin = begin;
						word.end = end;
						word.color = color;
						word.rubyhidden = rubyHidden;
						word.text = text;

						Q_ASSERT(pSentence);
						pSentence->wordlist.append(word);

						if (layer > 0)
						{
							rubyCountdown = layer;
							// has ruby
							pWord = &(pSentence->wordlist[pSentence->wordlist.size() - 1]);
						}

					}
					
				}				
			}
		}

	}

	file.close();


	QFileInfo info(path);
	QString strBase = info.absolutePath() + "/" + info.completeBaseName();
	QString strJson = strBase + Settings::getInstance()->jsonExtention;
	exportToJson(strJson);

	loadJson(strJson);

	return false;
}

void LyricJson::exportToASS(const QString& path, const QString& musicPath)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream stream(&file);
	stream.setGenerateByteOrderMark(true);

	stream << "[Script Info]" << "\n";
	stream << "title:" << _song.info.title << "\n";
	stream << "author:" << _song.info.author << "\n";
	stream << "artist:" << _song.info.artist << "\n";
	stream << "album:" << _song.info.album << "\n";
	stream << "by:" << _song.info.by << "\n";
	stream << "description:" << _song.info.description << "\n";

	stream << "maxline:" << _song.general.maxline << "\n";
	stream << "offset:" << _song.general.offset << "\n";

	stream << "[Aegisub Project Garbage]" << "\n";
	stream << "Export Encoding: Unicode (UTF-8)" << "\n";
	stream << "Audio File: " << musicPath << "\n";

	stream << "[Events]" << "\n";

	QString beginStr;
	QString endStr;
	Q_FOREACH(auto sentence, _song.lyric.sentencelist)
	{
		writeAssLineToStream(stream, 0, 1, sentence.begin, sentence.end, sentence.line, sentence.color, false, sentence.normaltext);
		Q_FOREACH(auto word, sentence.wordlist)
		{
			writeAssLineToStream(stream, word.rubylist.count(), 0, word.begin, word.end, -1, word.color, word.rubyhidden, word.text);
			Q_FOREACH(auto ruby, word.rubylist)
			{
				writeAssLineToStream(stream, 0, 0, ruby.begin, ruby.end, -1, -1, false, ruby.text);
			}
		}
		writeAssLineToStream(stream, 0, 2, sentence.begin, sentence.end, sentence.line, sentence.color, false, sentence.rubiedtext);
	}

	file.close();
}
