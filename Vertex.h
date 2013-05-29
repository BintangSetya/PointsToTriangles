#pragma once
#include "globaldef.h"
#include "Line.h"

class CVertex;
class CLine;
class CVertex
{
public:
  double x;
	double y;
	double z;//色として使う
	double truex;
	double truey;
	double truez;
	int ptst;
	//この点を"始点"として持つ任意の片辺へのポインタを持つ。終点だったら登録する必要ない。
	CLine* m_incidentEdge[10];//何個あればいいかな。。。



	CVertex& operator=(const CVertex& _in);
	CVertex operator-(const CVertex& _b);
	
	CVertex operator+(const CVertex& _b);
	CVertex operator/(const CVertex& _in);
	CVertex operator*(const CVertex& _in);
	CVertex operator/(const double _scalar);
	CVertex operator*(const double _scalar);
	CVertex(void);
	CVertex(double _x,double _y);
	void CalcVector(CVertex* _from,CVertex* _to);
	CVertex CalcMidPoint(CVertex *_anotherPoint);
	CVertex(const CVertex *source);
	CVertex(double _x,double _y,int _id);
	~CVertex(void);
	bool CalcLineFormula(CVertex *_src,PFORMULA _dest);
	bool IsSameVertex(CVertex* anotherPoint);
	bool operator==(const CVertex &a)const;
	bool operator<(const CVertex &a)const;
	bool operator!=(const CVertex &a)const;
	
	FORMULA left;
	FORMULA right;
	int myId;//三角形分割で使うよう。つまり、空間的な位置をまんべんなく散らしたインデックス
	int trueId;//本当のインデックス。.txtに並んでいる順番 ツリー探索を効率よくするためにランダムに順番を混ぜる前のインデックス
	double CalcInnerProduct(CVertex *_in);
	double GetTheta(CVertex *_in);
	double GetCosTheta(CVertex *_in);
	double GetThisLength();
	void SetVertex(double _x,double _y);
	double CalcLength(CVertex* anotherPoint);
	void Copy(CVertex *src);
	void PrintVertex(const char* message);
	void PrintVertex(const char* message,CVertex* _scale,CVertex* _offset);
	bool LexicographicOrdering(CVertex* _anotherpoint);
	double CrossProduct(CVertex &_in);
	int incidentEdgeNum;

	
	
};
