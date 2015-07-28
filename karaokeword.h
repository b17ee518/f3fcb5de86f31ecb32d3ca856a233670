#ifndef KARAOKEWORD_H
#define KARAOKEWORD_H

#include <QWidget>
#include <QSize>
#include <QGraphicsEffect>
#include "RubyChar.h"

class KaraokeWord : public QWidget
{
	Q_OBJECT

public:
	KaraokeWord(QWidget *parent);
	~KaraokeWord();

	void setLyric(const QString& lyric);
	void setBeginEnd(qint64 beginMS, qint64 endMS);

	void act(qint64 curMS);

	void addRubyChar(const QString& rubyText, qint64 beginMS, qint64 endMS);

	void rebuild();

	QSize boundingBoxSize();

protected:
	void paintEvent(QPaintEvent * e) override;
	void showEvent(QShowEvent *e) override;

private:
	void BuildWord();
	
private:

	void updateProportion(qreal prop);
	void ClearPixmap();

	QString _lyric;

	qint64 _beginMS = 0;
	qint64 _endMS = 1;

	QImage * _pre = NULL;
	QImage * _post = NULL;

	QList<RubyChar> _rubyList;

	qreal _proportion = 0.0;
	
};

#endif // KARAOKEWORD_H
