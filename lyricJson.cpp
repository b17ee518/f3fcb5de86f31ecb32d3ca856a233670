#include "LyricJson.h"
#include "Settings.h"


LyricJson::LyricJson()
{
}


LyricJson::~LyricJson()
{
}

bool LyricJson::loadJson(const QString& path)
{
	QJsonDocument document;

	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}
	QJsonParseError error;
	document = QJsonDocument::fromJson(file.readAll(), &error);
	file.close();
	if (error.error != QJsonParseError::NoError)
	{
		return false;
	}

	QJsonObject  root = document.object();
	_song.Clear();
	if (!_song.ReadFromJson(root))
	{
		return false;
	}
	prepare();

	return true;
}

void LyricJson::exportToJson(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}
	QJsonDocument doc;
	QJsonObject  songNode;
	_song.Export(songNode);
	doc.setObject(songNode);
	QTextStream stream(&file);
	stream.setGenerateByteOrderMark(true);
	stream << doc.toJson(/*QJsonDocument::Compact*/);
	file.close();
}

bool LyricJson::isControlOrInfoText(const QString& text)
{
	int textcount = text.count();
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
			return true;
		}
	}
	return false;
}

void LyricJson::treatSentecesAsControl()
{
	for (int i = 0; i < _song.lyric.sentencelist.count(); i++)
	{
		if (_song.lyric.sentencelist[i].wordlist.count() != 1)
		{
			continue;
		}
		QString text = _song.lyric.sentencelist[i].wordlist.first().text;
		if (!isControlOrInfoText(text))
		{
			text = "[control:" + text + "]";
			_song.lyric.sentencelist[i].wordlist[0].text = text;
		}
	}
}

KJsonSentence LyricJson::buildEmptyLine(int lineNum)
{
	KJsonSentence sentence;
	sentence.line = lineNum;
	sentence.color = 0;
	return sentence;
}

QString KJsonBase::subTextForElem(const QJsonObject & elem, const QString& name)
{
	if (elem.contains(name))
	{
		return elem[name].toString();
	}
	return "";
}

qint64 KJsonBase::subInt64ForElem(const QJsonObject & elem, const QString& name, qint64 def)
{
	if (elem.contains(name))
	{
		return elem[name].toDouble();
	}
	return def;
}

void KJsonBase::addTextToElem(QJsonObject & elem, const QString& name, const QString& text)
{
	elem[name] = text;
}

void KJsonBase::addInt64ToElem(QJsonObject & elem, const QString& name, qint64 value)
{
	elem[name] = value;
}

bool KJsonInfo::ReadFromJson(const QJsonObject & elem)
{
	title = subTextForElem(elem, "title");
	author = subTextForElem(elem, "author");
	artist = subTextForElem(elem, "artist");
	album = subTextForElem(elem, "album");
	by = subTextForElem(elem, "by");
	description = subTextForElem(elem, "description");

	return true;
}

void KJsonInfo::Export(QJsonObject & elem)
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

bool KJsonGeneral::ReadFromJson(const QJsonObject & elem)
{
	maxline = subInt64ForElem(elem, "maxline", maxline);
	offset = subInt64ForElem(elem, "offset", offset);

	return true;
}

void KJsonGeneral::Export(QJsonObject & elem)
{
	addInt64ToElem(elem, "maxline", maxline);
	if (offset != 0)
	{
		addInt64ToElem(elem, "offset", offset);
	}
}

bool KJsonRuby::ReadFromJson(const QJsonObject & elem)
{
	text = subTextForElem(elem, "text");
	begin = subInt64ForElem(elem, "begin", begin);
	end = subInt64ForElem(elem, "end", end);

	return true;
}

void KJsonRuby::Export(QJsonObject & elem)
{
	addTextToElem(elem, "text", text);
	if (begin >= 0)
	{
		addInt64ToElem(elem, "begin", begin);
	}
	if (end >= 0)
	{
		addInt64ToElem(elem, "end", end);
	}
}

bool KJsonWord::ReadFromJson(const QJsonObject & elem)
{
	text = subTextForElem(elem, "text");
	color = subInt64ForElem(elem, "color", color);
	begin = subInt64ForElem(elem, "begin", begin);
	end = subInt64ForElem(elem, "end", end);
	rubyhidden = subInt64ForElem(elem, "rubyhidden", rubyhidden);
	auto rubies = elem["ruby"];
	if (rubies.isArray())
	{
		auto rubyArray = rubies.toArray();
		for (int i = 0; i < rubyArray.count(); i++)
		{
			KJsonRuby ruby;
			ruby.ReadFromJson(rubyArray.at(i).toObject());
			rubylist.append(ruby);
		}
	}

	return true;
}

