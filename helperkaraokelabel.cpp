#include "helperkaraokelabel.h"

#include "Settings.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

HelperKaraokeLabel::HelperKaraokeLabel(QWidget *parent)
	: QLabel(parent)
{
	this->setAutoFillBackground(false);
	this->setAttribute(Qt::WA_TranslucentBackground);
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
		*ppimage = NULL;
	}

	auto setting = Settings::getInstance();
	_textColor = textCol;
	_strokeColor = strokeColor;

	// text
	setText(lyric);

	// set font
	QFont f;
	f.setFamily(setting->fontName());
	f.setWeight(99);
	f.setPixelSize(setting->fontSize());
	f.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 5);
	setFont(f);
	
	// calc size
	auto size = minimumSizeHint();

	//////////////////////////////////////////////////////////////////////////
	// ruby
	int rubyCount = ruby.size();
	qreal maxSubW = 0.0;
	qreal maxSubH = 0.0;
	QList<HelperKaraokeLabel*> subList;
	for (int i = 0; i < rubyCount; i++)
	{
		HelperKaraokeLabel* sub = new HelperKaraokeLabel(this);
		sub->_isRuby = true;
		subList.append(sub);
		sub->setText(ruby[i].ruby());
		sub->_textColor = _textColor;
		sub->_strokeColor = _strokeColor;

		// set font
		QFont f;
		f.setFamily(setting->fontName());
		f.setWeight(99);
		f.setPixelSize(setting->rubyFontSize());
		f.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 3);
		sub->setFont(f);

		auto subsize = sub->minimumSizeHint();
		if (maxSubW < subsize.width())
		{
			maxSubW = subsize.width();
		}
		if (maxSubH < subsize.height())
		{
			maxSubH = subsize.height();
		}

		/*
		auto geo = sub->geometry();
		geo.setX(geo.x() + i*size.width() / rubyCount);
		sub->setGeometry(geo);
		*/
	}

	// rearrange size
	if (rubyCount > 0)
	{
		qreal subSpaceW = 0.0;
		maxSubH += 1.0; // shadow
		maxSubW += subSpaceW;

		qreal subTotalW = maxSubW*rubyCount - subSpaceW;
		_rubyOffset = (size.width() - subTotalW) / 2.0;

		if (size.width() < subTotalW)
		{
			size.setWidth(maxSubW*rubyCount-subSpaceW);
		}
		qreal subSpaceH = 4.0;
		size.setHeight(size.height() + maxSubH);

		for (int i = 0; i < rubyCount; i++)
		{
			subList[i]->setFixedSize(QSize(maxSubW, maxSubH));
			auto geo = subList[i]->geometry();
			geo.setX(geo.x() + i*maxSubW + (_rubyOffset>0?_rubyOffset:0));
			subList[i]->setGeometry(geo);
		}
		/*
		auto geo = this->geometry();
		geo.setY(maxSubH + subSpaceH);
		this->setGeometry(geo);
		*/
	}
	
	setFixedSize(size);


	// render to image
	pimage = new QImage(size, QImage::Format_ARGB32);
	QPainter painter(pimage);
	pimage->fill(qRgba(0, 0, 0, 0));
	render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
	

	// assign return value
	*ppimage = pimage;
	
	return size;
}

void HelperKaraokeLabel::paintEvent(QPaintEvent *e)
{
//	QLabel::paintEvent(e);

	auto size = this->sizeHint();
	auto setting = Settings::getInstance();
	qreal y = size.height() + (_isRuby ? 0 : (setting->rubyFontSize() - setting->verticalSpace));

	QPainter painter(this);

	QPen pen;
	pen.setWidth(4);
	painter.setFont(font());

	pen.setColor(Qt::black);
	painter.setPen(pen);

	qreal xoffset = _rubyOffset < 0 ? -_rubyOffset : 0;

	painter.drawText(QPoint(xoffset+shadowDepth(), y + 1), text());

	pen.setColor(_strokeColor);
	painter.setPen(pen);

	painter.drawText(QPoint(xoffset-1, y - 1), text());
	painter.drawText(QPoint(xoffset - 1, y + 1), text());
	painter.drawText(QPoint(xoffset+1, y - 1), text());
	painter.drawText(QPoint(xoffset+1, y + 1), text());

	pen.setColor(_textColor);

	painter.setPen(pen);
	painter.drawText(QPoint(xoffset, y), text());
}

qreal HelperKaraokeLabel::shadowDepth()
{
	qreal val = font().pixelSize() * shadowDepthScale;
	if (val < 1.0)
	{
		val = 1.0;
	}
	if (val > 4.0)
	{
		val = 4.0;
	}
	return val;
}
