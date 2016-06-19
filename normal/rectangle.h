#ifndef RECTANGLE_H
#define RECTANGLE_H
#include <GLES2/gl2.h>
#include <string>

class Rectangle
{
private:
	static const std::string resourceDirectory;
	static const std::string vertexShaderFilename;
	static const std::string fragmentShaderFilename;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint programID;
	GLuint m_uiTexture[6];
	GLuint m_ui32Vbo;
	int m_iLocPosition;
public:
	void draw(unsigned char * otheraddr);
	void clear(void);
	Rectangle(unsigned char *cfront,unsigned char *crear,unsigned char *cleft,unsigned char * cright);
	virtual ~Rectangle();
};
#endif
