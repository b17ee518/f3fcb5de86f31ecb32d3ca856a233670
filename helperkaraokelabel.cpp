#include "helperkaraokelabel.h"

#include "Settings.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

HelperKaraokeLabel::HelperKaraokeLabel(QWidget *parent)
	: QLabel(parent)
{

}

HelperKaraokeLabel::~HelperKaraokeLabel()
{

}

QSize HelperKaraokeLabel::RenderToImage(QImage** ppimage, const QString& lyric, QList<RubyChar>& ruby, const QColor& textCol, const QColor& strokeColor)
{
	if (!ppimage)
	{
		return QSize();
	}
	auto pimage = *ppimage;
	if (pimage)
	{
		delete pimage;
		pimage = NULL;
	}
	this->setAutoFillBackground(false);

	auto setting = Settings::getInstance();
	_textColor = textCol;
	_strokeColor = strokeColor;

	setText(lyric);

	QFont f(setting->fontName(), setting->fontSize(), 99);
	f.setPixelSize(setting->fontSize());
	f.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 5);

	setFont(f);
	
	auto size = minimumSizeHint();
	setFixedSize(size);

	pimage = new QImage(size, QImage::Format_ARGB32);
	QPainter painter(pimage);
	render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);

	*ppimage = pimage;
	
	return size;
}

void HelperKaraokeLabel::paintEvent(QPaintEvent *e)
{
//	QLabel::paintEvent(e);

	auto size = this->sizeHint();
	qreal y = size.height() - Settings::getInstance()->verticalSpace;

	QPainter painter(this);

	QPen pen;
	pen.setWidth(4);
	painter.setFont(font());

	pen.setColor(Qt::black);
	painter.setPen(pen);

	painter.drawText(QPoint(4, y + 1), text());

	pen.setColor(_strokeColor);
	painter.setPen(pen);

	painter.drawText(QPoint(-1, y - 1), text());
	painter.drawText(QPoint(-1, y + 1), text());
	painter.drawText(QPoint(1, y - 1), text());
	painter.drawText(QPoint(1, y + 1), text());

	pen.setColor(_textColor);

	painter.setPen(pen);
	painter.drawText(QPoint(0, y), text());
}
