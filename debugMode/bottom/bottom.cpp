#include "bottom.h"

/************************************************************************/
/*				计算摄像头畸变校正转换矩阵                              */
/*              IplImage   **mapx:    x坐标的对应矩阵（out）            */
/*              IplImage   **mapy:    y坐标的对应矩阵 （out）           */
/************************************************************************/
bool GetUndistortMap(IplImage **mapx, IplImage **mapy, int direction)
{
	CvMat *intrinsic_matric = NULL, *distortion_coeffs = NULL;
	//读取摄像头参数
	switch(direction)
	{
	case DIRECTION_FRONT:
		intrinsic_matric = (CvMat*)cvLoad(INTRINSICS_FILE_NAME_FRONT);
		distortion_coeffs = (CvMat*)cvLoad(DISTORTION_FILE_NAME_FRONT);
		break;
	case DIRECTION_REAR:
		intrinsic_matric = (CvMat*)cvLoad(INTRINSICS_FILE_NAME_REAR);
		distortion_coeffs = (CvMat*)cvLoad(DISTORTION_FILE_NAME_REAR);
		break;
	case DIRECTION_LEFT:
		intrinsic_matric = (CvMat*)cvLoad(INTRINSICS_FILE_NAME_LEFT);
		distortion_coeffs = (CvMat*)cvLoad(DISTORTION_FILE_NAME_LEFT);
		break;
	case DIRECTION_RIGHT:
		intrinsic_matric = (CvMat*)cvLoad(INTRINSICS_FILE_NAME_RIGHT);
		distortion_coeffs = (CvMat*)cvLoad(DISTORTION_FILE_NAME_RIGHT);
		break;
	default:
		printf("param error\n");
		return false;
	}
	if (!intrinsic_matric || !distortion_coeffs)
	{
		printf("load mat failed\n");
		return false;
	}
	*mapx = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), IPL_DEPTH_32F, 1);
	*mapy = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), IPL_DEPTH_32F, 1);
	if (!*mapx || !*mapy)
	{
		printf("createImage failed\n");
		return false;
	}
	cvInitUndistortMap(intrinsic_matric, distortion_coeffs, *mapx, *mapy);

	cvReleaseMat(&intrinsic_matric);
	cvReleaseMat(&distortion_coeffs);

	return true;
}
/************************************************************************/
/*                      根据转换矩阵校正畸变图像                        */
/*		IplImage *srcImage: 畸变图像									*/
/*		IplImage *dstImage: 校正后的图像								*/
/*      IplImage   **mapx:    x坐标的对应矩阵（out）				    */
/*      IplImage   **mapy:    y坐标的对应矩阵 （out）				    */
/************************************************************************/
bool RemapImage(IplImage *srcImage, IplImage *dstImage, IplImage *mapx, IplImage *mapy)
{
	if (!srcImage || !dstImage || !mapx ||!mapy)
	{
		return false;
	}
	cvRemap(srcImage, dstImage, mapx, mapy);

	return true;
}
/************************************************************************/
/*				读取棋盘格参数                                          */
/*				param[0]: nboard_w										*/
/*				param[1]: nboard_h										*/
/*				param[2]: fboardwidth									*/
/*				param[3]: fboardheight									*/
/************************************************************************/
bool ReadBoardParam(float *param, char *filename)
{
	if (!param || !filename)
	{
		return false;
	}
	CvMat *boardparam = (CvMat*)cvLoad(filename);
	if (!boardparam)
	{
		return false;
	}
	param[0] = CV_MAT_ELEM(*boardparam, float, 0, 0);
	param[1] = CV_MAT_ELEM(*boardparam, float, 1, 0);
	param[2] = CV_MAT_ELEM(*boardparam, float, 2, 0);
	param[3] = CV_MAT_ELEM(*boardparam, float, 3, 0);

	cvReleaseMat(&boardparam);
	return true;
}
/************************************************************************/
/*				读取标记点位置坐标                                      */
/************************************************************************/
bool ReadMarkpoint(CvPoint2D32f *frontmarkpoint, CvPoint2D32f *rearmarkpoint, CvPoint2D32f *leftmarkpoint,
			CvPoint2D32f *rightmarkpoint, char *filename)
{
	if (!frontmarkpoint || !rearmarkpoint || !leftmarkpoint || !rightmarkpoint || !filename)
	{
		return false;
	}
	CvMat *tmpmat = (CvMat*)cvLoad(filename);
	if (!tmpmat)
	{
		printf("read markpoint failed\n");
		return false;
	}
	frontmarkpoint[0].x = CV_MAT_ELEM(*tmpmat, float, 0, 0);
	frontmarkpoint[0].y = CV_MAT_ELEM(*tmpmat, float, 0, 1);
	frontmarkpoint[1].x = CV_MAT_ELEM(*tmpmat, float, 0, 2);
	frontmarkpoint[1].y = CV_MAT_ELEM(*tmpmat, float, 0, 3);
	frontmarkpoint[2].x = CV_MAT_ELEM(*tmpmat, float, 0, 4);
	frontmarkpoint[2].y = CV_MAT_ELEM(*tmpmat, float, 0, 5);
	frontmarkpoint[3].x = CV_MAT_ELEM(*tmpmat, float, 0, 6);
	frontmarkpoint[3].y = CV_MAT_ELEM(*tmpmat, float, 0, 7);

	rearmarkpoint[0].x = CV_MAT_ELEM(*tmpmat, float, 1, 0);
	rearmarkpoint[0].y = CV_MAT_ELEM(*tmpmat, float, 1, 1);
	rearmarkpoint[1].x = CV_MAT_ELEM(*tmpmat, float, 1, 2);
	rearmarkpoint[1].y = CV_MAT_ELEM(*tmpmat, float, 1, 3);
	rearmarkpoint[2].x = CV_MAT_ELEM(*tmpmat, float, 1, 4);
	rearmarkpoint[2].y = CV_MAT_ELEM(*tmpmat, float, 1, 5);
	rearmarkpoint[3].x = CV_MAT_ELEM(*tmpmat, float, 1, 6);
	rearmarkpoint[3].y = CV_MAT_ELEM(*tmpmat, float, 1, 7);

	leftmarkpoint[0].x = CV_MAT_ELEM(*tmpmat, float, 2, 0);
	leftmarkpoint[0].y = CV_MAT_ELEM(*tmpmat, float, 2, 1);
	leftmarkpoint[1].x = CV_MAT_ELEM(*tmpmat, float, 2, 2);
	leftmarkpoint[1].y = CV_MAT_ELEM(*tmpmat, float, 2, 3);
	leftmarkpoint[2].x = CV_MAT_ELEM(*tmpmat, float, 2, 4);
	leftmarkpoint[2].y = CV_MAT_ELEM(*tmpmat, float, 2, 5);
	leftmarkpoint[3].x = CV_MAT_ELEM(*tmpmat, float, 2, 6);
	leftmarkpoint[3].y = CV_MAT_ELEM(*tmpmat, float, 2, 7);

	rightmarkpoint[0].x = CV_MAT_ELEM(*tmpmat, float, 3, 0);
	rightmarkpoint[0].y = CV_MAT_ELEM(*tmpmat, float, 3, 1);
	rightmarkpoint[1].x = CV_MAT_ELEM(*tmpmat, float, 3, 2);
	rightmarkpoint[1].y = CV_MAT_ELEM(*tmpmat, float, 3, 3);
	rightmarkpoint[2].x = CV_MAT_ELEM(*tmpmat, float, 3, 4);
	rightmarkpoint[2].y = CV_MAT_ELEM(*tmpmat, float, 3, 5);
	rightmarkpoint[3].x = CV_MAT_ELEM(*tmpmat, float, 3, 6);
	rightmarkpoint[3].y = CV_MAT_ELEM(*tmpmat, float, 3, 7);

	cvReleaseMat(&tmpmat);
	return true;
}
/************************************************************************/
/*               获取俯视图                                             */
/************************************************************************/
bool FindBirdImage(IplImage *srcImage, IplImage *birdImage, int board_w, int board_h, CvPoint2D32f *corners, 
		CvMat* matrix, float scale)
{
	if (!srcImage || !birdImage || !corners || !matrix)
	{
		printf("param error\n");
		return false;
	}
	int corner_count = 0;
	
	if (!GetCorners(srcImage, corners, &corner_count, board_w, board_h)) //检测棋盘
	{
		//如果检测不到棋盘，则立即返回
		printf("couldn't aquire chessboard\n only found %d corners\n", corner_count);
		for (int j = 0; j < corner_count; ++j)
		{
			cvCircle(srcImage, cvPointFrom32f(corners[j]), 2, cvScalar(0,0,255), 1);
		}
		return false;
	}
	sortcorner(corners, corner_count);	//调整棋盘点保存顺序
	GetBirdMatrix(srcImage, birdImage, corners, &matrix, scale, board_w, board_h);//计算俯视图变换矩阵
	cvWarpPerspective(srcImage, birdImage, matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS , cvScalarAll(0));//进行俯视图变换

	return true;
}
/************************************************************************/
/*				查找棋盘                                                */
/************************************************************************/
bool GetCorners(IplImage* srcImage, CvPoint2D32f* corners, int *corner_count, int board_w, int board_h)
{
	if (!srcImage || !corners || !corner_count)
	{
		return false;
	}

	int found = cvFindChessboardCorners(srcImage,cvSize(board_w, board_h), 
		corners, corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);//检测棋盘
	if (!found)
	{
		return false;			//检测不到棋盘，立即返回
	}
	//精确棋盘角点坐标
	IplImage* grayImage = cvCreateImage(cvSize(srcImage->width, srcImage->height), IPL_DEPTH_8U, 1);
	cvCvtColor(srcImage, grayImage, CV_BGR2GRAY);
	cvFindCornerSubPix(grayImage, corners, *corner_count, cvSize(11,11), 
		cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));	//求棋盘角点亚像素坐标
	cvReleaseImage(&grayImage);

	return true;
}
/************************************************************************/
/*              排序棋盘点                                              */
/************************************************************************/
void sortcorner(CvPoint2D32f* corners, int corner_count)
{
	int i = 0;
	int j = 0;
	CvPoint2D32f* tmp = new CvPoint2D32f[corner_count];
	//为保证以棋盘左上角为首点的顺序进行存储棋盘角点
	if (corners[corner_count-1].y > corners[0].y)
	{
		return;
	}else
	{
		for (i = 0, j = corner_count-1; i < corner_count; ++i, --j)
		{
			tmp[i] = corners[j];
		}
		for (i = 0; i < corner_count; ++i)
		{
			corners[i] = tmp[i];
		}
	}
	delete[] tmp;
}
/************************************************************************/
/*              求DLT变换矩阵                                           */
/************************************************************************/
bool DLT_Transform( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n, CvMat *H)
{
	CvMat *A, * B, X;
	double x[9];//数组x中的元素就是变换矩阵H中的值
	int i;

	//输入点对个数不够4
	if( n < 4 )
	{
		fprintf( stderr, "Warning: too few points in lsq_homog(), %s line %d\n",
			__FILE__, __LINE__ );
		return false;
	}

	//将变换矩阵H展开到一个8维列向量X中，使得AX=B，这样只需一次解线性方程组即可求出X，然后再根据X恢复H
	/* set up matrices so we can unstack homography into X; AX = B */
	A = cvCreateMat( 2*n, 8, CV_64FC1 );//创建2n*8的矩阵，一般是8*8
	B = cvCreateMat( 2*n, 1, CV_64FC1 );//创建2n*1的矩阵，一般是8*1
	X = cvMat( 8, 1, CV_64FC1, x );//创建8*1的矩阵，指定数据为x
	//H = cvCreateMat(3, 3, CV_64FC1);//创建3*3的矩阵
	cvZero( A );//将A清零

	//由于是展开计算，需要根据原来的矩阵计算法则重新分配矩阵A和B的值的排列
	for( i = 0; i < n; i++ )
	{
		cvmSet( A, i, 0, pts[i].x );//设置矩阵A的i行0列的值为pts[i].x
		cvmSet( A, i+n, 3, pts[i].x );
		cvmSet( A, i, 1, pts[i].y );
		cvmSet( A, i+n, 4, pts[i].y );
		cvmSet( A, i, 2, 1.0 );
		cvmSet( A, i+n, 5, 1.0 );
		cvmSet( A, i, 6, -pts[i].x * mpts[i].x );
		cvmSet( A, i, 7, -pts[i].y * mpts[i].x );
		cvmSet( A, i+n, 6, -pts[i].x * mpts[i].y );
		cvmSet( A, i+n, 7, -pts[i].y * mpts[i].y );
		cvmSet( B, i, 0, mpts[i].x );
		cvmSet( B, i+n, 0, mpts[i].y );
	}

	//调用OpenCV函数，解线性方程组
	cvSolve( A, B, &X, CV_SVD );//求X，使得AX=B
	x[8] = 1.0;//变换矩阵的[3][3]位置的值为固定值1
	X = cvMat( 3, 3, CV_64FC1, x );
	cvConvert( &X, H );//将数组转换为矩阵

	cvReleaseMat( &A );
	cvReleaseMat( &B );
	return true;
}
/************************************************************************/
/*             求俯视图变换矩阵                                         */
/*				IplImage *srcImage		: 源图像						*/
/*				IplImage *birdImage		: 俯视图						*/
/*				CvPoint2D32f *corners	: 棋盘点						*/
/*				CvMat **matrix			: 变换矩阵						*/
/*				float scale				: 缩放比例						*/
/*				int board_w				: 棋盘宽						*/
/*				int board_h				: 棋盘高						*/
/************************************************************************/
bool GetBirdMatrix(IplImage* srcImage, IplImage *birdImage, CvPoint2D32f *corners,
		CvMat **matrix, float scale, int board_w, int board_h)
{
	if (!srcImage || !corners || !matrix)
	{
		return false;
	}

	CvPoint2D64f objPts[4], imgPts[4];			//透视图与俯视图相对应的四点，imgPts[4]是透视图四个棋盘角点，objPts[4]为棋盘角点在俯视图中的相应坐标
	imgPts[0] = cvPoint2D64f(corners[0].x, corners[0].y);
	imgPts[1] = cvPoint2D64f(corners[board_w-1].x, corners[board_w-1].y);
	imgPts[2] = cvPoint2D64f(corners[(board_h-1)*board_w].x, corners[(board_h-1)*board_w].y);
	imgPts[3] = cvPoint2D64f(corners[(board_h-1)*board_w+board_w-1].x, corners[(board_h-1)*board_w+board_w-1].y);

AdjustScale:	objPts[0] = cvPoint2D64f(0, 0);
	objPts[1] = cvPoint2D64f((board_w-1)*scale, 0);
	objPts[2] = cvPoint2D64f(0, (board_h-1)*scale);
	objPts[3] = cvPoint2D64f((board_w-1)*scale, (board_h-1)*scale);

	DLT_Transform(imgPts, objPts, 4, *matrix);//计算变换矩阵

	CvPoint2D32f src[4];					//透视图的四个顶点
	CvPoint2D32f dstpoint[4];
	src[0] = cvPoint2D32f(0, 0);
	src[1] = cvPoint2D32f(srcImage->width, 0);
	src[2] = cvPoint2D32f(0, srcImage->height);
	src[3] = cvPoint2D32f(srcImage->width, srcImage->height);
	TransformPoint(src, 4, *matrix, dstpoint);	//计算透视图四个顶点变换后的对应点坐标

	//旋转目标点
	double kk = atan(fabs((dstpoint[3].y - dstpoint[2].y)/(dstpoint[3].x - dstpoint[2].x)));
	if (dstpoint[3].y > dstpoint[2].y)
	{
		objPts[0].x = 0;
		objPts[0].y = scale*(board_w-1)*sin(kk);
		objPts[1].x = scale*(board_w-1)*cos(kk);
		objPts[1].y = 0;
		objPts[2].x = scale*(board_h-1)*sin(kk);
		objPts[2].y = scale*(board_h-1)*cos(kk)+objPts[0].y;
		objPts[3].x = scale*(board_h-1)*sin(kk)+objPts[1].x;
		objPts[3].y = scale*(board_h-1)*cos(kk);
	}else
	{
		objPts[0].x = scale*(board_w-1)*sin(kk);
		objPts[0].y = 0;
		objPts[1].x = scale*(board_w-1)*cos(kk)+objPts[0].x;
		objPts[1].y = scale*(board_w-1)*sin(kk);
		objPts[2].x = 0;
		objPts[2].y = scale*(board_h-1)*cos(kk);
		objPts[3].x = scale*(board_w-1)*cos(kk);
		objPts[3].y = scale*(board_w-1)*sin(kk)+objPts[2].y;
	}
	DLT_Transform(imgPts, objPts, 4, *matrix);//计算变换矩阵
	TransformPoint(src, 4, *matrix, dstpoint);//计算透视图四个顶点变换后的对应点坐标
	if (abs(dstpoint[2].x-dstpoint[3].x) >= srcImage->width)
	{
		scale--;
		goto AdjustScale;							//缩放比例太大，重新计算
	}
	//平移目标点
	float xoffset, yoffset;
	xoffset = (birdImage->width-abs(dstpoint[3].x-dstpoint[2].x))/2-dstpoint[2].x;
	yoffset = birdImage->height-dstpoint[2].y;

	for (int i = 0 ; i < 4; ++i)
	{
		objPts[i].x += xoffset;
		objPts[i].y += yoffset;
	}
	DLT_Transform(imgPts, objPts, 4, *matrix);//计算变换矩阵

	return true;
}
/************************************************************************/
/*           计算点经矩阵H变换后的坐标									 */
/************************************************************************/
bool TransformPoint(CvPoint2D32f* srcpoint, int cnt, CvMat* H, CvPoint2D32f* dstPoint)
{
	if (!srcpoint || !H || !dstPoint)
	{
		return false;
	}
	//防止srcpoint与dstPoint指向同一片内存，拷贝输入点到临时内存中
	CvPoint2D32f *tmp = new CvPoint2D32f[cnt];
	for (int i = 0; i < cnt; ++i)
	{
		tmp[i].x = srcpoint[i].x;
		tmp[i].y = srcpoint[i].y;
	}
	double v2[] = {0, 0, 1};								
	double v1[3];
	//计算输入点经矩阵H变换后的坐标
	for (int i = 0; i < cnt; ++i)
	{
		v2[0] = tmp[i].x;
		v2[1] = tmp[i].y;
		CvMat V2 = cvMat(3, 1, CV_64FC1, v2);
		CvMat V1 = cvMat(3, 1, CV_64FC1, v1);
		cvGEMM(H, &V2, 1, 0, 1, &V1);							
		dstPoint[i].x = v1[0]/v1[2];					
		dstPoint[i].y = v1[1]/v1[2];
	}
	delete[] tmp;

	return true;
}
/************************************************************************/
/*			计算俯视图中标记点坐标                                      */
/************************************************************************/
bool CalculateMarkpoint(IplImage *undistorImage, IplImage *birdImage, CvPoint2D32f *corners, CvPoint2D32f *srcMarkpoint, CvPoint2D32f *birdMarkpoint,
	int nboard_w, int nboard_h, float fboardwidth, float fboardheight, CvMat * matrix)
{
	if (!birdImage || !corners || !srcMarkpoint || !birdMarkpoint)
	{
		return false;
	}
	//设置三个参考点，参考点1为棋盘左下角角点，参考点2为棋盘右下角角点，参考点三为棋盘右上角角点
	CvPoint2D32f referpoint1 =corners[nboard_w*(nboard_h-1)+nboard_w-1];
	CvPoint2D32f referpoint2 = corners[nboard_w*(nboard_h-1)];
	CvPoint2D32f referpoint3 = corners[nboard_w-1];
	//计算ux: 图像坐标系x方向单位像素
	//计算uy: 图像标标系y方向
	float ux = sqrt((referpoint1.x-referpoint2.x)*(referpoint1.x-referpoint2.x)+
		(referpoint1.y-referpoint2.y)*(referpoint1.y-referpoint2.y))/fboardwidth;
	float uy = sqrt((referpoint1.x-referpoint3.x)*(referpoint1.x-referpoint3.x)+
		(referpoint1.y-referpoint3.y)*(referpoint1.y-referpoint3.y))/fboardheight;

	birdMarkpoint[0].x = referpoint2.x - srcMarkpoint[0].x*ux;
	birdMarkpoint[0].y = referpoint2.y - srcMarkpoint[0].y*uy;
	birdMarkpoint[1].x = referpoint2.x - srcMarkpoint[1].x*ux;
	birdMarkpoint[1].y = referpoint2.y - srcMarkpoint[1].y*uy;
	birdMarkpoint[2].x = referpoint1.x + srcMarkpoint[2].x*ux;
	birdMarkpoint[2].y = referpoint1.y - srcMarkpoint[2].y*uy;
	birdMarkpoint[3].x = referpoint1.x + srcMarkpoint[3].x*ux;
	birdMarkpoint[3].y = referpoint1.y - srcMarkpoint[3].y*uy;

	CvMat *inmatrix = cvCreateMat(3, 3, CV_64FC1); 
	cvInvert(matrix, inmatrix, CV_SVD);
	TransformPoint(birdMarkpoint, 4, inmatrix, birdMarkpoint);
	findmarkpointsubpix(undistorImage, birdMarkpoint, 4, cvSize(10, 10));
	TransformPoint(birdMarkpoint, 4, matrix, birdMarkpoint);
	cvReleaseMat(&inmatrix);
	return true;
}
/************************************************************************/
/*           精确到亚像素                                               */
/************************************************************************/
void findmarkpointsubpix(IplImage* srcImage, CvPoint2D32f *markpoint, int count, CvSize win)
{
	IplImage* grayImage = cvCreateImage(cvSize(srcImage->width, srcImage->height), IPL_DEPTH_8U, 1);
	cvCvtColor(srcImage, grayImage, CV_BGR2GRAY);
	cvFindCornerSubPix(grayImage, markpoint, count, win, cvSize(-1,-1), 
		cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 50, 0.01));
	cvReleaseImage(&grayImage);
}
/************************************************************************/
/*          计算映射到全景图的映射矩阵		                            */
/************************************************************************/
bool GetStitchMat(IplImage *frontImage, IplImage *rearImage, IplImage *leftImage, IplImage *rightImage, CvPoint2D32f *frontbirdMarkpoint,
	CvPoint2D32f *rearbirdMarkpoint, CvPoint2D32f *leftbirdMarkpoint, CvPoint2D32f *rightbirdMarkpoint, CvMat **frontStitchMat, CvMat **rearStitchMat,
	CvMat **leftStitchMat, CvMat **rightStitchMat, IplImage *stitchImage)
{
	if (!frontImage || !rearImage || !leftImage || !rightImage || !frontbirdMarkpoint || !rearbirdMarkpoint || !leftbirdMarkpoint || 
		!rightbirdMarkpoint || !frontStitchMat || !rearStitchMat || !leftStitchMat || !rightStitchMat || !stitchImage)
	{
		return false;
	}
	//优化拼接缝
	CvPoint2D32f stitchMarkpoint[8];
	GetMarkPointInStitchImage(frontbirdMarkpoint, rearbirdMarkpoint, leftbirdMarkpoint, rightbirdMarkpoint, stitchImage, stitchMarkpoint);
	//计算标记点在全景图中的坐标位置
	CvPoint2D64f lefttop1, leftbottom1, righttop1, rightbottom1, lefttop2, leftbottom2, righttop2, rightbottom2;

	lefttop1		= cvPoint2D64f(stitchMarkpoint[0].x, stitchMarkpoint[0].y);
	lefttop2		= cvPoint2D64f(stitchMarkpoint[1].x, stitchMarkpoint[1].y);
	leftbottom1		= cvPoint2D64f(stitchMarkpoint[2].x, stitchMarkpoint[2].y);
	leftbottom2		= cvPoint2D64f(stitchMarkpoint[3].x, stitchMarkpoint[3].y);
	righttop1		= cvPoint2D64f(stitchMarkpoint[4].x, stitchMarkpoint[4].y);
	righttop2		= cvPoint2D64f(stitchMarkpoint[5].x, stitchMarkpoint[5].y);
	rightbottom1	= cvPoint2D64f(stitchMarkpoint[6].x, stitchMarkpoint[6].y);
	rightbottom2	= cvPoint2D64f(stitchMarkpoint[7].x, stitchMarkpoint[7].y);

	//drawpoint(stitchImage, cvPoint(lefttop1.x, lefttop1.y));
	//drawpoint(stitchImage, cvPoint(lefttop2.x, lefttop2.y));
	//drawpoint(stitchImage, cvPoint(leftbottom1.x, leftbottom1.y));
	//drawpoint(stitchImage, cvPoint(leftbottom2.x, leftbottom2.y));
	//drawpoint(stitchImage, cvPoint(righttop1.x, righttop1.y));
	//drawpoint(stitchImage, cvPoint(righttop2.x, righttop2.y));
	//drawpoint(stitchImage, cvPoint(rightbottom1.x, rightbottom1.y));
	//drawpoint(stitchImage, cvPoint(rightbottom2.x, rightbottom2.y));

	CvPoint2D64f srcpoint[4], dstpoint[4];

	//计算前向图像映射矩阵
	srcpoint[0].x = frontbirdMarkpoint[0].x;
	srcpoint[0].y = frontbirdMarkpoint[0].y;
	srcpoint[1].x = frontbirdMarkpoint[1].x;
	srcpoint[1].y = frontbirdMarkpoint[1].y;
	srcpoint[2].x = frontbirdMarkpoint[2].x;
	srcpoint[2].y = frontbirdMarkpoint[2].y;
	srcpoint[3].x = frontbirdMarkpoint[3].x;
	srcpoint[3].y = frontbirdMarkpoint[3].y;

	dstpoint[0].x = lefttop2.x;
	dstpoint[0].y = lefttop2.y;
	dstpoint[1].x = lefttop1.x;
	dstpoint[1].y = lefttop1.y;
	dstpoint[2].x = righttop2.x;
	dstpoint[2].y = righttop2.y;
	dstpoint[3].x = righttop1.x;
	dstpoint[3].y = righttop1.y;
	DLT_Transform(srcpoint, dstpoint, 4, *frontStitchMat);
	//计算后向图像映射矩阵
	srcpoint[0].x = rearbirdMarkpoint[0].x;
	srcpoint[0].y = rearbirdMarkpoint[0].y;
	srcpoint[1].x = rearbirdMarkpoint[1].x;
	srcpoint[1].y = rearbirdMarkpoint[1].y;
	srcpoint[2].x = rearbirdMarkpoint[2].x;
	srcpoint[2].y = rearbirdMarkpoint[2].y;
	srcpoint[3].x = rearbirdMarkpoint[3].x;
	srcpoint[3].y = rearbirdMarkpoint[3].y;

	dstpoint[0].x = rightbottom2.x;
	dstpoint[0].y = rightbottom2.y;
	dstpoint[1].x = rightbottom1.x;
	dstpoint[1].y = rightbottom1.y;
	dstpoint[2].x = leftbottom2.x;
	dstpoint[2].y = leftbottom2.y;
	dstpoint[3].x = leftbottom1.x;
	dstpoint[3].y = leftbottom1.y;
	DLT_Transform(srcpoint, dstpoint, 4, *rearStitchMat);
	//计算左向图像映射矩阵
	srcpoint[0].x = leftbirdMarkpoint[0].x;
	srcpoint[0].y = leftbirdMarkpoint[0].y;
	srcpoint[1].x = leftbirdMarkpoint[1].x;
	srcpoint[1].y = leftbirdMarkpoint[1].y;
	srcpoint[2].x = leftbirdMarkpoint[2].x;
	srcpoint[2].y = leftbirdMarkpoint[2].y;
	srcpoint[3].x = leftbirdMarkpoint[3].x;
	srcpoint[3].y = leftbirdMarkpoint[3].y;

	dstpoint[0].x = leftbottom2.x;
	dstpoint[0].y = leftbottom2.y;
	dstpoint[1].x = leftbottom1.x;
	dstpoint[1].y = leftbottom1.y;
	dstpoint[2].x = lefttop2.x;
	dstpoint[2].y = lefttop2.y;
	dstpoint[3].x = lefttop1.x;
	dstpoint[3].y = lefttop1.y;
	DLT_Transform(srcpoint, dstpoint, 4, *leftStitchMat);
	//计算右向图像映射矩阵
	srcpoint[0].x = rightbirdMarkpoint[0].x;
	srcpoint[0].y = rightbirdMarkpoint[0].y;
	srcpoint[1].x = rightbirdMarkpoint[1].x;
	srcpoint[1].y = rightbirdMarkpoint[1].y;
	srcpoint[2].x = rightbirdMarkpoint[2].x;
	srcpoint[2].y = rightbirdMarkpoint[2].y;
	srcpoint[3].x = rightbirdMarkpoint[3].x;
	srcpoint[3].y = rightbirdMarkpoint[3].y;

	dstpoint[0].x = righttop2.x;
	dstpoint[0].y = righttop2.y;
	dstpoint[1].x = righttop1.x;
	dstpoint[1].y = righttop1.y;
	dstpoint[2].x = rightbottom2.x;
	dstpoint[2].y = rightbottom2.y;
	dstpoint[3].x = rightbottom1.x;
	dstpoint[3].y = rightbottom1.y;
	DLT_Transform(srcpoint, dstpoint, 4, *rightStitchMat);

	return true;
}
/************************************************************************/
/*          描点                                                         */
/************************************************************************/
void drawpoint(IplImage* src, CvPoint point)
{
	cvLine(src, cvPoint(point.x-4, point.y), cvPoint(point.x+4, point.y), cvScalar(0,0,255), 1);
	cvLine(src, cvPoint(point.x, point.y-4), cvPoint(point.x, point.y+4), cvScalar(0,0,255), 1);
}
/************************************************************************/
/*         计算标记点在全景图中的坐标                                   */
/************************************************************************/
bool GetMarkPointInStitchImage(CvPoint2D32f *frontbirdMarkpoint, CvPoint2D32f *rearbirdMarkpoint, CvPoint2D32f *leftbirdMarkpoint,
	CvPoint2D32f *rightbirdMarkpoint, IplImage * stitchImage, CvPoint2D32f *stitchMarkpoint)
{
	if (!frontbirdMarkpoint || !rearbirdMarkpoint || !leftbirdMarkpoint || !rightbirdMarkpoint || !stitchMarkpoint || !stitchMarkpoint)
	{
		return false;
	}
	/************************************************************************/
	/*                      ******************                              */
	/*                      * *     1      * *                              */
	/*                      *   *  ***  *    *                              */
	/*                      *  3   * *     4 *                              */
	/*                      *    * ***  *    *                              */
	/*                      * *     2      * *                              */
	/*                      ******************                              */
	/************************************************************************/
	CvPoint2D32f tmpfrontMarkpoint[4], tmprearMarkpoint[4], tmpleftMarkpoint[4], tmprightMarkpoint[4];
	CvMat *frontRotamat = cvCreateMat(3, 3, CV_64FC1);
	CvMat *rearRotamat = cvCreateMat(3, 3, CV_64FC1);
	CvMat *leftRotamat = cvCreateMat(3, 3, CV_64FC1);
	CvMat *rightRotamat = cvCreateMat(3, 3, CV_64FC1);

	double frontAngle = 0, rearAngle = 200, leftAngle = 290, rightAngle = 110;
	GetRotationMat(frontAngle, &frontRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(rearAngle, &rearRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(rightAngle, &rightRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(leftAngle, &leftRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);

	TransformPoint(frontbirdMarkpoint, 4, frontRotamat, tmpfrontMarkpoint);
	TransformPoint(rearbirdMarkpoint, 4, rearRotamat, tmprearMarkpoint);
	TransformPoint(leftbirdMarkpoint, 4, leftRotamat, tmpleftMarkpoint);
	TransformPoint(rightbirdMarkpoint, 4, rightRotamat, tmprightMarkpoint);
	
	double k13 = atan((tmpfrontMarkpoint[0].y-tmpfrontMarkpoint[1].y)/(tmpfrontMarkpoint[0].x-tmpfrontMarkpoint[1].x))*180/CV_PI;
	double k14 = atan((tmpfrontMarkpoint[2].y-tmpfrontMarkpoint[3].y)/(tmpfrontMarkpoint[2].x-tmpfrontMarkpoint[3].x))*180/CV_PI;
	double k23 = atan((tmprearMarkpoint[2].y-tmprearMarkpoint[3].y)/(tmprearMarkpoint[2].x-tmprearMarkpoint[3].x))*180/CV_PI;
	double k24 = atan((tmprearMarkpoint[0].y-tmprearMarkpoint[1].y)/(tmprearMarkpoint[0].x-tmprearMarkpoint[1].x))*180/CV_PI;
	double k31 = atan((tmpleftMarkpoint[2].y-tmpleftMarkpoint[3].y)/(tmpleftMarkpoint[2].x-tmpleftMarkpoint[3].x))*180/CV_PI;
	double k32 = atan((tmpleftMarkpoint[0].y-tmpleftMarkpoint[1].y)/(tmpleftMarkpoint[0].x-tmpleftMarkpoint[1].x))*180/CV_PI;
	double k41 = atan((tmprightMarkpoint[0].y-tmprightMarkpoint[1].y)/(tmprightMarkpoint[0].x-tmprightMarkpoint[1].x))*180/CV_PI;
	double k42 = atan((tmprightMarkpoint[2].y-tmprightMarkpoint[3].y)/(tmprightMarkpoint[2].x-tmprightMarkpoint[3].x))*180/CV_PI;

	double dieta31 = k31-k13;
	double dieta32 = k32-k23;
	double dieta41 = k41-k14;
	double dieta42 = k42-k24;
	double sumdieta = dieta31*dieta31+dieta41*dieta41+dieta32*dieta32+dieta42*dieta42;
	double bestsum = sumdieta;
	//printf("k13=%f, k31=%f\n", k13, k31);
	//printf("k14=%f, k41=%f\n", k14, k41);
	//printf("k23=%f, k32=%f\n", k23, k32);
	//printf("k24=%f, k42=%f\n", k24, k42);
	//printf("dieta31=%f, dieta32=%f\n", dieta31, dieta32);
	//printf("dieta41=%f, dieta42=%f\n", dieta41, dieta42);
	//printf("sumdieta=%f\n", sumdieta);

	double BestleftAngle = 20;
	double BestrightAngle = 20;
	double BestrearAngle = 20;

	int i = 40, j = 40, m = 40;
	while (i)
	{
		rightAngle = 110 - i;
		GetRotationMat(rightAngle, &rightRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
		TransformPoint(rightbirdMarkpoint, 4, rightRotamat, tmprightMarkpoint);
		j = 40;
		while (j)
		{
			rearAngle = 200 - j;
			GetRotationMat(rearAngle, &rearRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);;
			TransformPoint(rearbirdMarkpoint, 4, rearRotamat, tmprearMarkpoint);
			m = 40;
			while(m)
			{
				leftAngle = 290 - m;
				GetRotationMat(leftAngle, &leftRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
				TransformPoint(leftbirdMarkpoint, 4, leftRotamat, tmpleftMarkpoint);
				k13 = atan((tmpfrontMarkpoint[0].y-tmpfrontMarkpoint[1].y)/(tmpfrontMarkpoint[0].x-tmpfrontMarkpoint[1].x))*180/CV_PI;
				k14 = atan((tmpfrontMarkpoint[2].y-tmpfrontMarkpoint[3].y)/(tmpfrontMarkpoint[2].x-tmpfrontMarkpoint[3].x))*180/CV_PI;
				k23 = atan((tmprearMarkpoint[2].y-tmprearMarkpoint[3].y)/(tmprearMarkpoint[2].x-tmprearMarkpoint[3].x))*180/CV_PI;
				k24 = atan((tmprearMarkpoint[0].y-tmprearMarkpoint[1].y)/(tmprearMarkpoint[0].x-tmprearMarkpoint[1].x))*180/CV_PI;
				k31 = atan((tmpleftMarkpoint[2].y-tmpleftMarkpoint[3].y)/(tmpleftMarkpoint[2].x-tmpleftMarkpoint[3].x))*180/CV_PI;
				k32 = atan((tmpleftMarkpoint[0].y-tmpleftMarkpoint[1].y)/(tmpleftMarkpoint[0].x-tmpleftMarkpoint[1].x))*180/CV_PI;
				k41 = atan((tmprightMarkpoint[0].y-tmprightMarkpoint[1].y)/(tmprightMarkpoint[0].x-tmprightMarkpoint[1].x))*180/CV_PI;
				k42 = atan((tmprightMarkpoint[2].y-tmprightMarkpoint[3].y)/(tmprightMarkpoint[2].x-tmprightMarkpoint[3].x))*180/CV_PI;
				dieta31 = k31-k13;
				dieta32 = k32-k23;
				dieta41 = k41-k14;
				dieta42 = k42-k24;
				sumdieta = dieta31*dieta31+dieta41*dieta41+dieta32*dieta32+dieta42*dieta42;
				if (sumdieta < bestsum)
				{
					bestsum = sumdieta;
					BestleftAngle = 20-m;
					BestrearAngle = 20-j;
					BestrightAngle = 20-i;
				}
				m--;
			}
			j--;
		}
		i--;
	}
	//printf("BestleftAngle=%f\nBestrearAngle=%f\nBestrightAngle=%f\n", BestleftAngle, BestrearAngle, BestrightAngle);
	i = 20;
	rearAngle = 180+BestrearAngle+1;
	leftAngle = 270+BestleftAngle+1;
	rightAngle = 90+BestrightAngle+1;
	GetRotationMat(rearAngle, &rearRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(rightAngle, &rightRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
	GetRotationMat(leftAngle, &leftRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
	TransformPoint(rearbirdMarkpoint, 4, rearRotamat, tmprearMarkpoint);
	TransformPoint(leftbirdMarkpoint, 4, leftRotamat, tmpleftMarkpoint);
	TransformPoint(rightbirdMarkpoint, 4, rightRotamat, tmprightMarkpoint);
	k13 = atan((tmpfrontMarkpoint[0].y-tmpfrontMarkpoint[1].y)/(tmpfrontMarkpoint[0].x-tmpfrontMarkpoint[1].x))*180/CV_PI;
	k14 = atan((tmpfrontMarkpoint[2].y-tmpfrontMarkpoint[3].y)/(tmpfrontMarkpoint[2].x-tmpfrontMarkpoint[3].x))*180/CV_PI;
	k23 = atan((tmprearMarkpoint[2].y-tmprearMarkpoint[3].y)/(tmprearMarkpoint[2].x-tmprearMarkpoint[3].x))*180/CV_PI;
	k24 = atan((tmprearMarkpoint[0].y-tmprearMarkpoint[1].y)/(tmprearMarkpoint[0].x-tmprearMarkpoint[1].x))*180/CV_PI;
	k31 = atan((tmpleftMarkpoint[2].y-tmpleftMarkpoint[3].y)/(tmpleftMarkpoint[2].x-tmpleftMarkpoint[3].x))*180/CV_PI;
	k32 = atan((tmpleftMarkpoint[0].y-tmpleftMarkpoint[1].y)/(tmpleftMarkpoint[0].x-tmpleftMarkpoint[1].x))*180/CV_PI;
	k41 = atan((tmprightMarkpoint[0].y-tmprightMarkpoint[1].y)/(tmprightMarkpoint[0].x-tmprightMarkpoint[1].x))*180/CV_PI;
	k42 = atan((tmprightMarkpoint[2].y-tmprightMarkpoint[3].y)/(tmprightMarkpoint[2].x-tmprightMarkpoint[3].x))*180/CV_PI;

	dieta31 = k31-k13;
	dieta32 = k32-k23;
	dieta41 = k41-k14;
	dieta42 = k42-k24;
	sumdieta = dieta31*dieta31+dieta41*dieta41+dieta32*dieta32+dieta42*dieta42;
	bestsum = sumdieta;
	while (i)
	{
		rightAngle -= 0.1;
		GetRotationMat(rightAngle, &rightRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
		TransformPoint(rightbirdMarkpoint, 4, rightRotamat, tmprightMarkpoint);
		j = 20;
		while (j)
		{
			rearAngle -= 0.1;
			GetRotationMat(rearAngle, &rearRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);;
			TransformPoint(rearbirdMarkpoint, 4, rearRotamat, tmprearMarkpoint);
			m = 20;
			while(m)
			{
				leftAngle -= 0.1;
				GetRotationMat(leftAngle, &leftRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
				TransformPoint(leftbirdMarkpoint, 4, leftRotamat, tmpleftMarkpoint);
				k13 = atan((tmpfrontMarkpoint[0].y-tmpfrontMarkpoint[1].y)/(tmpfrontMarkpoint[0].x-tmpfrontMarkpoint[1].x))*180/CV_PI;
				k14 = atan((tmpfrontMarkpoint[2].y-tmpfrontMarkpoint[3].y)/(tmpfrontMarkpoint[2].x-tmpfrontMarkpoint[3].x))*180/CV_PI;
				k23 = atan((tmprearMarkpoint[2].y-tmprearMarkpoint[3].y)/(tmprearMarkpoint[2].x-tmprearMarkpoint[3].x))*180/CV_PI;
				k24 = atan((tmprearMarkpoint[0].y-tmprearMarkpoint[1].y)/(tmprearMarkpoint[0].x-tmprearMarkpoint[1].x))*180/CV_PI;
				k31 = atan((tmpleftMarkpoint[2].y-tmpleftMarkpoint[3].y)/(tmpleftMarkpoint[2].x-tmpleftMarkpoint[3].x))*180/CV_PI;
				k32 = atan((tmpleftMarkpoint[0].y-tmpleftMarkpoint[1].y)/(tmpleftMarkpoint[0].x-tmpleftMarkpoint[1].x))*180/CV_PI;
				k41 = atan((tmprightMarkpoint[0].y-tmprightMarkpoint[1].y)/(tmprightMarkpoint[0].x-tmprightMarkpoint[1].x))*180/CV_PI;
				k42 = atan((tmprightMarkpoint[2].y-tmprightMarkpoint[3].y)/(tmprightMarkpoint[2].x-tmprightMarkpoint[3].x))*180/CV_PI;
				dieta31 = k31-k13;
				dieta32 = k32-k23;
				dieta41 = k41-k14;
				dieta42 = k42-k24;
				sumdieta = dieta31*dieta31+dieta41*dieta41+dieta32*dieta32+dieta42*dieta42;
				if (sumdieta < bestsum)
				{
					bestsum = sumdieta;
					BestleftAngle = leftAngle-270;
					BestrearAngle = rearAngle-180;
					BestrightAngle = rightAngle-90;
				}
				m--;
			}
			j--;
		}
		i--;
	}
	//printf("BestleftAngle=%f\nBestrearAngle=%f\nBestrightAngle=%f\n", BestleftAngle, BestrearAngle, BestrightAngle);

	GetRotationMat(180+BestrearAngle, &rearRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(90+BestrightAngle, &rightRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
	GetRotationMat(270+BestleftAngle, &leftRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
	TransformPoint(rearbirdMarkpoint, 4, rearRotamat, tmprearMarkpoint);
	TransformPoint(leftbirdMarkpoint, 4, leftRotamat, tmpleftMarkpoint);
	TransformPoint(rightbirdMarkpoint, 4, rightRotamat, tmprightMarkpoint);
	k13 = atan((tmpfrontMarkpoint[0].y-tmpfrontMarkpoint[1].y)/(tmpfrontMarkpoint[0].x-tmpfrontMarkpoint[1].x))*180/CV_PI;
	k14 = atan((tmpfrontMarkpoint[2].y-tmpfrontMarkpoint[3].y)/(tmpfrontMarkpoint[2].x-tmpfrontMarkpoint[3].x))*180/CV_PI;
	k23 = atan((tmprearMarkpoint[2].y-tmprearMarkpoint[3].y)/(tmprearMarkpoint[2].x-tmprearMarkpoint[3].x))*180/CV_PI;
	k24 = atan((tmprearMarkpoint[0].y-tmprearMarkpoint[1].y)/(tmprearMarkpoint[0].x-tmprearMarkpoint[1].x))*180/CV_PI;
	k31 = atan((tmpleftMarkpoint[2].y-tmpleftMarkpoint[3].y)/(tmpleftMarkpoint[2].x-tmpleftMarkpoint[3].x))*180/CV_PI;
	k32 = atan((tmpleftMarkpoint[0].y-tmpleftMarkpoint[1].y)/(tmpleftMarkpoint[0].x-tmpleftMarkpoint[1].x))*180/CV_PI;
	k41 = atan((tmprightMarkpoint[0].y-tmprightMarkpoint[1].y)/(tmprightMarkpoint[0].x-tmprightMarkpoint[1].x))*180/CV_PI;
	k42 = atan((tmprightMarkpoint[2].y-tmprightMarkpoint[3].y)/(tmprightMarkpoint[2].x-tmprightMarkpoint[3].x))*180/CV_PI;
	dieta31 = k31-k13;
	dieta32 = k32-k23;
	dieta41 = k41-k14;
	dieta42 = k42-k24;
	sumdieta = dieta31*dieta31+dieta41*dieta41+dieta32*dieta32+dieta42*dieta42;
	//printf("k13=%f, k31=%f\n", k13, k31);
	//printf("k14=%f, k41=%f\n", k14, k41);
	//printf("k23=%f, k32=%f\n", k23, k32);
	//printf("k24=%f, k42=%f\n", k24, k42);
	//printf("dieta31=%f, dieta32=%f\n", dieta31, dieta32);
	//printf("dieta41=%f, dieta42=%f\n", dieta41, dieta42);
	//printf("sumdieta=%f\n", sumdieta);

	stitchMarkpoint[0].x = (stitchImage->width-abs(frontbirdMarkpoint[3].x-frontbirdMarkpoint[1].x))/2;
	stitchMarkpoint[0].y = (stitchImage->height-abs(leftbirdMarkpoint[3].x-leftbirdMarkpoint[1].x))/2;
	stitchMarkpoint[1].x = stitchMarkpoint[0].x+(frontbirdMarkpoint[0].x-frontbirdMarkpoint[1].x);
	stitchMarkpoint[1].y = stitchMarkpoint[0].y+(frontbirdMarkpoint[0].y-frontbirdMarkpoint[1].y);
	stitchMarkpoint[2].x = stitchMarkpoint[0].x + (tmpleftMarkpoint[1].x-tmpleftMarkpoint[3].x);
	stitchMarkpoint[2].y = stitchMarkpoint[0].y + (tmpleftMarkpoint[1].y-tmpleftMarkpoint[3].y);
	stitchMarkpoint[3].x = stitchMarkpoint[2].x + (leftbirdMarkpoint[0].y - leftbirdMarkpoint[1].y);
	stitchMarkpoint[3].y = stitchMarkpoint[2].y + (leftbirdMarkpoint[1].x - leftbirdMarkpoint[0].x);
	stitchMarkpoint[4].x = stitchMarkpoint[0].x + (frontbirdMarkpoint[3].x-frontbirdMarkpoint[1].x);
	stitchMarkpoint[4].y = stitchMarkpoint[0].y + (frontbirdMarkpoint[3].y-frontbirdMarkpoint[1].y);
	stitchMarkpoint[5].x = stitchMarkpoint[4].x + (frontbirdMarkpoint[2].x-frontbirdMarkpoint[3].x);
	stitchMarkpoint[5].y = stitchMarkpoint[4].y + (frontbirdMarkpoint[2].y-frontbirdMarkpoint[3].y);
	stitchMarkpoint[6].x = stitchMarkpoint[4].x + (tmprightMarkpoint[3].x-tmprightMarkpoint[1].x);
	stitchMarkpoint[6].y = stitchMarkpoint[4].y + (tmprightMarkpoint[3].y-tmprightMarkpoint[1].y);
	stitchMarkpoint[7].x = stitchMarkpoint[6].x + (tmprightMarkpoint[2].x-tmprightMarkpoint[3].x);
	stitchMarkpoint[7].y = stitchMarkpoint[6].y + (tmprightMarkpoint[2].y-tmprightMarkpoint[3].y);


	cvReleaseMat(&frontRotamat);
	cvReleaseMat(&rearRotamat);
	cvReleaseMat(&leftRotamat);
	cvReleaseMat(&rightRotamat);
	return true;
}
/************************************************************************/
/*         计算标记点在全景图中的坐标                                   */
/************************************************************************/
bool GetMarkPointInStitchImage2(CvPoint2D32f *frontbirdMarkpoint, CvPoint2D32f *rearbirdMarkpoint, CvPoint2D32f *leftbirdMarkpoint,
	CvPoint2D32f *rightbirdMarkpoint, IplImage * stitchImage, CvPoint2D32f *stitchMarkpoint)
{
	if (!frontbirdMarkpoint || !rearbirdMarkpoint || !leftbirdMarkpoint || !rightbirdMarkpoint || !stitchMarkpoint || !stitchMarkpoint)
	{
		return false;
	}
	/************************************************************************/
	/*                      ******************                              */
	/*                      * *     1      * *                              */
	/*                      *   *  ***  *    *                              */
	/*                      *  3   * *     4 *                              */
	/*                      *    * ***  *    *                              */
	/*                      * *     2      * *                              */
	/*                      ******************                              */
	/************************************************************************/
	CvPoint2D32f tmpfrontMarkpoint[4], tmprearMarkpoint[4], tmpleftMarkpoint[4], tmprightMarkpoint[4];
	CvMat *frontRotamat = cvCreateMat(3, 3, CV_64FC1);
	CvMat *rearRotamat = cvCreateMat(3, 3, CV_64FC1);
	CvMat *leftRotamat = cvCreateMat(3, 3, CV_64FC1);
	CvMat *rightRotamat = cvCreateMat(3, 3, CV_64FC1);

	double frontAngle = 0, rearAngle = 180, leftAngle = 270, rightAngle = 90;
	GetRotationMat(frontAngle, &frontRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(rearAngle, &rearRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(rightAngle, &rightRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);
	GetRotationMat(leftAngle, &leftRotamat, IMAGE_WIDTH, IMAGE_HEIGHT);

	TransformPoint(frontbirdMarkpoint, 4, frontRotamat, tmpfrontMarkpoint);
	TransformPoint(rearbirdMarkpoint, 4, rearRotamat, tmprearMarkpoint);
	TransformPoint(leftbirdMarkpoint, 4, leftRotamat, tmpleftMarkpoint);
	TransformPoint(rightbirdMarkpoint, 4, rightRotamat, tmprightMarkpoint);

	double k13 = atan((tmpfrontMarkpoint[0].y-tmpfrontMarkpoint[1].y)/(tmpfrontMarkpoint[0].x-tmpfrontMarkpoint[1].x))*180/CV_PI;
	double k14 = atan((tmpfrontMarkpoint[2].y-tmpfrontMarkpoint[3].y)/(tmpfrontMarkpoint[2].x-tmpfrontMarkpoint[3].x))*180/CV_PI;
	double k31 = atan((tmpleftMarkpoint[2].y-tmpleftMarkpoint[3].y)/(tmpleftMarkpoint[2].x-tmpleftMarkpoint[3].x))*180/CV_PI;
	double k32 = atan((tmpleftMarkpoint[0].y-tmpleftMarkpoint[1].y)/(tmpleftMarkpoint[0].x-tmpleftMarkpoint[1].x))*180/CV_PI;
	double k41 = atan((tmprightMarkpoint[0].y-tmprightMarkpoint[1].y)/(tmprightMarkpoint[0].x-tmprightMarkpoint[1].x))*180/CV_PI;
	double k42 = atan((tmprightMarkpoint[2].y-tmprightMarkpoint[3].y)/(tmprightMarkpoint[2].x-tmprightMarkpoint[3].x))*180/CV_PI;

	//printf("k13=%f, k31=%f\n", k13, k31);
	//printf("k14=%f, k41=%f\n", k14, k41);
	
	GetRotationMat(k13-k31, &leftRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
	GetRotationMat(k14-k41, &rightRotamat, IMAGE_HEIGHT, IMAGE_WIDTH);
	TransformPoint(tmpleftMarkpoint, 4, leftRotamat, tmpleftMarkpoint);
	TransformPoint(tmprightMarkpoint, 4, rightRotamat, tmprightMarkpoint);

	k13 = atan((tmpfrontMarkpoint[0].y-tmpfrontMarkpoint[1].y)/(tmpfrontMarkpoint[0].x-tmpfrontMarkpoint[1].x))*180/CV_PI;
	k14 = atan((tmpfrontMarkpoint[2].y-tmpfrontMarkpoint[3].y)/(tmpfrontMarkpoint[2].x-tmpfrontMarkpoint[3].x))*180/CV_PI;
	k31 = atan((tmpleftMarkpoint[2].y-tmpleftMarkpoint[3].y)/(tmpleftMarkpoint[2].x-tmpleftMarkpoint[3].x))*180/CV_PI;
	k41 = atan((tmprightMarkpoint[0].y-tmprightMarkpoint[1].y)/(tmprightMarkpoint[0].x-tmprightMarkpoint[1].x))*180/CV_PI;

	//printf("k13=%f, k31=%f\n", k13, k31);
	//printf("k14=%f, k41=%f\n", k14, k41);
	
	stitchMarkpoint[0].x = (stitchImage->width-abs(frontbirdMarkpoint[3].x-frontbirdMarkpoint[1].x))/2;
	stitchMarkpoint[0].y = (stitchImage->height-abs(leftbirdMarkpoint[3].x-leftbirdMarkpoint[1].x))/2;
	stitchMarkpoint[1].x = stitchMarkpoint[0].x+(frontbirdMarkpoint[0].x-frontbirdMarkpoint[1].x);
	stitchMarkpoint[1].y = stitchMarkpoint[0].y+(frontbirdMarkpoint[0].y-frontbirdMarkpoint[1].y);
	stitchMarkpoint[2].x = stitchMarkpoint[0].x + (tmpleftMarkpoint[1].x-tmpleftMarkpoint[3].x);
	stitchMarkpoint[2].y = stitchMarkpoint[0].y + (tmpleftMarkpoint[1].y-tmpleftMarkpoint[3].y);
	stitchMarkpoint[3].x = stitchMarkpoint[2].x + (tmpleftMarkpoint[0].x - tmpleftMarkpoint[1].x);
	stitchMarkpoint[3].y = stitchMarkpoint[2].y + (tmpleftMarkpoint[0].y - tmpleftMarkpoint[1].y);
	stitchMarkpoint[4].x = stitchMarkpoint[0].x + (frontbirdMarkpoint[3].x-frontbirdMarkpoint[1].x);
	stitchMarkpoint[4].y = stitchMarkpoint[0].y + (frontbirdMarkpoint[3].y-frontbirdMarkpoint[1].y);
	stitchMarkpoint[5].x = stitchMarkpoint[4].x + (frontbirdMarkpoint[2].x-frontbirdMarkpoint[3].x);
	stitchMarkpoint[5].y = stitchMarkpoint[4].y + (frontbirdMarkpoint[2].y-frontbirdMarkpoint[3].y);
	stitchMarkpoint[6].x = stitchMarkpoint[4].x + (tmprightMarkpoint[3].x-tmprightMarkpoint[1].x);
	stitchMarkpoint[6].y = stitchMarkpoint[4].y + (tmprightMarkpoint[3].y-tmprightMarkpoint[1].y);
	stitchMarkpoint[7].x = stitchMarkpoint[6].x + (tmprightMarkpoint[2].x-tmprightMarkpoint[3].x);
	stitchMarkpoint[7].y = stitchMarkpoint[6].y + (tmprightMarkpoint[2].y-tmprightMarkpoint[3].y);


	cvReleaseMat(&frontRotamat);
	cvReleaseMat(&rearRotamat);
	cvReleaseMat(&leftRotamat);
	cvReleaseMat(&rightRotamat);
	return true;
}
/************************************************************************/
/*		  计算旋转矩阵                                                  */
/************************************************************************/
bool GetRotationMat(float angle, CvMat **rotationMat, int imagewidth, int imageheight)
{
	if (!rotationMat || !*rotationMat)
	{
		return false;
	}

	while (1)
	{
		if (angle > 360)
		{
			angle -= 360;
		}else
		{
			break;
		}
	}
	CvPoint2D64f srcpoint[4], dstpoint[4];
	srcpoint[0] = cvPoint2D64f(0, 0);
	srcpoint[1] = cvPoint2D64f(imagewidth, 0);
	srcpoint[2] = cvPoint2D64f(0, imageheight);
	srcpoint[3] = cvPoint2D64f(imagewidth, imageheight);

	double sita = angle/180*CV_PI;
	dstpoint[0].x = imageheight*sin(sita);
	dstpoint[0].y = 0;
	dstpoint[1].x = dstpoint[0].x + imagewidth*cos(sita);
	dstpoint[1].y = imagewidth*sin(sita);
	dstpoint[2].x = 0;
	dstpoint[2].y = imageheight*cos(sita);
	dstpoint[3].x = imagewidth*cos(sita);
	dstpoint[3].y = imageheight*cos(sita) + imagewidth*sin(sita);

	DLT_Transform(srcpoint, dstpoint, 4, *rotationMat);

	float xoffset = MIN(dstpoint[0].x, MIN(dstpoint[1].x, MIN(dstpoint[2].x, dstpoint[3].x)));
	float yoffset = MIN(dstpoint[0].y, MIN(dstpoint[1].y, MIN(dstpoint[2].y, dstpoint[3].y)));
	for (int i = 0; i < 4; ++i)
	{
		dstpoint[i].x -= xoffset;
		dstpoint[i].y -= yoffset;
	}

	DLT_Transform(srcpoint, dstpoint, 4, *rotationMat);

	return true;
}
/************************************************************************/
/*		  计算平移矩阵                                                  */
/************************************************************************/
bool GetMoveMat(int xdistance, int ydistance, CvMat **moveMat)
{
	if (!moveMat || !*moveMat)
	{
		return false;
	}
	CvPoint2D64f srcpoint[4], dstpoint[4];
	srcpoint[0] = cvPoint2D64f(0, 0);
	srcpoint[1] = cvPoint2D64f(0, 1);
	srcpoint[2] = cvPoint2D64f(1, 0);
	srcpoint[3] = cvPoint2D64f(1, 1);

	dstpoint[0] = cvPoint2D64f(xdistance, -ydistance);
	dstpoint[1] = cvPoint2D64f(xdistance, 1-ydistance);
	dstpoint[2] = cvPoint2D64f(1+xdistance, -ydistance);
	dstpoint[3] = cvPoint2D64f(1+xdistance, 1-ydistance);

	DLT_Transform(srcpoint, dstpoint, 4, *moveMat);
	
	return true;
}
/************************************************************************/
/*        裁剪图像                                                      */
/************************************************************************/
bool cutImage(IplImage* srcImage, CvPoint2D32f point1, CvPoint2D32f point2, int direction)
{
	int w = srcImage->width;
	int h = srcImage->height;
	int i, j;
	double k = (point2.x - point1.x)/(point2.y - point1.y);
	//printf("k=%f\n", k);
	CvPoint2D32f tmp;
	tmp.x = (point1.x + point2.x)/2;
	tmp.y = (point1.y + point2.y)/2;
	CvScalar tmpval;

	switch (direction)
	{
	case CUT_LEFT_BOTTOM:
		for (i = 0; i < h; ++i)
		{
			for (j = 0; j < w; ++j)
			{
				if (cvRound(tmp.x + k*(i - tmp.y)) > j)
				{
					cvSet2D(srcImage, i, j, cvScalar(0,0,0));
				}
				if (cvRound(tmp.x + k*(i - tmp.y)) == j)
				{
					tmpval = cvGet2D(srcImage, i, j);
					cvSet2D(srcImage, i, j, cvScalar(tmpval.val[0]/2, tmpval.val[1]/2, tmpval.val[2]/2));
				}
			}
		}
		break;
	case CUT_LEFT_TOP:
		for (i = 0; i < h; ++i)
		{
			for (j = 0; j < w; ++j)
			{
				if (cvRound(tmp.x + k*(i - tmp.y)) > j)
				{
					cvSet2D(srcImage, i, j, cvScalar(0,0,0));
				}

				if (cvRound(tmp.x + k*(i - tmp.y)) == j)
				{
					tmpval = cvGet2D(srcImage, i, j);
					cvSet2D(srcImage, i, j, cvScalar(tmpval.val[0]/2, tmpval.val[1]/2, tmpval.val[2]/2));
				}
			}
		}
		break;
	case CUT_RIGHT_BOTTOM:
		for (i = 0; i < h; ++i)
		{
			for (j = 0; j < w; ++j)
			{
				if (cvRound(tmp.x + k*(i - tmp.y)) < j)
				{
					cvSet2D(srcImage, i, j, cvScalar(0,0,0));
				}

				if (cvRound(tmp.x + k*(i - tmp.y)) == j)
				{
					tmpval = cvGet2D(srcImage, i, j);
					cvSet2D(srcImage, i, j, cvScalar(tmpval.val[0]/2, tmpval.val[1]/2, tmpval.val[2]/2));
				}
			}
		}
		break;
	case CUT_RIGHT_TOP:
		for (i = 0; i < h; ++i)
		{
			for (j = 0; j < w; ++j)
			{
				if (cvRound(tmp.x + k*(i - tmp.y)) < j)
				{
					cvSet2D(srcImage, i, j, cvScalar(0,0,0));
				}

				if (cvRound(tmp.x + k*(i - tmp.y)) == j)
				{
					tmpval = cvGet2D(srcImage, i, j);
					cvSet2D(srcImage, i, j, cvScalar(tmpval.val[0]/2, tmpval.val[1]/2, tmpval.val[2]/2));
				}
			}
		}
		break;
	default:
		break;
	}
	return true;
}
/**************************************************************************************/
/*                    p0--------1----------p1                                         */
/*                    |                    |                                          */
/*                    |     polygan        |                                          */
/*                    4                    2                                          */
/*                    |                    |                                          */
/*                    p3-------3-----------p2                                         */
/*					判断点是否在多边形内											  */
/*					CvPoint testPoint: 测试点										  */
/*					CvPoint2D32f *polygon: 多边形各个顶点，按顺时针顺序保存			  */
/*					int n: 顶点数													  */
/**************************************************************************************/
bool JudgeIsInside(CvPoint testPoint, CvPoint2D32f *polygon, int n)
{
	if (n < 3 || polygon == NULL)
	{
		printf("input error\n");
		return false;
	}
	int counter = 0;
	int i;
	double xinters;
	CvPoint2D32f p1, p2;

	for (i = 0; i < n; ++i)
	{
		p1 = polygon[i];
		p2 = polygon[(i+1)%n];
		if ( (testPoint.y > MIN(p1.y, p2.y)) &&
			(testPoint.y <= MAX(p1.y, p2.y)) &&
			(testPoint.x <= MAX(p1.x, p2.x)) )
		{
			if (p1.y != p2.y)
			{
				xinters = (testPoint.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
				if (p1.x==p2.x || testPoint.x <= xinters)
				{
					counter++;
				}
			}
		}
	}

	if (counter%2 == 0)
	{
		return false;
	}else
		return true;
}
/************************************************************************/
/*					根据透视图点的坐标求解与之对应的畸变图点的坐标		*/
/************************************************************************/
bool GetDistorPoint(CvPoint2D32f *srcPoint, int n, CvPoint2D32f *dstPoint, double fx, double fy, double cx, double cy,
	double k1, double k2, double k3, double p1, double p2)
{
	if (!srcPoint || !dstPoint)
	{
		return false;
	}
	CvPoint2D32f *tmppoint = new CvPoint2D32f[n];
	for (int i = 0; i < n; ++i)
	{
		tmppoint[i].x = srcPoint[i].x;
		tmppoint[i].y = srcPoint[i].y;
	}
	
	double yd, yd2, xd, xd2, r2, xp, yp;
	for (int i = 0; i < n; ++i)
	{
		yd = (srcPoint[i].y-cy)/fy;
		yd2 = yd*yd;
		xd = (srcPoint[i].x-cx)/fx;
		xd2 = xd*xd;
		r2 = xd2 + yd2;
		xp = (1 + k1*r2 + k2*r2*r2 + k3*r2*r2*r2)*xd + 2*p1*xd*yd + p2*(r2+2*xd2);
		yp = (1 + k1*r2 + k2*r2*r2 + k3*r2*r2*r2)*yd + p1*(r2+2*yd2) + 2*p2*xd*yd;
		dstPoint[i].x = fx*xp + cx;
		dstPoint[i].y = fy*yp + cy;
	}
	delete[] tmppoint;
	
	return true;
}

