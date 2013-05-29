#include "../Triangle.h"
#include "VertexDebug.h"
class CVertexDebug;

#pragma once
class CTriangleDebug :public CTriangle
{
public:
  CTriangleDebug(void);
	CTriangleDebug(CTriangle* _t,CVertex* _scale,CVertex* _offset);
	
	~CTriangleDebug(void);
	CVertexDebug* da;
	CVertexDebug* db;
	CVertexDebug* dc;
	
	CCircle* drawcircle;
	CVertexDebug* nodePos;//デバッグ用。ツリー上の位置
	
	void Set(CTriangle* _t,CVertex* _scale,CVertex* _offset);
	
	void DrawTriangle();
	void DrawTriangleText(CVertex* _area);
	void DrawCircumCircle();
	void DrawMyId();
	void FillTriangle(float _r,float _g,float _b,float _a);
	int side( CVertex *p, CLine *e );
	CCircle  CalcCircumCircle();
	
};

