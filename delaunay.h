#include <float.h>
#include <iostream>
#include <assert.h>
#include "Triangle.h"
#include <miffy/math/vec3.h>
using namespace std;
int pointnum=0;
int linecount=0;
int tcount=0;//¡‚Ü‚Åì‚Á‚½‚·‚×‚Ä‚ÌŽOŠpŒ`‚Ì”‚ÌƒJƒEƒ“ƒg triBuffer‚ÉŽg‚¤B
int finaltriangle=0;//ÅI“I‚Éo—Í‚·‚éŽOŠpŒ`‚Ì”‚ðƒJƒEƒ“ƒg
int droppedvertexnum=0;//‚Æ‚è‚à‚ç‚µ‚½“_‚Ì”
int section=0;//is‹ï‡•\Ž¦‚ÉŽg‚¤B
int focusr=-5;//3047;///;//ƒfƒoƒbƒO—p@â‘Î‚É‚±‚È‚¢iƒ}ƒCƒiƒX‚T‚Æ‚©j”‚É‚µ‚Ä‚¨‚¯‚Î•Ï‚È•\Ž¦‚ÍÁ‚¦‚é
int skipr=5168;//5168‚Æ5177‚Í“¯‚¶À•W‚Ì’¸“_
CTriangle **triBuffer;//ƒ|ƒCƒ“ƒ^‚Ì”z—ñ¡‚Ü‚Åì‚Á‚½‚·‚×‚Ä‚ÌŽOŠpŒ`Bi‚Q‰ñ–Ú‚Étraverse‚·‚é‚Æ‚È‚º‚©–³ŒÀƒ‹[ƒv‚É‚È‚é‚Ì‚ÅA‚»‚ê‘Îôj
CTriangle **finalTriangle;//‰Šú‰»ŽOŠpŒ`‚ðŠÜ‚Ü‚È‚¢ÅI“I‚Èo—ÍŽOŠpŒ`

CLine **lineBuffer;////•Ó‚ð’™‚ß‚Ä‚¨‚­BŠù‘¶‚Ì•Ó‚ªnextŠÖŒW‚ª•Ï‚í‚Á‚½‚è‚·‚éBí‚É¡Œ©‚¦‚Ä‚é•Ó‚Ì‚Ý‚Í‚¢‚Á‚Ä‚¢‚é
CVertex **vertexBuffer;//ƒtƒ@ƒCƒ‹‚©‚ç“Ç‚Ýž‚ñ‚¾“_ŒQ
CVertex *outVertex;
CVertex M;//‰ŠúŽOŠpŒ`‚ÉŽg‚¤B
CVertex origin;//“_ŒQ‚Ì^‚ñ’†
//root triangle‚ðì‚é‚Ì‚É•K—vB
CVertex mindata(DBL_MAX,DBL_MAX);
CVertex maxdata(-DBL_MAX,-DBL_MAX);
CVertex range;
double **inputdata;
//ƒ`ƒFƒbƒNƒ‹[ƒ`ƒ“
int *exceptsplit4;
int *illegalTri;
int *illegalVert;
int rightmostindex=0;//ˆê”Ô‰E‘¤‚Ì’¸“_‚ÌƒCƒ“ƒfƒbƒNƒXB‰ŠúŽOŠpŒ`‚ðì‚é‚Æ‚«‚É•K—vB

typedef struct tagoutput{//o—Íƒf[ƒ^‚ð“ü‚ê‚é‚Ì‚ÉŽg‚¤
  int size;
	double **vertex;//[POINTNUM*]
}OUTDATA,*POUTDATA;
OUTDATA out_data;

