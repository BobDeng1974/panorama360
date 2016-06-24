#include "./Api/Api.h"
#include <linux/fb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <time.h>
#include <pthread.h>
#include "g2d_driver.h"

//v4l2 buffer number
#define BUFNUM 5 
#define OTHERADDRP 0x48000000


StitchInfo front,rear,left,right;

unsigned int paddr;
unsigned char *pbigImage=NULL;
unsigned char *otheraddr=NULL;
unsigned char *addr=NULL;

int videoFD;
int g2dFD;
int memFD;
int fbFD;

g2d_blt tmp;

pthread_t readFrame;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

struct buffer
{
	char * start;
	unsigned int length;
	int index;
};

struct buffer * buffers=NULL;

//separate the initial image into four small images
//
int separateImage(IplImage* im1,IplImage* im2,IplImage* im3,IplImage* im4,unsigned char * pbigImage)
{
	for(int i=0;i<480;i++)
	{
		memcpy(im1->imageData+720*4*i,pbigImage+1440*4*i,720*4);
		memcpy(im2->imageData+720*4*i,pbigImage+1440*4*i+720*4,720*4);
		memcpy(im3->imageData+720*4*i,pbigImage+1440*4*(480+i),720*4);
		memcpy(im4->imageData+720*4*i,pbigImage+1440*4*(480+i)+720*4,720*4);
	}
	return 0;
}

//combine four small images into a big image
int combineImage(IplImage* im1,IplImage* im2,IplImage* im3,IplImage* im4,unsigned char *pbigimage)
{
	for(int i=0;i<480;i++)
	{
		memcpy(pbigimage+1440*4*i,im1->imageData+720*4*i,720*4);
		memcpy(pbigimage+1440*4*i+720*4,im2->imageData+720*4*i,720*4);
		memcpy(pbigimage+1440*4*(i+480),im3->imageData+720*4*i,720*4);
		memcpy(pbigimage+1440*4*(i+480)+720*4,im4->imageData+720*4*i,720*4);
	}
	return 0;
}

//YUV(nv12)ToARGB
int nv12toargb(int w,int h,unsigned int pnv,unsigned int potherad)
{
	tmp.flag               =G2D_BLT_FLIP_HORIZONTAL;
	tmp.src_image.addr[0]  =pnv;
	tmp.src_image.addr[1]  =pnv+w*h;
	tmp.src_image.w        =w;
	tmp.src_image.h        =h;
	tmp.src_image.format   =G2D_FMT_PYUV420UVC;
	tmp.src_image.pixel_seq=G2D_SEQ_NORMAL;
	tmp.src_rect.x         =0;
	tmp.src_rect.y		   =0;
	tmp.src_rect.w         =w;
	tmp.src_rect.h         =h;
	tmp.dst_image.addr[0]  =potherad;
	tmp.dst_image.w        =w;
	tmp.dst_image.h		   =h;
	tmp.dst_image.format   =G2D_FMT_XRGB8888; //G2D_FMT_XBGR8888 for GPU
	tmp.dst_image.pixel_seq=G2D_SEQ_NORMAL;
	tmp.dst_x              =0; 
	tmp.dst_y              =0;
	tmp.color              =0;
	tmp.alpha              =0;
																					if(ioctl(g2dFD,G2D_CMD_BITBLT,&tmp)<0)
	{																					printf("error bitblt\n");
		return -1;
	}																				return 0;
}

