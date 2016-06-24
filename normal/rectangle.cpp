#include "rectangle.h"
#include "Shader.h"
#include "Platform.h"
#include "VectorTypes.h"
#include <opencv2/opencv.hpp>

#include <cstring>

//coordinate of rectangle

GLfloat afVertices[] = {-1.0f,1.0f,0.0f,
						1.0f,1.0f,0.0f,
						-1.0f,-1.0f,0.0f,
						1.0f,-1.0f,0.0f
						};
using std::string;
using namespace MaliSDK;
const string Rectangle::resourceDirectory = "assets/";
const string Rectangle::vertexShaderFilename = "rectangle.vert";
const string Rectangle::fragmentShaderFilename = "rectangle.frag";

Rectangle::Rectangle(unsigned char *cfront,unsigned char *crear,unsigned char *cleft,unsigned char *cright)
{
	vertexShaderID = 0;
	fragmentShaderID = 0;
	programID = 0;

	LOGD("Rectangle initialization started..\n");

	// load vertex shader and fragment shader

	string vertexShader = resourceDirectory+vertexShaderFilename;
	Shader::processShader(&vertexShaderID,vertexShader.c_str(),GL_VERTEX_SHADER);
	string fragmentShader = resourceDirectory+fragmentShaderFilename;
	Shader::processShader(&fragmentShaderID,fragmentShader.c_str(),GL_FRAGMENT_SHADER);
	programID = GL_CHECK(glCreateProgram());
	GL_CHECK(glAttachShader(programID,vertexShaderID));
	GL_CHECK(glAttachShader(programID,fragmentShaderID));
	GL_CHECK(glLinkProgram(programID));
	GL_CHECK(glUseProgram(programID));


	m_iLocPosition = GL_CHECK(glGetAttribLocation(programID,"myVertex"));

	// 0 for texture0, 1 for texture1, and so on

	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_image"),0));
	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_front"),1));
	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_rear"),2));
	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_left"),3));
	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_right"),4));
	GL_CHECK(glUniform1i(glGetUniformLocation(programID,"sampler2d_car"),5));

	GL_CHECK(glGenTextures(6,m_uiTexture));

	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,m_uiTexture[0]));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1440,960,0,GL_RGBA,
				GL_UNSIGNED_BYTE,NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	

	GL_CHECK(glActiveTexture(GL_TEXTURE1));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,m_uiTexture[1]));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,900,480,0,GL_RGBA,
				GL_UNSIGNED_BYTE,NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,900,480,GL_RGBA,GL_UNSIGNED_BYTE,cfront));

	GL_CHECK(glActiveTexture(GL_TEXTURE2));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,m_uiTexture[2]));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,900,480,0,GL_RGBA,
				GL_UNSIGNED_BYTE,NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,900,480,GL_RGBA,GL_UNSIGNED_BYTE,crear));

	GL_CHECK(glActiveTexture(GL_TEXTURE3));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,m_uiTexture[3]));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,900,480,0,GL_RGBA,
				GL_UNSIGNED_BYTE,NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,900,480,GL_RGBA,GL_UNSIGNED_BYTE,cleft));

	GL_CHECK(glActiveTexture(GL_TEXTURE4));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,m_uiTexture[4]));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,900,480,0,GL_RGBA,
				GL_UNSIGNED_BYTE,NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,900,480,GL_RGBA,GL_UNSIGNED_BYTE,cright));

	IplImage *texture=cvLoadImage("./car.jpg",1);
	GL_CHECK(glActiveTexture(GL_TEXTURE5));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D,m_uiTexture[5]));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,312,161,0,GL_RGB,GL_UNSIGNED_BYTE,NULL));
	GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
	GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,312,161,GL_RGB,GL_UNSIGNED_BYTE,texture->imageData));

	
}

void Rectangle::draw(unsigned char *otheraddr)
{
	GL_CHECK(glUseProgram(programID));
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));
	// refresh the texture with original image
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,1440,960,GL_RGBA,GL_UNSIGNED_BYTE,otheraddr));
	GL_CHECK(glEnableVertexAttribArray(m_iLocPosition));
	GL_CHECK(glVertexAttribPointer(m_iLocPosition,3,GL_FLOAT,GL_FALSE,0,afVertices));
	GL_CHECK(glFlush());
	GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP,0,4));
}

void Rectangle::clear()
{
	printf("clear\n");
}

Rectangle::~Rectangle(void)
{
	clear();
	GL_CHECK(glDeleteTextures(6,m_uiTexture));
}