void KJsonWord::Export(QJsonObject & elem)
{
	addTextToElem(elem, "text", text);
	if (color >= 0)
	{
		addInt64ToElem(elem, "color", color);
	}
	if (begin >= 0)
	{
		addInt64ToElem(elem, "begin", begin);
	}
	if (end >= 0)
	{
		addInt64ToElem(elem, "end", end);
	}
	if (rubyhidden > 0)
	{
		addInt64ToElem(elem, "rubyhidden", rubyhidden);
	}

	QJsonArray rubyArray;
	Q_FOREACH(auto ruby, rubylist)
	{
		QJsonObject  rubyNode;
		ruby.Export(rubyNode);
		rubyArray.append(rubyNode);
	}
	if (rubyArray.size())
	{
		elem["ruby"] = rubyArray;
	}
}

bool KJsonSentence::ReadFromJson(const QJsonObject & elem)
{
	line = subInt64ForElem(elem, "line", line);
	color = subInt64ForElem(elem, "color", color);
	begin = subInt64ForElem(elem, "begin", begin);
	end = subInt64ForElem(elem, "end", end);

	normaltext = subTextForElem(elem, "normaltext");
	rubiedtext = subTextForElem(elem, "rubiedtext");

	auto words = elem["word"];
	if (words.isArray())
	{
		auto wordArray = words.toArray();
		for (int i = 0; i < wordArray.count(); i++)
		{
			KJsonWord word;
			word.ReadFromJson(wordArray.at(i).toObject());
			wordlist.append(word);
		}
	}

	return true;
}

void KJsonSentence::Export(QJsonObject & elem)
{
	if (line >= 0)
	{
		addInt64ToElem(elem, "line", line);
	}
	if (color >= 0)
	{
		addInt64ToElem(elem, "color", color);
	}
	if (begin >= 0)
	{
		addInt64ToElem(elem, "begin", begin);
	}
	if (end >= 0)
	{
		addInt64ToElem(elem, "end", end);
	}

	if (!normaltext.isEmpty())
	{
		addTextToElem(elem, "normaltext", normaltext);
	}
	if (!rubiedtext.isEmpty())
	{
		addTextToElem(elem, "rubiedtext", rubiedtext);
	}

	QJsonArray wordArray;
	Q_FOREACH(auto word, wordlist)
	{
		QJsonObject  wordNode;
		word.Export(wordNode);
		wordArray.append(wordNode);
	}
	if (wordArray.size())
	{
		elem["word"] = wordArray;
	}
}

bool KJsonLyric::ReadFromJson(const QJsonObject & elem)
{
	auto sentences = elem["sentence"];
	if (sentences.isArray())
	{
		auto sentenceArray = sentences.toArray();
		for (int i = 0; i < sentenceArray.count(); i++)
		{
			KJsonSentence sentence;
			sentence.ReadFromJson(sentenceArray.at(i).toObject());
			sentencelist.append(sentence);
		}
	}

	return true;
}

void KJsonLyric::Export(QJsonObject & elem)
{
	QJsonArray sentenceArray;
	Q_FOREACH(auto sentence, sentencelist)
	{
		QJsonObject  sentenceNode;
		sentence.Export(sentenceNode);
		sentenceArray.append(sentenceNode);
	}
	if (sentenceArray.size())
	{
		elem["sentence"] = sentenceArray;
	}
}

bool KJsonSong::ReadFromJson(const QJsonObject & elem)
{
	auto infoObj = elem["info"].toObject();
	info.ReadFromJson(infoObj);

	auto generalObj = elem["general"].toObject();
	general.ReadFromJson(generalObj);

	auto lyricObj = elem["lyric"].toObject();
	return lyric.ReadFromJson(lyricObj);
}

void KJsonSong::Export(QJsonObject & elem)
{
	QJsonObject  infoNode;
	info.Export(infoNode);
	elem["info"] = infoNode;

	QJsonObject  generalNode;
	general.Export(generalNode);
	elem["general"] = generalNode;

	QJsonObject  lyricNode;
	lyric.Export(lyricNode);
	elem["lyric"] = lyricNode;
}
