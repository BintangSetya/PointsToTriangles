#include <stdio.h>
//#include <freeglut.h>
#include "Line.h"
#pragma once
CLine::CLine(CLine* _in){
  this->MakeLine(_in->begin,_in->end);
	this->haveTwin=_in->haveTwin;
	this->twin=_in->twin;
}
CLine::CLine(CLine* _in,int _id){
	this->MakeLine(_in->begin,_in->end);
	this->haveTwin=_in->haveTwin;
	this->twin=_in->twin;
	this->myId=_id;
}

CLine::CLine(void)
{

	ZeroMemory(&m_formula,sizeof(FORMULA));
	length=0;
	haveTwin=false;
}
CLine::CLine(CVertex *_begin,CVertex *_end)
{
	ZeroMemory(&m_formula,sizeof(FORMULA));
	this->begin=_begin;
	this->end=_end;
	length=this->begin->CalcLength(_end);
	haveTwin=false;
}
CLine::CLine(CVertex *_begin,CVertex *_end,int _i)
{
	ZeroMemory(&m_formula,sizeof(FORMULA));
	this->begin=_begin;
	this->end=_end;
	length=this->begin->CalcLength(_end);
	haveTwin=false;
	this->myId=_i;
}
bool CLine::SetStartPoint(CVertex *p){
	this->begin=p;
	return true;
}
bool CLine::MakeLine(CVertex *p,CVertex *q){
	
	begin=p;
	end=q;
	length=this->begin->CalcLength(end);

	return true;
}
CLine& CLine::operator=(const CLine& _in){
	this->begin=_in.begin;
	this->end=_in.end;
	this->haveTwin=_in.haveTwin;
	this->twin=_in.twin;
	return *this;

}
CLine::~CLine(void)
{
}
double CLine::GetX(double _y){
	return (_y-this->m_formula.coefb)/this->m_formula.coefa;

}
double CLine::GetY(double _x){
	return this->m_formula.coefa*_x+this->m_formula.coefb+this->m_formula.coefc;
}

//void CLine::DrawLine(float r,float g,float b){
//	glColor3f(r,g,b);
//	
//	
//	glBegin(GL_LINES);
//	begin->glVertex();
//	end->glVertex();
//	
//	glEnd();
//	
//}

