#define FILENAME "../../data/pointlist_158279triangles.txt"
//DEPARE_22019triangles.dat"
#define POINTNUM 158279*3//22019*3

#include <stdio.h>
#include <GL/freeglut.h>
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <float.h>
#include <miffy/math/vertex.h>
#include <miffy/math/vec3.h>
#include <miffy/math/matrix.h>
#include <miffy/math/quaternion.h>
#include "Triangle.h"
#include "Vertex.h"
using namespace std;
//#define POINTNUM 19614*3
//#define POINTNUM 158279*3//ファイル名は三角形の数。だから点の数は３倍にするべし
//int pointnum=0;//あとでファイル名から全体の頂点数を判断する
#define POINTSIZE 1
#define MAXTEXT 255
#define WINWIDTH 1024
#define WINHEIGHT 768
CVertex vertexBuffer[POINTNUM];
int trianglecount=0;
float background[3]={0.0,0.0,0.0};//{1.0,1.0,1.0};
float myrotate[3]={0.0,0.0,0.0};
enum ROTATEAXIS{ RX,RYRZ}rotateAxis;
float speed=2.0;
float space=1.0;
float eyePoint[3]={0.0,0.0,space*2.0};
miffy::quat<float> current_quaternion(1.0,0.0,0.0,0.0);
miffy::quat<float> target_quaternion;
bool quaternionflag=true;
//回転マトリックス
miffy::mat4<float> rotation_matrix;
int previous_x;
int previous_y;
int mouse_down[2];
// Shapes scale
float g_Zoom = 1.0f;
//平行移動
float trans[3]={0,0,-4.0};//最初のズーム位置を決める
float area=1.0;
float offset=-0.5;
float offsetz=10;
float translate_speed=0.01;
float zoom_speed=0.2;
int input[POINTNUM*3];
  typedef struct tagvertex{
		miffy::vertex<double> *vert;
		miffy::vec3<double> max;
		miffy::vec3<double> min;
		miffy::vec3<double> range;
	}VERTEX,*PVERTEX;
	VERTEX data;
	double aspectratio=1.0;//y/xデータの縦横比
	int maxid=0;
	void NormalizeVertex(){
		printf("xrange:%.3f,min%.3f,max%.3f\n",data.range.x,data.min.x,	data.max.x);
		printf("yrange:%.3f,min%.3f,max%.3f\n",data.range.y,data.min.y,data.max.y);
		printf("zrange:%.3f,min%.3f,max%.3f\n",data.range.z,data.min.z,data.max.z);
		aspectratio=data.range.y/data.range.x;
		//縦横、大きいほうを１に合わせる。
		double range;
		if(aspectratio>1.0){//yのほうが大きかったら
			range=data.range.x;
		}else{range=data.range.y;}
		if(range==0){range=1.0;}
		for(int i=0;i<POINTNUM;i++){
				data.vert[i].z=(data.vert[i].z-data.min.z)/data.range.z;
				data.vert[i].x=(data.vert[i].x-data.min.x)/range;
				data.vert[i].y=(data.vert[i].y-data.min.y)/range;
			

			vertexBuffer[i].x=data.vert[i].x*area+offset;
			vertexBuffer[i].y=data.vert[i].y*area+offset;
			vertexBuffer[i].z=data.vert[i].z*area+offset;
			vertexBuffer[i].truex=data.vert[i].x;
			vertexBuffer[i].truey=data.vert[i].y;
			vertexBuffer[i].truez=data.vert[i].z;
			vertexBuffer[i].index=i;
		}
		
	}
	void ReadBinary(){
			//まずFILENAMEから点の数を判断する_の直後が点の数だ。
		//FILENAME
		char buf[MAXTEXT];
		ZeroMemory(&data,sizeof(VERTEX));//data={0};
		for(int i=0;i<3;i++){	
			data.max=-DBL_MAX;//0x0;
			data.min=DBL_MAX;//0xffffffffffffffff;//fが16個
		}
		data.vert=new miffy::vertex<double>[POINTNUM];
		FILE* fp;
		fp=fopen(FILENAME,"rb"); // 読み取り専用で開く
		if(fp==NULL){
			assert(!"ファイルが存在しません。ファイル名が間違ってるかも\n");
		}
		fread(input,sizeof(int),POINTNUM*3,fp);
		for(int i=0;i<POINTNUM;i++){
			data.vert[i].x=(double)input[i*3+0];
			data.vert[i].y=(double)input[i*3+1];
			data.vert[i].z=(double)input[i*3+2];
			if(data.vert[i].x>data.max.x){data.max.x=data.vert[i].x;}
			if(data.vert[i].y>data.max.y){data.max.y=data.vert[i].y;}
			if(data.vert[i].z>data.max.z){data.max.z=data.vert[i].z;}
			if(data.vert[i].x<data.min.x){data.min.x=data.vert[i].x;}
			if(data.vert[i].y<data.min.y){data.min.y=data.vert[i].y;}
			if(data.vert[i].z<data.min.z){data.min.z=data.vert[i].z;}
		}

		data.range=data.max-data.min;
		fclose(fp);
		printf("");
		NormalizeVertex();//すべて長さ1の立方体に収まるようにスケールを変える！
	}
	void ReadTXT2(){
		//まずFILENAMEから点の数を判断する_の直後が点の数だ。
		//FILENAME
		char buf[MAXTEXT];
		ZeroMemory(&data,sizeof(VERTEX));//data={0};
		for(int i=0;i<3;i++){	
			data.max=-DBL_MAX;//0x0;
			data.min=DBL_MAX;//0xffffffffffffffff;//fが16個
		}
		data.vert = new miffy::vertex<double>[POINTNUM];
		
		FILE* fp;
		fp=fopen(FILENAME,"rb"); // 読み取り専用で開く
		if(fp==NULL){
			assert(!"ファイルが存在しません。ファイル名が間違ってるかも\n");
		}
		char* ret;
		for(int i=0;i<POINTNUM;i++){
			fgets(buf,200,fp);
			ret=strtok(buf,",");
			data.vert[i].x=atof(ret);
			ret=strtok(NULL,",");
			data.vert[i].y=atof(ret);
			ret=strtok(NULL,"\n");
			data.vert[i].z=atof(ret);
			//printf("%f,%f,%f\n",data.vertex[i][0],data.vertex[i][1],data.vertex[i][2]);
			if(data.vert[i].x>data.max.x){data.max.x=data.vert[i].x;}
			if(data.vert[i].y>data.max.y){data.max.y=data.vert[i].y;}
			if(data.vert[i].z>data.max.z){data.max.z=data.vert[i].z;}
			if(data.vert[i].x<data.min.x){data.min.x=data.vert[i].x;}
			if(data.vert[i].y<data.min.y){data.min.y=data.vert[i].y;}
			if(data.vert[i].z<data.min.z){data.min.z=data.vert[i].z;}
		}//i

		data.range=data.max-data.min;
		fclose(fp);
		printf("");
		NormalizeVertex();//すべて長さ1の立方体に収まるようにスケールを変える！
	}
	void idle(){
		
		//rotate+=1;
		//eyePoint[0]+=1.0;
		glutPostRedisplay();
	}
	void reshape (int w, int h)
	{
		//射影変換行列
		glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity (); 
		gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
		glMatrixMode (GL_MODELVIEW);
	}
	void DrawCoordinate(){//座標軸を描く
		float length=0.5;
		glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(length,0.0,0.0);

		glColor3f(1.0,1.0,0.0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,length,0.0);

		glColor3f(0.0,0.0,1.0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.0,length);


		glEnd();
	}
	void DrawPrimitives(){
		glColor3f(0.04,0.41,0.04);
		glPointSize(POINTSIZE);
		//glBegin(GL_POINTS);//GL_TRIANGLE_FAN);

		//glEnd();
		int count=0;

		for(int i=0;i<POINTNUM/3;i++){
			GLfloat glfLightAmbient[]  = {0.5,0.5,0.5,1.0};//{0.04, 0.41, 0.04, 1.0};
			GLfloat glfLightDiffuse[]  ={0.5,0.5,0.5,1.0}; //{0.21, 0.67, 0.17, 1.0};
			GLfloat glfLightSpecular[] ={1.0,1.0,1.0,1.0}; //{0.36, 0.97, 0.1, 1.0};

			//光の値の配列をOpenGLに与える。
			glLightfv(GL_LIGHT0, GL_AMBIENT, glfLightAmbient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, glfLightDiffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR,glfLightSpecular);
			glMaterialf(GL_FRONT, GL_SHININESS, 90.0f);
			glEnable(GL_LIGHTING);

			glEnable(GL_LIGHT0);
			//z 深いほど赤　浅いほど青
			double rr=0;double gg=0;double bb=0;
			rr=1.0-data.vert[i*3].z;
			bb=data.vert[i*3].z;

			glColor3f(rr,gg,bb);	
			GLfloat col[4]={rr,gg,bb,1.0};
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
			glBegin(GL_TRIANGLES);
			//法線計算
			//CVertex a=vertexBuffer[i*3+0]-vertexBuffer[i*3+1];
			//CVertex b=vertexBuffer[i*3+2]-vertexBuffer[i*3+1];
			CVertex a=vertexBuffer[i*3+1]-vertexBuffer[i*3+0];
			CVertex b=vertexBuffer[i*3+1]-vertexBuffer[i*3+2];
			CVertex normal=a.GetNormalVector(&b);

			normal.glNormal();
			vertexBuffer[i*3+0].glVertex();
			normal.glNormal();
			vertexBuffer[i*3+1].glVertex();
			normal.glNormal();
			vertexBuffer[i*3+2].glVertex();
			glEnd();
			glDisable(GL_LIGHTING);
		}

	}
	void display(void)
	{
		//printf("%f,%f,%f\n",background[0],background[1], background[2]);
		glClearColor(background[0],background[1], background[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
		glEnable(GL_DEPTH_TEST);

		//次に、どの光源を使用するか伝えます。
		glLoadIdentity ();/* clear the matrix */
		/* viewing transformation  */
		gluLookAt (eyePoint[0], eyePoint[1], eyePoint[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);//zが上であるが、そうするのが難しい。
//		glPushMatrix();
		glTranslatef(trans[0],trans[1],trans[2]);//平行移動
        glMultMatrixf(rotation_matrix.m);//クォータニオンによる回転
//		glScalef(g_Zoom, g_Zoom, g_Zoom);
		//原点
		DrawPrimitives();
		//DrawCoordinate();
		//画面に文字を出力
		/*glColor3f(1.0,1.0,1.0);
		glRasterPos3d(-0.08,-0.014,1.00);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*)"fustumisaki");//"富津岬");
		glRasterPos3d(-0.154,-0.108,1.00);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*)"Tokyo International Airport");//"東京国際空港");
		glRasterPos3d(vertexBuffer[0].x,vertexBuffer[0].y,vertexBuffer[0].z);//glRasterPos3d(-0.039,-0.219,1.00);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*)"Funabashi");//"船橋");
		*/
		//glutBitmapString(GLUT_BITMAP_HELVETICA_12,reinterpret_cast<const unsigned char*>(""));
//		glPopMatrix();
		glutSwapBuffers();

	}
	void key(unsigned char key , int x , int y) {
		glutPostRedisplay();
	}
    void wheel(int button, int direction, int x, int y) 
    { 
        if(direction>0){
            trans[2]-=zoom_speed;
        }else{
            trans[2]+=zoom_speed;
        }
        glutPostRedisplay();
    }
    void mouse(int button , int state , int x , int y) {
 
        if(button == GLUT_RIGHT_BUTTON){//平行移動
            quaternionflag=false;
            if(state==GLUT_DOWN){
                mouse_down[0]=x;
                mouse_down[1]=y;
            }else if(state==GLUT_UP){
                trans[0]+=translate_speed*(x-mouse_down[0]);
                trans[1]+=-translate_speed*(y-mouse_down[1]);//yは上下逆
            }
 
        }
        else 
            if(button == GLUT_LEFT_BUTTON){
                quaternionflag=true;
                switch(state){        
                case GLUT_DOWN://Quaternion:マウスボタンを押した位置を記憶 
                    previous_x = x;
                    previous_y = y;
                case GLUT_UP://Quaternion:姿勢を保存
                    current_quaternion=target_quaternion;
                    break;
                default:
                    break;
                }
 
            }
            glutPostRedisplay();
 
    }
	void move(int x, int y)
    {
        if(quaternionflag){
            //移動量を計算 画面の中で何％ぐらい動いたか？
            float dx = (x - previous_x) * 1.0/WINWIDTH;
            float dy = (y - previous_y) * 1.0/WINHEIGHT;
 
            //クォータニオンの長さ
            float length = sqrt(dx * dx + dy * dy);
 
            if (length != 0.0) {
                //M_PIは適当な換算係数　piにしておくと、画面いっぱい動かした時にちょうど一回転になる
                float radian = length * M_PI;
                float theta = sin(radian) / length;
                miffy::quat<float> after( cos(radian), dy * theta, dx * theta, 0.0);//回転後の姿勢
                target_quaternion = after * current_quaternion;
                target_quaternion.toMat4(rotation_matrix.m);
 
            }
        }else{//平行移動
            trans[0]+=translate_speed*(x-mouse_down[0]);
            trans[1]+=-translate_speed*(y-mouse_down[1]);//yは上下逆
            mouse_down[0]=x;
            mouse_down[1]=y;
        }

    }
	int main(int argc, char *argv[])
	{
		glutInit(&argc, argv);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
		glutInitWindowSize (WINWIDTH, WINHEIGHT); 
		glutInitWindowPosition (0, 0);
		glutCreateWindow(argv[0]);
		ReadTXT2();
		//ReadBinary();
		
		glutDisplayFunc(display);
		glutIdleFunc(idle);
		glutMouseFunc(mouse);
        glutMotionFunc(move);
		glutReshapeFunc(reshape);
		glutKeyboardFunc(key);
		glutMouseWheelFunc(wheel);
		glutMainLoop();

		printf("終了");
		exit(0);
		return 0;
	}
