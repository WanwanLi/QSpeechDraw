#include <QPainterPath>
#include "QStrokes.h"

class QSketch : public QPainterPath
{
	public:
	bool load(QString fileName);
	bool save(QString fileName);
	void resize(QSize size);
	QStrokes strokes;
	void update();
	void clear();
	QSize size;

	private:
	enum{MOVE, LINE, CUBIC};
};
