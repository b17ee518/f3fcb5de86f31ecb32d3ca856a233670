#pragma once

#include <QtXml>


class KXMLInfo
{
public:
	KXMLInfo(){ Clear(); }

	bool ReadFromXML(const QDomElement& elem);
	void Clear()
	{
		name = "";
		singer = "";
		description = "";
	}

	QString name;
	QString singer;
	QString description;
};

class KXMLGeneral
{
public:
	KXMLGeneral(){ Clear(); }

	bool ReadFromXML(const QDomElement& elem);
	void Clear()
	{
		maxline = 2;
	}

	int maxline = 2;
};

class KXMLRuby
{
public:
	KXMLRuby(){ Clear(); }

	bool ReadFromXML(const QDomElement& elem);
	void Clear()
	{
		text = "";
		birth = -1;
		duration = -1;
	}

	QString text;
	qint64 birth = -1;	// done
	qint64 duration = -1;	// no birth no duration OR all but last duration set
};

class KXMLWord
{
public:
	KXMLWord(){ Clear(); }

	bool ReadFromXML(const QDomElement& elem);
	void Clear()
	{
		text = "";
		color = -1;
		birth = -1;
		duration = -1;
		rubyhidden = 0;
		rubylist.clear();
	}

	QString text;
	int color = -1;	// done
	qint64 birth = -1;	// first must, done
	qint64 duration = -1;	// must set
	int rubyhidden = 0;	// no need to care
	QList<KXMLRuby> rubylist;
};

class KXMLSentence
{
public:
	KXMLSentence(){ Clear(); }

	bool ReadFromXML(const QDomElement& elem);
	void Clear()
	{
		line = 0;
		color = -1;
		birth = -1;
		duration = -1;
		wordlist.clear();

		_birthCalc = -1;
		_durationCalc = -1;
	}

	int line = -1;	// done
	int color = -1;	// done
	qint64 birth = -1;	// last step
	qint64 duration = -1;	// last step

	qint64 _birthCalc = -1;
	qint64 _durationCalc = -1;

	QList<KXMLWord> wordlist;
};

class KXMLLyric
{
public:
	KXMLLyric(){ Clear(); }

	bool ReadFromXML(const QDomElement& elem);
	void Clear()
	{
		sentencelist.clear();
	}

	QList<KXMLSentence> sentencelist;
};

class KXMLSong
{
public:
	KXMLSong(){ Clear(); }

	bool ReadFromXML(const QDomElement& elem);
	void Clear()
	{
		info.Clear();
		general.Clear();
		lyric.Clear();
	}

	KXMLInfo info;
	KXMLGeneral general;
	KXMLLyric lyric;
};


class LyricXML
{
private:

	class ParagraphData
	{
	public:
		ParagraphData(){}

		qint64 birth = 0;
		qint64 duration = 0;
	};

	LyricXML();
	LyricXML(LyricXML const&);
	LyricXML& operator=(LyricXML const&);
	~LyricXML();

public:
	static LyricXML* getInstance() {
		static LyricXML instance;
		return &instance;
	}

public:
	bool loadXML(const QString& path);
	inline const KXMLSong song(){ return _song; }

private:
	void prepare();
	KXMLSentence buildEmptyLine(int lineNum);

public:
	QString subTextForElem(const QDomElement& elem, const QString& name);

private:
	KXMLSong _song;

};

