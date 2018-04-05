#include "QDefine.h"
#include <QPainter>

class QVertex
{
	public:
	//QVertex(int i):text(num(i)), index(i){}
	QVertex(int i):index(i){}
	QVertex(){} QString text=""; int index=0;
};
class QGraph
{
	public:
	bool isClose(int i, int j);
	QGraph(veci& p):path(p){}
	QPoint getPoint(int index);
	void operator+=(QVertex* edge);
	int getVertexIndex(QPoint point);
	QPoint getPoint(QString vertexText);
	void setPoint(int index, QPoint point);
	void drawVertices(QPainter& painter);
	QVector<QPoint> getPoints(int index);
	QPoint getVertexPoint(int vertexIndex);
	QString getVertexText(int vertexIndex);
	void operator--(int), updateEdge(), clear();
	void setPoints(int index, QVector<QPoint> points);
	QPoint getPoint(QString vertexText, int& vertexIndex);
	bool setVertexText(int vertexIndex, QString vertexText, bool isVertex);

	private:
	veci& path;
	QVector<int*> edges;
	void updateVertices();
	qreal minDistance=30.0;
	enum{MOVE, LINE, CUBIC};
	QVector<QVertex> vertices;
	int add(QVertex vertex), append(QVertex vertex);
	void addVertex(QVector<QVertex> vertices, int index);
};
