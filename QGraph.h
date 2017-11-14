#include "QDefine.h"
#include <QPainter>

class QVertex
{
	public:
	QVertex(int i):text(num(i)), index(i){}
	QVertex(){} QString text=""; int index=0; 
	QVertex(QString s, int i):text(s), index(i){}
};
class QGraph
{
	public:
	QGraph(veci& p):path(p){}
	QPoint getPoint(int index);
	void operator+=(QVertex* edge);
	QPoint getPoint(QString vertexText);
	void setPoint(int index, QPoint point);
	void drawVertices(QPainter& painter);
	void operator--(int), updateEdge(), clear();
	QPoint getPoint(QString vertexText, int& vertexIndex);
	void setVertexText(int vertexIndex, QString vertexText);

	private:
	veci& path;
	QVector<int*> edges;
	void updateVertices();
	bool isClose(int i, int j);
	qreal minDistance=30.0;
	enum{MOVE, LINE, CUBIC};
	QVector<QVertex> vertices;
	int add(QVertex vertex), append(QVertex vertex);
	void addVertex(QVector<QVertex> vertices, int index);
};