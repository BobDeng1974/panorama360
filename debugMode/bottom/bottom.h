#ifndef _BOTTOM_H
#define _BOTTOM_H

#include "stdio.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <time.h>

#define IMAGE_WIDTH (720)			//源图像宽
#define IMAGE_HEIGHT (480)			//源图像高	

#define STITCH_IMAGE_WIDTH (300)	//拼接图像宽
#define STITCH_IMAGE_HEIGHT (480)	//拼接图像高	

#define CHANNELS (4)				//图像通道	

#define DIRECTION_FRONT 1			//方向前
#define DIRECTION_REAR	2			//方向后	
#define DIRECTION_LEFT	3			//方向左
#define DIRECTION_RIGHT	4			//方向右

#define CUT_LEFT_TOP		4		//裁剪左上角
#define CUT_LEFT_BOTTOM		5		//裁剪左下角
#define CUT_RIGHT_TOP		6		//裁剪右上角
#define CUT_RIGHT_BOTTOM	7		//裁剪右下角

/************************************************************************/
/*				摄像头内外参数：根据不同分辨率选择                      */
/*				有720*480的和360*240两种								*/
/************************************************************************/
#if IMAGE_WIDTH == (720)
#define INTRINSICS_FILE_NAME_FRONT		".//bottom//lensParameter//Intrinsics720front.xml"
#define DISTORTION_FILE_NAME_FRONT		".//bottom//lensParameter//Distortion720front.xml"
#define INTRINSICS_FILE_NAME_REAR		".//bottom//lensParameter//Intrinsics720rear.xml"
#define DISTORTION_FILE_NAME_REAR		".//bottom//lensParameter//Distortion720rear.xml"
#define INTRINSICS_FILE_NAME_LEFT		".//bottom//lensParameter//Intrinsics720left.xml"
#define DISTORTION_FILE_NAME_LEFT		".//bottom//lensParameter//Distortion720left.xml"
#define INTRINSICS_FILE_NAME_RIGHT		".//bottom//lensParameter//Intrinsics720right.xml"
#define DISTORTION_FILE_NAME_RIGHT		".//bottom//lensParameter//Distortion720right.xml"
#define MARKPOINT_FILE_NAME				".//bottom//markpoint//markpoint.xml"
#else 
#define INTRINSICS_FILE_NAME_FRONT		".//bottom//lensParameter//Intrinsics360front.xml"
#define DISTORTION_FILE_NAME_FRONT		".//bottom//lensParameter//Distortion360front.xml"
#define INTRINSICS_FILE_NAME_REAR		".//bottom//lensParameter//Intrinsics360rear.xml"
#define DISTORTION_FILE_NAME_REAR		".//bottom//lensParameter//Distortion360rear.xml"
#define INTRINSICS_FILE_NAME_LEFT		".//bottom//lensParameter//Intrinsics360left.xml"
#define DISTORTION_FILE_NAME_LEFT		".//bottom//lensParameter//Distortion360left.xml"
#define INTRINSICS_FILE_NAME_RIGHT		".//bottom//lensParameter//Intrinsics360right.xml"
#define DISTORTION_FILE_NAME_RIGHT		".//bottom//lensParameter//Distortion360right.xml"
#define MARKPOINT_FILE_NAME				".//bottom//markpoint//markpoint360.xml"
#endif

