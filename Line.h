#pragma once
#include "globaldef.h"
#include "Vertex.h"
#include <sstream>
using namespace std;
class CLine;
class CVertex;
class CTriangle;
//これが辺レコードということにしておくか。
class CLine
{
public:
  FORMULA m_formula;
	//媒介変数directrixでbeginとendを表そうか
	double bt,et;//begin tとend t(tは媒介変数)使ってない
	CVertex* begin;//始点へのポインタ
	CVertex* end;//終点へのポインタ(蓄えておく必要はない。)
	CLine* twin;//双子辺へのポインタ
	
	CTriangle* incidentTriangle;//自身が境界となってる面（三角形）へのポインタ
	CLine* next;
	CLine* prev;
	CLine* normal;//法線（マウスをクリックしたら一番近い線をハイライトするのに使う）
	int myId;//lineBuffer[myId]　インデックス
	
	
	double length;
	double theta;//傾き
	bool haveTwin;
	CLine(void);
	CLine(CVertex *_begin,CVertex *_end);
	CLine(CLine* _in,int _id);
	CLine(CVertex *_begin,CVertex *_end,int _i);
	CLine(CLine* _in);
	CLine& operator=(const CLine& _in);
	
	bool MakeLine(CVertex *p,CVertex *q);
	//void DrawLine(float r,float g,float b);
	~CLine(void);
	double GetX(double _y);
	double GetY(double _x);
	double CalcDecline();
	double CalcDistance(CVertex* _pt);
	double InnerProduct(CLine* _in);
	CLine* CalcNormal();
	double CalcLength();
	void PrintLine();
	void PrintLine(const char* message);
	bool SetStartPoint(CVertex *p);
	double CrossProductPrint(CLine* _anotherline);
	void DrawLineText(float r,float g,float b,void *font);
	void DrawLineText(int _winwidth=0,int _winheight=0);
	//void DrawMyNormal();
	void GetMyLineInfoString(stringstream &ss);
	void DrawMyId();
	double CrossProduct(CLine* _anotherline);
	CLine GetInvertLine();//begin end逆さの辺をゲット
	double GetCos(CLine* _in);


};
