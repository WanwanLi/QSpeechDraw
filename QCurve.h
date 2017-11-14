#include <QPoint>
#include "QDefine.h"

class QCurve
{
	public:
	int size();
	void clear();
	QPoint last();
	qreal length();
	bool isCubic();
	bool isLinear();
	vec2 direction();
	vec2 ctrlTangent;
	vec2 ctrlPoints[4];
	qreal fittingError;
	int minLength=10;
	void operator--(int);
	qreal dirError=1.0;
	qreal minError=2.0;
	qreal maxError=4.0;
	int maxIterations=800;
	vec2& operator[](int index);
	QVector<vec2> curvePoints;
	void operator+=(QPoint& point);

	private:
	vec2 bezierPoints[4];
	vec2 tangent(int i, int j);
	vec2 pointAt(qreal param);
	qreal* getChordLengthParameters();
	qreal getFittingError(qreal* params);
	void optimizeParameters(qreal* params);
	bool isCloseTo(qreal src, qreal dest, qreal error);
	qreal optimize(vec2& curvePoint, qreal param);
	qreal B0(qreal u), B1(qreal u), B2(qreal u), B3(qreal u);
	vec2 pointAt(vec2 ctrlPoints[], qreal param, int degree);
	void getCtrlPoints(qreal* params, vec2& leftTangent, vec2& rightTangent);
};
