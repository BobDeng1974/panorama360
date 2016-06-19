#include <GLES2/gl2.h>
#include <sys/time.h>
#include <EGL/egl.h>
#include <string>
#include "Platform.h"
#include "EGLRuntime.h"
#include "Shader.h"
#include "Matrix.h"
#include "sphere.h"
#include "video.h"
#include "rectangle.h"
#include <math.h>
#include <signal.h>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

#define PI 3.141592653
#define T (PI/180)
#define WINDOW_W 720
#define WINDOW_H 480

using namespace std;
using namespace MaliSDK;

string resourceDirectory = "assets/";
string vertexShaderFilename = "sphere.vert";
string fragmentShaderFilename = "sphere.frag";

GLuint programID;
GLint iLocPosition;
GLint iLocColor;
GLint iLocMVP;
GLint iLocMyUV;

int directionx = 0;
int directiony = 0;
int directionz = 0;
int mode = 1;

int windowWidth = -1;
int windowHeight = -1;
GLuint myTexture;
GLuint iFBO = 0;

extern unsigned char *cfront,*crear,*cleft,*cright;
extern pthread_spinlock_t spinlock;
extern int flag;
extern unsigned char *otheraddr; 

Rectangle * texture;
IplImage *panorama;

//read a picture as a texture for gpu
bool setupTextureFromPIC()
{
	IplImage *texture=cvLoadImage("./1.jpg",1);
	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_image"),5));
	GL_CHECK(glGenTextures(1,&myTexture));
	GL_CHECK(glActiveTexture(GL_TEXTURE5));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,myTexture));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,300,480,0,GL_RGB,
				GL_UNSIGNED_BYTE,NULL));
	//filter
	GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
	GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,300,480,GL_RGB,
			GL_UNSIGNED_BYTE,texture->imageData));
	return true;
}

//use gpu to draw a texture 
bool setupTextureFromGPU()
{
	texture = new Rectangle(cfront,crear,cleft,cright);
}

// initialize gpu program including load shader setup texture and so on
bool setupGraphics(int width,int height)
{
	
	windowWidth = width;
	windowHeight = height;

	string vertexShaderPath = resourceDirectory+vertexShaderFilename;
	string fragmentShaderPath = resourceDirectory+fragmentShaderFilename;

	GL_CHECK(glEnable(GL_BLEND));
	GL_CHECK(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));

	//setup texture from gpu
	setupTextureFromGPU();

	GLuint fragmentShaderID,vertexShaderID;

	Shader::processShader(&vertexShaderID,vertexShaderPath.c_str(),GL_VERTEX_SHADER);
	Shader::processShader(&fragmentShaderID,fragmentShaderPath.c_str(),GL_FRAGMENT_SHADER);

	programID = GL_CHECK(glCreateProgram());

	GL_CHECK(glAttachShader(programID,vertexShaderID));
	GL_CHECK(glAttachShader(programID,fragmentShaderID));
	GL_CHECK(glLinkProgram(programID));
	GL_CHECK(glUseProgram(programID));

	//setup Texture for sphere
	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_image"),6));
	GL_CHECK(glGenTextures(1,&myTexture));
	GL_CHECK(glActiveTexture(GL_TEXTURE6));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,myTexture));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,300,480,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL));
//	GL_CHECK(glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,300,480,0));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE));
	
	GL_CHECK(glGenFramebuffers(1,&iFBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER,iFBO));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,myTexture,0));
	GLenum iResult = GL_CHECK(glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if(iResult != GL_FRAMEBUFFER_COMPLETE)
	{
		LOGE("Framebuffer incomplete at %s:%i\n",__FILE__,__LINE__);
		return false;
	}

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER,0));

	iLocMyUV = GL_CHECK(glGetAttribLocation(programID,"myUV"));
	iLocPosition = GL_CHECK(glGetAttribLocation(programID,"av4position"));


	LOGD("iLocMyUV = %i\n",iLocMyUV);
	LOGD("iLocPosition = %i\n",iLocPosition);

	iLocMVP = GL_CHECK(glGetUniformLocation(programID,"mvp"));

	LOGD("iLocMVP = %i\n",iLocMVP);

//	GL_CHECK(glFrontFace(GL_CCW));
	GL_CHECK(glEnable(GL_DEPTH_TEST));
//	GL_CHECK(glEnable(GL_CULL_FACE));

	GL_CHECK(glClearColor(0.0f,0.0f,0.0f,1.0f));

