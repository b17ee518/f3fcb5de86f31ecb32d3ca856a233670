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
	void setTextColor(int colorIndex);
	void setTextColor(const QColor& preTextColor, const QColor& postTextColor);
	void setTextColor(const QColor& preTextColor, const QColor& preStrokeColor, const QColor& postTextColor, const QColor& postStrokeColor);
	void setBeginEnd(qint64 beginMS, qint64 endMS);

	void setIsControlText(bool bValue);

	void act(qint64 curMS);

	void addRubyChar(const QString& rubyText, qint64 beginMS, qint64 endMS);
	void setRubyHidden(bool bHidden);

	void rebuild();
	void doPaint();

	QSizeF boundingBoxSize();

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

	QColor _preTextColor = Qt::white;
	QColor _preStrokeColor = Qt::black;
	QColor _postTextColor = Qt::red;
	QColor _postStrokeColor = Qt::white;

	QList<RubyChar> _rubyList;

	qreal _proportion = 0.0;
	qreal _rubyOffset = 0.0;

	bool _isRubyHidden = false;
	bool _isControlText = false;

	bool _locked = false;
};

#endif // KARAOKEWORD_H
