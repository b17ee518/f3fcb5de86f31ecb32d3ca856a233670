#ifndef LYRICFRAME_H
#define LYRICFRAME_H

#include <QFrame>
#include <QSizeGrip>

class LyricFrame : public QFrame
{
	Q_OBJECT

public:
	LyricFrame(QWidget *parent);
	~LyricFrame();

	void setMoveHandlingWidget(QWidget *pw);
	void stopMoving();
	
protected:
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;

	void resizeEvent(QResizeEvent *e) override;

private:
	bool _bMoving = false;
	QPoint _lastMovingPos;
	QTimer* _pMovingUpdateTimer = NULL;
	QWidget* _pHandlingWidget = NULL;

	QSizeGrip* _pSizeGripLB = NULL;
	QSizeGrip* _pSizeGripRB = NULL;

private slots:
	void slotMovingUpdateTimer();
};

#endif // LYRICFRAME_H
