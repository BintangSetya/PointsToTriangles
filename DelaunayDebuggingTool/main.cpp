#include <windows.h>
#include <string>
#include <stdio.h>
#include <gl/freeglut.h>
#include <string>
#include <iostream>
#include<sstream>
#include <iomanip>
#include <fstream>
#include <float.h>
#include "../delaunay.h"
#include "TriangleDebug.h"
#include <miffy/math/vec3.h>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#define FILENAME "../../data/pointlist.txt"
//#define FILENAME "../../data/42_3D.dat"
int POINTNUM;
#define KEINIT 1//20000
#define BEGIN 0

//縦横比1:1じゃないと変になる。
int focusv=24;
int displayW=1024;  
int displayH=768;
#define POINTSIZE 1
#define AREA 0.19
double onepixel=1.0/displayW;
using namespace std;
int ke=KEINIT;//0;//1592;//POINTNUM;//0;
int nearesttri=0;
int nearestpt=0;
class CCircle;
class CVertex;
class CTriangle;
class CTriangleDebug;
class CVertexDebug;
//時間計測用
LARGE_INTEGER  before;
LARGE_INTEGER  after;
LARGE_INTEGER  freq;
CVertex **myvertexBuffer;//[POINTNUM+3];//ファイルから読み込んだ点群
OUTDATA myoutdata;
bool animation=false;
bool showdebuginfo=false;
bool showvertnum=false;
bool runsweepline=false;
bool bottundown=false;
float background[3]={0.0,0.0,0.0};//{0.47,0.83,0.85};//背景色
float speed=2.0;
float space=1.0;
float eyePoint[3]={0.0,0.0,space*2.0};
//画面出力用フォント
void *font = GLUT_BITMAP_HELVETICA_10;
void *midfont = GLUT_BITMAP_HELVETICA_12;
void *largefont = GLUT_BITMAP_HELVETICA_18;
stringstream tris;
miffy::vec3<int>* input;//読み込み用
typedef struct tagvertex{//ファイルから頂点を読み込むときに使う
	miffy::vec3<float> *vert;//[POINTNUM][3];
	miffy::vec3<float> max;
	miffy::vec3<float> min;
	miffy::vec3<float> range;
}VERTEX,*PVERTEX;
VERTEX data;
class CTriangle;
void display();
CVertex areav(1.8,1.8);
CVertex offsetv(0.0,0.0);
double scale=1.0;
double *zvalue;//[POINTNUM];色に使うのかな。
CVertexDebug* showvert;//[POINTNUM+3];
/*CVertexDebug vertgroup[PTNUM*PTNUM];//点群*/
CTriangleDebug *tri;//[(9*POINTNUM+3)*3];//ここでエラー
int *idBuf;//[(9*POINTNUM+3)*3];

