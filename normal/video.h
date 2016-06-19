#ifndef VIDEO_H
#define VIDEO_H

#define BUF_NUM 2
#define KERNEL_PHY_ADDR 0x48000000

int setupVideo(void);
int streamOn(void);
int miscInit(void);
int videoclear(void);
#endif
