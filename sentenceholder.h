#ifndef SENTENCEHOLDER_H
#define SENTENCEHOLDER_H

#include <QFrame>
#include "karaokeword.h"
#include <QHBoxLayout>
#include <QSpacerItem>

#define DIRECTION_LEFT		0
#define DIRECTION_RIGHT	1

class SentenceHolder : public QFrame
{
	Q_OBJECT

public:
	SentenceHolder(QWidget *parent);
	~SentenceHolder();

	void addWord(KaraokeWord* word);
	void clearSentence();
	void setBeginMarginSpace(qreal space);

	void setClearTime(qint64 clearTime);

	void setDirection(int dir);

	void act(qint64 curMS);

public:
	inline qint64 clearTime(){ return _clearTime; }

private:
	QHBoxLayout * _layout = NULL;
	int _direction = DIRECTION_LEFT;

	QSpacerItem* _horizontalSpacer = NULL;

	qreal _beginMarginSpace = 0.0;

	qint64 _clearTime = 0;
	
public slots :
	void settingsChanged();
	
};

#endif // SENTENCEHOLDER_H
