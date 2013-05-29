#include "TriangleDebug.h"
#include <gl/freeglut.h>

CTriangleDebug::CTriangleDebug(void)
{
  this->a = new CVertex();
	this->b = new CVertex();
	this->c = new CVertex();
	this->da = new CVertexDebug();
	this->db = new CVertexDebug();
	this->dc = new CVertexDebug();
	this->childnum=0;
	
	nodePos=0;
}
CTriangleDebug::CTriangleDebug(CTriangle* _t,CVertex* _scale,CVertex* _offset){
	
	this->a = _t->a;
	this->b = _t->b;
	this->c = _t->c;
	this->childnum=_t->childnum;
	
	this->myId=_t->myId;
	this->da = new CVertexDebug(_t->a,_scale,_offset);
	this->db = new CVertexDebug(_t->b,_scale,_offset);
	this->dc = new CVertexDebug(_t->c,_scale,_offset);
	this->da->trueId=a->trueId;
	this->db->trueId=b->trueId;
	this->dc->trueId=c->trueId;
	nodePos=0;
	
}


void CTriangleDebug::Set(CTriangle* _t,CVertex* _scale,CVertex* _offset){
	this->a = _t->a;
	this->b = _t->b;
	this->c = _t->c;
	this->childnum=_t->childnum;
	/*for(int i=0;i<childnum;i++){
		if(dchildren[i]==0){
			this->dchildren[i]= new CTriangleDebug(_t->children[i],_scale,_offset);
		}else{
			this->dchildren[i]->Set(_t->children[i],_scale,_offset);
		}
	}*///Žq‚Ì“o˜^‚Í‚ ‚Æ‚Å
	//•`‰æ—p’¸“_‚Ì“o˜^
	CVertex zero(0,0);
	CVertex one(1.0,1.0);
	
	this->da->Set(_t->a,_scale,_offset);
	this->db->Set(_t->b,_scale,_offset);
	this->dc->Set(_t->c,_scale,_offset);
	this->myId=_t->myId;
	this->da->trueId=a->trueId;
	this->db->trueId=b->trueId;
	this->dc->trueId=c->trueId;
	nodePos=0;
	
}
CTriangleDebug::~CTriangleDebug(void)
{
}
void CTriangleDebug::DrawTriangle(){
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINE_LOOP);
	if(da->trueId>=0){this->da->glVertex();}
	if(db->trueId>=0){this->db->glVertex();}
	if(dc->trueId>=0){this->dc->glVertex();}
	/*this->da->PrintVertex("da");
	this->db->PrintVertex("db");
	this->dc->PrintVertex("dc");*/
	glEnd();
}

void CTriangleDebug::DrawTriangleText(CVertex* _area){
	double posx=0;
	double posy=0;
	posx=0.8;
	posy=0.8-this->myId*0.05;
	glColor3f(1.0,1.0,1.0);
	glRasterPos2d(-_area->x*0.9,_area->y*0.9);
	stringstream message;
	//message<<"triufBuf["<<this->myId<<"]:"<<a->trueId<<","<<b->trueId<<","<<c->trueId;
//	message<<"triufBuf["<<this->myId<<"]:"<<this->initialedge->myId<<","<<this->initialedge->next->myId<<","<<this->initialedge->prev->myId;
	glutBitmapString(GLUT_BITMAP_HELVETICA_10,reinterpret_cast<const unsigned char*>(message.str().c_str()));

}

void CTriangleDebug::FillTriangle(float _r,float _g,float _b,float _a){
	
	glEnable(GL_BLEND);
glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glColor4f(_r,_g,_b,_a);
	glBegin(GL_TRIANGLES);
	this->da->glVertex();
	this->db->glVertex();
	this->dc->glVertex();
	glEnd();
	glDisable(GL_BLEND);
}
void CTriangleDebug::DrawMyId(){//ŽOŠpŒ`‚ÌdS•”•ª‚ÉŽ©•ª‚Ìid‚ð•`‚­
	CVertex barycenter((this->initialedge->begin->x+this->initialedge->end->x+this->initialedge->prev->begin->x)/3.0,(this->initialedge->begin->y+this->initialedge->end->y+this->initialedge->prev->begin->y)/3.0);
	glColor3f(0.0,1.0,1.0);
	glRasterPos2d(barycenter.x,barycenter.y);
	stringstream message;
	//message<<this->myBirthYear;
	glutBitmapString(GLUT_BITMAP_HELVETICA_18,reinterpret_cast<const unsigned char*>(message.str().c_str()));
}
void CTriangleDebug::DrawCircumCircle(){
	if(circumCircle==0){this->CalcCircumCircle();}
	this->drawcircle->DrawCircle();
}
int CTriangleDebug::side( CVertex *p, CLine *e )
{
	CVertex p1 = *p;
	CVertex p2 = *(e->begin); // —LŒüü•ª e ‚ÌŽn“_
	CVertex p3 = *(e->end); // —LŒüü•ª e ‚ÌI“_

	// —LŒüü•ª (p2,p1), (p2,p3) ‚ÌŠOÏ‚Ì z ¬•ª‚ð‹‚ß‚é
	const int n  = p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y);
	if      ( n > 0 ) return  1; // ¶
	else if ( n < 0 ) return -1; // ‰E
	else              return  0; // üã
}//side

CCircle  CTriangleDebug::CalcCircumCircle()//ŠOÚ‰~‚ÌŒvŽZ
{
	//ŠOS‚ÌŒvŽZ
	//area

	double ab=this->da->CalcLength(db);
	double bc=this->db->CalcLength(dc);
	double ca=this->dc->CalcLength(da);
	double s=(ab+bc+ca)/2.0;
	area=sqrt(s*(s-ab)*(s-bc)*(s-ca));
	double maxlength=0;
	if(maxlength<ab){maxlength=ab;longestEdge=C;}
	if(maxlength<bc){maxlength=bc;longestEdge=A;}
	if(maxlength<ca){maxlength=ca;longestEdge=B;}
	
	switch(longestEdge){
		case A:
			ha=2*area/bc;
			angleB=asin(ha/ab);
			angleC=asin(ha/ca);
			angleA=M_PI-angleB-angleC;
			break;
		case B:
			hb=2*area/ca;
			angleA=asin(hb/ab);
			angleC=asin(hb/bc);
			angleB=M_PI-angleC-angleA;
			break;
		case C:
			hc=2*area/ab;
			angleB=asin(hc/bc);
			angleA=asin(hc/ca);
			angleC=M_PI-angleA-angleB;
			break;
	}
	double A = sin(2.0*angleA);
	double B = sin(2.0*angleB);
	double C = sin(2.0*angleC);
	CVertex v( (this->da->x * A + this->db->x * B + this->dc->x * C) / (A+B+C),
		(this->da->y * A + this->db->y * B + this->dc->y * C) / (A+B+C));
	//”¼Œa‚ÌŒvŽZ	
	double rad=bc / sin(angleA) / 2.0 ;
	this->drawcircle=new CCircle(v,rad);
	return *drawcircle;


}
