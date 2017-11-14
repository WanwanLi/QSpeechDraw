#include <QFile>
#include <QDebug>
#include <QTextStream>
#include "QSketch.h"

bool QSketch::load(QString fileName)
{
	QFile file(fileName); QStringList line;
	if(!file.open(QIODevice::ReadOnly))return false;	
	QTextStream textStream(&file); this->clear();
	bool isNotValidSketch=true; 	this->strokes.clear(); 
	while(!textStream.atEnd())
	{
		line=textStream.readLine().split(" ");
		if(line[0]=="s"&&line.size()>=3)
		{
			int w=line[1].toInt();
			int h=line[2].toInt();
			this->strokes<<w<<h;
			size=QSize(w, h);
			isNotValidSketch=false;
		}
		if(isNotValidSketch)continue;
		if(line[0]=="m")
		{
			int x=line[1].toInt();
			int y=line[2].toInt();
			this->strokes<<MOVE;
			this->strokes<<x<<y;
		}
		else if(line[0]=="l")
		{
			int x=line[1].toInt();
			int y=line[2].toInt();
			this->strokes<<LINE;
			this->strokes<<x<<y;
		}
		else if(line[0]=="c")
		{
			int c1=line[1].toInt();
			int c2=line[2].toInt();
			int c3=line[3].toInt();
			int c4=line[4].toInt();
			int c5=line[5].toInt();
			int c6=line[6].toInt();
			this->strokes<<CUBIC;
			this->strokes<<c1<<c2<<c3;
			this->strokes<<c4<<c5<<c6;
		}
	}
	if(!isNotValidSketch)this->strokes.createGraph();
	return isNotValidSketch?false:true;
}
bool QSketch::save(QString fileName)
{
	QFile file(fileName); QString endl="\r\n";
	if(!file.open(QIODevice::WriteOnly))return false;
	QTextStream textStream(&file); 
	textStream<<"s "<<strokes[0]<<" ";
	textStream<<strokes[1]<<endl;
	for(int i=2; i<strokes.size(); i++)
	{
		if(strokes[i]==MOVE)
		{
			textStream<<"m "<<strokes[++i]<<" ";
			textStream<<strokes[++i]<<endl;
		}
		else if(strokes[i]==LINE)
		{
			textStream<<"l "<<strokes[++i]<<" ";
			textStream<<strokes[++i]<<endl;
		}
		else if(strokes[i]==CUBIC)
		{
			textStream<<"c "<<strokes[++i]<<" ";
			textStream<<strokes[++i]<<" ";
			textStream<<strokes[++i]<<" ";
			textStream<<strokes[++i]<<" ";
			textStream<<strokes[++i]<<" ";
			textStream<<strokes[++i]<<endl;
		}
	}
	return true;
}
void QSketch::update()
{
	this->swap(QPainterPath());
	for(int i=2; i<strokes.size(); i++)
	{
		if(strokes[i]==MOVE)
		{
			int x=strokes[++i];
			int y=strokes[++i];
			this->moveTo(x, y);
		}
		else if(strokes[i]==LINE)
		{
			int x=strokes[++i];
			int y=strokes[++i];
			this->lineTo(x, y);
		}
		else if(strokes[i]==CUBIC)
		{
			int c1=strokes[++i], c2=strokes[++i];
			int c3=strokes[++i], c4=strokes[++i];
			int c5=strokes[++i], c6=strokes[++i];
			this->cubicTo(c1, c2, c3, c4, c5, c6);
		}
	}
}
void QSketch::resize(QSize size)
{
	this->size=size;
	if(strokes.size()==0)
	{
		this->strokes<<size.width();
		this->strokes<<size.height();
	}
	else
	{
		this->strokes[0]=size.width();
		this->strokes[1]=size.height();
	}
}
void QSketch::clear()
{
	this->strokes.clear(); 
	this->strokes<<size.width();
	this->strokes<<size.height();
	this->swap(QPainterPath());
}
