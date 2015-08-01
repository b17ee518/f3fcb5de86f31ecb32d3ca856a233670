#pragma once

#include <QtXml>

class KXMLBase
{
public:
	KXMLBase(){ }

	virtual bool ReadFromXML(const QDomElement& elem) = 0;
	virtual void Export(QDomElement& elem) = 0;
	virtual void Clear() = 0;


public:
	QString subTextForElem(const QDomElement& elem, const QString& name);
	void addTextToElem(QDomElement& elem, const QString& name, const QString& text);
};

class KXMLInfo : KXMLBase
{
public:
	KXMLInfo(){ Clear(); }

	virtual bool ReadFromXML(const QDomElement& elem) override;
	virtual void Export(QDomElement& elem) override;
	virtual void Clear() override
	{
		title = "";
		author = "";
		artist = "";
		album = "";
		by = "";
		description = "";
	}

	QString title;
	QString author;
	QString artist;
	QString album;
	QString by;
	QString description;
};

class KXMLGeneral : KXMLBase
{
public:
	KXMLGeneral(){ Clear(); }

	virtual bool ReadFromXML(const QDomElement& elem) override;
	virtual void Export(QDomElement& elem) override;
	virtual void Clear() override
	{
		maxline = 2;
		offset = 0;
	}

	int maxline = 2;
	qint64 offset;
};

class KXMLRuby : KXMLBase
{
public:
	KXMLRuby(){ Clear(); }

	virtual bool ReadFromXML(const QDomElement& elem) override;
	virtual void Export(QDomElement& elem) override;
	virtual void Clear() override
	{
		text = "";
		birth = -1;
		duration = -1;
	}

	QString text;
	qint64 birth = -1;	// done
	qint64 duration = -1;	// no birth no duration OR all but last duration set
};

class KXMLWord : KXMLBase
{
public:
	KXMLWord(){ Clear(); }

	virtual bool ReadFromXML(const QDomElement& elem) override;
	virtual void Export(QDomElement& elem) override;
	virtual void Clear() override
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

class KXMLSentence : KXMLBase
{
public:
	KXMLSentence(){ Clear(); }

	virtual bool ReadFromXML(const QDomElement& elem) override;
	virtual void Export(QDomElement& elem) override;
	virtual void Clear() override
	{
		line = -1;
		color = -1;
		birth = -1;
		duration = -1;
		wordlist.clear();

		_birthCalc = -1;
		_durationCalc = -1;

		normaltext = "";
		rubiedtext = "";
	}

	int line = -1;	// done
	int color = -1;	// done
	qint64 birth = -1;	// last step
	qint64 duration = -1;	// last step

	qint64 _birthCalc = -1;
	qint64 _durationCalc = -1;

	QString normaltext;
	QString rubiedtext;

	QList<KXMLWord> wordlist;
};

class KXMLLyric : KXMLBase
{
public:
	KXMLLyric(){ Clear(); }

	virtual bool ReadFromXML(const QDomElement& elem) override;
	virtual void Export(QDomElement& elem) override;
	virtual void Clear() override
	{
		sentencelist.clear();
	}

	QList<KXMLSentence> sentencelist;
};

class KXMLSong : KXMLBase
{
public:
	KXMLSong(){ Clear(); }

	virtual bool ReadFromXML(const QDomElement& elem) override;
	virtual void Export(QDomElement& elem) override;
	virtual void Clear() override
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
	bool loadLRC(const QString& path);
	inline const KXMLSong song(){ return _song; }

	void exportToXML(const QString& path);

private:
	void prepare();
	void lrcWordSeparate();
	KXMLSentence buildEmptyLine(int lineNum);

private:
	KXMLSong _song;

};

