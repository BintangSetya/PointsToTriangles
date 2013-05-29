#include "../Vertex.h"
#pragma once
class CVertexDebug :public CVertex
{
public:
  CVertexDebug(void);
	CVertexDebug(CVertex* _src,CVertex* _scale,CVertex* _offset);
	CVertexDebug(double _x,double _y);
	~CVertexDebug(void);
	//double x;
	//double y;
	//double z;
	
	
	void Set(CVertex* _src,CVertex* _scale,CVertex* _offset);
	CVertexDebug& operator=(const CVertex& _in);
	void glVertex();
	void DrawVertexText(void *font);
};