//棋格参数文件名
#define FRONT_BOARD_PARAM_FILE_NAME		".//bottom//boardParameter//frontboardparam.xml"
#define REAR_BOARD_PARAM_FILE_NAME		".//bottom//boardParameter//rearboardparam.xml"
#define LEFT_BOARD_PARAM_FILE_NAME		".//bottom//boardParameter//leftboardparam.xml"
#define RIGHT_BOARD_PARAM_FILE_NAME		".//bottom//boardParameter//rightboardparam.xml"
//全景表坐标文件名
#define FRONT_COORDINATE_X_FILE_NAME	".//bottom//map//frontcoordinatex.xml"
#define REAR_COORDINATE_X_FILE_NAME		".//bottom//map//rearcoordinatex.xml"
#define LEFT_COORDINATE_X_FILE_NAME		".//bottom//map//leftcoordinatex.xml"
#define RIGHT_COORDINATE_X_FILE_NAME	".//bottom//map//rightcoordinatex.xml"
#define FRONT_COORDINATE_Y_FILE_NAME	".//bottom//map//frontcoordinatey.xml"
#define REAR_COORDINATE_Y_FILE_NAME		".//bottom//map//rearcoordinatey.xml"
#define LEFT_COORDINATE_Y_FILE_NAME		".//bottom//map//leftcoordinatey.xml"
#define RIGHT_COORDINATE_Y_FILE_NAME	".//bottom//map//rightcoordinatey.xml"
//全景表权值文件名
#define FRONT_WEIGHT_FILE_NAME			".//bottom//map//frontweight.xml"
#define REAR_WEIGHT_FILE_NAME			".//bottom//map//rearweight.xml"
#define LEFT_WEIGHT_FILE_NAME			".//bottom//map//leftweight.xml"
#define RIGHT_WEIGHT_FILE_NAME			".//bottom//map//rightweight.xml"
bool GetUndistortMap(IplImage **mapx, IplImage **mapy, int direction);
bool RemapImage(IplImage *srcImage, IplImage *dstImage, IplImage *mapx, IplImage *mapy);
bool ReadBoardParam(float *param, char *filename);
bool ReadMarkpoint(CvPoint2D32f *frontmarkpoint, CvPoint2D32f *rearmarkpoint, CvPoint2D32f *leftmarkpoint,
	CvPoint2D32f *rightmarkpoint, char *filename);
bool FindBirdImage(IplImage *srcImage, IplImage *birdImage, int board_w, int board_h, CvPoint2D32f *corners, 
	CvMat* matrix, float scale);
bool GetCorners(IplImage* srcImage, CvPoint2D32f* corners, int *corner_count, int board_w, int board_h);
void sortcorner(CvPoint2D32f* corners, int corner_count);
bool DLT_Transform( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n, CvMat *H);
bool GetBirdMatrix(IplImage* srcImage, IplImage *birdImage, CvPoint2D32f *corners,
	CvMat **matrix, float scale, int board_w, int board_h);
bool TransformPoint(CvPoint2D32f* srcpoint, int cnt, CvMat* H, CvPoint2D32f* dstPoint);
bool CalculateMarkpoint(IplImage *srcImage, IplImage *birdImage, CvPoint2D32f *corners, CvPoint2D32f *srcMarkpoint, CvPoint2D32f *birdMarkpoint,
	int nboard_w, int nboard_h, float fboardwidth, float fboardheight, CvMat * matrix);
void findmarkpointsubpix(IplImage* srcImage, CvPoint2D32f *markpoint, int count, CvSize win);
bool GetStitchMat(IplImage *frontImage, IplImage *rearImage, IplImage *leftImage, IplImage *rightImage, CvPoint2D32f *frontbirdMarkpoint,
	CvPoint2D32f *rearbirdMarkpoint, CvPoint2D32f *leftbirdMarkpoint, CvPoint2D32f *rightbirdMarkpoint, CvMat **frontStitchMat, CvMat **rearStitchMat,
	CvMat **leftStitchMat, CvMat **rightStitchMat, IplImage *stitchImage);
void drawpoint(IplImage* src, CvPoint point);
bool GetMarkPointInStitchImage(CvPoint2D32f *frontbirdMarkpoint, CvPoint2D32f *rearbirdMarkpoint, CvPoint2D32f *leftbirdMarkpoint,
	CvPoint2D32f *rightbirdMarkpoint, IplImage * stitchImage, CvPoint2D32f *stitchMarkpoint);
bool GetRotationMat(float angle, CvMat **rotationMat, int imagewidth, int imageheight);
bool GetMoveMat(int xdistance, int ydistance, CvMat **moveMat);
bool GetMarkPointInStitchImage2(CvPoint2D32f *frontbirdMarkpoint, CvPoint2D32f *rearbirdMarkpoint, CvPoint2D32f *leftbirdMarkpoint,
	CvPoint2D32f *rightbirdMarkpoint, IplImage * stitchImage, CvPoint2D32f *stitchMarkpoint);
bool cutImage(IplImage* srcImage, CvPoint2D32f point1, CvPoint2D32f point2, int direction);
bool JudgeIsInside(CvPoint testPoint, CvPoint2D32f *polygon, int n);
bool GetDistorPoint(CvPoint2D32f *srcPoint, int n, CvPoint2D32f *dstPoint, double fx, double fy, double cx, double cy,
	double k1, double k2, double k3, double p1, double p2);

#endif