//	setupTextureFromPIC();

	return true;
}

//draw a frame
void renderFrame(void)
{
	GL_CHECK(glUseProgram(programID));
	GL_CHECK(glEnableVertexAttribArray(iLocPosition));
	GL_CHECK(glEnableVertexAttribArray(iLocMyUV));

	//copy texture
//	GL_CHECK(glActiveTexture(GL_TEXTURE5));
//	GL_CHECK(glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,300,480));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER,0));

	float svertices[18];
	float sMyUV[12];

	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));
	static float angleX, angleY, angleZ;

	//play different video depending on specific mode
	switch(mode)
	{
		case 2:
			angleX = 0,angleY = 0,angleZ = 0;              
			break;
		case 3:
			angleX = directionx , angleY = directiony , angleZ = directionz; //sphere               
			break;
		default:
			angleX = 0,angleY = 0,angleZ = 0;              
	}
//	static float angleX = 345,angleY = 0,angleZ = direction; //sphere               

	Matrix modelView = Matrix::createRotationX(angleX);
	Matrix rotation = Matrix::createRotationY(angleY);
	
	modelView = rotation*modelView;

	rotation = Matrix::createRotationZ(angleZ);
//	modelView =rotation*modelView;
	modelView =modelView*rotation;

	switch(mode)
	{
		case 2:
			modelView[14]-=2.0; 
			break;
		case 3:
			modelView[14]-=1.0; //sphere
			break;
		default:
			modelView[14]-=2.0; 
	}

	
	Matrix perspective = Matrix::matrixPerspective(45.0f,windowWidth/(float)windowHeight,0.01f,100.0f);
	Matrix modelViewPerspective = perspective*modelView;

	GL_CHECK(glUniformMatrix4fv(iLocMVP,1,GL_FALSE,modelViewPerspective.getAsArray()));
#if 0          //rotate the sphere
	angleX +=3;
	angleY +=2;
	angleZ +=10;
	
	if(angleX>=360)angleX-=360;
	if(angleX<0)angleX+=360;
	if(angleY>=360)angleY-=360;
	if(angleY<0)angleY+=360;
	if(angleZ>=360)angleZ-=360;
	if(angleZ<0)angleZ+=360;
#endif
	int theta,phi;
	int dtheta=5;
	int dphi=10;
	//draw sphere and compute the texture coordinate
	for(theta=-90;theta<=-dtheta;theta+=dtheta)
	{
		for(phi=0;phi<=360-dphi;phi+=dphi)
		{
			//coordinate
			svertices[0]=cos(theta*T)*cos(phi*T);
			svertices[1]=cos(theta*T)*sin(phi*T);
			svertices[2]=sin(theta*T);

			//texture coordinate
			sMyUV[0]=(svertices[0]/(1.0-svertices[2])+1)/2.0;
			sMyUV[1]=(svertices[1]/(1.0-svertices[2])+1)/2.0;


			svertices[3]=cos((theta+dtheta)*T)*cos(phi*T);
			svertices[4]=cos((theta+dtheta)*T)*sin(phi*T);
			svertices[5]=sin((theta+dtheta)*T);

			sMyUV[2]=(svertices[3]/(1.0-svertices[5])+1)/2.0;
			sMyUV[3]=(svertices[4]/(1.0-svertices[5])+1)/2.0;

			svertices[6]=cos((theta+dtheta)*T)*cos((phi+dphi)*T);
			svertices[7]=cos((theta+dtheta)*T)*sin((phi+dphi)*T);
			svertices[8]=sin((theta+dtheta)*T);

			sMyUV[4]=(svertices[6]/(1.0-svertices[8])+1)/2.0;
			sMyUV[5]=(svertices[7]/(1.0-svertices[8])+1)/2.0;

			svertices[9]=svertices[0];
			svertices[10]=svertices[1];
			svertices[11]=svertices[2];

			sMyUV[6]=(svertices[9]/(1.0-svertices[11])+1)/2.0;
			sMyUV[7]=(svertices[10]/(1.0-svertices[11])+1)/2.0;

			svertices[12]=svertices[6];
			svertices[13]=svertices[7];
			svertices[14]=svertices[8];

			sMyUV[8]=(svertices[12]/(1.0-svertices[14])+1)/2.0;
			sMyUV[9]=(svertices[13]/(1.0-svertices[14])+1)/2.0;

			svertices[15]=cos(theta*T)*cos((phi+dphi)*T);
			svertices[16]=cos(theta*T)*sin((phi+dphi)*T);
			svertices[17]=sin(theta*T);

			sMyUV[10]=(svertices[15]/(1.0-svertices[17])+1)/2.0;
			sMyUV[11]=(svertices[16]/(1.0-svertices[17])+1)/2.0;

			GL_CHECK(glVertexAttribPointer(iLocMyUV,2,GL_FLOAT,GL_FALSE,0,sMyUV));
			GL_CHECK(glVertexAttribPointer(iLocPosition,3,GL_FLOAT,GL_FALSE,0,svertices));
			//draw a rectangle
			GL_CHECK(glDrawArrays(GL_TRIANGLES,0,6));  
		}
	}
}