void CLine::PrintLine(){
	//printf("y=%.2fx + %.2f\n",this->m_formula.coefa,this->m_formula.coefb);
	//this->begin->PrintVertex("from");
	//this->end->PrintVertex("to");
	//printf("normal");
	this->begin->PrintVertex("from");
	this->end->PrintVertex("to");
	printf("havetwin?%d\n",this->haveTwin);
	//stringstream ss;
	//ss<<"normal["<<this->myId<<"]("<<this->begin-<<","<<this->end->myId<<")";
//	printf("%s\n",ss.str().c_str());
}
void CLine::PrintLine(const char* message){
	stringstream ss;
	ss<<message;
	//ss<<"["<<this->myId<<"]"<<this->haveTwin;
	ss<<"["<<this->myId<<"]("<<this->begin->trueId<<","<<this->end->trueId<<")twin?"<<this->haveTwin;
//	ss<<"next("<<this->next->begin->myId<<"to"<<next->end->myId<<")prev("<<this->prev->begin->myId<<"to"<<prev->end->myId<<")";
	printf("%s\n",ss.str().c_str());
	//this->begin->PrintVertex("begin");
	//this->end->PrintVertex("end");

}
void CLine::GetMyLineInfoString(stringstream &ss){

	
	//ss<<"["<<this->myId<<"]"<<this->haveTwin;
	ss<<"["<<this->myId<<"]("<<this->begin->myId<<","<<this->end->myId<<")twin?"<<this->haveTwin;
	ss<<"next("<<this->next->begin->myId<<"to"<<next->end->myId<<")prev("<<this->prev->begin->myId<<"to"<<prev->end->myId<<")";
	if(this->haveTwin){ss<<"twin next end id["<<this->twin->next->end->myId;}
}
//void CLine::DrawLineText(float r,float g,float b,void *font){
//this->begin->DrawVertexText(font);
//	//this->end->DrawVertexText(r,g,b,font);
//}
//void CLine::DrawLineText(int _winwidth,int _winheight){
//	double posx=0;
//	double posy=0;
//	posx=_winwidth-400;
//	posy=32+this->myId*30;
//	glColor3f(1.0,1.0,1.0);
//	glRasterPos2d(posx,posy);
//	stringstream message;
//	message<<"lineBuf["<<this->myId<<"]:"<<this->begin->trueId<<"to"<<this->end->trueId;
//	glutBitmapString(GLUT_BITMAP_HELVETICA_10,reinterpret_cast<const unsigned char*>(message.str().c_str()));
//}
//void CLine::DrawMyId(){
//	CVertex midpoint((this->begin->x+this->end->x)/2,(this->begin->y+this->end->y)/2);
//	glColor3f(1.0,1.0,0.0);
//	glRasterPos2d(midpoint.x,midpoint.y);
//	stringstream message;
//	message<<"["<<this->myId<<"]";
//	glutBitmapString(GLUT_BITMAP_HELVETICA_12,reinterpret_cast<const unsigned char*>(message.str().c_str()));
//}
double CLine::CalcDecline(){
	double decline=(end->y-begin->y)/(end->x-begin->x);
		return decline;
}
double CLine::CalcDistance(CVertex* _pt){
	CalcNormal();
	CVertex vector;
	//vector.CalcVector(this->begin,this->end);
	vector.CalcVector(_pt,this->begin);
	CVertex nvec;
	//normal->PrintLine();
	nvec.CalcVector(normal->begin,normal->end);
	
	return fabs(nvec.CalcInnerProduct(&vector))/normal->CalcLength();



}
double CLine::CalcLength(){
	double x=this->end->x-this->begin->x;
	double y=this->end->y-this->begin->y;
	return sqrt(x*x+y*y);

}
//void CLine::DrawMyNormal(){
//	this->CalcNormal();
//	this->normal->DrawLine(1.0,0.0,0.0);
//}
CLine* CLine::CalcNormal(){
	normal = new CLine();
	normal->begin=this->begin;
	double decline=CalcDecline();
	decline=(-1/decline);//直交する傾き。
	normal->end=new CVertex(this->begin->x+100,this->begin->y+100*decline);
	//長さを１にするぞ
	

	
	return normal;

}
double CLine::InnerProduct(CLine* _in){
	CVertex vector;
	vector.CalcVector(this->begin,this->end);
	CVertex invec;
	invec.CalcVector(_in->begin,_in->end);
	return vector.CalcInnerProduct(&invec);
}
double CLine::GetCos(CLine* _in){
	CVertex vector;
	vector.CalcVector(this->begin,this->end);
	CVertex invec;
	invec.CalcVector(_in->begin,_in->end);
	double innerproduct=vector.CalcInnerProduct(&invec);
	//長さで割る
	innerproduct=innerproduct/(this->CalcLength()*_in->CalcLength());
		return innerproduct;
}
double CLine::CrossProductPrint(CLine* _anotherline){
	//マイナスインデックスを含む場合は注意が必要。
	this->begin->PrintVertex("thisbegin");
	this->end->PrintVertex("thisend");
	_anotherline->begin->PrintVertex("anotehrbegin");
	_anotherline->end->PrintVertex("anotherend");
	
	CVertex vector;
	vector.CalcVector(this->begin,this->end);
	vector.PrintVertex("このベクトルと");

	CVertex invec;
	
	invec.CalcVector(_anotherline->begin,_anotherline->end);
	invec.PrintVertex("このベクトルの");
	double ret=vector.CrossProduct(invec);
	printf("外せきを計算%.16f\n",ret);
	return ret;
	//if(ret>0){
	//	//凸
	//}else{
	//	//凹み
	//}
}
double CLine::CrossProduct(CLine* _anotherline){
	//マイナスインデックスを含む場合は注意が必要。
	CVertex vector;
	vector.CalcVector(this->begin,this->end);
	CVertex invec;
	invec.CalcVector(_anotherline->begin,_anotherline->end);
	double ret=vector.CrossProduct(invec);
	return ret;
	//if(ret>0){
	//	//凸
	//}else{
	//	//凹み
	//}
}
CLine CLine::GetInvertLine(){//begin end逆さの辺をゲット
CLine ret;
ret.begin=this->end;
ret.end=this->begin;
return ret;
}
