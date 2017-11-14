#include "QGraph.h"
#define toQPoint(p) QPoint(p.x(), p.y())

void QGraph::operator+=(QVertex* edge)
{
	int begin=add(edge[0]), end=append(edge[1]);
	this->edges<<new int[2]{begin, end};
}
void QGraph::operator--(int)
{
	if(edges.isEmpty())return;
	this->edges.removeLast();
	this->updateVertices();
}
void QGraph::setVertexText(int vertexIndex, QString vertexText)
{
	for(QVertex& vertex : vertices)if(vertex.index==vertexIndex)vertex.text=vertexText;
}
QPoint QGraph::getPoint(QString vertexText)
{
	for(int i=0; i<vertices.size(); i++)
	{
		if(vertices[i].text.toLower()==vertexText.toLower())
		{
			return getPoint(vertices[i].index);
		}
	}
	return QPoint(0, 0);
}
QPoint QGraph::getPoint(QString vertexText, int& vertexIndex)
{
	for(QVertex vertex : vertices)
	{
		if(vertex.text.toLower()==vertexText.toLower())
		{
			vertexIndex=vertex.index;
			return getPoint(vertexIndex);
		}
	}
	vertexIndex=-1;
	return QPoint(0, 0);
}
void QGraph::updateEdge()
{
	if(edges.isEmpty())return;
	int* edge=edges.last();
	for(int i=0; i<vertices.size(); i++)
	{
		if(i==edge[1])continue;
		if(isClose(vertices[i].index, vertices[edge[1]].index))
		{
			this->vertices.removeLast();
			edge[1]=i; return;
		}
	}
}
int QGraph::append(QVertex vertex)
{
	int index=vertices.size();
	this->vertices<<vertex;
	return index;
}
int indexOf(QVector<QVertex> vertices, QVertex vertex)
{
	for(int i=0; i<vertices.size(); i++)
	{
		if(vertex.index==vertices[i].index)return i;
	}
	return -1;
}
int QGraph::add(QVertex vertex)
{
	int index=indexOf(vertices, vertex);
	if(index!=-1)return index;
	for(int i=0; i<vertices.size(); i++)
	{
		if(isClose(vertices[i].index, vertex.index))return i;
	}
	return append(vertex);
}
void QGraph::updateVertices()
{
	QVector<QVertex> vertices;
	for(int* edge : edges)
	{
		this->addVertex(vertices, edge[0]);
		this->addVertex(vertices, edge[1]);
	}
	this->vertices.clear();
	this->vertices.append(vertices);
	vertices.clear();
}
void QGraph::addVertex(QVector<QVertex> vertices, int index)
{
	if(index==vertices.size())vertices<<this->vertices[index];
}
bool QGraph::isClose(int i, int j)
{
	vec2 x=vec2(getPoint(i)), y=vec2(getPoint(j));
	if(x.distanceToPoint(y)<minDistance)
	{
		this->setPoint(j, toQPoint(x)); return true;
	}
	else return false;
}
void QGraph::drawVertices(QPainter& painter)
{
	for(int i=0, r=10; i<vertices.size(); i++)
	{
		QPoint p=getPoint(vertices[i].index);
		painter.drawEllipse(p.x()-r/2, p.y()-r/2, r, r);
		painter.drawText(p.x()-r, p.y()-r, vertices[i].text);
	}
}
QPoint QGraph::getPoint(int index)
{
	if(path[index]==MOVE)return QPoint(path[index+1], path[index+2]);
	if(path[index]==LINE)return QPoint(path[index+1], path[index+2]);
	if(path[index]==CUBIC)return QPoint(path[index+5], path[index+6]);
	return QPoint(0, 0);
}
void QGraph::setPoint(int index, QPoint point)
{
	if(path[index]==MOVE){path[index+1]=point.x(); path[index+2]=point.y();}
	else if(path[index]==LINE){path[index+1]=point.x(); path[index+2]=point.y();}
	else if(path[index]==CUBIC){path[index+5]=point.x(); path[index+6]=point.y();}
}
void QGraph::clear()
{
	this->vertices.clear();
	this->edges.clear();
}
