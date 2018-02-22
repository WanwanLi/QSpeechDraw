#include <QPen>
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QPaintEvent>
#include <QBasicTimer>
#include "QSpeech.h"
#include "QSketch.h"

class QCanvas : public QWidget
{
	Q_OBJECT
	public:
	QCanvas(QWidget* widget=0);
	bool loadImage(QString fileName);
	bool saveImage(QString fileName, const char* fileFormat);

	public slots:
	void undo();
	void clear();
	bool isModified();
	void resizeImage();
	void listenEvent(QListenEvent* event);

	private:
	QPoint point;
	QColor bgColor;
	QImage* image;
	QSketch sketch;
	QWidget* widget;
	void clearImage();
	QPen pen, marker;
	QThread* listener;
	QSpeech* speech;
	void startListener();
	bool isMousePressed;
	QString vertexText="";
	bool isImageModified;
	void updateVertexText();
	QBasicTimer basicTimer;
	void drawLine(QPoint point);
	void resizeWindow(int margin);
	int keyTimer=0, keyInterval=3;
	void paintEvent(QPaintEvent* event);
	void timerEvent(QTimerEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
};