//change color space from RGBA to BGRA
int RGBToBGR(char *image, int width, int height)
{
	char tmp = 0;
	char *tmpBuf = (char *)malloc(width*4);
	char *tmpImage = image;
	for(int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			tmp = *tmpImage;
			*tmpImage = *(tmpImage+2);
			*(tmpImage+2) = tmp;
			tmpImage += 4;
		}
	}

	tmpImage = image;
	for(int i=0;i<height/2;i++)
	{
		memcpy(tmpBuf,tmpImage,width*4);
		memcpy(tmpImage,image+(height-i-1)*width*4,width*4);
		memcpy(image+(height-i-1)*width*4,tmpBuf,width*4);
		tmpImage += width*4;
	}
}

//save a panorama
int saveImage()
{
	pthread_spin_lock(&spinlock);
	glReadPixels(0,0,720,480,GL_RGBA,GL_UNSIGNED_BYTE,panorama->imageData);
	RGBToBGR(panorama->imageData,720,480);
	cvSaveImage("panorama.jpg",panorama);
	pthread_spin_unlock(&spinlock);
	cout<<"save image finish!"<<endl;
	exit(0);
}


int main(int argc,char *argv[])
{
	//show the help information
	if(string(argv[1]) == "-h")
	{
		cout<<"./demo mode [directionx directiony directionz]"<<endl;
		return 1;
	}
	//which mode
	mode = atoi(argv[1]);
	if(mode !=1&&mode!=2&&mode!=3||(mode!=1&&argc!=5))
	{
		cout<<"input parameters error!"<<endl;
		return -1;
	}
	if(mode==3)
	{
		directionx = atoi(argv[2]);  //get a point of view
		directiony = atoi(argv[3]);  
		directionz = atoi(argv[4]);
	}
	Platform *platform = Platform::getInstance();
	platform->createWindow(WINDOW_W,WINDOW_H);

	EGLRuntime::initializeEGL(EGLRuntime::OPENGLES2);
	EGL_CHECK(eglMakeCurrent(EGLRuntime::display,EGLRuntime::surface,EGLRuntime::surface,EGLRuntime::context));
	
	setupVideo(); 
	// read the lookup table
	miscInit();
	//start to capture the image
	streamOn();

	setupGraphics(WINDOW_W,WINDOW_H);

	//compute fps
	struct timeval startT,endT;

	bool end = false;
	struct sigaction sigact;
	memset(&sigact,0,sizeof(sigact));
	sigact.sa_handler = (__sighandler_t)saveImage;
	sigaction(SIGINT,&sigact,NULL);

	panorama = cvCreateImage(cvSize(720,480),IPL_DEPTH_8U,4);

	while(!end)
	{
		if(flag)
		{
			gettimeofday(&startT,NULL);
			pthread_spin_lock(&spinlock);
			flag=0;
			//mode 2 or 3 need to draw the panorama of vertical view on the texture
			if(mode != 1)
				GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER,iFBO));
			texture->draw(otheraddr);
			pthread_spin_unlock(&spinlock);
			//mode 2 or 3 need to render a hemisphere
			if(mode != 1)
				renderFrame();
			//display a frame
			eglSwapBuffers(EGLRuntime::display,EGLRuntime::surface);
			gettimeofday(&endT,NULL);
			printf("fps:%ld\n",1000000*(endT.tv_sec-startT.tv_sec)+endT.tv_usec-startT.tv_usec);
		}
	}

	EGLRuntime::terminateEGL();
	platform->destroyWindow();

	delete platform;

	return 0;
}