ofstream out;
bool buttondown=false;
int history=0;
double nodesize=30;
double nodey=onepixel*300;
double transy=0;
double transx=0;
double pushx=0;
double pushy=0;
void MyArrayInit(){
	input=new miffy::vec3<int>[POINTNUM+BEGIN];
	data.vert = new miffy::vec3<float>[POINTNUM+BEGIN];
	zvalue=new double[POINTNUM];
	showvert=new CVertexDebug [POINTNUM+3];
	tri=new CTriangleDebug[(9*POINTNUM+3)*3];//ここでエラー
	idBuf=new int[(9*POINTNUM+3)*3];
	myvertexBuffer=new CVertex*[POINTNUM+3];//ファイルから読み込んだ点群
}
void NormalizeVertex(){	
	offsetv=areav*(-0.5);
	for(int i=0;i<POINTNUM+2;i++){	
		showvert[i].trueId=vertexBuffer[i]->trueId;
		showvert[i].myId=i;
		if(i>=POINTNUM){
			showvert[i].trueId=-(i-POINTNUM);
		}
		showvert[i].Set(vertexBuffer[i],&areav,&offsetv);
		//if(i<100)
			//showvert[i].PrintVertex("showvert");
	}
}
void ReadBinary(){
	char buf[MAXTEXT];
	ZeroMemory(&data,sizeof(VERTEX));//data={0};
		data.max=-FLT_MAX;//0x0;
		data.min=FLT_MAX;//0xffffffffffffffff;//fが16個
	
	FILE *fp;
	fopen_s(&fp,FILENAME,"rb"); // 読み取り専用で開く
	if(fp==NULL){assert(!"ファイルが存在しません。");}
	fseek(fp,0,SEEK_END);//ファイル終端から+0ファイルポインタの位置を移動させる
	int filebytes=ftell(fp);//現在のファイルポインタの位置を返す
	POINTNUM=20000;//filebytes/sizeof(float)/3;
	printf("頂点の数%d\n",POINTNUM);
	MyArrayInit();
	rewind(fp);//ファイルポインタを最初に戻す
	char* ret;
	//xyが同じ座標のものを除去する
		int counter=0;
		bool sameflag=false;
	for(int i=0;i<POINTNUM+BEGIN;i++){
		fread(&input[i].x,sizeof(miffy::vec3<int>),1,fp);
		sameflag=false;
		for(int j=0;j<i;j++){
			if((data.vert[j].x==(float)input[i].x) && (data.vert[j].y==(float)input[i].y) )
			{//もし同じ座標があったら
				sameflag=true;
				
			}
		}
		if(!sameflag){
			data.vert[counter].x=(float)input[i].x;
			data.vert[counter].y=(float)input[i].y;
			data.vert[counter].z=(float)input[i].z;
			if(data.vert[counter].x<0){
			printf("[%d]%f,%f,%f\n",i,data.vert[counter].x,data.vert[counter].y,data.vert[counter].z);
			assert(!"stop!!");
		}
			//max-minはbeginindexで指定された範囲のみ測定する
		if(i>=BEGIN){
			if(data.max.x<data.vert[counter].x){data.max.x=data.vert[counter].x;}
			if(data.min.x>data.vert[counter].x){data.min.x=data.vert[counter].x;}	   
			if(data.max.y<data.vert[counter].y){data.max.y=data.vert[counter].y;}
			if(data.min.y>data.vert[counter].y){data.min.y=data.vert[counter].y;}
			if(data.max.z<data.vert[counter].z){data.max.z=data.vert[counter].z;}
			if(data.min.z>data.vert[counter].z){data.min.z=data.vert[counter].z;}
		}
		
			counter++;
		}
		
	}
	printf("重複を取り除いた後の点の数%d\n",counter);
	data.range=data.max-data.min;
	fclose(fp);
	//初期化三角形の頂点以外のすべての頂点が画面内におさまるように直す時に必要な、点群の範囲、min,maxを計算。
	//dataにはいっぱい入っているが、beginIndex-beginIndex+POINTNUMの範囲だけ取り出す
	printf("rangeX:%f,min%f,max%f\n",data.range.x,data.min.x,data.max.x);
	printf("rangeY:%f,min%f,max%f\n",data.range.y,data.min.y,data.max.y);
	printf("rangeZ:%f,min%f,max%f\n",data.range.z,data.min.z,data.max.z);
	POINTNUM=counter;
	//0-1に正規化 areaとoffsetがどんな値か考えるのが大変だったから正規化した。
	for(int i=0;i<POINTNUM;i++){
		data.vert[i].x=(data.vert[i].x-data.min.x)/data.range.x;
		data.vert[i].y=(data.vert[i].y-data.min.y)/data.range.y;
		zvalue[i]=(data.vert[i].z-data.min.z)/data.range.z;//0-1に正規化
	}
}
void ReadTXT(){

	ZeroMemory(&data,sizeof(VERTEX));//data={0};
	data.max=-FLT_MAX;//0x0;
	data.min=FLT_MAX;//0xffffffffffffffff;//fが16個
	data.vert = new miffy::vec3<float>[POINTNUM+BEGIN];
	FILE *fp;
	fp=fopen(FILENAME,"r"); // 読み取り専用で開く
	if(fp==NULL){assert(!"ファイルが存在しません。");}
	fseek(fp,0,SEEK_END);//ファイル終端から+0ファイルポインタの位置を移動させる
	int filebytes=ftell(fp);//現在のファイルポインタの位置を返す*/
	printf("filesize=%d\n",filebytes);
	POINTNUM=filebytes/47;
	printf("頂点の数%d\n",POINTNUM);
	rewind(fp);//ファイルポインタを先頭に戻す
	char *buf=new char[filebytes];
	fread(buf,filebytes,1,fp);

	fclose(fp);
	POINTNUM/=10;
	MyArrayInit();
	printf("%s\n",FILENAME);
	string str;
	boost::char_separator<char> sep( " ,\n"); //separator is space or comma.
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer1;
	using boost::lexical_cast;
	str = lexical_cast<string>( buf );
	tokenizer1 tok1( str,sep);
	tokenizer1::iterator it=tok1.begin();
	
	for(int i=0;i<POINTNUM+BEGIN;i++){

		//printf("[%d]%s\n",i,str.c_str());
		data.vert[i].x=lexical_cast<float>( *it );
		it++;
		data.vert[i].y=lexical_cast<float>( *it );
		it++;
		data.vert[i].z=lexical_cast<float>( *it );
		// cout << "TOKEN: " << *it << endl;
		it++;
		//   cout << "TOKEN: " << *it << endl;
	}

	//ifs.close();
	for(int i=0;i<POINTNUM+BEGIN;i++){
		if(i>=BEGIN){
			if(data.max.x<data.vert[i].x){data.max.x=data.vert[i].x;}
			if(data.min.x>data.vert[i].x){data.min.x=data.vert[i].x;}	   
			if(data.max.y<data.vert[i].y){data.max.y=data.vert[i].y;}
			if(data.min.y>data.vert[i].y){data.min.y=data.vert[i].y;}
			if(data.max.z<data.vert[i].z){data.max.z=data.vert[i].z;}
			if(data.min.z>data.vert[i].z){data.min.z=data.vert[i].z;}
		}
	}
	data.range=data.max-data.min;

	//初期化三角形の頂点以外のすべての頂点が画面内におさまるように直す時に必要な、点群の範囲、min,maxを計算。
	//dataにはいっぱい入っているが、beginIndex-beginIndex+POINTNUMの範囲だけ取り出す
	/*printf("rangex:%.3f,min%.3f,max%.3f\n",data.range.x,data.min.x,data.max.x);
	printf("rangey:%.3f,min%.3f,max%.3f\n",data.range.y,data.min.y,data.max.y);
	printf("rangez:%.3f,min%.3f,max%.3f\n",data.range.z,data.min.z,data.max.z);
	*/
	//0-1に正規化 areaとoffsetがどんな値か考えるのが大変だったから正規化した。
	for(int i=0;i<POINTNUM;i++){
		data.vert[i].x=(data.vert[i].x-data.min.x)/data.range.x;
		data.vert[i].y=(data.vert[i].y-data.min.y)/data.range.y;
		zvalue[i]=(data.vert[i].z-data.min.z)/data.range.z;//0-1に正規化
	}


}
void UpdateTri(){
	for(int i=0;i<tcount;i++){
		if(triBuffer[i]->IsMyVertex(vertexBuffer[pointnum+0]) || triBuffer[i]->IsMyVertex(vertexBuffer[pointnum+1])){
			//マイナスインデックスに付随する三角形を排除			
		}else{
			//葉じゃないものも含み、コピーする（あとでツリーを表示したいから）
			tri[i].Set(triBuffer[i],&areav,&offsetv);
		}
	}
}
void Init(){

	string filename=FILENAME;
	
	if(string::npos==filename.find_last_of("txt")){
		printf("バイナリデータ\n");
		ReadBinary();
	}else{
		printf("テキストデータ\n");
		ReadTXT();
	}
	//ZeroMemory(&myoutdata,sizeof(OUTDATA));
	tris.setf(ios::fixed,ios::basefield);
	tris.precision(0);
	printf("SetUpData\n");
	SetUpData(data.vert,POINTNUM);//vertexBufferに入れる
	NormalizeVertex();
	printf("NormalizeVertex\n");
	for(int i=1;i<ke;i++){
		InsertNewPoint(i);
	}
	printf("UpdateTri\n");
	UpdateTri();
	printf("done UpdateTri\n");
}
void SaveTriangleData(){//出来上がった三角形を保存するための関数	
	LARGE_INTEGER  be;
	LARGE_INTEGER  af;
	LARGE_INTEGER  fr;
	QueryPerformanceCounter(&be);
	char drive[200];
	char dir[200];
	char fname[200];
	char ext[200]; 
	_splitpath(FILENAME,drive,dir,fname,ext);
	stringstream outfilename;
	outfilename<<drive<<dir<<fname<<myoutdata.size/3<<"_triangles"<<ext;
	printf("outfilename[%s]\n",outfilename.str().c_str());	
	out.open(outfilename.str().c_str(),std::ios::trunc);
	printf("write to .txt file\n");
	int section=myoutdata.size/100;
	if(section==0){section=1;}
	for(int i=0;i<myoutdata.size;i++){
		//入力時はランダムに散らしたので、元に戻すとき気をつける。
		out<<myoutdata.vertex[i][0]<<","<<myoutdata.vertex[i][1]<<","<<myoutdata.vertex[i][2]<<endl;

		if(i%section==0){//進行状況の表示
			int progress=((double)i/(double)finaltriangle)*100.0;
			printf("進行状況%d％ \r", progress);
		}
	}
	out.close();
	QueryPerformanceCounter(&af);
	QueryPerformanceFrequency(&fr);
	DWORD time = (DWORD)((af.QuadPart - be.QuadPart) * 1000 / fr.QuadPart);
	printf("ファイル出力にかかった時間[%d]頂点%dミリセカンド\n",POINTNUM-BEGIN,time);
}
void reshape (int w, int h)
{
	displayW=w;
	displayH=h;
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);//この設定はOpenGLのデフォルトと同じ。この時点で座標は画面真ん中が原点　最大1.0というかんじになる。
	//// ウィンドウ内のピクセル座標がそのまま利用される が、右上原点で上下逆なのが気に入らない。
	//// ＝直交投影[Orthogonal Projection]
	//glMatrixMode (GL_PROJECTION); // 今から投影行列を設定すると宣言
	//glLoadIdentity ();            // 単位行列をセット
	//// 直交投影
	//glOrtho(0, w, h, 0, -1.0f, 1.0f);
	// gluOrtho2D (0.0,(GLdouble) w,  (GLdouble) h,0.0);
	// gluLookAt(0.0,0.0,-0.8 ,0,0,0.0, 0.0,1.0,0.0);
	// 

}
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 27://escape key
		exit(0);
		break;
	case 'r':
		printf("reset position");
		scale=1.0;
		transx=0.0;
		transy=0.0;
		break;
	case 'k':
		printf("\n\ninsert myId=[%d]trueId[%d] ke=%d\n",vertexBuffer[ke]->myId,vertexBuffer[ke]->trueId,ke);
		if(ke<POINTNUM){
			InsertNewPoint(ke);
		}
		if(ke<POINTNUM+1){
			//	printf("ke=%d\n",ke);
			ke++;
			//printf("ke=%d\n",ke);
		}
		UpdateTri();
		break;

	}
	glutPostRedisplay();
}
void DrawOpenGLText(float r,float g,float b,float x,float y,stringstream *message){
	//画面に文字を出力
	glColor3f(r,g,b);
	glRasterPos2d(x,y);
	glutBitmapString(font,reinterpret_cast<const unsigned char*>(message->str().c_str()));
}
void TraverseTreeInfo(CTriangle *_t,CTriangle *_parent,int _childId){
	//画面左上のツリー表示用関数
	//UpdateTriのとき、子がうまくコピーできないから出来ない。
	if(tri[_t->myId].nodePos==0){
		double newx=tri[_parent->myId].nodePos->x+onepixel*50+_childId*nodesize*onepixel*5;
		double newy=tri[_parent->myId].nodePos->y-nodesize*onepixel*5;
		tri[_t->myId].nodePos=new CVertexDebug(newx,newy);
		tri[_t->myId].nodePos->PrintVertex("noe");
	}

	glColor3f(0.0,0.0,1.0);
	//親から子へ線をひく
	glBegin(GL_LINES);
	tri[_parent->myId].nodePos->glVertex();
	tri[_t->myId].nodePos->glVertex();
	glEnd();
	if(_t->a->trueId<0||_t->b->trueId<0||_t->c->trueId<0){
		if(triBuffer[_t->myId]->PtInTriangleSpecial(vertexBuffer[ke],false))
		{glColor3f(1.0,0.0,0.0);}else{glColor3f(1.0,1.0,1.0);}
	}else{
		if(triBuffer[_t->myId]->PtInTriangle2(vertexBuffer[ke],false))
		{glColor3f(1.0,0.0,0.0);}else{glColor3f(1.0,1.0,1.0);}
	}
	//自分自身を描く。
	glPointSize(30);
	glBegin(GL_POINTS);
	tri[_t->myId].nodePos->glVertex();//これが自身のノード	
	glEnd();
	stringstream ss;
	ss<<_t->myId;
	DrawOpenGLText(1.0,0.0,1.0,tri[_t->myId].nodePos->x,tri[_t->myId].nodePos->y,&ss);


	//子がいるなら更に探索。

	double previousnodey=nodey;

	for(int c=0;c<_t->childnum;c++){
		if(_t->childnum>2){	
			TraverseTreeInfo(_t->children[c],_t,c);
		}else{
			TraverseTreeInfo(_t->children[c],_t,c+2);
		}

	}


	nodey=previousnodey;


}
void display(void)
{
	glClearColor(background[0],background[1], background[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glScalef(scale,scale,scale);
	glTranslatef(transx,transy,0);
	glColor3f(1.0,1.0,0.0);
	glLineWidth(1.0);
	for(int i=0;i<tcount;i++){
	if(tri[i].childnum==0){//葉ならば
		tri[i].DrawTriangle();//描く。	
		}
	}
	glPointSize(2);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_POINTS);
	for(int i=0;i<POINTNUM;i++){showvert[i].glVertex();}
	glEnd();
	glPopMatrix();
	glutSwapBuffers();
}
void motion( int x , int y){
	double modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	double projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	int viewport[4];
	double z;
	glGetIntegerv(GL_VIEWPORT, viewport);
	double posx,posy;
	gluUnProject(x, displayH -y,0,modelview,projection,viewport,&posx,&posy,&z);	
	//printf("motion=%.8f,motion=%.8f\n",transx,transy);
	transx+=(posx-pushx)/scale;//*0.1;
	transy+=(posy-pushy)/scale;//*0.1;
	pushx=posx;
	pushy=posy;
	glutPostRedisplay();


}
void mouse(int button , int state , int x , int y) {
	//if(k==2){}
	if (button != GLUT_LEFT_BUTTON ) return;
	buttondown=true;
	double modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	double projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	int viewport[4];
	double z;
	glGetIntegerv(GL_VIEWPORT, viewport);
	glutPostRedisplay();
}
void wheel(int button,int direction,int x,int y){
	if(direction>0){
		scale/=0.8;
	}else{
		scale*=0.8;
	}
	//	scale*=0.3*direction;
	glutPostRedisplay();
}
int main(int argc, char *argv[])
{
	//SaveTriangleData();	
	glutInit(&argc, argv);
	glutInitWindowSize (displayW, displayH); 
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH );
	glutCreateWindow("「k」を押してステップを進めよう");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	Init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(wheel);
	glutMainLoop();

	return 0;
}
