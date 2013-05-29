#include <windows.h>

#include <stdio.h>
#include <math.h>
#include <sstream>


#include "Vertex.h"
#pragma once
using namespace std;
void CVertex::CalcVector(CVertex* _from,CVertex* _to){
  this->x=_to->x-_from->x;
	this->y=_to->y-_from->y;
}

double CVertex::CrossProduct(CVertex &_in){
	
	return this->x*_in.y-this->y*_in.x;
	
}
bool CVertex::operator!=(const CVertex &a)const{
	if(this->x!=a.x){return true;}else 
		if(this->y!=a.y){return true;}else{return false;}
}
CVertex CVertex::operator-(const CVertex& _b){//２項演算子。今までおかしかった！！
	return CVertex(this->x-_b.x,this->y-_b.y,this->z-_b.z);
}
CVertex CVertex::operator+(const CVertex& _b){
	return CVertex(this->x+_b.x,this->y+_b.y,this->z+_b.z);
}

CVertex CVertex::operator/(const CVertex& _in){
	CVertex ret;
	ret.x=this->x/_in.x;
	ret.y=this->y/_in.y;
	ret.z=this->z/_in.z;
	return ret;
}
CVertex CVertex::operator*(const CVertex& _in){
	CVertex ret;
	ret.x=this->x*_in.x;
	ret.y=this->y*_in.y;
	ret.z=this->z*_in.z;
	return ret;
}
CVertex CVertex::operator/(const double _scalar){
	CVertex ret;
	ret.x=this->x/_scalar;
	ret.y=this->y/_scalar;
	ret.z=this->z/_scalar;
	return ret;
}


CVertex CVertex::operator*(const double _scalar){
	CVertex ret;
	ret.x=this->x*_scalar;
	ret.y=this->y*_scalar;
	ret.z=this->z*_scalar;
	return ret;
}
CVertex& CVertex::operator=(const CVertex& _in){
	this->x=_in.x;
	this->y=_in.y;
	this->myId=_in.myId;
	this->trueId=_in.trueId;
	return *this;

}
CVertex::CVertex(void)
{
	x=0;
	y=0;
	ZeroMemory(&left,sizeof(FORMULA));
	ZeroMemory(&right,sizeof(FORMULA));
	incidentEdgeNum=0;
	this->myId=0;
	this->trueId=0;

}
// コピーコンストラクタは、メンバー変数のコピーなので必要ないが一応作っておく
    // [const]を忘れると、エラーになる
CVertex::CVertex(const CVertex *source){
        x  = source->x;
        y = source->y;
		ZeroMemory(&left,sizeof(FORMULA));
		ZeroMemory(&right,sizeof(FORMULA));
		incidentEdgeNum=0;
		this->myId=source->myId;
		this->trueId=source->trueId;

}

    // 比較演算子(findで必要)
    // [const]を忘れるとエラー
bool CVertex::operator==(const CVertex &a)const{
        return this->trueId==a.trueId;//this->x == a.x && y ==a.y ;
    }

    // 比較演算子(sortで必要)
    // [const]を忘れるとエラー
bool CVertex::operator<(const CVertex &a)const{
        return x < a.x;
    }
CVertex::CVertex(double _x,double _y){
	x=_x;
	y=_y;
	incidentEdgeNum=0;

}
CVertex::CVertex(double _x,double _y,int _id){
	x=_x;
	y=_y;
	incidentEdgeNum=0;
	this->myId=_id;

}
void CVertex::SetVertex(double _x,double _y){
	x=_x;
	y=_y;

}
void CVertex::Copy(CVertex *src){
	x=src->x;
	y=src->y;
	this->myId=src->myId;
	this->trueId=src->trueId;
}
double CVertex::GetThisLength(){
	return sqrt(this->x*this->x+this->y*this->y);
}
double CVertex::GetTheta(CVertex *_in){
	return acos(this->GetCosTheta(_in));
}
double CVertex::GetCosTheta(CVertex *_in){
	return this->CalcInnerProduct(_in)/(this->GetThisLength()*_in->GetThisLength());
}
double  CVertex::CalcInnerProduct(CVertex *_in){
	return this->x*_in->x+this->y*_in->y;
}
double CVertex::CalcLength(CVertex* anotherPoint){
	double length=0;
	//length=sqrt(pow(fabs(this->x-anotherPoint->x),2)+pow(fabs(this->y-anotherPoint->y),2));
	double x2=(this->x-anotherPoint->x)*(this->x-anotherPoint->x);
	double y2=(this->y-anotherPoint->y)*(this->y-anotherPoint->y);
	length=sqrt(x2+y2);
	return length;
}
bool CVertex::LexicographicOrdering(CVertex* _anotherpoint){
	//printf("y=%.8f,another=%.8f\n",y,_anotherpoint->y);
	if(y>=_anotherpoint->y){//もしyが同じ値の場合はxを比較する
				if(y==_anotherpoint->y){
					if(x>_anotherpoint->x){
						return true;//i am higher than anotherpoint
					}else{
						return false;
					}
				}else{//p>q
					
					return true;		
				}
	}else{
		return false;
	}
}

bool CVertex::IsSameVertex(CVertex* anotherPoint)
{
	if(anotherPoint->x == this->x && anotherPoint->y == this->y){
		return true;
		
	}else{
		return false;
	}
}
void CVertex::PrintVertex(const char* message){

	printf("%s:[%d]trueId[%d](%.16f,%.16f)\n",message,myId,trueId,x,y);
}
void CVertex::PrintVertex(const char* message,CVertex* _scale,CVertex* _offset){
printf("%s:[%d](%.8f,%.8f)\n",message,this->trueId,this->x,this->y);
	printf("%s:[%d](%.8f,%.8f)\n",message,this->trueId,this->x*_scale->x+_offset->x,this->y*_scale->y+_offset->y);
}
CVertex::~CVertex(void)
{
}



bool CVertex::CalcLineFormula(CVertex *_src,PFORMULA _dest){
	if(this->IsSameVertex(_src)){return false;}
	if(_src->x == this->x){
		_dest->coefa=_src->x;
	}else{
	_dest->coefa=(this->y-_src->y)/(this->x-_src->x);//傾き
	}
	_dest->coefb= -1.0*_dest->coefa;
	_dest->coefc= _src->y;
	
	return true;
	
}

CVertex CVertex::CalcMidPoint(CVertex *_anotherPoint){
	CVertex ret;
	ret.x=(this->x+_anotherPoint->x)*0.5;
	ret.y=(this->y+_anotherPoint->y)*0.5;
	return ret;

}
