#include <gl/freeglut.h>
#include "VertexDebug.h"


CVertexDebug::CVertexDebug(void)
{
  this->x=0;
	this->y=0;
	this->z=0;
}
CVertexDebug::CVertexDebug(CVertex* _src,CVertex* _scale,CVertex* _offset){
	x=_src->x*_scale->x+_offset->x;
	y=_src->y*_scale->y+_offset->y;
	z=_src->z;
}
void CVertexDebug::Set(CVertex* _src,CVertex* _scale,CVertex* _offset){
	x=_src->x*_scale->x+_offset->x;
	y=_src->y*_scale->y+_offset->y;
	z=_src->z;
}
CVertexDebug::CVertexDebug(double _x,double _y){
	x=_x;
	y=_y;
}
CVertexDebug& CVertexDebug::operator=(const CVertex& _in){
	this->x=_in.x;
	this->y=_in.y;
	this->z=_in.z;
	return *this;

}
CVertexDebug::~CVertexDebug(void)
{
}
void CVertexDebug::glVertex(){
	//z 近いほど赤　遠いほど青
	//double r=0;double g=0;double b=0;

	//r=z;
	//b=1.0-z;
	//glColor3f(r,g,b);
	glVertex2d(this->x,this->y);

}
void CVertexDebug::DrawVertexText(void *font){
	if(this->trueId>=0){
	if(this->x<1.0 && this->x>-1.0 && this->y<1.0 && this->y>-1.0){
	glRasterPos3d(this->x,this->y,0);//オブジェクト座標そのままでいい！！
	}else{	//表示ウィンドウからはみ出たせないための処理
		double newx,newy;
		newx=this->x;
		newy=this->y;
		if(this->x>=1.0){newx=0.95;}
		if(this->y>=1.0){newy=0.95;}
		glRasterPos3d(newx,newy,0);
	}
	
	stringstream ss;
	ss.setf(ios::floatfield,8);
	ss.str();
	
	ss<<"true["<<this->trueId<<"]["<<this->myId<<"]";//("<<this->x<<","<<this->y<<")";//,"<<this->z<<")";
	glutBitmapString(font,reinterpret_cast<const unsigned char*>(ss.str().c_str()));
	
	}
}



