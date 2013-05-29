#include <math.h>
#include <windows.h>

#pragma once
#define M_PI 3.1415926535897932384626433832795 
#define CIRCLEREALITY 80//円がどれだけ滑らかになるかどうか
#define MAXTEXT 255


#define EPSIRON 0.0//辺に点が乗っているかどうかの判定に使う。数値が大きいと判定が甘くなる。
enum POINTSTATE{OUTSIDE,INSIDE,INIEDGE,NEXTEDGE,PREVEDGE};//点と三角形の関係。

enum RESULTSTATUS{NON,TWO,SINGLE};

typedef struct tagline{
  double coefa;//傾き
	double coefb;//ｋ
	double coefc;
}FORMULA,*PFORMULA;
typedef struct tagtwovert{//方程式の解
	double plusx;
	double plusy;
	double minusx;
	double minusy;

	RESULTSTATUS status;

}TWOVERT,*PTWOVERT;
typedef struct tagstatus{//２次方程式の解に使う（今回は不使用）
	int plus;
	int minus;
	TWOVERT points;

}STATUS,*PSTATUS;

