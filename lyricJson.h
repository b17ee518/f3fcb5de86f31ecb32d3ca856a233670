#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>

class KJsonBase
{
public:
	KJsonBase(){ }

	virtual bool ReadFromJson(const QJsonObject & elem) = 0;
	virtual void Export(QJsonObject & elem) = 0;
	virtual void Clear() = 0;


public:
	QString subTextForElem(const QJsonObject & elem, const QString& name);
	void addTextToElem(QJsonObject & elem, const QString& name, const QString& text);
};

class KJsonInfo : KJsonBase
{
public:
	KJsonInfo(){ Clear(); }

	virtual bool ReadFromJson(const QJsonObject & elem) override;
	virtual void Export(QJsonObject & elem) override;
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

class KJsonGeneral : KJsonBase
{
public:
	KJsonGeneral(){ Clear(); }

	virtual bool ReadFromJson(const QJsonObject & elem) override;
	virtual void Export(QJsonObject & elem) override;
	virtual void Clear() override
	{
		maxline = 2;
		offset = 0;
	}

	int maxline = 2;
	qint64 offset;
};

class KJsonRuby : KJsonBase
{
public:
	KJsonRuby(){ Clear(); }

	virtual bool ReadFromJson(const QJsonObject & elem) override;
	virtual void Export(QJsonObject & elem) override;
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

class KJsonWord : KJsonBase
{
public:
	KJsonWord(){ Clear(); }

	virtual bool ReadFromJson(const QJsonObject & elem) override;
	virtual void Export(QJsonObject & elem) override;
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
	QList<KJsonRuby> rubylist;
};

class KJsonSentence : KJsonBase
{
public:
	KJsonSentence(){ Clear(); }

	virtual bool ReadFromJson(const QJsonObject & elem) override;
	virtual void Export(QJsonObject & elem) override;
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

	QList<KJsonWord> wordlist;
};

class KJsonLyric : KJsonBase
{
public:
	KJsonLyric(){ Clear(); }

	virtual bool ReadFromJson(const QJsonObject & elem) override;
	virtual void Export(QJsonObject & elem) override;
	virtual void Clear() override
	{
		sentencelist.clear();
	}

	QList<KJsonSentence> sentencelist;
};

class KJsonSong : KJsonBase
{
public:
	KJsonSong(){ Clear(); }

	virtual bool ReadFromJson(const QJsonObject & elem) override;
	virtual void Export(QJsonObject & elem) override;
	virtual void Clear() override
	{
		info.Clear();
		general.Clear();
		lyric.Clear();
	}

	KJsonInfo info;
	KJsonGeneral general;
	KJsonLyric lyric;
};

class LyricJson
{
private:

	class ParagraphData
	{
	public:
		ParagraphData(){}

		qint64 birth = 0;
		qint64 duration = 0;
	};

	LyricJson();
	LyricJson(LyricJson const&);
	LyricJson& operator=(LyricJson const&);
	~LyricJson();

public:
	static LyricJson* getInstance() {
		static LyricJson instance;
		return &instance;
	}

public:
	bool loadJson(const QString& path);

	bool loadLRC(const QString& path, const QString& musicPath);
	bool loadASS(const QString& path);

	inline const KJsonSong song(){ return _song; }

	void exportToJson(const QString& path);
	void exportToASS(const QString& path, const QString& musicPath);

private:
	void prepare();

	void lrcWordSeparate();
	QString MStoTimeStr(qint64 msec);
	qint64 TimeStrToMS(const QString& timestr);
	void writeAssLineToStream(QTextStream& stream, 
		int rubyCount, int sentenceCode, 
		qint64 birth, qint64 duration, 
		int line, int color, bool rubyHidden, 
		const QString& text);

	KJsonSentence buildEmptyLine(int lineNum);

	const int operationCodeMask = 100000;

private:
	KJsonSong _song;

};