void ArrayInit(){

	vertexBuffer=new CVertex*[pointnum+2];
	exceptsplit4 = new int[pointnum+2];
	

	finalTriangle=new CTriangle*[(9*pointnum+1)*3];//nŒÂ‚Ì’¸“_‚Åo—ˆ‚éŽOŠpŒ`‚ÌŠú‘Ò’l‚Í‚X(9*n+1)ŒÂ‚ç‚µ‚¢B‚Å‚àA‚»‚ê‚¶‚á‘«‚è‚È‚©‚Á‚½‚©‚ç“K“–‚ÉŽO”{‚É‚µ‚½B
	triBuffer=new CTriangle*[(9*pointnum+1)*3];
	lineBuffer=new CLine*[(9*pointnum+1)*3];
	for(int i=0;i<pointnum+2;i++){
		exceptsplit4[i]=0;
	}
	
	

}
void ConnectTriangleLines(int _begin,int _next,int _end){
	//MakeTriangleConnection
		lineBuffer[_begin]->next=lineBuffer[_next];
		lineBuffer[_begin]->prev=lineBuffer[_end];
		lineBuffer[_next]->next=lineBuffer[_end];
		lineBuffer[_next]->prev=lineBuffer[_begin];
		lineBuffer[_end]->next=lineBuffer[_begin];
		lineBuffer[_end]->prev=lineBuffer[_next];
		
}
int MakeTriangle(int _a,int _b,int _c,int _ini){//3’¸“_‚ÌƒCƒ“ƒfƒbƒNƒX‚ð”½ŽžŒv‰ñ‚è‚ÅŽw’è‚µ‚½‚çŽOŠpŒ`‚ðì‚Á‚Ä‚­‚ê‚éB
	//ini‚ÍŠù‘¶‚Ìü
	if((_a>=pointnum &&_b>=pointnum &&_c>=pointnum )/*not minus */
		&&((vertexBuffer[_a]->x==vertexBuffer[_b]->x &&vertexBuffer[_b]->x==vertexBuffer[_c]->x )
		||(vertexBuffer[_a]->y==vertexBuffer[_b]->y &&vertexBuffer[_b]->y==vertexBuffer[_c]->y ))){
			printf("k‘ÞŽOŠpŒ`‚Íì‚ç‚¹‚È‚¢B\n");
			return -1;
	}
	//a->b‚Í‚·‚Å‚É‚ ‚é‚©‚çì‚ç‚È‚¢B
	
	lineBuffer[linecount]=new CLine(vertexBuffer[_b],vertexBuffer[_c],linecount);
	
	int next=linecount;
	linecount++;
	
	lineBuffer[linecount]=new CLine(vertexBuffer[_c],vertexBuffer[_a],linecount);
	
	int end=linecount;
	linecount++;
	ConnectTriangleLines(_ini,next,end);
	triBuffer[tcount]=new CTriangle(lineBuffer[_ini],tcount);
	tcount++;
	return tcount-1;
}
int MakeFlipTriangle(int _begin,int _next,int _end){
	//Šù‚É‚ ‚éü‚¾‚¯‚Åì‚éê‡
	if((lineBuffer[_begin]->begin->myId>=pointnum &&lineBuffer[_begin]->end->myId>=pointnum &&lineBuffer[_next]->end->myId>=pointnum )/*not minus */
		&&((lineBuffer[_begin]->begin->x==lineBuffer[_begin]->end->x &&lineBuffer[_begin]->end->x==lineBuffer[_next]->end->x )
		||(lineBuffer[_begin]->begin->y==lineBuffer[_begin]->end->y &&lineBuffer[_begin]->end->y==lineBuffer[_next]->end->y ))){
			printf("k‘ÞŽOŠpŒ`‚ðì‚ç‚¹‚È‚¢B\n");
			return -1;
	}
	ConnectTriangleLines(_begin,_next,_end);
	triBuffer[tcount]=new CTriangle(lineBuffer[_begin],tcount);
	tcount++;
	return tcount-1;

}
CTriangle* MakeChild(int _child,/*CLine* _pipj,*/CTriangle *_parent){
	//if(tcount==158){_t->PrintMyVertexInfo("i am parent");}
	int parentIndex=_parent->myId;
	int childnum=_parent->childnum;
	//[tcount]=new CTriangle(_pipj,tcount);
	_parent->children[_parent->childnum]=triBuffer[_child];//‚±‚±‚Å‚È‚º‚©myId‚ª‰ó‚ê‚é
	_parent->myId=parentIndex;//‰ó‚ê‚é‚©‚ç•Û‘¶‚µ‚Ä‚¨‚­
	_parent->childnum=childnum;
	
	//if(tcount==158){
	//	_t->children[_t->childnum]->PrintMyVertexInfo("here is my children");}
	_parent->childnum++;
	
	//tcount++;
	return _parent->children[_parent->childnum-1];
	
}
bool IllegalTest(CLine *_pipj){	
	//ŽOŠpŒ`_tri‚É‘®‚·‚é•Ópi,pj‚ª•s³‚Å‚ ‚é‚©‚Ç‚¤‚©‚ÌƒeƒXƒg •s³‚¾‚Á‚½‚çtrue‚ð•Ô‚·B
	//•Ópi,pj‚ª“ñ‚Â‚ÌŽOŠpŒ`pipjpr‚Æpipjpk‚ÉÚ‚µ‚Ä‚¢‚é‚à‚Ì‚Æ‚·‚éB
	//‰~C‚ðApi,pj,pr‚ð’Ê‚é‰~‚Æ‚·‚éB
	//‚±‚Ì‚Æ‚«Bpk‚ª‰~C‚Ì“à•”‚ÉŠÜ‚Ü‚ê‚Ä‚¢‚½‚çpipj‚Í•s³‚È•Ó‚Å‚ ‚éB
	//‚³‚ç‚ÉApi,pjpk,pr‚ª“ÊŽlŠpŒ`‚ð¬6‚µ“¯‚¶‰~‚Ìã‚É‚Í‚È‚¢‚Æ‚«Bpi,pj‚Æpr,pk‚Ì‚¤‚¿ˆê•û‚¾‚¯‚ª•s³‚È•Ó‚Å‚ ‚éB
	//currentCircle=*(_pipj->incidentTriangle->circumCircle);
	//‚Ü‚¸‚ÍA—×Ú‚·‚éŽOŠpŒ`‚ª‚ ‚é‚Ì‚©‚Ç‚¤‚©‚¾B
	
	if(_pipj->incidentTriangle->IsInMyCircumCircle2(_pipj->twin->next->end)){return true;}else{return false;}
//IsInMyCircumCircle2s—ñŽ®ƒo[ƒWƒ‡ƒ“‚æ‚è‚à
//	IsInMyCircumCircle()@ŠOÚ‰~ƒf[ƒ^‚ðƒLƒƒƒbƒVƒ…‚É‚½‚ß‚Ä‚¨‚­ƒo[ƒWƒ‡ƒ“‚Ì‚Ù‚¤‚ªŽáŠ±‘¬“x‚ª‘¬‚©‚Á‚½B
}
void RelateTwin(int _i,int _j){
	//—×ÚŠÖŒW‚ðÝ’è‚·‚é
	lineBuffer[_i]->haveTwin=true;lineBuffer[_i]->twin=lineBuffer[_j];
	lineBuffer[_j]->haveTwin=true;lineBuffer[_j]->twin=lineBuffer[_i];

}
bool IsConvex(int _i,int _j,int _k,int _r){//‚±‚±‚É—ˆ‚é‚Ì‚ÍmyId(”z—ñ‚ÌƒCƒ“ƒfƒbƒNƒX)
	if(focusr==_r){
		vertexBuffer[_i]->PrintVertex("i");
		vertexBuffer[_j]->PrintVertex("j");
		vertexBuffer[_k]->PrintVertex("k");
		vertexBuffer[_r]->PrintVertex("r");
		printf("‚±‚ê‚Í“Ê‚©H\n");
	}
	//’¼ür-i@‚æ‚è‚à”½ŽžŒv‰ñ‚è³‚¾‚Á‚½‚çƒtƒŠƒbƒv‚µ‚È‚¢‚Å—~‚µ‚¢Bi‰š‚¾‚©‚çj
	//ƒ}ƒCƒiƒXƒCƒ“ƒfƒbƒNƒX‚ðŠÜ‚Þê‡‚Ì‰š“ÊƒeƒXƒg‚É‚Í’ˆÓ‚ª•K—v‚©B
	//i‚Æj‚Ì¶‰E‚É‚æ‚Á‚Ä‚ÍƒeƒXƒg‚É’ˆÓ‚ª•K—vB
	//         pk@@@@@@pr
	//      ^   _       ^   _
	//     pj     pi    pi      pj
	//      _  ^       _    ^
	//        pl(pr)       pk
	//       (a)           (b)ã‰º‹t‚³ƒo[ƒWƒ‡ƒ“
	//(a)‚Ìƒpƒ^[ƒ“‚Æ(b)‚Ìƒpƒ^[ƒ“‚Å‚Íˆá‚¤ƒeƒXƒg‚ð‚µ‚È‚¯‚ê‚Î‚È‚ç‚È‚¢B
	bool patterna=false;
	if(vertexBuffer[_r]->y<vertexBuffer[_k]->y){
				patterna=true;
	}

	CLine pipl(vertexBuffer[_i],vertexBuffer[_r]);//_pipj->prev‚¾‚Æplpi‚É‚È‚Á‚Ä‚µ‚Ü‚¤B‹t‚É‚µ‚½‚¢‚©‚çiiindex‚ð‚í‚´‚í‚´‚Æ‚Á‚½B;
	CLine pipk(vertexBuffer[_i],vertexBuffer[_k]);
	
	//i•t‹ß‚Ì‰š“Ê‚ðƒ`ƒFƒbƒN
	if(int minusi=vertexBuffer[_i]->trueId<0){
		if((minusi==-1 && !patterna) ||(minusi==-2 &&patterna)){
			if(focusr==_r){printf("i‚Í‰š‚Ý‚Å‚µ‚½patter %d\n",patterna);}
			return false;}
	}else{//i‚Íƒ}ƒCƒiƒX‚¶‚á‚È‚¢B‚ªAk‚ªƒ}ƒCƒiƒX‚¾‚Á‚½‚çH
		if((vertexBuffer[_k]->trueId==-1&&!patterna)||(vertexBuffer[_k]->trueId==-2&&patterna)){
			return false;//‰š‚Ý
		}else{
			if((patterna &&pipk.CrossProduct(&pipl)<=0)||(!patterna &&pipl.CrossProduct(&pipk)>=0) ){
				if(focusr==_r){printf("i‚Í‰š‚Ý‚Å‚µ‚½patter %d\n",patterna);}
				return false;//:TODO:==0‚Ì‚Æ‚«‚Í•Ó‚Ìã‚É‚Ì‚Á‚Ä‚é•Ï‚È‚â‚Â
			}else{
				if(_r==focusr){printf("i‚Í“Ê‚Å‚µ‚½\n");}
			}
		}//k‚ªƒ}ƒCƒiƒX‚Ìê‡
}//itestI‚í‚è

	//itest‡Ši‚¶‚á‚È‚¢‚Æ‚±‚ÌêŠ‚É‚±‚ê‚È‚¢
	//j‘¤‚Ì‰š“Êƒ`ƒFƒbƒN
	//         pk@@@@@@pl
	//      ^   _       ^   _
	//     pj     pi    pi      pj
	//      _  ^       _    ^
	//        pl(pr)       pk
	//       (a)           (b)ã‰º‹t‚³ƒo[ƒWƒ‡ƒ“
		int minusj=vertexBuffer[_j]->trueId;
	if( minusj<0){
			if((minusj==-2 && !patterna) ||(minusj==-1 &&patterna)){
				if(focusr==_r){printf("j‚Í‰š‚Ý‚Å‚µ‚½\n");}
				return false;}
			else{
				if(_r==focusr){printf("pattern%d,j=%d\n",patterna,minusj);}
				return true;}
		}else{//j‚Íƒ}ƒCƒiƒX‚¶‚á‚È‚¢
		CLine pjpl(vertexBuffer[_j],vertexBuffer[_r]);
		CLine pjpk=CLine(vertexBuffer[_j],vertexBuffer[_k]);//_pipj->twin->prev=‚¾‚Æpkpj‚É‚È‚Á‚Ä‚µ‚Ü‚¤‚Ì‚Åjindex‚ð‚Æ‚é•K—v‚ª‚ ‚é;
		if((vertexBuffer[_k]->trueId==-2&&patterna)||(vertexBuffer[_k]->trueId==-1&&!patterna)){//k‚ªƒ}ƒCƒiƒX‚Ìê‡
				return false;
			}else{
			if((patterna &&pjpl.CrossProduct(&pjpk)<=0) || (!patterna && pjpl.CrossProduct(&pjpk)<=0)){
				if(_r==focusr){printf("‰š\n");}
				return false;//:TODO:==0‚Ì‚Æ‚«‚Í•Ó‚Ìã‚É‚Ì‚Á‚Ä‚é•Ï‚È‚â‚Â
			}else{ 
				if(_r==focusr){
					pjpk.CrossProductPrint(&pjpl);
					printf("Š®‘S‚É“Ê pattern=%d cross(pjpl->pjpk)=%.16f\n",patterna,pjpl.CrossProduct(&pjpk));}
				return true;//Š®‘S‚É“Ê
			}
			}//k‚ªƒ}ƒCƒiƒX‚Ìê‡
	}//j‚Íƒ}ƒCƒiƒX‚¶‚á‚È‚¢
}
bool LegalizeEdgePreProcess(int _r,CLine* _pipj,CTriangle *_t){
	
int k=0;
//“ú–{Œê”Åp238ŽQÆ
////http://www.cs.uu.nl/geobook/interpolation.pdf ‚Ìp204ŽQÆ
	if(!_pipj->haveTwin){//==_pipj->begin->trueId<0 && _pipj->end->trueId<0){//ij‚ªƒ}ƒCƒiƒX‚È‚Ì‚ÅƒtƒŠƒbƒv‚µ‚È‚­‚Ä‚æ‚¢
		if(_r==focusr){_pipj->PrintLine("pipj•‰‚È‚Ì‚Å ƒtƒŠƒbƒv‚µ‚È‚¢\n");}	
		return false;
	}else{ 
			k=_pipj->twin->next->end->trueId;
			if(_r==focusr){printf("\ni%d,j%d,k%d,r%d\n",_pipj->begin->trueId,_pipj->end->trueId,k,_r);}
			
			int i=_pipj->begin->trueId;
			int j=_pipj->end->trueId;
			if(k>=0 && i>=0 && j>=0){
				//ijkl‚·‚×‚Ä³‚Å‚ ‚éB_r‚ª³‚È‚Ì‚Í“–‘R‚Å‚ ‚éB
				if(_r==focusr){printf("‚·‚×‚Ä³‚È‚Ì‚ÅŠOÚ‰~ƒeƒXƒg");}
				return IllegalTest(_pipj);//‚±‚ê‚ª•’Ê‚Ìê‡‚Å‚ ‚éB
			}else{//all other cases ijl‚Ì‚¤‚¿ˆê‚Â‚¾‚¯•‰ik‚ª•‰‚Ìê‡‚ÍŠù‚ÉœŠOÏ‚Ýj
				//         pk
				//      ^   _
				//     pj     pi
				//      _  ^
				//        pl
				//printf("all other case\n");
				//min(k,l)<min(i,j)‚È‚ç‚Î³“–@‚»‚¤‚Å‚È‚¯‚ê‚Î•s³
				int minkl,minij;
				if(k<vertexBuffer[_r]->trueId){minkl=k;}else{minkl=vertexBuffer[_r]->trueId;}
				if(i<j){minij=i;}else{minij=j;}
				if(minkl<minij){
					if(_r==focusr){printf("minkl<minijƒtƒŠƒbƒv‚µ‚È‚¢\n");}	
					return false;
				}else{//ijk‚¢‚¸‚ê‚©ˆê‚Â‚Í•K‚¸•‰@i‚Æj‚ª—¼•û•‰‚Æ‚¢‚¤‚±‚Æ‚Í‚È‚¢B
					//lexicographic ordering ƒeƒXƒg
					int kindex=_pipj->twin->next->end->myId;
					if(IsConvex(_pipj->begin->myId,_pipj->end->myId,kindex,_r) )
					{	if(_r==focusr){printf("Š®‘S‚É“Ê‚È‚Ì‚ÅƒtƒŠƒbƒv\n");}	
						return true;
					}else{
						if(_r==focusr){printf("‰š‚Ý‚È‚Ì‚ÅƒtƒŠƒbƒv‚µ‚È‚¢\n");}	
						return false;
					}//ijkl‚Í“Ê‚È‚Ì‚©‚Ç‚¤‚©
				}//min kl minijƒeƒXƒg
			}//end all other cases ijl‚Ì‚¤‚¿ˆê‚Â‚¾‚¯•‰ik‚ª•‰‚Ìê‡‚ÍŠù‚ÉœŠOÏ‚Ýj
		}//ij•‰‚Ìê‡‚©‚Ç‚¤‚©

	}
