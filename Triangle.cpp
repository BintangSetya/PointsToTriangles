#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "Triangle.h"
#include <float.h>
#include <assert.h>
#define EPSILON    2.2204460492503131e-011//14//016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#pragma once
CTriangle::CTriangle(void)
{
  //this->initialedge= new CLine();
	childnum=0;
	this->circumCircle=0;
}
CTriangle::CTriangle(CVertex *_a,CVertex *_b,CVertex *_c){//デバッグ用
	this->a=_a;
	this->b=_b;
	this->c=_c;
	this->circumCircle=0;
}
CTriangle::CTriangle(CLine* _initialedge){//外接円を計算するときに使う・
	this->circumCircle=0;
	this->initialedge=_initialedge;
	Set3Vertex();
	SetBelongsTo();
	IsDegenerateTriangle(true);
	childnum=0;
}
bool CTriangle::IsDegenerateTriangle(bool _assertion){//縮退三角形は事前にチェックしてるからもういらない
	if((a->trueId>=0 &&b->trueId>=0 &&c->trueId>=0 )/*not minus */&&((a->x==b->x &&b->x==c->x )||(a->y==b->y &&b->y==c->y ))){
		a->PrintVertex("a");
		b->PrintVertex("b");
		c->PrintVertex("c");
		this->PrintMyVertexInfo("縮退三角形現る");
		if(_assertion){
		//	assert(!"縮退した三角形が作られました。！！");
		}
		return true;
	}else{
		return false;
	}
}
CTriangle::CTriangle(CLine* _initialedge,int _id){
	this->circumCircle=0;
	this->initialedge=_initialedge;
	this->myId=_id;
	Set3Vertex();//initialedgeから３頂点を登録
	
	SetBelongsTo();//incidentTriangleは自分であると知らせる
	IsDegenerateTriangle(true);
	childnum=0;
}

