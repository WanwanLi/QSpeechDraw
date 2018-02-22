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
	bool is(QString state);
	QString vertexText="";
	int& operator[](int index);
	static QString SET_VERTEX;
	static QString ADD_VERTEX;
	static QString INITIAL_STATE;
	QGraph graph=QGraph(path);
	void operator=(QPoint point);
	void operator+=(QPoint point);
	void operator*=(QPoint point);
	QVector<QPoint> connectPoints;
	QStrokes& operator<<(int path);
	static QString CONNECT_POINTS;
	void drawStatus(QPainter& painter);
	static QString LISTEN_VERTEX_NAME;
	QStrokes& operator>>(QString vertexText);
	void drawConnectPoints(QPainter& painter);

	private:
	veci path;
	int vertex=-1;
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
	bool setVertexText(QString vertexText);
	void startPath(QPoint point, vec2 direction);
	void mergeCloseEndPointsIndices(int& beginIndex, int& endIndex);
};
