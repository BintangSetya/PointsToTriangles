#include "globaldef.h"
#include "Vertex.h"
#pragma once
class CCircle;
class CCircle
{
public:
  double radius;//”¼Œa
	CVertex* center;
	
	CCircle(void);
	CCircle(CVertex &_center,double _radius);
	void SetCircle(CVertex *_center,double _radius);
	~CCircle(void);
	void DrawCircle();
	void DrawCircle(float _r,float _g,float _b);
	bool PointInCircle(CVertex *point);
	CVertex GetDownPoint();
	void printCircleInfo();
};
