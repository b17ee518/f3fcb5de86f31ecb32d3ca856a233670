#include "qdirectslider.h"
#include <QMouseEvent>

QDirectSlider::QDirectSlider(QWidget *parent)
	: QSlider(parent)
{

}

QDirectSlider::~QDirectSlider()
{

}

void QDirectSlider::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (orientation() == Qt::Vertical)
		{
			setValue(minimum() + ((maximum() - minimum()) * (height() - event->y())) / height());
		}
		else
		{
			setValue(minimum() + ((maximum() - minimum()) * event->x()) / width());
		}

		emit directJumped(value());

		event->accept();
	}
	QSlider::mousePressEvent(event);
}