void CTriangle::Set3Vertex(){
	this->a=this->initialedge->begin;
	this->b=this->initialedge->end;
	this->c=this->initialedge->prev->begin;
	
	

}
void CTriangle::MakeTriangle(CLine *_initialedge){
	this->circumCircle=0;
	this->initialedge=_initialedge;
	Set3Vertex();
	SetBelongsTo();
	childnum=0;
}
void CTriangle::MakeTriangle(CTriangle *_t){
	if(_t->circumCircle!=0){this->circumCircle=_t->circumCircle;}else{this->circumCircle=0;}
	this->initialedge=_t->initialedge;
	this->myId=_t->myId;
	Set3Vertex();
	
	SetBelongsTo();
	childnum=_t->childnum;
	for(int i=0;i<childnum;i++){this->children[i]=_t->children[i];}
	
	
}
void CTriangle::CopyTriangle(CTriangle* _src){
	_src->initialedge=_src->initialedge;
	this->myId=_src->myId;
	//SetBelongsTo();

	childnum=_src->childnum;
	for(int i=0;i<childnum;i++){this->children[i]=_src->children[i];}
}
//　２次元平面上の点が有向線分の進行方向に対して左右どちら側にあるかを調べる関数を作る。
//*********************************************************
// 点 p が有向線分 e(a,b) の左右どちら側にあるか調べる。
// 点 p が有向線分 e の　左側にある場合  1 を、
// 　   　有向線分 e の直線上にある場合  0 を、
// 　   　有向線分 e の　右側にある場合 -1 を返す。
//*********************************************************
int CTriangle::side( CVertex *p, CLine *e )
{
	CVertex p1 = *p;
	CVertex p2 = *(e->begin); // 有向線分 e の始点
	CVertex p3 = *(e->end); // 有向線分 e の終点

	// 有向線分 (p2,p1), (p2,p3) の外積の z 成分を求める
	const int n  = p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y);
	if      ( n > 0 ) return  1; // 左
	else if ( n < 0 ) return -1; // 右
	else              return  0; // 線上
}//side
POINTSTATE CTriangle::PtInTriangle3(CVertex *_pt){//これが一番のオーバーヘッドこの手法は微妙。正しく出来ない。
	//http://katsura-kotonoha.sakura.ne.jp/prog/c/tip00030.shtml
	CLine ab, bc, ca;
	ab.begin = this->a; ab.end = this->b;
	bc.begin = this->b; bc.end = this->c;
	ca.begin = this->c; ca.end = this->a;
	const int pab = side( _pt, &ab ); // 有向線分 ab から見た点 p の方向
	const int pbc = side( _pt, &bc ); // 有向線分 bc から見た点 p の方向
	const int pca = side( _pt, &ca ); // 有向線分 ca から見た点 p の方向

	// 三角形の内側にあれば正
	//if ( (0 < pab) && (0 < pbc) && (0 < pca) )
		//return 1; // 点 p が常に左側にある（時計回り）
	if ( (0 > pab) && (0 > pbc) && (0 > pca) )
		return INSIDE; // 点 p が常に右側にある（反時計回り）

	// 三角形の線上にあれば負
	//if ( (0 <= pab) && (0 <= pbc) && (0 <= pca) )
	//	return -1; // 点 p が常に左側にある（時計回り）
	if ( (0 >= pab) && (0 >= pbc) && (0 >= pca) )
		return INSIDE; // 点 p が常に右側にある（反時計回り）

	// 三角形の外側にあれば 0
	return OUTSIDE;

			
		

}
POINTSTATE CTriangle::PtInTriangleSpecial(CVertex *_pt,bool _debug){//三角形にマイナスインデックスを含む場合は特別な処理をする
	POINTSTATE lowehigher;//lowehigherは何エッジなのか？
	//2つがマイナスインデックスの場合
	if(a->trueId<0 && b->trueId<0){
		if(c->LexicographicOrdering(_pt)){
			if(_debug){this->PrintMyVertexInfo("内側");}
			return INSIDE;
		}else{
			if(_debug){c->PrintVertex("cよりも下だった。外側");}
			return OUTSIDE;
		}
	}
	if(b->trueId<0 && c->trueId<0){
		if(a->LexicographicOrdering(_pt)){
			if(_debug){this->PrintMyVertexInfo("内側");}
			return INSIDE;
		}else{
			if(_debug){a->PrintVertex("aよりも下だった。外側");}
			return OUTSIDE;
		}
	}
	if(c->trueId<0 && a->trueId<0){
		if(b->LexicographicOrdering(_pt)){
			if(_debug){this->PrintMyVertexInfo("内側");}
			return INSIDE;
		}else{
			if(_debug){b->PrintVertex("bよりも下だった。外側");}
			return OUTSIDE;
		}
	}

	//pdf /http://www.cs.uu.nl/geobook/interpolation.pdf p204
	//三角形の頂点が一つ、マイナスインデックスを含んでいる。
	CVertex *higher,*lower;//マイナスインデックスじゃない頂点
	int minusid;
	if(a->trueId>=0){
		if(b->trueId>=0){//cがマイナスインデックス
			minusid=c->trueId;
			lowehigher=INIEDGE;
			//反時計回りに並んでいることを利用する
			if(minusid==-2){higher=b;lower=a;}else{higher=a;lower=b;}
			
		}else{//bがマイナスインデックス
			minusid=b->trueId;
			lowehigher=PREVEDGE;
			if(minusid==-2){higher=a;lower=c;}else{higher=c;lower=a;}
		}
	}else{//aがマイナスインデックスの場合
		minusid=a->trueId;
		lowehigher=NEXTEDGE;
		if(minusid==-2){higher=c;lower=b;}else{higher=b;lower=c;}
	}
	//lower,higherが求まった。
	//pt,pi.pjのlexicographycal orderを求める。
	//if(lower->LexicographicOrdering(_pt)){//lowerよりも_ptが下だった。
	//	higher->PrintVertex("higherよりも_ptが下だった。");		
	//	return OUTSIDE;
	//}else{
	//こういう初期三角形の設定
	//p-2、
	// ＼　＼
	//   ＼   ＼p0
	//     ＼:::|
	//　　　　＼:|
	//         ヽ|
	//          p-1
	if(higher->LexicographicOrdering(_pt) && _pt->LexicographicOrdering(lower)){//insideの可能性あり
		////次はマイナスじゃない点同士の辺の左右どっち側にあるか判定する。
			CLine line(lower,higher);
			CLine line2(lower,_pt);
				if(minusid==-1){//-1=lowerな方
					double cross=line.CrossProduct(&line2);
					if(cross>0){// && cross!=1.0){
						if(_debug){printf("minus -1 lower->higher方向のベクトルの左側だったら外側。\n");}
						return OUTSIDE;
					}else if(cross==0){//||cross==1.0){//
						if(_debug){printf("★minus -１ lower->higher方向のベクトルのcross=0はonedge%.16f★\n",cross);}
						//line==iniedgeならiniedge 
						return lowehigher;
					}else{//
						if(_debug){printf("minus -1 lower->higher方向のベクトルの右側だったら内側。\n");}
						
						return INSIDE;
					}
				}else{//minusid==-2 higherなほうの点の場合
					//lowe->higher方向のベクトルの左側だったら内側。
					double cross=line.CrossProduct(&line2);
					if(cross>0){// && cross!=1.0){
						if(_debug){
							this->PrintMyVertexInfo("tri");
							lower->PrintVertex("lower");
							_pt->PrintVertex("pt");
							higher->PrintVertex("higher");
							printf("minus -2 lower->higher方向のベクトルの左側だったら内側。cross=%.16f\n",cross);
						}
						return INSIDE;
					}else if(cross==0){//||cross==1.0){//ここにひっかかると後に縮退した三角形になってしまう
						if(_debug){printf("★minus -2 lower->higher方向のベクトルのcross=0はonedgecross=%.16f★\n",cross);}
						
						return lowehigher;
					}else{//
						if(_debug){printf("minus -2 owe->higher方向のベクトルの右側だったら外側。\n");}
						return OUTSIDE;
					}
				}
		}else{//
			if(_debug){
				this->PrintMyVertexInfo("");
				lower->PrintVertex("lower");
				_pt->PrintVertex("pt");
				higher->PrintVertex("higher");
				printf("lower higherの範囲外\n");
			}
			return OUTSIDE;
		}
	//}
	
}
POINTSTATE CTriangle::PtInTriangle2(CVertex *_pt,bool _debug){//これが一番のオーバーヘッド
	//Barycentric Technique
	//http://www.blackpawn.com/texts/pointinpoly/default.html
	// Compute vectors        
	//if(_pt->myId==3047){_debug=true;}//3047番目がまったく同じデータ
	CVertex v0 = (*this->c) - (*this->a);
	CVertex v1 = (*this->b) - (*this->a);
	CVertex v2 = *_pt - (*this->a);
// Compute dot products
	double dot00 = v0.CalcInnerProduct(&v0);
	double dot01 = v0.CalcInnerProduct(&v1);
	double dot02 = v0.CalcInnerProduct(&v2);
	double dot11 = v1.CalcInnerProduct(&v1);
	double dot12 = v1.CalcInnerProduct(&v2);

	// Compute barycentric coordinates
	if( (dot00 * dot11 - dot01 * dot01)==0){
		printf("★devide by ０ at PtInTriangle★\n");//xy平面上に重複する点があるとこれになるようだ。
		
		this->a->PrintVertex("a");
		this->b->PrintVertex("b");
		this->c->PrintVertex("c");

		
		//assert(!"★devide by ０★\n");
	}
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
				if(_debug){
				this->PrintMyVertexInfo("tri");
				this->a->PrintVertex("a");
				this->b->PrintVertex("b");
				this->c->PrintVertex("c");
				
				_pt->PrintVertex("testpoint");
				printf("e version u=%.16e,v=%.16e 1-(u+v=)%.16e\n",u,v,1.0-(u+v));
				printf("普通の表記u=%.16f,v=%.16f 1-(u+v)=%.16f\n",u,v,1.0-(u+v));
				}
	// Check if point is in triangle+
	
	//★ものすごく小さい値を比較する場合は注意が必要★普通にa-b<EPSILONとやると正しく比較できない。
	//http://ja.wikipedia.org/wiki/%E6%B5%AE%E5%8B%95%E5%B0%8F%E6%95%B0%E7%82%B9%E6%95%B0
	//Intelのプロセッサの場合はレジスタのときだけ80bit,メモリのときは64bitになるからそれも注意が必要かも。
	//a,bの絶対値を取り、大きいほうをかける方法
	//http://d.hatena.ne.jp/tociyuki/20090620/1245503707
	//|u-v|<EPSILON*|u| かつ|u-v|<EPSILON*|v|という方法 
	//http://boost.cppll.jp/HEAD/libs/test/doc/floating_point_comparison.htm
		
		if(u<=EPSILON && u>=-EPSILON && v<=1.0&&v>0.0){if(_debug){printf("★INIEDGE辺の上★\n");}return INIEDGE;}
		else 
			if(v<=EPSILON && v>=-EPSILON && u<=1.0&&u>0.0){if(_debug){printf("★PREVEDGE辺の上★\n");}return PREVEDGE;}
		else 
			if(u + v <= 1.0+EPSILON && u+v >=1.0-EPSILON) {if(_debug){printf("★NEXTEDGE辺の上★\n");}return NEXTEDGE;}
		else 
	//unsigned long long a=0x8000000000000000ull;
		if( (u >= EPSILON/*(double*)&a */) && (v >=EPSILON /*(double*)&a*/ ) && (u + v <= 1)){
			if(_debug){printf("★内側★\n");}
			return INSIDE;}
		else{
			if(_debug){printf("★外側★\n");}
			return OUTSIDE;
			}
		
			
		

}

