#include "LyricXML.h"
#include "Settings.h"


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

	normaltext = subTextForElem(elem, "normaltext");
	rubiedtext = subTextForElem(elem, "rubiedtext");

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

	if (!normaltext.isEmpty())
	{
		addTextToElem(elem, "normaltext", normaltext);
	}
	if (!rubiedtext.isEmpty())
	{
		addTextToElem(elem, "rubiedtext", rubiedtext);
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
