#include "LyricXML.h"


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
	else
	{
		if (!document.setContent(&file))
		{
			return false;
		}
		file.close();
	}

	QDomElement root = document.firstChildElement();
	_song.Clear();
	_song.ReadFromXML(root);
	prepare();

	/*
	auto elem = root;
	auto gen = elem.elementsByTagName("general").at(0).toElement();
	auto line = gen.elementsByTagName("maxline").at(0).toElement();
	auto te = line.text();
	*/

	return true;
}

void LyricXML::prepare()
{

}

QString LyricXML::subTextForElem(const QDomElement& elem, const QString& name)
{
	auto elements = elem.elementsByTagName(name);
	if (!elements.count())
	{
		return "";
	}
	return elements.at(0).toElement().text();
}

bool KXMLInfo::ReadFromXML(const QDomElement& elem)
{
	auto xml = LyricXML::getInstance();

	name = xml->subTextForElem(elem, "name");
	singer = xml->subTextForElem(elem, "singer");
	description = xml->subTextForElem(elem, "description");

	return true;
}

bool KXMLGeneral::ReadFromXML(const QDomElement& elem)
{
	auto xml = LyricXML::getInstance();

	auto maxlineText = xml->subTextForElem(elem, "maxline");
	if (!maxlineText.isEmpty())
	{
		maxline = maxlineText.toInt();
	}

	return true;
}

bool KXMLRuby::ReadFromXML(const QDomElement& elem)
{
	auto xml = LyricXML::getInstance();

	text = xml->subTextForElem(elem, "text");
	auto birthText = xml->subTextForElem(elem, "birth");
	if (!birthText.isEmpty())
	{
		birth = birthText.toLongLong();
	}
	auto durationText = xml->subTextForElem(elem, "duration");
	if (!durationText.isEmpty())
	{
		duration = durationText.toLongLong();
	}

	return true;
}

bool KXMLWord::ReadFromXML(const QDomElement& elem)
{
	auto xml = LyricXML::getInstance();

	text = xml->subTextForElem(elem, "text");
	auto colorText = xml->subTextForElem(elem, "color");
	if (!colorText.isEmpty())
	{
		color = colorText.toInt();
	}
	auto birthText = xml->subTextForElem(elem, "birth");
	if (!birthText.isEmpty())
	{
		birth = birthText.toLongLong();
	}
	auto durationText = xml->subTextForElem(elem, "duration");
	if (!durationText.isEmpty())
	{
		duration = durationText.toLongLong();
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

bool KXMLSentence::ReadFromXML(const QDomElement& elem)
{
	auto xml = LyricXML::getInstance();

	auto lineText = xml->subTextForElem(elem, "line");
	if (!lineText.isEmpty())
	{
		line = lineText.toInt();
	}
	auto colorText = xml->subTextForElem(elem, "color");
	if (!colorText.isEmpty())
	{
		color = colorText.toInt();
	}
	auto birthText = xml->subTextForElem(elem, "birth");
	if (!birthText.isEmpty())
	{
		birth = birthText.toLongLong();
	}
	auto durationText = xml->subTextForElem(elem, "duration");
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