POINTSTATE CTriangle::PtInTriangle2(CVertex *_pt,CVertex* _scale,CVertex* _offset,bool _debug){
	//if(IsDegenerateTriangle(false)){return OUTSIDE;}
	//Barycentric Technique
	//http://www.blackpawn.com/texts/pointinpoly/default.html
	// Compute vectors    
	CVertex aa(a->x*(_scale->x)+_offset->x,a->y*(_scale->y)+_offset->y);
	CVertex bb(b->x*(_scale->x)+_offset->x,b->y*(_scale->y)+_offset->y);
	CVertex cc(c->x*(_scale->x)+_offset->x,c->y*(_scale->y)+_offset->y);


	CVertex v0 = (cc) - (aa);
	CVertex v1 = (bb) - (aa);
	CVertex v2 = *_pt - (aa);


	// Compute dot products
	double dot00 = v0.CalcInnerProduct(&v0);
	double dot01 = v0.CalcInnerProduct(&v1);
	double dot02 = v0.CalcInnerProduct(&v2);
	double dot11 = v1.CalcInnerProduct(&v1);
	double dot12 = v1.CalcInnerProduct(&v2);
	if( (dot00 * dot11 - dot01 * dot01)==0 &&_debug){
		_pt->PrintVertex("devide 0vert");
		this->PrintMyVertexInfo("devide 0 tri");
		this->a->PrintVertex("a");
		this->b->PrintVertex("b");
		this->c->PrintVertex("c");
		printf("★devide by ０★\n");
		assert(!"★devide by ０★\n");
	}
	// Compute barycentric coordinates
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
	//printf("u=%.16f,v=%.16f u+v=%.16f\n",u,v,u+v);
	if(u<=EPSILON && u>=-EPSILON){
		//this->PrintMyVertexPosition("t");
		//printf("u=%.16f,v=%.16f u+v=%.16f\n",u,v,u+v);
		return INIEDGE;}
	else 
		if(v<=EPSILON && v>=-EPSILON){
			//this->PrintMyVertexPosition("t");
			//printf("u=%.16f,v=%.16f u+v=%.16f\n",u,v,u+v);
			return PREVEDGE;}
		else 
			if(u + v <= 1.0+EPSILON && u+v >=1.0-EPSILON) {
				//this->PrintMyVertexPosition("t");
				//printf("u=%.16f,v=%.16f u+v=%.16f\n",u,v,u+v);
				return NEXTEDGE;}
			else 
				//unsigned long long a=0x8000000000000000ull;
				if( (u >= EPSILON/*(double*)&a */) && (v >=EPSILON /*(double*)&a*/ ) && (u + v <= 1)){
					return INSIDE;}
				else{

					return OUTSIDE;
				}
}

