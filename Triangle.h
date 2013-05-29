#include "globaldef.h"
#include "Line.h"
#include "Circle.h"
#include <sstream>
#pragma once
using namespace std;
enum NAMEOFPOINTS{A,B,C};

class CTriangle;
class CCircle;

class CTriangle
{
public:
  
	CLine* initialedge;//外側境界上のある片辺へのポインタ。

	CCircle *circumCircle;
	CTriangle *children[3];//何個必要なのか不明瞭だ。実験したところ、本のいうとおり最大３だった。
	double matrix[9];//点の内外判定に使う
	//点の内外判定には、頂点が都合が良いので頂点の情報を取っておく。辺のnext関係は好きに変わっても良い
	CVertex* a;
	CVertex* b;
	CVertex* c;
	


	int myId;//このIDはfullTriangleBuffer[]のインデックス
	int childnum;
	
	//外接円の計算にしよう
	double angleA;
	double angleB;
	double angleC;
	
	double area;//面積
	double ha,hb,hc;//高さ
	double sharpness;

	FORMULA f_ab;
	FORMULA f_bc;
	FORMULA f_ca;
	
	NAMEOFPOINTS longestEdge;
	
	

	bool operator!=(const CTriangle& _in)const;
	bool operator==(const CTriangle& _in)const;
	CTriangle& operator=(const CTriangle& _in);
	
	bool IsDegenerateTriangle(bool _assertion);
	bool IsInMyCircumCircle(CVertex *_p);
	bool IsInMyCircumCircle2(CVertex *_p);
	bool MakeTriangle(CVertex *_a,CVertex *_b,CVertex *_c);
	void MakeTriangle(CLine *_initialedge);
	void MakeTriangle(CTriangle *_t);
	bool IsMyVertex(CVertex *testPoint);
	bool IsSameTriangle(CTriangle *_testTriangle);
	bool IsIntersect(CVertex *_a,CVertex *_b);
	bool SolveLinerFormula(PFORMULA _a,PFORMULA _b,CVertex *result);
	bool IsOnEdge(CVertex *_testpoint,NAMEOFPOINTS _whichedge);
	bool PtInTriangle(CVertex *_pt);
	bool PtOnEdge(CVertex* _pt,int* _onedge);
	bool PtOnEdge2(CVertex* _pt,int* _onedge);
	POINTSTATE PtInTriangle2(CVertex *_pt,CVertex* _scale,CVertex* _offset,bool _debug);
	POINTSTATE PtInTriangleSpecial(CVertex *_pt,bool _debug);
	POINTSTATE PtInTriangle2(CVertex *_pt,bool _debug);

	void AllocateEdge();
	void AllocateVertex(CVertex *_a,CVertex *_b,CVertex *_c);
	CTriangle(CVertex *_a,CVertex *_b,CVertex *_c);
	CTriangle(CLine *_edgea,CLine *_edgeb,CLine *_edgec);
	CTriangle(CLine* _initialedge);
	CTriangle(CLine* _initialedge,int _id);
	
	CTriangle(void);
	~CTriangle(void);
	void Set3Vertex();
	void CopyTriangle(CTriangle* _src);	
	double GetSharpness();
	void MakeTriangle(CLine *_edgea,CLine *_edgeb,CLine *_edgec);
	
	CCircle CalcCircumCircle();//外接円の計算
	void SetBelongsTo();//接続面の登録
	void GetMyVertexString(int _i,stringstream& _in);
	void PrintMyVertexInfo(const char* message);
	void PrintMyEdgeInfo(const char* message);
	void PrintMyVertexPosition(const char* message);
	POINTSTATE PtInTriangle3(CVertex *_pt);
	int side( CVertex *p, CLine *e );
	
	};
