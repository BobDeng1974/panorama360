#include "video.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include "g2d_driver.h"
#include "./Api/Api.h"

unsigned int potheraddr = KERNEL_PHY_ADDR; 
int videoFD;
int g2dFD;
int memFD;

struct buffer
{
	char * start;
	unsigned int length;
	int index;
};

struct buffer * buffers = NULL;

int flag=0;
g2d_blt tmp;
unsigned char * otheraddr = NULL;
pthread_spinlock_t spinlock;
pthread_t readbuf;

unsigned int *front,*rear,*left,*right;
unsigned char *cfront,*crear,*cleft,*cright;

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
	    printf("VIDIOC_S_FMT error!\n");												return -1;
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
	req.count=BUF_NUM;
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
		{																		            printf("error querybuf\n");											            return -1;
		}

		buffers[n].length=buf.length;
		buffers[n].start=(char*)mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,videoFD,buf.m.offset);
		buffers[n].index=n;
		printf("index %d:buf length:%u\n",buf.length);
		printf("MMAP:%p OFF:%u\n",buffers[n].start,buf.m.offset);
		if(buffers[n].start==MAP_FAILED)
		{
			printf("mmap failed\n");														return -1;
	    }

		if(ioctl(videoFD,VIDIOC_QBUF,&buf)<0)
	    {																		            printf("VIDIOC_QBUF failed\n");										            return -1;
		}
	}
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

int initMEM()
{
	memFD=open("/dev/mem",O_RDWR);
	if(memFD<0)
	{
		printf("error open mem\n");
		return -1;
	}
	otheraddr=(unsigned char*)mmap(NULL,1440*960*4,PROT_READ|PROT_WRITE,MAP_SHARED,memFD,potheraddr);
	return 0;
}


int nv12toargb(int w,int h,unsigned int pnv,unsigned int potherad)
{
	tmp.flag               =G2D_BLT_NONE;
	tmp.src_image.addr[0]  =pnv;
	tmp.src_image.addr[1]  =pnv+w*h;
	tmp.src_image.w        =w;
	tmp.src_image.h        =h;
	tmp.src_image.format   =G2D_FMT_PYUV420UVC;
	tmp.src_image.pixel_seq=G2D_SEQ_NORMAL;
	tmp.src_rect.x         =0;
	tmp.src_rect.y         =0;
	tmp.src_rect.w         =w;
	tmp.src_rect.h         =h;
	tmp.dst_image.addr[0]  =potherad;
	tmp.dst_image.w        =w;
	tmp.dst_image.h        =h;
	tmp.dst_image.format   =G2D_FMT_XBGR8888; //G2D_FMT_XBGR8888 for GPU
	tmp.dst_image.pixel_seq=G2D_SEQ_NORMAL;
	tmp.dst_x              =0;
	tmp.dst_y              =0;
	tmp.color              =0;
	tmp.alpha              =0;

	if(ioctl(g2dFD,G2D_CMD_BITBLT,&tmp)<0)
	{
		printf("error bitblt\n");
		return -1;
	}
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

		if(buff.index&1)
	    {
			pthread_spin_lock(&spinlock);
			flag=1;
		//	gettimeofday(&start,NULL);
			nv12toargb(1440,960,buff.m.offset,potheraddr);
		//	gettimeofday(&end,NULL);
			pthread_spin_unlock(&spinlock);
        //  printf("stretching time:%ld\n",1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec);
		}
        if(ioctl(videoFD,VIDIOC_QBUF,&buff)<0)
		{
			printf("error qbuf\n");
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
	pthread_spin_init(&spinlock,0);
	pthread_create(&readbuf,NULL,readframe,NULL);
	return 0;
}


int setupVideo()
{
	initV4L2();
	initG2D();
	initMEM();
}

int miscInit()
{
	InitStitchMap_cs(&front,&rear,&left,&right);
	cfront=(unsigned char*)front;
	crear=(unsigned char*)rear;
	cleft=(unsigned char*)left;
	cright=(unsigned char*)right;
}

int videoClear()
{
	pthread_cancel(readbuf);
	usleep(50000);
	enum v4l2_buf_type type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(videoFD,VIDIOC_STREAMOFF,&type)<0)
	{
		printf("error streamoff \n");
		return -1;
	}
	printf("stream off\n");

	for(int j=0;j<BUF_NUM;j++)
	{
		if(munmap(buffers[j].start,buffers[j].length)<0)
		{
			printf("munmap error\n");
			return -1;
		}
	}
	free(buffers);
	printf("free buffers\n");

	if(munmap(otheraddr,1440*960*4)<0)
	{
		printf("error munmap otheraddr \n");
		return -1;
	}

	close(videoFD);
	printf("close video fd\n");
	close(g2dFD);
	printf("close g2d fd\n");
	close(memFD);
	printf("close mem fd\n");
}

