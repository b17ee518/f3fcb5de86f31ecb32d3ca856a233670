#ifndef QDIRECTSLIDER_H
#define QDIRECTSLIDER_H

#include <QSlider>

class QDirectSlider : public QSlider
{
	Q_OBJECT

public:
	QDirectSlider(QWidget *parent);
	~QDirectSlider();

protected:
	void mousePressEvent(QMouseEvent * event);
private:
	
signals :
	void directJumped(int position);
};

#endif // QDIRECTSLIDER_H
