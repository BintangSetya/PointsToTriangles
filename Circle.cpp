#include <math.h>
#include <stdio.h>
#include <gl/glut.h>
#include "Circle.h"
#pragma once

CCircle::CCircle(void)
{
  radius=0;
}
CCircle::CCircle(CVertex &_center,double _radius){
	this->center=new CVertex(_center.x,_center.y);
	this->radius=_radius;
}
void CCircle::SetCircle(CVertex *_center,double _radius){
	this->center=_center;
	this->radius=_radius;
}
bool CCircle::PointInCircle(CVertex *point){
	//point‚Æ‰~‚Ì’†S‚Ì‹——£‚ª”¼ŒaˆÈ‰º‚¾‚Á‚½‚ç“à‘¤‚Å‚ ‚éB
	if(this->center->CalcLength(point)>=this->radius){
		return false;
	}else{
		return true;
	}
}
void CCircle::DrawCircle(){
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINE_LOOP);
	
	 // ‰~‚ð•`‰æ
  for (int i = 0; i < CIRCLEREALITY; i++) {
    // À•W‚ðŒvŽZ
	  double x,y;
   double  rate = (double)i / CIRCLEREALITY;
   x =this->center->x+ radius * cos(2.0 * M_PI * rate);
    y =this->center->y+ radius * sin(2.0 * M_PI * rate);
    glVertex2d(x, y); // ’¸“_À•W‚ðŽw’è
  }
  glEnd();
}
void CCircle::DrawCircle(float _r,float _g,float _b){
	glColor3f(_r,_g,_b);
	glBegin(GL_LINE_LOOP);

	 // ‰~‚ð•`‰æ
  for (int i = 0; i < CIRCLEREALITY; i++) {
    // À•W‚ðŒvŽZ
	  double x,y;
   double  rate = (double)i / CIRCLEREALITY;
   x =this->center->x+ radius * cos(2.0 * M_PI * rate);
    y =this->center->y+ radius * sin(2.0 * M_PI * rate);
    glVertex2d(x, y); // ’¸“_À•W‚ðŽw’è
  }
  glEnd();
}

CCircle::~CCircle(void)
{
}

CVertex CCircle::GetDownPoint(){
	CVertex result;
	 result.x =this->center->x+ radius * cos(M_PI *0.25);
    result.y =this->center->y+ radius * sin( M_PI * 0.25);
	return result;
}
void CCircle::printCircleInfo(){
	this->center->PrintVertex("center");
	printf("rad=%.2f\n",this->radius);
}
