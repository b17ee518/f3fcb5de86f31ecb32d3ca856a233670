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

QSizeF HelperKaraokeLabel::RenderToImage(QImage** ppimage, const QString& lyric, QList<RubyChar>& ruby, const QColor& textCol, const QColor& strokeColor)
{
	if (!ppimage)
	{
		return QSizeF();
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
	f.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 1);
	setFont(f);
	
	// calc size
	QSizeF size = minimumSizeHint();

	//////////////////////////////////////////////////////////////////////////
	// ruby
	int rubyCount = ruby.size();
	_maxSubW = 0.0;
	_maxSubH = setting->rubyFontSize();
	QList<HelperKaraokeLabel*> subList;
	for (int i = 0; i < rubyCount; i++)
	{
		HelperKaraokeLabel* sub = new HelperKaraokeLabel(this);
		sub->_isRuby = true;
		sub->setRubyHidden(_isRubyHidden);
		subList.append(sub);
		sub->setText(ruby[i].ruby());
		sub->_textColor = _textColor;
		sub->_strokeColor = _strokeColor;

		// set font
		QFont f;
		f.setFamily(setting->fontName());
		f.setWeight(99);
		f.setPixelSize(setting->rubyFontSize());
		f.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 1);
		sub->setFont(f);

		QSizeF subsize = sub->minimumSizeHint();
		if (_maxSubW < subsize.width())
		{
			_maxSubW = subsize.width();
		}
		// should be even!!
		/*
		if (_maxSubH < subsize.height())
		{
			_maxSubH = subsize.height();
		}*/
	}

	// rearrange size
	_maxSubH += 1.0; // shadow

	//  /--/                // _rubyOffset > 0
	//      /<--->/      // _maxSubW
	//  |--|a|----|b|----|c|--|
	//  AAAAAAAAAAAAAAAAA

	//
	// |a|b|c|
	//	---A---
	// /--/                // _rubyOffset < 0

	size.setHeight(size.height() + _maxSubH + setting->rubyVSpace());
	if (rubyCount > 0)
	{
		qreal subTotalW = _maxSubW*rubyCount;
		qreal oWidth = size.width();

		if (oWidth < subTotalW)
		{
			size.setWidth(_maxSubW*rubyCount);
			_rubyOffset = (oWidth - subTotalW) / 2.0;
		}
		else
		{
			auto oMaxSubW = _maxSubW;
			/*
			_maxSubW = ((qreal)(oWidth)) / rubyCount;
			_rubyOffset = oMaxSubW/2;
			*/
			_rubyOffset = (oWidth - rubyCount*oMaxSubW) / rubyCount / 2.0;
			_maxSubW = oMaxSubW + _rubyOffset * 2.0;
		}

		for (int i = 0; i < rubyCount; i++)
		{
			subList[i]->setFixedSize(QSize(_maxSubW, _maxSubH+setting->rubyVSpace()));
			subList[i]->_maxSubW = _maxSubW;
			subList[i]->_maxSubH = _maxSubH;
			auto geo = subList[i]->geometry();
			geo.setX(geo.x() + i*_maxSubW + (_rubyOffset>0 ? _rubyOffset : 0));
			subList[i]->setGeometry(geo);
		}
		/*
		auto geo = this->geometry();
		geo.setY(maxSubH + subSpaceH);
		this->setGeometry(geo);
		*/
	}
	
	setFixedSize(size.toSize());


	// render to image
	pimage = new QImage(size.toSize(), QImage::Format_ARGB32);
	QPainter painter(pimage);
	pimage->fill(qRgba(0, 0, 0, 0));
	render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
	

	// assign return value
	*ppimage = pimage;
	
	return size;
}

void HelperKaraokeLabel::setRubyHidden(bool bHidden)
{
	_isRubyHidden = bHidden;
}

void HelperKaraokeLabel::paintEvent(QPaintEvent *e)
{
//	QLabel::paintEvent(e);

	if (_isRuby && _isRubyHidden)
	{
		return;
	}

	QSizeF size = this->rect().size();
	auto setting = Settings::getInstance();
	qreal y = size.height()*0.75 + (_isRuby ? /*(-setting->rubyVSpace())*/0 : (-setting->verticalSpace)) -1;

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
