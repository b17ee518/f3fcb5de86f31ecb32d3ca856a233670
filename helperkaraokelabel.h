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

	QSizeF RenderToImage(QImage** pixmap, const QString& lyric, QList<RubyChar>& ruby, const QColor& textCol, const QColor& strokeColor);

	void setRubyHidden(bool bHidden);

public:
	inline qreal rubyOffset(){ return _rubyOffset; }

protected:
	void paintEvent(QPaintEvent *e) override;

private:
	qreal shadowDepth();
	
private:
	QColor _textColor;
	QColor _strokeColor;

	qreal _rubyOffset = 0.0;
	bool _isRuby = false;

	qreal _maxSubW = 0.0;
	qreal _maxSubH = 0.0;

	bool _isRubyHidden = false;

	const qreal shadowDepthScale = 0.06;
	
};

#endif // HELPERKARAOKELABEL_H