int initV4L2()
{
	struct v4l2_format fmtPriv;
	videoFD=open("/dev/video10",O_RDWR|O_NONBLOCK,0);
	memset(&fmtPriv,0,sizeof(fmtPriv));
	fmtPriv.type=V4L2_BUF_TYPE_PRIVATE;
	fmtPriv.fmt.raw_data[0]=0; //interface
	fmtPriv.fmt.raw_data[1]=0; //system
	fmtPriv.fmt.raw_data[2]=0; //format 1=mb
	fmtPriv.fmt.raw_data[8]=2; //row
	fmtPriv.fmt.raw_data[9]=2; //column
	fmtPriv.fmt.raw_data[10]=1; //channel index
	fmtPriv.fmt.raw_data[11]=2; //channel index
	fmtPriv.fmt.raw_data[12]=3; //channel index
	fmtPriv.fmt.raw_data[13]=4; //channel index

	if(ioctl(videoFD,VIDIOC_S_FMT,&fmtPriv)==-1)
	{
		printf("VIDIOC_S_FMT error!\n");
		return -1;
	}

	int width=fmtPriv.fmt.raw_data[8]*(fmtPriv.fmt.raw_data[2]?704:720);
	int height=fmtPriv.fmt.raw_data[9]*(fmtPriv.fmt.raw_data[1]?576:480);
	printf("display width is %d\n",width);
	printf("display height is %d\n",height);
	usleep(100000); //delay 100ms if you want to check the status after set fmt

	memset(&fmtPriv,0,sizeof(fmtPriv));
	fmtPriv.type=V4L2_BUF_TYPE_PRIVATE;
	if(ioctl(videoFD,VIDIOC_G_FMT,&fmtPriv)==-1)
	{
		printf("VIDIOC_G_FMT error! \n");
		return -1;
	}

	printf("interface=%d\n", fmtPriv.fmt.raw_data[0]);
	printf("system=%d\n", fmtPriv.fmt.raw_data[1]);
	printf("format=%d\n", fmtPriv.fmt.raw_data[2]);
	printf("row=%d\n", fmtPriv.fmt.raw_data[8]);
	printf("column=%d\n", fmtPriv.fmt.raw_data[9]);
	printf("channel_index[0]=%d\n", fmtPriv.fmt.raw_data[10]);
	printf("channel_index[1]=%d\n", fmtPriv.fmt.raw_data[11]);
	printf("channel_index[2]=%d\n", fmtPriv.fmt.raw_data[12]);
	printf("channel_index[3]=%d\n", fmtPriv.fmt.raw_data[13]);
	printf("status[0]=%d\n", fmtPriv.fmt.raw_data[16]);
	printf("status[1]=%d\n", fmtPriv.fmt.raw_data[17]);
	printf("status[2]=%d\n", fmtPriv.fmt.raw_data[18]);
	printf("status[3]=%d\n", fmtPriv.fmt.raw_data[19]);

	struct v4l2_requestbuffers req;
	memset(&req,0,sizeof(req));
	req.count=BUFNUM;
	req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory=V4L2_MEMORY_MMAP;
	if(ioctl(videoFD,VIDIOC_REQBUFS,&req)<0)
	{
		printf("error request buffers \n");
		return -1;
	}
	
	buffers=(struct buffer*)calloc(req.count,sizeof(struct buffer));
	
	for(int n=0;n<req.count;n++)
	{
		struct v4l2_buffer buf;
		memset(&buf,0,sizeof(buf));
		buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory=V4L2_MEMORY_MMAP;
		buf.index=n;
		if(ioctl(videoFD,VIDIOC_QUERYBUF,&buf)<0)
		{
			printf("error querybuf\n");
			return -1;
		}

		buffers[n].length=buf.length;
		buffers[n].start=(char*)mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,videoFD,buf.m.offset);
		buffers[n].index=n;
		printf("index %d:buf length:%u\n",buf.length);
		printf("MMAP:%p OFF:%u\n",buffers[n].start,buf.m.offset);
		if(buffers[n].start==MAP_FAILED)
		{
			printf("mmap failed\n");
			return -1;
		}

		if(ioctl(videoFD,VIDIOC_QBUF,&buf)<0)
		{
			printf("VIDIOC_QBUF failed\n");
			return -1;
		}
	}

}

int initFB()
{
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	fbFD=open("/dev/fb0",O_RDWR);
	if(ioctl(fbFD,FBIOGET_FSCREENINFO,&finfo)<0)
	{
		printf("Error reading fixed information\n");
		return -1;
	}
	if(ioctl(fbFD,FBIOGET_VSCREENINFO,&vinfo)<0)
	{
		printf("Error reading variable information\n");
		return -1;
	}
	printf("The physical address is :%lu\n",finfo.smem_start);
	printf("The mem is :%d\n",finfo.smem_len);
	printf("the line_lenght is :%d\n",finfo.line_length);
	printf("the xres is :%d\n",vinfo.xres);
	printf("the yres is :%d\n",vinfo.yres);
	printf("bits_per_pixel is :%d\n",vinfo.bits_per_pixel);
	paddr=finfo.smem_start;
	addr=(unsigned char*)mmap(NULL,1440*960*4,PROT_READ|PROT_WRITE,MAP_SHARED,fbFD,0);
	printf("fb addr:%p\n",addr);
	return 0;

}

int initG2D()
{
	g2dFD=open("/dev/g2d",O_RDWR);
	if(g2dFD<0)
	{
		printf("error open g2d\n");
		return -1;
	}
	return 0;
}

int initMem()
{
	memFD=open("/dev/mem",O_RDWR);
	if(memFD<0)
	{
		printf("error open mem\n");
		return -1;
	}
	otheraddr=(unsigned char*)mmap(NULL,1440*960*4,PROT_READ|PROT_WRITE,MAP_SHARED,memFD,OTHERADDRP);
	return 0;
}

void *readframe(void*)
{
	struct v4l2_buffer buff;
	buff.memory = V4L2_MEMORY_MMAP;
	buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	struct timeval start,end;
	while(1)
	{
		fd_set fds;
		struct timeval tv;
		int r;
		FD_ZERO(&fds);
		FD_SET(videoFD,&fds);

		tv.tv_sec=2;
		tv.tv_usec=0;

		r=select(videoFD+1,&fds,NULL,NULL,&tv);
		if(r==-1)
		{
			if(EINTR==errno)
			{
				continue;
			}
			printf("select err\n");
		}
		if(r==0)
		{
			printf("error select timeout\n");
			pthread_exit(0);
		}
		
		if(ioctl(videoFD,VIDIOC_DQBUF,&buff)<0)
		{
			printf("error dqbuffer\n");
			pthread_exit(0);
		}
		
		pthread_mutex_lock(&mutex);
		gettimeofday(&start,NULL);
		nv12toargb(1440,960,buff.m.offset,OTHERADDRP);
		gettimeofday(&end,NULL);
		pthread_mutex_unlock(&mutex);
//		printf("color space conversion time:%ld\n",1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec);
		if(ioctl(videoFD,VIDIOC_QBUF,&buff)<0)
		{
			printf("error dbuf\n");
			pthread_exit(0);
		}

	}
	pthread_exit(0);
}