bool CTriangle::PtInTriangle(CVertex *_pt){
	//Same Side Technique
	//http://www.blackpawn.com/texts/pointinpoly/default.html
	CVertex ab;
	ab.CalcVector(this->initialedge->begin,this->initialedge->end);
	CVertex ap;
	ap.CalcVector(this->initialedge->begin,_pt);
	if(ab.CrossProduct(ap)<0){//内側
		CVertex ca;
		ca.CalcVector(this->initialedge->prev->begin,this->initialedge->begin);
		CVertex cp;
		cp.CalcVector(this->initialedge->prev->begin,_pt);
		if(ca.CrossProduct(cp)<0){
			CVertex bc;
			bc.CalcVector(this->initialedge->end,this->initialedge->prev->begin);
			CVertex bp;
			bp.CalcVector(this->initialedge->end,_pt);
			if(bc.CrossProduct(bp)<0){return true;}else{return false;}
		}else{return false;}
	}else{return false;}

}
bool CTriangle::operator!=(const CTriangle& _in)const{
	if(this->initialedge==_in.initialedge){return false;}
	else{return true;}
}
bool CTriangle::operator==(const CTriangle& _in)const{
	if(a==_in.a && b==_in.b && c==_in.c){return true;}
	//一巡して同じなら同じ三角形だ！
	else if(a==_in.b && b==_in.c && c==_in.a){
		return true;
	}else if(a==_in.c && b==_in.a && c==_in.b){
		return true;
	}
	else{return false;}
}
void CTriangle::GetMyVertexString(int _i,stringstream& _in){
	
	_in<<"["<<_i<<"]myId["<<this->myId<<"]("<<this->initialedge->begin->trueId<<","<<this->initialedge->end->trueId<<","<<this->initialedge->next->end->trueId<<")childnum="<<this->childnum;

}
bool CTriangle::IsInMyCircumCircle(CVertex *_p){
	if(this->IsMyVertex(_p))return false;
	if(this->circumCircle==0){this->CalcCircumCircle();}
	return this->circumCircle->PointInCircle(_p);
}
bool CTriangle::IsInMyCircumCircle2(CVertex *_p){
	//平面上の４点の行列式バージョン
	if(this->IsMyVertex(_p))return false;
	//
	//|ax,ay,ax^2+ay^2,1| |ax-px,ay-py,(ax^2-px^2)+(ay^2-py^2)|
	//|bx,by,bx^2+by^2,1|=|bx-px,by-py,(bx^2-px^2)+(by^2-py^2)|>0
	//|cx,cy,cx^2+cy^2,1| |cx-px,cy-py,(cx^2-px^2)+(cy^2-py^2)|
	//|px,py,px^2+py^2,1| 
	//右辺の3×3の行列をつくる
	//|0,1,2|
	//|3,4,5|
	//|6,7,8|
	//a,b,cは反時計周りに並んでいなければならない。
	
	double matrix[9];
	double pvalue=(_p->x*_p->x+_p->y*_p->y);
	matrix[0]=a->x-_p->x;matrix[1]=a->y-_p->y;matrix[2]=(a->x*a->x+a->y*a->y)-pvalue;
	matrix[3]=b->x-_p->x;matrix[4]=b->y-_p->y;matrix[5]=(b->x*b->x+b->y*b->y)-pvalue;
	matrix[6]=c->x-_p->x;matrix[7]=c->y-_p->y;matrix[8]=(c->x*c->x+c->y*c->y)-pvalue;
	
	
	//サラスの公式
	double det=
	(matrix[0]*matrix[4]*matrix[8]+matrix[3]*matrix[7]*matrix[2]+matrix[6]*matrix[1]*matrix[5])
	-
	(matrix[2]*matrix[4]*matrix[6]+matrix[5]*matrix[7]*matrix[0]+matrix[1]*matrix[3]*matrix[8]);
	
	if(det>0){return true;}else{return false;}


}
CTriangle& CTriangle::operator=(const CTriangle& _in){
	this->initialedge=_in.initialedge;
	this->myId=_in.myId;
	SetBelongsTo();
	childnum=_in.childnum;
	for(int i=0;i<childnum;i++){this->children[i]=_in.children[i];}
	return *this;

}
bool CTriangle::IsMyVertex(CVertex *testPoint){
	if(a->trueId==testPoint->trueId){//a->IsSameVertex(testPoint)){
		return true;
	}else if(b->trueId==testPoint->trueId){//b->IsSameVertex(testPoint)){
		return true;
	}else if(c->trueId==testPoint->trueId){//c->IsSameVertex(testPoint)){
		return true;
	}else{
		return false;
	}
}
bool CTriangle::IsSameTriangle(CTriangle *_testTriangle){
	if(this->initialedge==_testTriangle->initialedge){return true;}
	else{return false;}
}
bool CTriangle::PtOnEdge(CVertex* _pt ,int* _onedge){
	//initial edge test
	double tx=0;
	double ty=0;
	tx=(_pt->x-this->initialedge->begin->x)/(this->initialedge->end->x-this->initialedge->begin->x);
	ty=(_pt->y-this->initialedge->begin->y)/(this->initialedge->end->y-this->initialedge->begin->y);
	if(/*tx==ty &&*/ tx>0 && tx<1 && ty>0 && ty<1){*_onedge=this->initialedge->myId;return true;}
	//next edge test
	tx=(_pt->x-this->initialedge->next->begin->x)/(this->initialedge->next->end->x-this->initialedge->next->begin->x);
	ty=(_pt->y-this->initialedge->next->begin->y)/(this->initialedge->next->end->y-this->initialedge->next->begin->y);
	if(/*tx==ty &&*/ tx>0 && tx<1 && ty>0 && ty<1){*_onedge=this->initialedge->next->myId;return true;}
	//prev edge test
	tx=(_pt->x-this->initialedge->prev->begin->x)/(this->initialedge->prev->end->x-this->initialedge->prev->begin->x);
	ty=(_pt->y-this->initialedge->prev->begin->y)/(this->initialedge->prev->end->y-this->initialedge->prev->begin->y);
	if(/*tx==ty &&*/ tx>0 && tx<1 && ty>0 && ty<1){*_onedge=this->initialedge->prev->myId;return true;}
	return false;
}
bool CTriangle::PtOnEdge2(CVertex* _pt,int* _onedge){
	//Barycentric Technique
	// Compute vectors        
	CVertex v0 = *(this->initialedge->prev->begin) - *(this->initialedge->begin);
	CVertex v1 = *(this->initialedge->end) - *(this->initialedge->begin);
	CVertex v2 = *_pt - *(this->initialedge->begin);

	// Compute dot products
	double dot00 = v0.CalcInnerProduct(&v0);
	double dot01 = v0.CalcInnerProduct(&v1);
	double dot02 = v0.CalcInnerProduct(&v2);
	double dot11 = v1.CalcInnerProduct(&v1);
	double dot12 = v1.CalcInnerProduct(&v2);

	// Compute barycentric coordinates
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	if(u==0){*_onedge=this->initialedge->prev->myId;return true;}
	if(v == 0){*_onedge=this->initialedge->myId;return true;}
	if(u + v == 1){*_onedge=this->initialedge->next->myId;return true;}
	return false;
}

