#ifndef HELPERKARAOKELABEL_H
#define HELPERKARAOKELABEL_H

#include <QLabel>
#include "RubyChar.h"

class HelperKaraokeLabel : public QLabel
{
	Q_OBJECT
	
public:
	HelperKaraokeLabel(QWidget *parent);
	~HelperKaraokeLabel();

	QSize RenderToImage(QImage** pixmap, const QString& lyric, QList<RubyChar>& ruby, const QColor& textCol, const QColor& strokeColor);

protected:
	void paintEvent(QPaintEvent *e) override;
	
private:
	QColor _textColor;
	QColor _strokeColor;

	qreal _rubyOffset = 0.0;
	
};

#endif // HELPERKARAOKELABEL_H
