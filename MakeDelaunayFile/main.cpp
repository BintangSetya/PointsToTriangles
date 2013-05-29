#define FILENAME "../../data/42_3D.dat"
#include <windows.h>
#include <string>
#include <stdio.h>
#include <GL/freeglut.h>
//#include <glui.h>
#include <string>
#include <iostream>
#include<sstream>
#include <iomanip>
#include <fstream>
#include <float.h>
#include <miffy/math/vec3.h>
#include "../delaunay.h"

int POINTNUM;//= 80000;
#define BEGIN 0
//時間計測用
LARGE_INTEGER  before;
LARGE_INTEGER  after;
LARGE_INTEGER  freq;
miffy::vec3<int> *input;
typedef struct tagvertex{//ファイルから頂点を読み込むときに使う
  miffy::vec3<float> *vertex;//[POINTNUM][3];
	double max[3];
	double min[3];
	double range[3];
}VERTEX,*PVERTEX;
VERTEX data;
OUTDATA myoutdata;
ofstream out;
void ReadBinary(){
	FILE *fp;
	fp=fopen(FILENAME,"rb"); // 読み取り専用で開く
	if(fp==NULL){assert(!"ファイルが存在しません。");}
	fseek(fp,0,SEEK_END);//ファイル終端から+0ファイルポインタの位置を移動させる
	int filebytes=ftell(fp);//現在のファイルポインタの位置を返す
	rewind(fp);
	POINTNUM=filebytes/sizeof(float)/3;
	printf("頂点の数%d\n",POINTNUM);
	input=new miffy::vec3<int>[POINTNUM];
	data.vertex=new miffy::vec3<float>[POINTNUM];
	bool sameflag=false;
	int counter=0;
	for(int i=0;i<POINTNUM+BEGIN;i++){
		fread(&input[i].x,sizeof(miffy::vec3<int>),1,fp);
		//xyが同じ座標のものを除去する
		for(int j=0;j<i;j++){
			if((data.vertex[j].x==(float)input[i].x) && (data.vertex[j].y==(float)input[i].y) )
			{//もし同じ座標があったら
				sameflag=true;
			}
		}
		if(!sameflag){
			data.vertex[counter].x=(float)input[i].x;
			data.vertex[counter].y=(float)input[i].y;
			data.vertex[counter].z=(float)input[i].z;
			counter++;
		}
		sameflag=false;
	}
	POINTNUM=counter;
	printf("重複除去後の点の数%d\n",POINTNUM);
	fclose(fp);
}
void ReadTXT(){

	char buf[MAXTEXT];

	 ZeroMemory(&data,sizeof(VERTEX));//data={0};
	 for(int i=0;i<3;i++){	
		 data.max[i]=-DBL_MAX;//0x0;
		 data.min[i]=DBL_MAX;//0xffffffffffffffff;//fが16個
	 }
	FILE *fp;
	fp=fopen(FILENAME,"rb"); // 読み取り専用で開く
	if(fp==NULL){assert(!"ファイルが存在しません。");}
	fseek(fp,0,SEEK_END);//ファイル終端から+0ファイルポインタの位置を移動させる
	int filebytes=ftell(fp);//現在のファイルポインタの位置を返す
	rewind(fp);
	POINTNUM=filebytes/sizeof(float)/3;
	printf("頂点の数%d\n",POINTNUM);
	fclose(fp);
	ifstream ifs;
	  ifs.open(FILENAME, ios::in); // 読み取り専用で開く
	  char* ret;
	for(int i=0;i<POINTNUM+BEGIN;i++){
		ifs.getline(buf,200);//1行ずつ読む
		ret= strtok(buf,",");//x
		data.vertex[i].x= atof(ret);
		ret= strtok(NULL,",");//y
		data.vertex[i].y= atof(ret);
		ret= strtok(NULL,"\n");//z
		data.vertex[i].z= atof(ret);
	}

    ifs.close();
	
}
void Init(){
	ZeroMemory(&myoutdata,sizeof(OUTDATA));
	QueryPerformanceCounter(&before);
	myoutdata=Delaunay(data.vertex,POINTNUM);
	QueryPerformanceCounter(&after);
	QueryPerformanceFrequency(&freq);
	DWORD time = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);
	printf("処理にかかった時間[%d]頂点%dミリセカンド\n",POINTNUM-BEGIN,time);
	printf("ファイル書込み中");
}
void SaveTriangleBinary(){//出来上がった三角形を保存するための関数
	char drive[200];
	char dir[200];
	char fname[200];
	char ext[200]; 
	_splitpath(FILENAME,drive,dir,fname,ext);
	stringstream outfilename;
	outfilename<<"../../data/"<<fname<<"_"<<myoutdata.size/3<<"triangles"<<ext;
	printf("outfilename[%s]\n",outfilename.str().c_str());
	int* output=new int[myoutdata.size*3];
	for(int i=0;i<myoutdata.size;i++){
		output[i*3+0]=(int)myoutdata.vertex[i][0];
		output[i*3+1]=(int)myoutdata.vertex[i][1];
		output[i*3+2]=(int)myoutdata.vertex[i][2];
	}
	FILE* fp;
	fp=fopen(outfilename.str().c_str(),"wb");
	printf("write to .txt file　頂点の数=%d 三角形の数=%d\n",myoutdata.size,myoutdata.size/3);
	fwrite(output,sizeof(int),myoutdata.size*3,fp);
	
	delete[] output;
	int a;
	printf("終了");
	scanf("%d",&a);

}
void SaveTriangleData(){//出来上がった三角形を保存するための関数
	char drive[200];
	char dir[200];
	char fname[200];
	char ext[200]; 
	_splitpath(FILENAME,drive,dir,fname,ext);
	stringstream outfilename;
	outfilename<<fname<<myoutdata.size/3<<"triangles"<<ext;
	printf("outfilename[%s]\n",outfilename.str().c_str());
	out.open(outfilename.str().c_str(),std::ios::trunc);
	printf("write to .txt file　頂点の数=%d 三角形の数=%d\n",myoutdata.size,myoutdata.size/3);
	int section=(double)100/(double)myoutdata.size;
	if(section==0){section=1;}
	for(int i=0;i<myoutdata.size;i++){
		//入力時はランダムに散らしたので、元に戻すとき気をつける。
		out<<myoutdata.vertex[i][0]<<","<<myoutdata.vertex[i][1]<<","<<myoutdata.vertex[i][2]<<endl;
		if(i%section==0){//進行状況の表示
			int progress=((double)i*(double)section);
			printf("ファイル書き込み進行状況%d％ \r", progress);
		}
	}
	out.close();
	int a;
	scanf("%d",&a);

}
void main(){
	ReadBinary();
	//ReadTXT();
	Init();
	
	SaveTriangleBinary();
	DeleteOutData(myoutdata);
}
