#include <QDebug>
#include "QStrokes.h"
#include <Eigen/Dense>
using namespace Eigen;

QString QStrokes::CONNECT_POINTS="@";
QString QStrokes::LISTEN_VERTEX_NAME="#";
QString QStrokes::SET_VERTEX="SET_VERTEX";
QString QStrokes::ADD_VERTEX="ADD_VERTEX";
QString QStrokes::INITIAL_STATE="INITIAL_STATE";
QString QStrokes::CHANGE_VERTEX="CHANGE_VERTEX";

QStrokes::QStrokes()
{
	this->state=INITIAL_STATE;
}
QVertex* QStrokes::edge(int i, int j)
{
	return new QVertex[2]{QVertex(i), QVertex(j)};
}
int& QStrokes::operator[](int index)
{
	return this->path[index];
}
bool QStrokes::setVertexText(QString vertexText)
{
	bool hasVertex=graph.setVertexText(vertexIndex, vertexText, changeVertexText);
	if(changeVertexText)return hasVertex;
	int index=-1; QPoint point=graph.getPoint(vertexText, index);
	if(index==vertexIndex)index=-1;
	#define get(x) graph.getPoint(x)
	if(hasVertex&&index!=-1)
	{
		int size=connectPoints.size();
		this->connectPoints.insert(size/2, point);
		this->connectIndices.insert(size/2, index);
		this->connectPoints<<get(vertexIndex);
		this->connectIndices<<vertexIndex;
		if(connectPoints.size()==4)
		{
			this->connectTwoStrokes();
			this->connectPoints.clear();
			this->connectIndices.clear();
		}
	}
	#undef get(x)
	return hasVertex;
}
void QStrokes::operator=(QPoint point)
{
	this->path<<MOVE<<point.x()<<point.y();
	this->startPath(point, vec2(0, 0));
	if(vertexText!="")
	{
		this->setVertexText(vertexText);
		this->vertexText="";
		this->vertexIndex=-1;
	}
}
void QStrokes::operator+=(QPoint point)
{
	this->curve+=point;
	if(curve.length()<curve.minLength)this->setPath(LINE);
	else if(curve.isLinear())this->setPath(LINE);
	else if(curve.isCircular())this->setPath(CUBIC);
	else if(curve.isCubic())this->setPath(CUBIC);
	else {this->curve--; this->startPath(point);}
}
void QStrokes::operator*=(QPoint point)
{
	this->vertex=graph.getVertexIndex(point);
	this->changeVertexText=vertex>=0;
}
QStrokes& QStrokes::operator<<(int path)
{
	this->path<<path;
	return *this;
}
void QStrokes::startPath(QPoint point)
{
	if(curve.isLinear())this->setPath(LINE);
	else if(curve.isCubic())this->setPath(CUBIC);
	this->startPath(point, curve.direction());
}
void QStrokes::drawStatus(QPainter& painter)
{
	if(changeVertexText)
	{
		QPoint p=graph.getVertexPoint(vertex); int r=20;
		painter.drawRect(p.x()-r/2, p.y()-r/2, r, r);
		QString s=graph.getVertexText(vertex); 
		painter.drawText(30, 30, "Rename Vertex: "+s);
	}
	else if(vertexText!="")painter.drawText(30, 30, "Draw Vertex: "+vertexText);
}
void QStrokes::drawConnectPoints(QPainter& painter)
{
	if(is(this->CONNECT_POINTS)){painter.drawText(30, 30, "Connect Points"); return;}
	if(connectPoints.size())painter.drawText(30, 30, "Connect Two Strokes...");
	int r=20; for(QPoint p : connectPoints)painter.drawEllipse(p.x()-r/2, p.y()-r/2, r, r);
}
QMatrix getTransformation(QPoint srcPoint0, QPoint srcPoint1, QPoint destPoint0, QPoint destPoint1)
{
	#define q0 srcPoint0
	#define q1 srcPoint1
	#define p0 destPoint0
	#define p1 destPoint1
	MatrixXd A(4, 4); VectorXd b(4);
	VectorXd X0(4); X0<<p0.x(), p0.y(), 1, 0; A.row(0)=X0; b(0)=q0.x();
	VectorXd Y0(4); Y0<<p0.y(), -p0.x(), 0, 1; A.row(1)=Y0; b(1)=q0.y();
	VectorXd X1(4); X1<<p1.x(), p1.y(), 1, 0; A.row(2)=X1; b(2)=q1.x();
	VectorXd Y1(4); Y1<<p1.y(), -p1.x(), 0, 1; A.row(3)=Y1; b(3)=q1.y();
	VectorXd h=A.jacobiSvd(ComputeThinU|ComputeThinV).solve(b);
	return QMatrix(h(0), -h(1), h(1), h(0), h(2), h(3));
	#undef q0
	#undef q1
	#undef p0
	#undef p1
}
void QStrokes::mergeCloseEndPointsIndices(int& beginIndex, int& endIndex)
{
	for(int b=beginIndex, i=prev(b); i>0&&graph.isClose(b, i); i=prev(i))beginIndex=i;
	for(int e=endIndex, i=next(e); i<path.size()&&graph.isClose(e, i); i=next(i))endIndex=i;
}
void QStrokes::connectTwoStrokes()
{
	#define min(x, y) (x<y?x:y)
	#define max(x, y) (x>y?x:y)
	#define p(i) connectPoints[i]
	#define d(i) connectIndices[i]
	QMatrix transform=getTransformation(p(0), p(1), p(2), p(3));
	int beginIndex=min(d(2), d(3)), endIndex=max(d(2), d(3));
	mergeCloseEndPointsIndices(beginIndex, endIndex);
	for(int i=beginIndex; i<=endIndex; i=next(i))
	{
		QVector<QPoint> points=graph.getPoints(i);
		for(QPoint& p : points)p=transform.map(p);
		graph.setPoints(i, points);
	}
	#undef min(x, y)
	#undef max(x, y)
	#undef p(i)
	#undef d(i)
}
QStrokes& QStrokes::operator>>(QString vertexText)
{
	if(vertexText==CONNECT_POINTS)
	{
		this->state=CONNECT_POINTS;
		this->vertexText="";
		return *this;
	}
	if(vertexText==ADD_VERTEX)
	{
		this->vertexIndex=next(begin); 
		return *this;
	}
	if(vertexText==CHANGE_VERTEX)
	{
		this->vertexIndex=vertex; 
		return *this;
	}
	if(vertexText==SET_VERTEX)
	{
		this->vertexIndex=begin; 
		this->startPath(curve.last()); 
		return *this;
	}
	int index=-1; QPoint point=graph.getPoint(vertexText, index);
	if(is(CONNECT_POINTS)&&index!=-1)
	{
		(*this)+=point; this->state=INITIAL_STATE;
	}
	else if(vertexIndex!=-1)
	{
		if(!setVertexText(vertexText))
		this->vertexText=vertexText; 
		else 
		{	this->vertexIndex=-1;
			this->changeVertexText=false; 
		}
	}
	return *this;
}
void QStrokes::undo()
{
	this->removeLast();
	this->index=prev(size());
	this->graph--;
	this->begin=prev(begin);
	this->end=prev(end);
	while(index>2&&path[index]==MOVE)
	{
		this->removeLast();
		this->index=prev(size());
		this->begin=prev(begin);
		this->end=prev(end);
	}
}
void QStrokes::createGraph()
{
	for(int i=2, n=next(i); n<size(); i=n, n=next(i))
	{
		if(path[n]==MOVE)continue;
		this->graph+=edge(i, n);
	}
	this->begin=size();
}
void QStrokes::updateGraph()
{
	this->end=index;
	this->graph+=edge(begin, end);
	this->begin=end;
}
int QStrokes::prev(int index)
{
	for(int i=index-1; i>=2; i--)
	{
		if(path[i]==MOVE||path[i]==LINE||path[i]==CUBIC)return i;
	}
	return 2;
}
int QStrokes::next(int index)
{
	if(index==0)return 2;
	if(path[index]==MOVE)return index+3;
	if(path[index]==LINE)return index+3;
	if(path[index]==CUBIC)return index+7;
}
void QStrokes::clear()
{
	this->index=0;
	this->begin=0;
	this->path.clear();
	this->graph.clear();
	this->state=INITIAL_STATE;
	this->connectPoints.clear();
	this->connectIndices.clear();
}
void QStrokes::update()
{
	this->graph.updateEdge();
}
int QStrokes::size()
{
	return this->path.size();
}
void QStrokes::startPath(QPoint point, vec2 direction)
{
	this->index=size();
	this->curve.clear();
	this->curve+=point;
	this->path<<LINE<<point.x()<<point.y();
	if(direction.length()==0)this->begin=next(begin);
	this->updateGraph();
}
void QStrokes::setPath(int type)
{
	this->removeLast();
	vec2* points=curve.ctrlPoints;
	if(type==LINE)
	{
		this->path<<LINE<<points[3].x()<<points[3].y();
	}
	else if(type==CUBIC)
	{
		this->path<<CUBIC;
		this->path<<points[1].x()<<points[1].y();
		this->path<<points[2].x()<<points[2].y();
		this->path<<points[3].x()<<points[3].y();
	}
}
void QStrokes::removeLast()
{
	if(index<=2)return;
	if(path[index]==MOVE)this->removeLast(3);
	else if(path[index]==LINE)this->removeLast(3);
	else if(path[index]==CUBIC)this->removeLast(7);
}
void QStrokes::removeLast(int length)
{
	for(int i=0; i<length; i++)this->path.removeLast();
}
bool QStrokes::is(QString state)
{
	return this->state==state;
}