bool LegalizeEdge(int _r,CLine* _pipj,CTriangle *_t){
	
	if(!LegalizeEdgePreProcess(_r,_pipj,_t)){return false;}

		//pipj‚ðpkpl‚É‚æ‚Á‚Ä’u‚«Š·‚¦‚é
		//«‚±‚¤‚¢‚¤”z’u‚Å‚ ‚é‚æ‚¤‚É’¼‚³‚È‚¢‚Æ‚¨‚©‚µ‚­‚È‚éB
		//         pk
		//      ^   _
		//     pj     pi
		//      _  ^
		//        pr
		//“ú–{Œê”Åp238ŽQÆ

		int pipjindex=_pipj->myId;
		CTriangle *neighborTri=triBuffer[_pipj->twin->incidentTriangle->myId];
		int pjprindex=_pipj->next->myId;
		int prpiindex=_pipj->prev->myId;
		int pjpiindex=_pipj->twin->myId;
		int pipkindex=_pipj->twin->next->myId;
		int pkpjindex=_pipj->twin->prev->myId;
		int pkindex=_pipj->twin->prev->begin->myId;
		//ŽOŠpŒ`prpkpj‚Ìì¬
		//pipj‚ðpkpr‚É’u‚«Š·‚¦‚é
		//          pk        @    pk
		//        ^  _    @    ^ |_
		//      ^      _  @  ^   |  _
		//    pj________pi  ¨ pj    |    pi
		//     _       ^   @ _   |   ^
		//       _   ^      @  _ | ^
		//          pr         @    pr
		
		
		//pk-prü‚ªini‚É‚È‚é‚æ‚¤‚Éì‚é
		lineBuffer[pipjindex]->MakeLine(vertexBuffer[pkindex],vertexBuffer[_r]);
		int pkprindex=pipjindex;
		
		int ch0index=MakeFlipTriangle(pkprindex,prpiindex,pipkindex);
		lineBuffer[pjpiindex]->MakeLine(vertexBuffer[_r],vertexBuffer[pkindex]);
		int prpkindex=pjpiindex;
		int ch1index=MakeFlipTriangle(prpkindex,pkpjindex,pjprindex);
		
		//ŽŸ‚Í—×Úî•ñ‚¾B
		if(ch0index!=-1&&ch1index!=-1){RelateTwin(pkprindex,prpkindex);}
		//‚±‚±‚Ü‚Å‚ÅAŠù‘¶‚Ì‚ÌŽOŠpŒ`‚ÌnextŠÖŒW‚Í•Ï‚í‚Á‚½B
		//          pk        @    pk
		//        ^  _    @    ^ |_
		//     ^neghbor_  @  ^   |  _
		//   pj___________pi¨ pj 1 | 0   pi
		//     _       ^   @ _   |   ^
		//       _   ^      @  _ | ^
		//          pr         @    pr
		//e‚¾‚Æ‚¢‚¤‚±‚Æ‚ð’m‚ç‚¹‚é
		if(ch0index!=-1){
				CTriangle* ch1p=MakeChild(ch0index,_t);//prpkpj
				//neighbor‚É‚Æ‚Á‚Ä‚àe‚Å‚ ‚é
				neighborTri->children[neighborTri->childnum]=ch1p;
				neighborTri->childnum++;
				//pk‘¤‚ðX‚Éleganize
				LegalizeEdge(_r,lineBuffer[pipkindex],ch1p);//more leganize
		}
		if(ch1index!=-1){
			CTriangle* ch2p=MakeChild(ch1index,_t);//pkprpi
			neighborTri->children[neighborTri->childnum]=ch2p;
			neighborTri->childnum++;
			LegalizeEdge(_r,lineBuffer[pkpjindex],ch2p);
		}
	
	return true;
	
}
int Split3Triangle(int _r,CTriangle* _parent,CTriangle* _children[3]){//“_r‚É‚æ‚Á‚Ä‚R‚Â‚É•ªŠ„I
	//p236 }9.7‚ðŽQÆ‚É‚µ‚½
	//p236ŽQÆ
	
	if(_r==focusr){_parent->PrintMyVertexInfo("i am parent");}
	CVertex* pr=vertexBuffer[_r];
	int id=_parent->myId;
	
	
	int pjpkindex=_parent->initialedge->next->myId;
	int pkpiindex=_parent->initialedge->prev->myId;
	int pipjindex=_parent->initialedge->myId;
	
	CLine* pipj=_parent->initialedge;
	
	
	/*CVertex* pi=pipj->begin;
	CVertex* pj=pipj->end;
	CVertex* pk=pipj->next->end;*/
	int pi=pipj->begin->myId;
	int pj=pipj->end->myId;
	int pk=pipj->next->end->myId;
	////6–{‚ÌV‚µ‚¢ü‚ªo—ˆ‚é‚Í‚¸B
	//          pk
	//        ^| _
	//      ^  |   _
	//    ^ 2  pr  1 _
	//  ^    ^0 _    _
	//pi____^____________pj
	//—×Úî•ñ‚ð—^‚¦‚Ä‚©‚çŽOŠpŒ`‚ðì‚éB
	//initialedge‚ÍŠO‘¤‚Ì•Ó‚É‚È‚é‚æ‚¤‚Éì‚éB
	int childcount=0;
	int ch0index=MakeTriangle(pi,pj,_r,pipjindex);
	if(ch0index!=-1){
		_children[childcount]=MakeChild(ch0index,_parent);
		childcount++;
	}
	int ch1index=MakeTriangle(pj,pk,_r,pjpkindex);
	if(ch1index!=-1){
		_children[childcount]=MakeChild(ch1index,_parent);
		
		childcount++;	
	}
	int ch2index=MakeTriangle(pk,pi,_r,pkpiindex);
	if(ch2index!=-1){
		_children[childcount]=MakeChild(ch2index,_parent);
		
		childcount++;
	}
	//          pk
	//        ^| _
	//      ^  |   _
	//    ^ 2  pr  1 _
	//  ^    ^0 _    _
	//pi____^____________pj
//ŽOŠpŒ`ì‚é‚Ì‚É¬Œ÷‚µ‚½‚à‚Ì‚¾‚¯‚É—×ÚŠÖŒW‚ðì‚è‚½‚¢
	if(ch0index!=-1&&ch1index!=-1){
		RelateTwin(triBuffer[ch0index]->initialedge->next->myId,triBuffer[ch1index]->initialedge->prev->myId);
		
	}
	if(ch1index!=-1&&ch2index!=-1){
		RelateTwin(triBuffer[ch1index]->initialedge->next->myId ,triBuffer[ch2index]->initialedge->prev->myId);
		
	}
	if(ch2index!=-1&&ch0index!=-1){
		RelateTwin(triBuffer[ch2index]->initialedge->next->myId ,triBuffer[ch0index]->initialedge->prev->myId);
		
	}
	return childcount;
	


}
void Split4Triangle(int _r,int _pipj,CTriangle* _children[4]){
	//•Ó‚Ìã‚É“_‚ª‚ ‚Á‚ÄA‚»‚Ì•Ó‚ª—×ÚŽOŠpŒ`‚ðŽ‚Á‚Ä‚¢‚éê‡
	CVertex* pr=vertexBuffer[_r];
	
	CLine* pipj=lineBuffer[_pipj];
	CTriangle* parent=pipj->incidentTriangle;
	CTriangle* neighborTri=pipj->twin->incidentTriangle;
	
	int pi=pipj->begin->myId;
	int pk=pipj->next->end->myId;
	int pj=pipj->end->myId;
	int pl=pipj->twin->prev->begin->myId;
	int pjpkindex=pipj->next->myId;
	int pkpiindex=pipj->prev->myId;//Šù‘¶‚Ì—×ÚŠÖŒW‚ðŽ‚Á‚Ä‚¢‚é
	int piplindex=pipj->twin->next->myId;
	int plpjindex=pipj->twin->prev->myId;
	//“ú–{Œê”Åp236ŽQÆ
	//          pi
	//        ^| _
	//      ^ 2| 0 _
	//     pl___pr____pk
	//      _ 3| 1 ^
	//        _| ^
	//          pj
	//V‚µ‚­o—ˆ‚é•Ï‚Í‚W–{@ŽÌ‚Ä‚é•Ó2–{ ŠO‘¤‚ªiniedge‚É‚È‚é‚æ‚¤‚Éì‚é
	int ch0index=MakeTriangle(pk,pi,_r,pkpiindex);
	int childcount=0;
	if(ch0index!=-1){
		_children[childcount]=MakeChild(ch0index,parent);
		childcount++;
	}
	int ch1index=MakeTriangle(pj,pk,_r,pjpkindex);
	if(ch1index!=-1){
		_children[childcount]=MakeChild(ch1index,parent);
		childcount++;
	}
	//—×Ú‚ª‚È‚¢‚Æo—ˆ‚È‚¢‚Ù‚¤
	int ch2index=MakeTriangle(pi,pl,_r,piplindex);
	if(ch2index!=-1){
		_children[childcount]=MakeChild(ch2index,neighborTri);
		childcount++;
	}
	int ch3index=MakeTriangle(pl,pj,_r,plpjindex);
	if(ch3index!=-1){
		_children[childcount]=MakeChild(ch3index,neighborTri);
		childcount++;
	}
	if(focusr==_r){
		parent->PrintMyVertexInfo("e");
		neighborTri->PrintMyVertexInfo("e‚Q");
		for(int c=0;c<childcount;c++){_children[c]->PrintMyVertexInfo("split4‚Åo—ˆ‚½‚à‚Ì");}}
	//          pi
	//        ^| _
	//      ^ 2|  0_
	//     pl___pr____pk
	//      _ 3| 1 ^
	//        _| ^
	//          pj
	if(ch0index!=-1&&ch1index!=-1){RelateTwin(triBuffer[ch0index]->initialedge->prev->myId,triBuffer[ch1index]->initialedge->next->myId);}
	if(ch0index!=-1&&ch2index!=-1){RelateTwin(triBuffer[ch0index]->initialedge->next->myId ,triBuffer[ch2index]->initialedge->prev->myId);}
	if(ch2index!=-1&&ch3index!=-1){RelateTwin(triBuffer[ch2index]->initialedge->next->myId ,triBuffer[ch3index]->initialedge->prev->myId);}
	if(ch3index!=-1&&ch1index!=-1){RelateTwin(triBuffer[ch3index]->initialedge->next->myId ,triBuffer[ch1index]->initialedge->prev->myId);}
	
	
	
				
		
	

}
void Split2Triangle(int _r,int _pipj,CTriangle* _children[2]){
	//
	////•Ó‚Ìã‚É“_‚ª‚ ‚Á‚ÄA‚»‚Ì•Ó‚ª—×ÚŽOŠpŒ`‚ðŽ‚Á‚Ä‚¢‚È‚¢ê‡
	//CVertex* pr=vertexBuffer[_r];

	//
	//CLine* pipj=lineBuffer[_pipj];
	//CTriangle* _parent=pipj->incidentTriangle;
	//int pjpkindex=pipj->next->myId;
	//int pkpiindex=pipj->prev->myId;
	//CVertex* pi=pipj->begin;
	//CVertex* pk=pipj->next->end;
	//CVertex* pj=pipj->end;
	//
	//
	//lineBuffer[linecount]=new CLine(pi,pr,linecount);//pipr
	//int piprindex=linecount;
	//linecount++;
	//lineBuffer[linecount]=new CLine(pr,pk,linecount);//prpk
	//int prpkindex=linecount;
	//linecount++;
	//lineBuffer[linecount]=new CLine(pr,pj,linecount);//prpj
	//int prpjindex=linecount;
	//linecount++;
	//lineBuffer[linecount]=new CLine(pk,pr,linecount);//pkpr
	//int pkprindex=linecount;
	//linecount++;
	//
	////2‚Âì‚Á‚½‚çŽŸ‚Í—×Úî•ñ‚ð—^‚¦‚é
	//RelateTwin(prpkindex,pkprindex);
	//
	//
	//	MakeTriangle(piprindex,prpkindex,pkpiindex);
	//	_children[0]=MakeChild(lineBuffer[piprindex],_parent);
	//	
	//	MakeTriangle(prpjindex,pjpkindex,pkprindex);
	//	_children[1]=MakeChild(lineBuffer[pjpkindex],_parent);

	//	
	//

}
void CaseInside(CTriangle *_t,int _r){
				//ŽOŠpŒ`‚Ì“à‘¤‚É“_‚ª‚ ‚Á‚½ê‡
				int id=_t->initialedge->myId;
				int next=_t->initialedge->next->myId;
				int prev=_t->initialedge->prev->myId;
				CTriangle *children[3];
				int chnum=Split3Triangle(_r,_t,children);
				if(_r==focusr){
					printf("chnum=%d\n",chnum);
					children[0]->PrintMyVertexInfo("ch0");
				children[1]->PrintMyVertexInfo("ch1");
				children[2]->PrintMyVertexInfo("ch2");
				
					printf("ch0havetwin?%d\n",children[0]->initialedge->haveTwin);
					printf("ch1havetwin?%d\n",children[1]->initialedge->haveTwin);
					printf("ch2havetwin?%d\n",children[2]->initialedge->haveTwin);
				}
				//ŠO‘¤‚ªini‚É‚È‚é‚æ‚¤‚Éì‚Á‚Ä‚¢‚éB
				//          pk
				//        ^| _
				//      ^  |   _
				//    ^ 2  pr  1 _
				//  ^    ^0 _    _
				//pi____^____________pj
				if(_r==focusr){children[0]->initialedge->PrintLine("leganaize ch0");}
				LegalizeEdge(_r,children[0]->initialedge,children[0]);
				if(_r==focusr){children[1]->initialedge->PrintLine("leganaize ch1");}
				LegalizeEdge(_r,children[1]->initialedge,children[1]);
				if(_r==focusr){children[2]->initialedge->PrintLine("leganaize ch2");}		
				LegalizeEdge(_r,children[2]->initialedge,children[2]);

}
void CasOnEdge(CTriangle *_t,int _pipj,int _r){
	if(lineBuffer[_pipj]->haveTwin){
		CTriangle *children[4];
		Split4Triangle(_r,_pipj,children);
		
		LegalizeEdge(_r,children[0]->initialedge->prev,children[0]);//pkpi
		LegalizeEdge(_r,children[1]->initialedge->next,children[1]);//pjpk
		LegalizeEdge(_r,children[2]->initialedge->next,children[2]);//pipl
		LegalizeEdge(_r,children[3]->initialedge->prev,children[3]);//plpj
	}else{//•Ó‚Ìã‚É“_‚ª‚ ‚Á‚ÄA‚»‚Ì•Ó‚ª—×ÚŽOŠpŒ`‚ðŽ‚Á‚Ä‚¢‚È‚¢ê‡
		printf("™•Ó‚Ìã‚É“_‚ª‚ ‚Á‚ÄA‚»‚Ì•Ó‚ª—×ÚŽOŠpŒ`‚ðŽ‚Á‚Ä‚¢‚È‚¢ê‡™");
		//CTriangle *children[2];
		//Split2Triangle(_r,_pipj,children);
		//LegalizeEdge(_r,children[0]->initialedge->prev,children[0]);//pkpi
		//LegalizeEdge(_r,children[1]->initialedge->next,children[1]);//pjpk
	}
}
void TraverseTree2(CTriangle *_t,int _r,POINTSTATE _ptst,bool _debug){//whileƒo[ƒWƒ‡ƒ“

	while(1){
		if(_debug ||_r==focusr){
			_t->PrintMyVertexInfo("ƒgƒ‰ƒo[ƒXiwhile‚Ìæ“ªj");
			printf("childenum=%d ptst=%d\n",_t->childnum,_ptst);
		}

		if(_t->IsMyVertex(vertexBuffer[_r])){//’¸“__r‚ª_t‚ð‚È‚·’¸“_‚Æ“¯‚¶‚¾‚Á‚½ê‡‚±‚±‚àASSERT()
			_t->PrintMyVertexInfo("error’¸“__r‚ª_t‚ð‚È‚·’¸“_‚Æ“¯‚¶‚¾‚Á‚½ê‡ ‚ ‚Á‚Ä‚Í‚È‚ç‚È‚¢B‚È‚º‚È‚çA_r‚ÍV‚µ‚¢“_‚¾‚©‚çI");
			assert(!"’¸“__r‚ª_t‚ð‚È‚·’¸“_‚Æ“¯‚¶‚¾‚Á‚½ê‡ ‚ ‚Á‚Ä‚Í‚È‚ç‚È‚¢B‚È‚º‚È‚çA_r‚ÍV‚µ‚¢“_‚¾‚©‚çI");
			break;
		}else{//’¸“__r‚ª_t‚ð‚È‚·’¸“_‚Æ“¯‚¶‚¶‚á‚È‚©‚Á‚½ê‡
			if(_t->childnum<=0){//‚±‚ê‚Í—t‚Á‚Ï‚Å‚ ‚éê‡
				if(_ptst==INSIDE){	
					if(_debug||_r==focusr){_t->PrintMyVertexInfo("case inside");}
					//‚È‚ºonedge‚È‚Ì‚É‚±‚±‚É—ˆ‚Ä‚µ‚Ü‚¤‚Ì‚©H
					CaseInside(_t,_r);

					break;//_r‚ÉŠÖ‚·‚éˆ—‚ÍI‚í‚èB
				}else
				{//_t‚Í—t‚©‚Â_r‚ª•Ó‚Ìã‚É“_‚ª‚ ‚Á‚½ê‡B
					exceptsplit4[_r]=-1;
					
					switch(_ptst){
					case INIEDGE:
						//vertexBuffer[_r]->PrintVertex("r");
						//_t->CalcCircumCircle();
						//_t->circumCircle->center->PrintVertex("circum center");
						//_t->PrintMyVertexInfo("onini");
						CasOnEdge(_t,_t->initialedge->myId,_r);
						break;
					case NEXTEDGE:
						//_t->PrintMyVertexInfo("on next");
						CasOnEdge(_t,_t->initialedge->next->myId,_r);
						break;
					case PREVEDGE:
						//_t->PrintMyVertexInfo("on prev");
						CasOnEdge(_t,_t->initialedge->prev->myId,_r);
						break;
					}//switch‚ÌI‚í‚è
					////4‚ÂAor2‚ÂŽq‚ª¶‚Ü‚ê‚½B
					////’âŽ~ðŒ‚±‚êˆÈãÄ‹A‚µ‚È‚¢B
					break;//_r‚ÉŠÖ‚·‚éˆ—‚ÍI‚í‚èB
				}//•Ó‚Ìã‚É‚ ‚éê‡‚ÌI‚í‚è

			}else{//_t‚Í—t‚Å‚È‚©‚Á‚½ê‡
				int i;//=_t->childnum;
				if(_debug){_t->PrintMyVertexInfo("—t‚Å‚Í‚È‚¢");}
				POINTSTATE ptst;
				for(i=0;i<_t->childnum;i++){
					//ŽOŠpŒ`‚Éƒ}ƒCƒiƒXƒCƒ“ƒfƒbƒNƒX‚ðŠÜ‚Þê‡‚Í“Á•Ê‚Èˆ—‚ð‚·‚é
					if(_t->children[i]->a->trueId<0 || _t->children[i]->c->trueId<0 ||_t->children[i]->b->trueId<0){
						if(_r==focusr){_t->children[i]->PtInTriangleSpecial(vertexBuffer[_r],true);}
						ptst=_t->children[i]->PtInTriangleSpecial(vertexBuffer[_r],_debug);
					}else
					{//ŽOŠpŒ`‚ÌÀ•W‘S•”‚í‚©‚Á‚Ä‚éê‡‚Í•’Ê‚ÌŠô‰½Šw“IŒvŽZ‚Å“àŠO”»’è‚·‚é
						if(_r==focusr){_t->children[i]->PtInTriangle2(vertexBuffer[_r],true);}
						ptst=_t->children[i]->PtInTriangle2(vertexBuffer[_r],_debug);}
					
					if(ptst>0){//ŠO‘¤‚¶‚á‚È‚¢Žq‚ª‚Ý‚Â‚©‚Á‚½Žž“_‚Åcihldƒ‹[ƒv‚ð”²‚¯‚éB
						break;
					}

				}//childƒ‹[ƒv
				if(i<_t->childnum){//‰½”Ô–Ú‚É“à‘¤Žq‚ª‚Ý‚Â‚©‚Á‚½‚Ì‚©’m‚è‚½‚¢‚©‚çi‚ð•Û‘¶‚µ‚Ä‚¢‚é
						_ptst=ptst;
						if(_r==focusr){printf("%d”Ô–Ú‚ÉŒ©‚Â‚©‚è‚Ü‚µ‚½B",i);_t->children[i]->PrintMyVertexInfo("i am insider");}
					
						_t=_t->children[i];//ŽŸ‚Éwhile‚Ìæ“ª‚És‚­‚×‚«t‚ðƒZƒbƒg‚·‚é
						if(_debug){_t->PrintMyVertexInfo("find my tri");}
					
					}else{//Žq‚ð‚·‚×‚Ä’Tõ‚µ‚½‚¯‚Ç(i‚ð3‚Ü‚ÅƒCƒ“ƒNƒŠƒƒ“ƒg‚µ‚½‚¯‚Ç)OUTSIDE‚¾‚Á‚½ê‡ i‚ ‚Á‚Ä‚Í‚È‚ç‚È‚¢Bj
						//‚±‚±‚É—ˆ‚éðŒ‚Íi==3 ‚©‚Âchildnum==3‚Æ‚©‚Ìê‡
						//e‚ª"•Ó‚Ìã"‚¾‚Á‚½ê‡‚Í‚±‚±‚É—ˆ‚Ä‚à—Ç‚¢B
						if(_ptst>0){break;/*“_‚Í–³Ž‹‚³‚êAŽæ‚èŽc‚³‚ê‚éB*/}
						vertexBuffer[_r]->PrintVertex("_r");
						exceptsplit4[_r]=0;
						printf("še‚Ì’²¸š\n");
						if(_t->a->trueId<0 || _t->c->trueId<0 ||_t->b->trueId<0){
							ptst=_t->PtInTriangleSpecial(vertexBuffer[_r],true);
						}else{
							_t->PtInTriangle2(vertexBuffer[_r],true);}
						printf("šŽq‚Ì’²¸š\n");
						for(int c=0;c<_t->childnum;c++){
							if(_t->children[c]->a->trueId<0 || _t->children[c]->c->trueId<0 ||_t->children[c]->b->trueId<0){
								//_t->children[c]->PrintMyVertexInfo("ƒ}ƒCƒiƒXƒCƒ“ƒfƒbƒNƒXŠÜ‚Þ");
								ptst=_t->children[c]->PtInTriangleSpecial(vertexBuffer[_r],true);}else{
									ptst=_t->children[c]->PtInTriangle2(vertexBuffer[_r],true);}
						}
						_t->PrintMyVertexInfo("Žq‚ð‚·‚×‚Ä’Tõ‚µ‚½‚¯‚ÇOUTSIDE‚¾‚Á‚½ê‡ ??\n");
						assert(!"Žq‚ð‚·‚×‚Ä’Tõ‚µ‚½‚¯‚ÇOUTSIDE‚¾‚Á‚½ê‡ i‚ ‚Á‚Ä‚Í‚È‚ç‚È‚¢Bj");//‚±‚Ì’†‚Éâ‘Î¬‚è—§‚ÂðŒŽå’£‚·‚éB¬‚è—§‚½‚È‚©‚Á‚½‚çAƒGƒ‰[
						//ASSERT(1)³‰ð@ASSERT(0)•s³‰ð@‚»‚±‚Å’âŽ~‚·‚é
						break;
					}//‰½”Ô–Ú‚É“à‘¤Žq‚ª‚Ý‚Â‚©‚Á‚½‚©‚Ç‚¤‚©


				//_t‚â_ptst‚ÌŒ`‚ð•Ï‚¦Awhileƒ‹[ƒv‚ÌÅ‰‚É–ß‚éB

			}//end _t‚Í—t‚©‚Ç‚¤‚©

		}//end ’¸“__r‚ª_t‚ð‚È‚·’¸“_‚Æ“¯‚¶‚©‚Ç‚¤‚©
	}//end while loop

}
bool ExistInTriangleBuffer(CTriangle *_t){
	for(int i=0;i<finaltriangle;i++){
		if(*finalTriangle[i]==*_t){return true;}

	}
	return false;
}
void MakeVertics(int _id,int _trueid){
	vertexBuffer[_id]=new CVertex(inputdata[_trueid][0],inputdata[_trueid][1],_id);
	vertexBuffer[_id]->z=inputdata[_trueid][2];
	vertexBuffer[_id]->trueId=_trueid;
	//ƒ‰ƒ“ƒ_ƒ€‚É’¸“_‚ð“ü‚ê‚È‚ª‚çAmax,min,‚ð‘ª’è‚·‚é maxdata.y‚É‚Íid‚ð“ü‚ê‚Ä‚¨‚­iŒã‚Å‰ŠúŽOŠpŒ`‚ÉŽg‚¤‚©‚çj
	if(maxdata.x<inputdata[_trueid][0]){maxdata.x=inputdata[_trueid][0];rightmostindex=_trueid;}
	if(mindata.x>=inputdata[_trueid][0]){mindata.x=inputdata[_trueid][0];}	   
	  //step1 (p200)
	//Let p0 be the lexicographically highest point of P, that is,the rightmost among the points with largest y-coordinate
	//‚Ü‚¸yÀ•W‚Åˆê”Ôã‚Ì‚ð‘I‚ÔB
	  if(maxdata.y<=inputdata[_trueid][1]){ 
		  if(maxdata.y==inputdata[_trueid][1]){
		   //‚à‚µˆê”Ôã‚ª2ŒÂˆÈã‚ ‚Á‚½‚çAˆê”Ô‰E‘¤ixÀ•W’l‚ª‘å‚«‚¢‚à‚Ìj‚ð‘I‚ÔB
					  if(vertexBuffer[maxdata.myId]->x<inputdata[_trueid][0]){
							//V‚µ‚¢•û‚ª¶‘¤‚¾‚Á‚½‚ç
							 maxdata.y=inputdata[_trueid][1];
							 maxdata.myId=_id;
							 maxdata.trueId=_trueid;
						}//Šù‘¶‚Ìymax‚Ì•û‚ª¶‘¤‚¾‚Á‚½‚çÅ‘å’lXV‚µ‚È‚¢B
		 }//end y==
		  else{//y<input.y
			maxdata.y=inputdata[_trueid][1];
			maxdata.myId=_id;
			maxdata.trueId=_trueid;
		  }
	  }
	  
	  //maxdata.PrintVertex("every maxdata");
	  //vertexBuffer[maxdata.myId]->PrintVertex("ymax vbuf");
	  if(mindata.y>=inputdata[_trueid][1]){
		   //‚à‚µˆê”Ô‰º‚ª2ŒÂˆÈã‚ ‚Á‚½‚çAˆê”Ô¶‘¤ixÀ•W’l‚ª¬‚³‚¢‚³‚¢‚à‚Ìj‚ð‘I‚ÔB
			if(mindata.y==inputdata[_trueid][1]){		 
				  if(vertexBuffer[mindata.myId]->x>inputdata[_trueid][0]){
									//V‚µ‚¢•û‚ª‰E‘¤‚¾‚Á‚½‚ç
									 mindata.y=inputdata[_trueid][1];
									 mindata.myId=_id;
									 mindata.trueId=_trueid;
								}//Šù‘¶‚Ìymax‚Ì•û‚ª‰E‘¤‚¾‚Á‚½‚çÅ‘å’lXV‚µ‚È‚¢B
			 } else{
						  mindata.y=inputdata[_trueid][1];
						  mindata.myId=_id;
						  mindata.trueId=_trueid;//p1‚ð‘I‚Ô‚½‚ß‚Éminindex‚ð•Û‘¶‚µ‚Ä‚¨‚­B
					  }
	  }
	  if(maxdata.z<inputdata[_trueid][2]){maxdata.z=inputdata[_trueid][2];}
	  if(mindata.z>inputdata[_trueid][2]){mindata.z=inputdata[_trueid][2];}
	 // vertexBuffer[_id]->PrintVertex("every");
	  
	 
}
void SetVertics(){
	////‡”Ô‚Ç‚¨‚è“_‚ð“ü‚ê‚éƒo[ƒWƒ‡ƒ“
	//for(int i=0;i<pointnum;i++){
	//	MakeVertics(i,i);
	//}
	//‚Ü‚ñ‚×‚ñ‚È‚­ŽU‚ç‚·ƒo[ƒWƒ‡ƒ“iPOINTNUM‚ª4‚Ì”{”‚Å‚È‚¯‚ê‚Î‚È‚ç‚È‚¢j‚Å‚È‚¢‚ÆAÅŒã‚Ì‚Ù‚¤‚Ì“_‚ðE‚Á‚Ä‚­‚ê‚È‚¢
	////‚»‚êˆÈ~‚ÍPOINTNUM/4‚¸‚Â‚Æ‚Î‚µ‚Å
	//‚S‚Ì”{”‚Å‚È‚¯‚ê‚Î‚¢‚¯‚È‚¢A‚Ì‚Í’¼‚¹‚é‚©‚àB
	int offset=0;
	int devidable4=(pointnum/4)*4;//4‚Ì”{”
	while(offset<pointnum/4){
		MakeVertics(offset*4,offset+devidable4/2);
		MakeVertics(offset*4+1,offset);
		MakeVertics(offset*4+2,offset+devidable4*3/4);
		MakeVertics(offset*4+3,offset+devidable4/4);
		offset++;
	}
	int amari=pointnum%4;
	if(amari!=0){//pointnum‚ª4‚Ì”{”‚È‚È‚©‚Á‚½‚çAÅŒã‚Ì‚Ù‚¤‚ÌŽc‚Á‚½“_‚ð“K“–‚É“ü‚ê‚é
		int notouch=4*(pointnum/4);//notouch`pointnum‚Ü‚Å‚Íƒm[ƒ^ƒbƒ`—Ìˆæ
		for(int i=0;i<amari;i++){
		MakeVertics(notouch+i,notouch+i);
		}
	}
	//‚·‚×‚ÄI‚í‚Á‚½‚çA“_‚Ì”ÍˆÍ‚ð‹‚ß‚é
	range=maxdata-mindata;
	/*range.PrintVertex("range");
	maxdata.PrintVertex("maxdata");
	mindata.PrintVertex("mindata");
	for(int i=0;i<pointnum;i++){
		vertexBuffer[i]->PrintVertex("init");
	}
		*/
		
	

}
void SwapAandB(int _a,int _b){//a‚Æb‚ðŒðŠ·
	CVertex* temp=vertexBuffer[_a];
	
	int astrueid=vertexBuffer[_a]->trueId;
	
	vertexBuffer[_a]=vertexBuffer[_b];
	vertexBuffer[_a]->myId=_a;
	vertexBuffer[_a]->trueId=vertexBuffer[_b]->trueId;

	vertexBuffer[_b]=temp;
	vertexBuffer[_b]->myId=_b;
	vertexBuffer[_b]->trueId=astrueid;

}
void CalcRootTriangleNew(){//‘æ3”Å‚Ìƒo[ƒWƒ‡ƒ“‚Å‚Ì‰ŠúŽOŠpŒ`‚Ìì‚è•ûB
	//‚±‚±‚É—ˆ‚éŽž“_‚ÅvertexBuffer[0-pointnum]‚É’l‚ª“ü‚Á‚Ä‚¢‚é
	//http://www.cs.uu.nl/geobook/interpolation.pdf 
	//p199
	//We will add two extra points p-1,p-2 that,together with the highest point p0 of P,
	//p204
	//‚±‚¤‚¢‚¤‰ŠúŽOŠpŒ`‚ÌÝ’è
	//p-2A
	// _@_
	//   _   _p0
	//     _:::|
	//@@@@_:|
	//         R|
	//          p-1
	//p0‚Ìì¬
	if(maxdata.myId!=0){
		SwapAandB(maxdata.myId,0);
	}
	if(mindata.myId==0){/*‚à‚µª‚Ì“ü‚ê‘Ö‚¦æ‚ªmindata‚¾‚Á‚½‚çp1‚ðì‚é‚Æ‚«¢‚é‚Ì‚Å*/mindata.myId=maxdata.myId;}
	//swapI—¹
	vertexBuffer[pointnum]=new CVertex();//-DBL_MAX,DBL_MAX);
	vertexBuffer[pointnum]->trueId=-2;
	vertexBuffer[pointnum]->myId=pointnum;
	vertexBuffer[pointnum+1]=new CVertex();//DBL_MAX,-DBL_MAX);
	vertexBuffer[pointnum+1]->trueId=-1;
	vertexBuffer[pointnum+1]->myId=pointnum+1;
	//0 -> -2  -> -1 p199,p204
	
	//0 to -2
	lineBuffer[linecount]=new CLine(vertexBuffer[0],vertexBuffer[pointnum],linecount);
	int begin=linecount;
	linecount++;
	//-2 to -1
	lineBuffer[linecount]=new CLine(vertexBuffer[pointnum],vertexBuffer[pointnum+1],linecount);
	int next=linecount;
	linecount++;
	//-1 to 0
	lineBuffer[linecount]=new CLine(vertexBuffer[pointnum+1],vertexBuffer[0],linecount);
	int end=linecount;
	linecount++;
	ConnectTriangleLines(begin,next,end);
	triBuffer[tcount]=new CTriangle(lineBuffer[begin],tcount);
	tcount++;
	
	//p1‚Íˆê”Ô‰º‚Ì‚â‚Â‚É‚·‚éi–{‚É‚Í‘‚¢‚Ä‚¢‚È‚¢‚ªA‚±‚Ì•û‚ª“s‡‚ª‚æ‚¢‚©‚È‚ÆŽv‚Á‚½B“_ŒQ‚ð‚Ü‚Á‚Õ‚½‚Â‚É•ª‚¯‚ê‚»‚¤‚¾‚©‚çBj
	if(mindata.myId!=1){
		SwapAandB(mindata.myId,1);
	}
	
}
void CalcRootTriangle(){//‚·‚×‚Ä‚Ì“_‚ðŠÜ‚Þ‚æ‚¤‚È‘å‚«‚ÈŽOŠpŒ`‚ðì‚é
	
	//M=range/2;
	//“_‚Ì’†Smin+range/2
	 M=range/2.0;
	origin=mindata+M;
	
	vertexBuffer[pointnum+0]=new CVertex(origin.x+3*M.x,origin.y,pointnum+1);//-1=(3M.0)
	vertexBuffer[pointnum+1]=new CVertex(origin.x,origin.y+3*M.y,pointnum+2);//-2=(0,3M)
	vertexBuffer[pointnum+2]=new CVertex(origin.x-3*M.x,origin.y-3*M.y,pointnum+3);//-3=(-3M,3M)//
	vertexBuffer[pointnum+0]->trueId=-1;
	vertexBuffer[pointnum+1]->trueId=-2;
	vertexBuffer[pointnum+2]->trueId=-3;
	//”½ŽžŒvŽü‚è‚É-1¨-2¨-3‚Æ‚µ‚½

	
	//•Ó‚ðŽO‚Âì‚é
	//-1 to -2
	lineBuffer[linecount]=new CLine(vertexBuffer[pointnum+0],vertexBuffer[pointnum+1],linecount);
	linecount++;
	//-2 to -3
	lineBuffer[linecount]=new CLine(vertexBuffer[pointnum+1],vertexBuffer[pointnum+2],linecount);
	linecount++;
	//-3 to -1
	lineBuffer[linecount]=new CLine(vertexBuffer[pointnum+2],vertexBuffer[pointnum+0],linecount);
	linecount++;
	
	ConnectTriangleLines(0,1,2);	
	triBuffer[0]=new CTriangle(lineBuffer[0],0);
	tcount=1;
	
	
	
	
}
void CheckVertexDrop(){
	for(int i=0;i<pointnum;i++){
		if(exceptsplit4[i]==0){
			droppedvertexnum++;}
	}
	printf("Žæ‚è‚à‚ç‚µ’¸“_=%d\n",droppedvertexnum);
}
void CheckCircumCircle(){
	 illegalVert = new int[pointnum+2];
	illegalTri=new int[finaltriangle];
	for(int i=0;i<pointnum+2;i++){
		illegalVert[i]=0;
	}
	for(int i=0;i<finaltriangle;i++){
		illegalTri[i]=0;
	}
	int illegalptnum=0;
	int illegaltrinum=0;
	int section=finaltriangle/100;
	if(section==0){section=1;}
	for(int i=0;i<finaltriangle;i++){
		bool inside=false;
		for(int j=0;j<pointnum;j++){
			if(finalTriangle[i]->IsInMyCircumCircle2(vertexBuffer[j])){
				
				if(exceptsplit4[i]!=-1){//split4‚Ì‚¹‚¢‚Åinside‚É‚È‚Á‚Ä‚µ‚Ü‚Á‚½‚à‚Ì‚ÍÈ‚­
					inside=true;
					illegalVert[j]++;
					illegalptnum++;
					
				}//split4‚Ì‚¹‚¢‚Åinside‚É‚È‚Á‚Ä‚µ‚Ü‚Á‚½‚à‚Ì‚ÍÈ‚­	
			}//ŠOÚ‰~ƒeƒXƒg
		}//j
		if(inside){illegalTri[i]++;illegaltrinum++;printf("illegaltrinum=%d@\r",illegaltrinum);}
		
				int section=(double)finaltriangle/100.0;
				if(section==0){section=1;}
					if(i%section==0){//isó‹µ‚Ì•\Ž¦
						printf("illegalptnum=%d@\r",illegalptnum);
						int progress=(double)i/(double)finaltriangle*100;
							printf("ŠOÚ‰~ƒ`ƒFƒbƒNisó‹µ%d“ \r", progress);
					}
	}//i
	/*ofstream out;
	stringstream outfilename;
	outfilename<<"C:\\kaijo\\illegalTri.txt";
	out.open(outfilename.str().c_str(),std::ios::trunc);
	for(int i=0;i<finaltriangle;i++){
		out<<illegalTri[i]<<endl;
	}
	out.close();
	outfilename.str("");
	outfilename<<"C:\\kaijo\\illegalVert"<<pointnum<<".txt";
	out.open(outfilename.str().c_str(),std::ios::trunc);
	for(int i=0;i<pointnum;i++){
		out<<illegalVert[i]<<endl;
	}
	out.close();
	printf("ŠOÚ‰~‚Ì‚È‚©‚É‚ ‚é“_‚Ì”%d\n",illegalptnum);*/
}
void MakeOutPutData(){
	int progress=0;
	section=tcount/100;
	if(section==0){section=1;}
	finaltriangle=0;
	int illegaltrinum=0;
	int skipnum=0;
	for(int i=0;i<tcount;i++){	
		if(triBuffer[i]->childnum==0){//—t‚È‚ç‚Î
			if(triBuffer[i]->IsMyVertex(vertexBuffer[pointnum+0]) 
				|| triBuffer[i]->IsMyVertex(vertexBuffer[pointnum+1])  
				/*|| triBuffer[i]->IsMyVertex(vertexBuffer[pointnum+2])*/ ){
			//‰ŠúŽOŠpŒ`‚Ì’¸“_‚É•t‚µ‚½•Ó‚ðŽæ‚èœ‚­ì‹ÆB	
					//nothing 
					skipnum++;
			}else{
				finalTriangle[finaltriangle]=triBuffer[i];
				finaltriangle++;
			}
		}//end leaf
		if(i%section==0){//isó‹µ‚Ì•\Ž¦
			int progress=((double)i/(double)tcount)*100.0;
			printf("MakeOutPutDataisó‹µ%d“ \r", progress);
		}
	}
	printf("d•¡‚·‚éŽOŠpŒ` %d\n",illegaltrinum);
	printf("skipnum=%d\n",skipnum);
	
	

	


}
void DeleteOutData(OUTDATA& _outdata){
	 
	for(int i=0;i<finaltriangle*3;i++){
		delete[] _outdata.vertex[i];
	}
	delete [] _outdata.vertex;
}
void SetOutData(){
	ZeroMemory(&out_data,sizeof(OUTDATA));//data={0};
	 out_data.size=finaltriangle*3;//o—Í’¸“_‚Ì”
	 out_data.vertex=new double*[finaltriangle*3];
	for(int i=0;i<finaltriangle*3;i++){
		out_data.vertex[i]=new double[3];
	}
		for(int i=0;i<finaltriangle;i++){
		//“ü—ÍŽž‚Íƒ‰ƒ“ƒ_ƒ€‚ÉŽU‚ç‚µ‚½‚Ì‚ÅAŒ³‚É–ß‚·‚Æ‚«‹C‚ð‚Â‚¯‚éB
		CTriangle *_t =finalTriangle[i];
		int indexa=_t->a->trueId;
		out_data.vertex[i*3][0]=inputdata[indexa][0];
		out_data.vertex[i*3][1]=inputdata[indexa][1];
		out_data.vertex[i*3][2]=inputdata[indexa][2];
		
		int indexb=_t->b->trueId;
		out_data.vertex[i*3+1][0]=inputdata[indexb][0];
		out_data.vertex[i*3+1][1]=inputdata[indexb][1];
		out_data.vertex[i*3+1][2]=inputdata[indexb][2];
		int indexc=_t->c->trueId;
		out_data.vertex[i*3+2][0]=inputdata[indexc][0];
		out_data.vertex[i*3+2][1]=inputdata[indexc][1];
		out_data.vertex[i*3+2][2]=inputdata[indexc][2];
		if(i%section==0){//isó‹µ‚Ì•\Ž¦
			int progress=((double)i/(double)finaltriangle)*100.0;
			//printf("isó‹µ%d“ \r", progress);
		}
	}
	 //DeleteArrays();//finalTriangleˆÈŠO‚Ì‚à‚Ì‚ð•Ð•t‚¯‚é
	
}
void SetOutDataVert(){
	outVertex = new CVertex[pointnum];
		for(int i=0;i<finaltriangle;i++){
		//“ü—ÍŽž‚Íƒ‰ƒ“ƒ_ƒ€‚ÉŽU‚ç‚µ‚½‚Ì‚ÅAŒ³‚É–ß‚·‚Æ‚«‹C‚ð‚Â‚¯‚éB
		CTriangle *_t =finalTriangle[i];
		int indexa=_t->a->trueId;
		outVertex[i*3]=*vertexBuffer[indexa];
		
		int indexb=_t->b->trueId;
		outVertex[i*3+1]=*vertexBuffer[indexb];
		int indexc=_t->c->trueId;
		outVertex[i*3+2]=*vertexBuffer[indexc];
		if(i%section==0){//isó‹µ‚Ì•\Ž¦
			int progress=((double)i/(double)finaltriangle)*100.0;
			printf("isó‹µ%d“ \r", progress);
		}
	}
	 //DeleteArrays();//finalTriangleˆÈŠO‚Ì‚à‚Ì‚ð•Ð•t‚¯‚é
	
}
void InsertNewPoint(int _r){//ƒfƒoƒbƒO—p@ƒƒ“ƒXƒeƒbƒv‚¸‚ÂŒ©‚½‚¢‚Æ‚«‚ÉŽg‚¤B
	TraverseTree2(triBuffer[0],_r,INSIDE,false);
}
template <typename T>
void SetUpData(miffy::vec3<T>* _data,int _pointnum){
	pointnum=_pointnum;
	ArrayInit();
	
	inputdata = new double*[pointnum];
	for(int i=0;i<pointnum;i++){
		inputdata[i]=new double[3];
			inputdata[i][0]=(double)_data[i].x;
			inputdata[i][1]=(double)_data[i].y;
			inputdata[i][2]=(double)_data[i].z;
		
	}
	SetVertics( );//mina‚âmax‚à‹‚ß‚éB“_‚ðƒ‰ƒ“ƒ_ƒ€‚É“ü‚ê‚éB‚È‚Ç‚Ìì‹Æ‚ð‚·‚éB
	CalcRootTriangleNew();//‚·‚×‚Ä‚Ì“_‚ð•ïŠ‡‚·‚éu‰Šú‰»ŽOŠpŒ`v‚ðì‚é
}
void SetUpData(double _data[][3],int _pointnum){
	pointnum=_pointnum;
	ArrayInit();
	
	inputdata = new double*[pointnum];
	for(int i=0;i<pointnum;i++){
		inputdata[i]=new double[3];
		for(int j=0;j<3;j++){
			inputdata[i][j]=_data[i][j];
			
		}
	}
	SetVertics( );//mina‚âmax‚à‹‚ß‚éB“_‚ðƒ‰ƒ“ƒ_ƒ€‚É“ü‚ê‚éB‚È‚Ç‚Ìì‹Æ‚ð‚·‚éB
	CalcRootTriangleNew();//‚·‚×‚Ä‚Ì“_‚ð•ïŠ‡‚·‚éu‰Šú‰»ŽOŠpŒ`v‚ðì‚é
}
void SetUpData(CVertex* _data,int _pointnum){
	pointnum=_pointnum;
	ArrayInit();
	inputdata = new double*[pointnum];
	for(int i=0;i<pointnum;i++){
		inputdata[i]=new double[3];
		
			inputdata[i][0]=_data[i].x;
			inputdata[i][1]=_data[i].y;
			inputdata[i][2]=_data[i].z;
		
	}
	SetVertics( );
	CalcRootTriangleNew();//‚·‚×‚Ä‚Ì“_‚ð•ïŠ‡‚·‚éu‰Šú‰»ŽOŠpŒ`v‚ðì‚é
}
void SetUpData(double **_data,int _pointnum){
	pointnum=_pointnum;
	ArrayInit();
	inputdata = new double*[pointnum];
	for(int i=0;i<pointnum;i++){
		inputdata[i]=new double[3];
		for(int j=0;j<3;j++){
			inputdata[i][j]=_data[i][j];	
		}
		
	}
	SetVertics( );
	CalcRootTriangleNew();//‚·‚×‚Ä‚Ì“_‚ð•ïŠ‡‚·‚éu‰Šú‰»ŽOŠpŒ`v‚ðì‚é
}
void DeleteArrays(){
	for(int i=0;i<pointnum;i++){ delete vertexBuffer[i];}
	for(int i=0;i<tcount;i++){ delete  triBuffer[i];}
	for(int i=0;i<linecount;i++){  delete lineBuffer[i];}
}
OUTDATA Delaunay(double **_data,int _pointnum){
	printf("SetUpData\n");
	SetUpData(_data,_pointnum);
	printf("Delaunay ŒvŽZ’†\n");
	LARGE_INTEGER before,after,freq;
	QueryPerformanceCounter(&before);
	
	for(int r=1;r<pointnum;r++){
	//printf("\ninsert %d\n",r);
	
		TraverseTree2(triBuffer[0],r,INSIDE,true);//Å‰‚É‘}“ü‚·‚é“_‚Í“à‘¤‚ÉŒˆ‚Ü‚Á‚Ä‚¢‚é‚Ì‚ÅINSIDE‚É‚µ‚½
		
	}
	QueryPerformanceCounter(&after);
	QueryPerformanceFrequency(&freq);
	DWORD time = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);
	printf("ŒvŽZ‚É‚©‚©‚Á‚½ŽžŠÔ[%d]’¸“_%dƒ~ƒŠƒZƒJƒ“ƒh\n",pointnum,time);
	
	printf("Žæ‚è‚à‚ç‚µ‚½“_‚Ì”%dŒÂ\n",droppedvertexnum);
	MakeOutPutData();//out_data‚É’l‚ð“ü‚ê‚é
	//ƒ`ƒFƒbƒNƒ‹[ƒ`ƒ“
	//CheckCircumCircle();
	//CheckVertexDrop();
	SetOutData();//out_data‚É’l‚ð“ü‚ê‚é
	DeleteArrays();
	
	
	return out_data;
}
template <typename T>
OUTDATA Delaunay(miffy::vec3<T>* _data,int _pointnum){
	SetUpData(_data,_pointnum);
	LARGE_INTEGER before,after,freq;
	QueryPerformanceCounter(&before);
	
	for(int r=1;r<pointnum;r++){
		//printf("\ninsert %d trueId=%d\n",r,vertexBuffer[r]->trueId);
		TraverseTree2(triBuffer[0],r,INSIDE,false);//Å‰‚É‘}“ü‚·‚é“_‚Í“à‘¤‚ÉŒˆ‚Ü‚Á‚Ä‚¢‚é‚Ì‚ÅINSIDE‚É‚µ‚½
		
	}
	QueryPerformanceCounter(&after);
	QueryPerformanceFrequency(&freq);
	DWORD time = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);
	printf("ŒvŽZ‚É‚©‚©‚Á‚½ŽžŠÔ[%d]’¸“_%dƒ~ƒŠƒZƒJƒ“ƒh\n",pointnum,time);
	
	MakeOutPutData();
	//ƒ`ƒFƒbƒNƒ‹[ƒ`ƒ“
	// CheckCircumCircle();
	// CheckVertexDrop();
	SetOutData();//out_data‚É’l‚ð“ü‚ê‚é
	DeleteArrays();
	
	return out_data;
}
OUTDATA Delaunay(double _data[][3],int _pointnum){
	SetUpData(_data,_pointnum);
	LARGE_INTEGER before,after,freq;
	QueryPerformanceCounter(&before);
	
	for(int r=1;r<pointnum;r++){
		printf("\ninsert %d trueId=%d\n",r,vertexBuffer[r]->trueId);
		TraverseTree2(triBuffer[0],r,INSIDE,true);//Å‰‚É‘}“ü‚·‚é“_‚Í“à‘¤‚ÉŒˆ‚Ü‚Á‚Ä‚¢‚é‚Ì‚ÅINSIDE‚É‚µ‚½
		
	}
	QueryPerformanceCounter(&after);
	QueryPerformanceFrequency(&freq);
	DWORD time = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);
	printf("ŒvŽZ‚É‚©‚©‚Á‚½ŽžŠÔ[%d]’¸“_%dƒ~ƒŠƒZƒJƒ“ƒh\n",pointnum,time);
	
	MakeOutPutData();
	//ƒ`ƒFƒbƒNƒ‹[ƒ`ƒ“
	 CheckCircumCircle();
	 CheckVertexDrop();
	SetOutData();//out_data‚É’l‚ð“ü‚ê‚é
	DeleteArrays();
	
	return out_data;
}