CCircle  CTriangle::CalcCircumCircle()//外接円の計算
{
	//外心の計算
	//area

	double ab=this->a->CalcLength(b);
	double bc=this->b->CalcLength(c);
	double ca=this->c->CalcLength(a);
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
	CVertex v( (this->a->x * A + this->b->x * B + this->c->x * C) / (A+B+C),
		(this->a->y * A + this->b->y * B + this->c->y * C) / (A+B+C));
	//半径の計算	
	double rad=bc / sin(angleA) / 2.0 ;
	this->circumCircle=new CCircle(v,rad);
	return *circumCircle;


}
void CTriangle::PrintMyVertexInfo(const char* message){
	printf("%s:",message);
	//printf("myId[%d](%d,%d,%d)\n",this->myId,this->initialedge->begin->trueId,this->initialedge->end->trueId,this->initialedge->prev->begin->trueId);
	printf("myId[%d](%d,%d,%d)\n",this->myId,a->trueId,b->trueId,c->trueId);
	printf("childnum=%d\n",this->childnum);
	//this->a->PrintVertex("a");this->b->PrintVertex("b");this->c->PrintVertex("c");
	//this->a->PrintVertex("a");this->b->PrintVertex("b");this->c->PrintVertex("c");
//	printf("line(%d,%d,%d)\n",this->initialedge->myId,this->initialedge->next->myId,this->initialedge->prev->myId);
	
}
void CTriangle::PrintMyVertexPosition(const char* message){
	printf("%s:",message);
	printf("[%d]",this->myId);
	this->a->PrintVertex("a");
	this->b->PrintVertex("b");
	this->c->PrintVertex("c");
	//printf("myId[%d](%d,%d,%d)\n",this->myId,this->initialedge->begin->trueId,this->initialedge->end->trueId,this->initialedge->prev->begin->trueId);
	//printf("myId[%d](%d,%d,%d)\n",this->myId,a->trueId,b->trueId,c->trueId);
	//this->a->PrintVertex("a");this->b->PrintVertex("b");this->c->PrintVertex("c");
	//this->a->PrintVertex("a");this->b->PrintVertex("b");this->c->PrintVertex("c");
//	printf("line(%d,%d,%d)\n",this->initialedge->myId,this->initialedge->next->myId,this->initialedge->prev->myId);
	
}
void CTriangle::PrintMyEdgeInfo(const char* message){
	printf("%s:",message);
	//printf("myId[%d](%d,%d,%d)\n",this->myId,this->initialedge->begin->trueId,this->initialedge->end->trueId,this->initialedge->prev->begin->trueId);
	printf("myId[%d](%d,%d,%d)\n",this->myId,this->initialedge->begin->trueId,this->initialedge->end->trueId,this->initialedge->prev->begin->trueId);
	//this->a->PrintVertex("a");this->b->PrintVertex("b");this->c->PrintVertex("c");
	//this->a->PrintVertex("a");this->b->PrintVertex("b");this->c->PrintVertex("c");
//	printf("line(%d,%d,%d)\n",this->initialedge->myId,this->initialedge->next->myId,this->initialedge->prev->myId);
	
}

CTriangle::~CTriangle(void)
{
}


void CTriangle::SetBelongsTo(){//接続面の登録この時点でidが引き継がれていない。
	this->initialedge->incidentTriangle=this;
	this->initialedge->next->incidentTriangle=this;
	this->initialedge->prev->incidentTriangle=this;
	
	//this->initialedge->incidentTriangle->PrintMyVertexInfo("set belongs to");
}

