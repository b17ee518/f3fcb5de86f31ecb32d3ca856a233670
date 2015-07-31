#ifndef LYRICFRAME_H
#define LYRICFRAME_H

#include <QFrame>
#include <QSizeGrip>
#include <QList>
#include <QElapsedTimer>

#include "sentenceholder.h"
#include <QVBoxLayout>

class LyricFrame : public QFrame
{
	Q_OBJECT

public:
	LyricFrame(QWidget *parent);
	~LyricFrame();

	void setMoveHandlingWidget(QWidget *pw);
	void stopMoving();

	void setMaxSentences(int count);

	void BuildByXML();

	void Play();
	void Pause();
	void Stop();
	void Jumped(qint64 oldPosition);
	
protected:
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;

	void resizeEvent(QResizeEvent *e) override;

	void showEvent(QShowEvent *e) override;

private:
	bool _bMoving = false;
	QPoint _lastMovingPos;
	QTimer* _pMovingUpdateTimer = NULL;
	QWidget* _pHandlingWidget = NULL;

	QSizeGrip* _pSizeGripLB = NULL;
	QSizeGrip* _pSizeGripRB = NULL;

	QList<SentenceHolder*> _sentences;

	QTimer* _updateTimer = NULL;
	QElapsedTimer* _elapsedTimer = NULL;

	QVBoxLayout * _layout = NULL;

	qint64 _previousMSec = std::numeric_limits<qint64>::min();

private slots:
	void slotMovingUpdateTimer();
	void slotOnUpdateTimer();
};

#endif // LYRICFRAME_H
