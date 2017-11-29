#include "QCurve.h"
#define dot vec2::dotProduct
#define cross vec3::crossProduct

vec2& QCurve::operator[](int index)
{
	return this->curvePoints[index];
}
void QCurve::operator+=(QPoint& point)
{
	this->curvePoints<<vec2(point);
	this->ctrlPoints[3]=curvePoints.last();
}
void QCurve::operator--(int)
{
	this->curvePoints.removeLast();
}
QPoint QCurve::last()
{
	return toQPoint(curvePoints.last());
}
bool QCurve::isLinear()
{
	vec2 begin=curvePoints.first();
	vec2 end=curvePoints.last(); qreal error=0;
	vec2 direction=(end-begin).normalized();
	for(vec2 point : curvePoints)
	{
		error+=point.distanceToLine(begin, direction);
	}
	return error/length()<minError;
}
bool QCurve::isCircular()
{
	#define p curvePoints
	int n=size(); qreal xx, yy, xy, xz, yz; 
	vec2 s=vec2(0, 0);
	for(int i=0; i<n; s+=p[i++]);
	s/=n; xx=yy=xy=xz=yz=0;
	for(int i=0; i<n; i++)
	{
		qreal x=p[i].x()-s.x();
		qreal y=p[i].y()-s.y();
		qreal z=x*x+y*y;
		xx+=x*x; yy+=y*y;
		xy+=x*y; xz+=x*z; yz+=y*z;
	}
	xx/=n; yy/=n; xy/=n; xz/=n; yz/=n;
	qreal G11=sqrt(xx), G12=xy/G11;
	qreal G22=sqrt(yy-G12*G12);
	qreal D1=xz/G11, D2=(yz-D1*G12)/G22;
	qreal b=D2/G22/2, a=(D1-G12*b)/G11/2;
	vec2 center=vec2(a+s.x(), b+s.y());
	qreal radius=sqrt(a*a+b*b+xx+yy);
	qreal error=distanceToCircle(center, radius);
	if(error<minError)
	{
		if(this->ctrlTangents[0].length()==0)
		this->ctrlTangents[0]=tangent(center, ctrlPoints[0]);
		this->ctrlTangents[1]=tangent(center, ctrlPoints[3]);
		if(this->isCubic())return true;
	}
	this->ctrlTangents[0]=vec2(0, 0);
	this->ctrlTangents[1]=vec2(0, 0);
	return false;
}
vec2 QCurve::tangent(const vec2& center, const vec2& point)
{
	vec3 normal=vec3(0, 0, 1);
	vec3 direction=vec3(point-center);
	return cross(direction.normalized(), normal).toVector2D();
}
qreal QCurve::distanceToCircle(const vec2& center, qreal radius)
{
	#define p curvePoints
	qreal distance=0; int n=size();
	for(int i=0; i<n; i++)
	{
		qreal dx=p[i].x()-center.x();
		qreal dy=p[i].y()-center.y();
		qreal dr=sqrt(dx*dx+dy*dy);
		distance+=dr-radius;
	}
	return distance/length();
}
bool QCurve::isCubic()
{
	vec2 leftTangent=tangent(0, 1); 
	qreal tangentError=dot(leftTangent, ctrlTangents[0]);
	if(ctrlTangents[0].length()==0)tangentError=-1.0;
	if(isCloseTo(tangentError, 1.0, dirError))leftTangent=ctrlTangents[0];
	vec2 rightTangent=ctrlTangents[1];
	if(rightTangent.length()==0)rightTangent=tangent(size()-1, size()-2);
	qreal* params=getChordLengthParameters();
	getCtrlPoints(params, leftTangent, rightTangent);
	this->fittingError=getFittingError(params);
	if(fittingError<minError)return true;
	if(fittingError<maxError)
	{
		for(int i=0; i<maxIterations; i++)
		{
	    		optimizeParameters(params);
			getCtrlPoints(params, leftTangent, rightTangent);
			this->fittingError=getFittingError(params);
	    		if(fittingError<minError)return true;
		}
	}
	return false;
}
void QCurve::clear()
{
	this->curvePoints.clear();
}
int QCurve::size()
{
	return curvePoints.size();
}
qreal QCurve::length()
{
	qreal curveLength=0;
	for(int i=0; i<size()-1; i++)
	{
		curveLength+=curvePoints[i].distanceToPoint(curvePoints[i+1]);
	}
	return curveLength;
}
qreal* QCurve::getChordLengthParameters()
{
	#define p curvePoints
	qreal* params=new qreal[size()];
	qreal distance=0; params[0]=distance;
	for(int i=1; i<size(); i++)
	{
		distance=p[i-1].distanceToPoint(p[i]);
		params[i]=params[i-1]+distance;
	}
	for(int i=1; i<size(); params[i++]/=params[size()-1]);
	return params;
}
void QCurve::optimizeParameters(qreal* params)
{
	for(int i=0; i<size(); i++)
	{
		params[i]=optimize(curvePoints[i], params[i]);
	}
}
qreal QCurve::optimize(vec2& curvePoint, qreal param)
{
	#define u param
	#define C ctrlPoints
	#define P curvePoint
	vec2 Q, Q1, Q2, C1[3], C2[2];
	for(int i=0; i<=2; C1[i]=3*(C[i+1]-C[i]), i++);
	for(int i=0; i<=1; C2[i]=2*(C1[i+1]-C1[i]), i++);
	Q=pointAt(u); Q1=pointAt(C1, u, 2); Q2=pointAt(C2, u, 1);
	return u-dot(Q-P, Q1)/(dot(Q1, Q1)+dot(Q-P, Q2));
}
vec2 QCurve::tangent(int i, int j)
{
	return (curvePoints[j]-curvePoints[i]).normalized();
}
void QCurve::getCtrlPoints(qreal* params, vec2& leftTangent, vec2& rightTangent)
{
	#define u params
	#define d curvePoints
	vec2 (*A)[2]=new vec2[size()][2];
	vec2 V0, V1, V2, V3, V00, V01, V32, V33;
	V0=d[0]; V3=d[size()-1]; qreal C[2][2], X[2]; 
	C[0][0]=C[0][1]=C[1][0]=C[1][1]=X[0]=X[1]=0;
	for(int i=0; i<size(); i++)
	{
		A[i][0]=leftTangent*B1(u[i]);
		A[i][1]=rightTangent*B2(u[i]);
		C[0][0]+=dot(A[i][0], A[i][0]);
		C[0][1]+=dot(A[i][0], A[i][1]);
		C[1][0]+=dot(A[i][1], A[i][0]);
		C[1][1]+=dot(A[i][1], A[i][1]);
		V00=V0*B0(u[i]); V01=V0*B1(u[i]); 
		V32=V3*B2(u[i]); V33=V3*B3(u[i]); 
		X[0]+=dot(A[i][0], d[i]-(V00+V01+V32+V33));
		X[1]+=dot(A[i][1], d[i]-(V00+V01+V32+V33));
	}
	qreal detC01=C[0][0]*C[1][1]-C[1][0]*C[0][1];
	qreal detC0X=C[0][0]*X[1]-C[1][0]*X[0];
	qreal detXC1=X[0]*C[1][1]-X[1]*C[0][1];
	qreal a1=(detC01==0)?0:detXC1/detC01;
	qreal a2=(detC01==0)?0:detC0X/detC01;
	V1=V0+leftTangent*a1; V2=V3+rightTangent*a2;  	
	this->ctrlPoints[0]=V0; this->ctrlPoints[1]=V1;
	this->ctrlPoints[2]=V2; this->ctrlPoints[3]=V3;
}
qreal QCurve::getFittingError(qreal* params)
{
	qreal error=0.0;
	#define u params
	#define d curvePoints
	for(int i=0; i<size(); i++)
	{
		error+=pointAt(u[i]).distanceToPoint(d[i]);
	}
	return error/length();
}
vec2 QCurve::pointAt(qreal param)
{
	return pointAt(ctrlPoints, param, 3);
}
vec2 QCurve::pointAt(vec2 ctrlPoints[], qreal param, int degree)
{
	#define t param
	#define V ctrlPoints
	#define B bezierPoints
	for(int i=0; i<=degree; B[i]=V[i], i++);
	for(int i=1; i<=degree; i++) 
	{	
		for(int j=0; j<=degree-i; j++)
		{
	    		B[j]=(1.0-t)*B[j]+t*B[j+1];
		}
	}
	return B[0];
}
vec2 QCurve::direction()
{
	return (ctrlPoints[3]-ctrlPoints[2]).normalized();
}
bool QCurve::isCloseTo(qreal src, qreal dest, qreal error)
{
	return qAbs(dest-src)<=error;
}
qreal QCurve::B0(qreal u)
{
	return (1.0-u)*(1.0-u)*(1.0-u);
}
qreal QCurve::B1(qreal u)
{
	return 3*u*(1.0-u)*(1.0-u);
}
qreal QCurve::B2(qreal u)
{
	return  3*u*u*(1.0-u);
}
qreal QCurve::B3(qreal u)
{
	return u*u*u;
}
