#include "lyricframe.h"
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QResizeEvent>

#define SIZEGRIP_SIZE	10

LyricFrame::LyricFrame(QWidget *parent)
	: QFrame(parent)
{
	_pMovingUpdateTimer = new QTimer(this);
	connect(_pMovingUpdateTimer, SIGNAL(timeout()), this, SLOT(slotMovingUpdateTimer()));

	_pSizeGripLB = new QSizeGrip(this);
	_pSizeGripRB = new QSizeGrip(this);
}

LyricFrame::~LyricFrame()
{

}

void LyricFrame::setMoveHandlingWidget(QWidget *pw)
{
	_pHandlingWidget = pw;

}

void LyricFrame::stopMoving()
{
	_bMoving = false;
}

void LyricFrame::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		_bMoving = true;
		_lastMovingPos = QCursor::pos();
		_pMovingUpdateTimer->start(16);
	}
	QFrame::mousePressEvent(event);

}

void LyricFrame::mouseReleaseEvent(QMouseEvent *event)
{
	if (_bMoving && event->button() == Qt::LeftButton)
	{
		_bMoving = false;
	}
	QFrame::mouseReleaseEvent(event);
}

void LyricFrame::resizeEvent(QResizeEvent *e)
{
	if (_pSizeGripLB)
	{
		_pSizeGripLB->setGeometry(0, e->size().height() - SIZEGRIP_SIZE, SIZEGRIP_SIZE, SIZEGRIP_SIZE);
	}
	if (_pSizeGripRB)
	{
		_pSizeGripRB->setGeometry(e->size().width() - SIZEGRIP_SIZE, e->size().height() - SIZEGRIP_SIZE, SIZEGRIP_SIZE, SIZEGRIP_SIZE);
	}
}

void LyricFrame::slotMovingUpdateTimer()
{
	if (!QApplication::mouseButtons().testFlag(Qt::LeftButton))
	{
		_bMoving = false;
	}
	if (!_bMoving)
	{
		_pMovingUpdateTimer->stop();
		return;
	}
	QPoint newpos = QCursor::pos();
	_pHandlingWidget->move(_pHandlingWidget->pos() + newpos - _lastMovingPos);
	_lastMovingPos = newpos;
	QCursor::pos();

}