int streamOn()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(videoFD,VIDIOC_STREAMON,&type)<0)
	{
		printf("VIDIOC_STREAMON failed\n");
		return -1;
	}

	pthread_create(&readFrame,NULL,readframe,NULL);
	return 0;
}

int main(int argc,char *argv[])
{
	int type=atoi(argv[1]);   //1:display 0:generate
	if(initV4L2()<0)
	{
		printf("init v4l2 failed\n");
		return -1;
	}

	if(initG2D()<0)
	{
		printf("init g2d failed\n");
		return -1;
	}

	if(initMem()<0)
	{
		printf("init mem failed\n");
		return -1;
	}
/*
	if(initFB()<0)
	{
		printf("init fb failed\n");
		return -1;
	}
*/
	if(streamOn()<0)
	{
		printf("stream on failed\n");
		return -1;
	}

	if(!InitParam(&front,&rear,&left,&right))
	{
		printf("init parameter failed\n");
		return -1;
	}
	int cnt=10;
	while(1)
	{
		/*
		if(type==1)
		{
			pthread_mutex_lock(&mutex);
			separateImage(front.srcImage,rear.srcImage,left.srcImage,right.srcImage,otheraddr);
			pthread_mutex_unlock(&mutex);
			combineImage(front.srcImage,rear.srcImage,left.srcImage,right.srcImage,addr);
			continue;
		}
		*/
		if(cnt>0)
		{
			pthread_mutex_lock(&mutex);
			separateImage(front.srcImage,rear.srcImage,left.srcImage,right.srcImage,otheraddr);
			pthread_mutex_unlock(&mutex);
	//		combineImage(front.srcImage,rear.srcImage,left.srcImage,right.srcImage,addr);
			cnt--;
			continue;
		}


		pthread_mutex_lock(&mutex);
		separateImage(front.srcImage,rear.srcImage,left.srcImage,right.srcImage,otheraddr);
		pthread_mutex_unlock(&mutex);
//		combineImage(front.srcImage,rear.srcImage,left.srcImage,right.srcImage,addr);
		if(!GetBirdImage(&front))
		{
			printf("get front birdImage failed\n");
			continue;
		}

		if(!GetBirdImage(&rear))
		{
			printf("get rear birdImage failed\n");
			continue;
		}

		if(!GetBirdImage(&left))
		{
			printf("get left birdImage failed\n");
			continue;
		}

		if(!GetBirdImage(&right))
		{
			printf("get right birdImage failed\n");
			continue;
		}
		GetMarkPoint(&front,&rear,&left,&right);
		StitchImage(&front,&rear,&left,&right);
		WriteStitchMap2(&front,&rear,&left,&right);
		printf("finish one frame\n");
		for(int i=0;i<4;i++)
		{
			drawpoint(front.birdImage,cvPoint(front.birdMarkPoint[i].x,front.birdMarkPoint[i].y));
			drawpoint(rear.birdImage,cvPoint(rear.birdMarkPoint[i].x,rear.birdMarkPoint[i].y));
			drawpoint(left.birdImage,cvPoint(left.birdMarkPoint[i].x,left.birdMarkPoint[i].y));
			drawpoint(right.birdImage,cvPoint(right.birdMarkPoint[i].x,right.birdMarkPoint[i].y));
		}

		
		cvSaveImage("frontsrcImage.jpg",front.srcImage);
		cvSaveImage("rearsrcImage.jpg",rear.srcImage);
		cvSaveImage("leftsrcImage.jpg",left.srcImage);
		cvSaveImage("rightsrcImage.jpg",right.srcImage);

		cvSaveImage("frontundistorImage.jpg",front.undistorImage);
		cvSaveImage("rearundistorImage.jpg",rear.undistorImage);
		cvSaveImage("leftundistorImage.jpg",left.undistorImage);
		cvSaveImage("rightundistorImage.jpg",right.undistorImage);

		cvSaveImage("frontbirdImage.jpg",front.birdImage);
		cvSaveImage("rearbirdImage.jpg",rear.birdImage);
		cvSaveImage("leftbirdImage.jpg",left.birdImage);
		cvSaveImage("rightbirdImage.jpg",right.birdImage);
		printf("saving image\n");
		cvSaveImage("stitchImage.jpg",front.stitchImage);
		ReleaseParam(&front,&rear,&left,&right);
		return 1;
	}
	return 1;	
}
