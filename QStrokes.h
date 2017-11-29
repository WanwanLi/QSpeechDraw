#include <QPoint>
#include "QCurve.h"
#include "QGraph.h"

class QStrokes
{
	public:
	int size();
	QStrokes();
	QString state;
	void update();
	void createGraph();
	void undo(), clear();
	veci connectIndices;
	QString vertexText="";
	int& operator[](int index);
	void operator=(QPoint point);
	void operator+=(QPoint point);
	QGraph graph=QGraph(path);
	QVector<QPoint> connectPoints;
	QStrokes& operator<<(int path);
	QString SET_VERTEX="SET_VERTEX";
	QString ADD_VERTEX="ADD_VERTEX";
	QString CONNECT_TWO_STROKES="@";
	QString INITIAL_STATE="INITIAL_STATE";
	QStrokes& operator>>(QString vertexText);
	void drawConnectPoints(QPainter& painter);

	private:
	veci path;
	QCurve curve;
	void removeLast();
	void updateGraph();
	void setPath(int type);
	QVertex* edge(int i, int j);
	void connectTwoStrokes();
	enum{MOVE, LINE, CUBIC};
	void removeLast(int length);
	void startPath(QPoint point);
	int next(int index), prev(int index);
	int begin=0, end, index, vertexIndex;
	void startPath(QPoint point, vec2 direction);
};
