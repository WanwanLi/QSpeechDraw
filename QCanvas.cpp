#include <QPainter>
#include "QCanvas.h"

QCanvas::QCanvas(QWidget* widget) : QWidget(widget)
{
	this->widget=widget;
	this->resize(widget->size());
	this->sketch.resize(size());
	this->pen=QPen(Qt::green, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	this->marker=QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	this->image=new QImage(size(), QImage::Format_RGB32);
	this->bgColor=qRgb(255, 255, 255);
	this->basicTimer.start(255, this);
	this->isImageModified=false;
	this->clearImage();
	this->setFocus();
}
bool QCanvas::loadImage(QString fileName)
{
	if(image->load(fileName))
	{
		this->isImageModified=false;
		this->resize(image->size());
		this->resizeWindow(20);
		return true;
	}
	else if(sketch.load(fileName))
	{
		this->isImageModified=false;
		this->resize(sketch.size);
		this->image=new QImage(size(), QImage::Format_RGB32);
		this->clearImage();
		this->resizeWindow(20);
		QPainter painter(image);
		painter.setPen(pen);
		painter.drawPath(sketch);
		this->update();
		return true;
	}
	else return false;
}
void QCanvas::keyPressEvent(QKeyEvent* event)
{
	if(!keyTimer)
	{
		if(isMousePressed)this->sketch.strokes>>sketch.strokes.NEW_VERTEX;
		this->keyTimer=keyInterval;
	}
	this->vertexText+=event->text();
}
bool QCanvas::saveImage(QString fileName, const char* fileFormat)
{
	if(tr(fileFormat)=="sky")return this->sketch.save(fileName);
	if(!image->save(fileName, fileFormat))return false;
	this->isImageModified=false;
        	return true;
}
void QCanvas::resizeWindow(int margin)
{
	this->widget->resize(QSize(image->width(), image->height()+margin));
}
void QCanvas::resizeImage()
{
	QImage* image=new QImage(size(), QImage::Format_RGB32);
	QPainter painter(image);
	painter.drawImage(QPoint(0, 0), this->image->scaled(size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
	this->isImageModified=true;
	this->sketch.resize(size());
	this->image=image;
	this->update();
}
void QCanvas::clear()
{
	this->clearImage();
	this->sketch.clear();
}
void QCanvas::undo()
{
	this->sketch.strokes.undo();
}
void QCanvas::clearImage()
{
	this->image->fill(bgColor);
	this->update();
}
void QCanvas::paintEvent(QPaintEvent* event)
{
	QRect rect=event->rect();
	QPainter painter(this);
	painter.drawImage(rect,*image, rect);
}
void QCanvas::updateVertexText()
{
	if(keyTimer)
	{
		this->keyTimer--;
		if(!keyTimer)
		{
			sketch.strokes>>vertexText;
			this->vertexText="";
		}
	}
}
void QCanvas::timerEvent(QTimerEvent* event)
{
	this->updateVertexText();
	this->sketch.update();
	this->image->fill(bgColor);
	QPainter painter(image);
	painter.setPen(pen);
	painter.drawPath(sketch);
	painter.setPen(marker);
	sketch.strokes.drawConnectPoints(painter);
	sketch.strokes.graph.drawVertices(painter);
	if(vertexText!="")painter.drawText(50, 50, "Vertex: "+vertexText);
	this->update();
}
void QCanvas::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton) 
	{
		this->point=event->pos();
		this->sketch.strokes=event->pos();
		this->isMousePressed=true;
	}
}
void QCanvas::mouseMoveEvent(QMouseEvent* event)
{
	if(isMousePressed)
	{
		this->drawLine(event->pos());
		this->sketch.strokes+=event->pos();
	}
}
void QCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	if(isMousePressed)
	{
		this->drawLine(event->pos());
		this->sketch.strokes.update();
        		this->isMousePressed=false;
	}
}
void QCanvas::drawLine(QPoint point)
{
	QPainter painter(image);
	painter.setPen(pen);
	painter.drawLine(this->point, point);
	this->isImageModified=true;
	this->point=point;
	this->update();
}
bool QCanvas::isModified()
{
	return this->isImageModified;
}
