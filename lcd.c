#include <sys/types.h>
#include <sys/stat.h>
#include "lcd.h"

struct lcd lcdDev;

int lcd_open() {
	lcdDev.lcdfd = open(LCD, O_RDWR);
	if(lcdDev.lcdfd == -1) {
		perror("驱动打开失败\n");
		return -1;
	}
    
	lcdDev.FB = (unsigned int*)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, lcdDev.lcdfd, 0);
	if(lcdDev.FB == MAP_FAILED) {
		perror("内存映射失败\n");
		return -1;
	}

	return lcdDev.lcdfd;
}

int lcd_close() {
	lcdDev.ret = munmap(lcdDev.FB, SIZE);
	if(lcdDev.ret == -1) {
		perror("映射内存关闭失败\n");
		return -1;
	}
    
	lcdDev.ret = close(lcdDev.lcdfd);
	if(lcdDev.ret == -1) {
		perror("设备关闭失败\n");
		return -1;
	}

	return 0;
}

int show_bmp(int x, int y, const char*pathname) {
	int bmpfd = open(pathname, O_RDWR);
	if(bmpfd == -1) {
		perror("图片打开失败\n");
		return -1;
	}
	unsigned char info[54] = {0};
	read(bmpfd, info, 54);
	if(info[0] != 'B' && info[1] != 'M') {
		perror("该图片格式错误，不是bmp图片\n");
		return -1;
	}
	unsigned int bmp_w = info[18] << 0 | info[19] << 8 | info[20] << 16 | info[21] << 24;
	unsigned int bmp_h = info[22] << 0 | info[23] << 8 | info[24] << 16 | info[25] << 24;
	printf("当前文件的宽度和高度为:(%d, %d)\n", bmp_w, bmp_h);
	unsigned char *bmpbuf = malloc(bmp_w * bmp_h * 3);
	int ret = read(bmpfd, bmpbuf, bmp_w * bmp_h * 3);
	unsigned int lcdbuf [bmp_w * bmp_h];
	for (int i = 0; i < bmp_w * bmp_h; i++) {
		lcdbuf[i] = bmpbuf[3*i+0] << 0 | bmpbuf[3*i+1] << 8 | bmpbuf[3 * i + 2] << 16 | 0x00 << 24;
	}
	for (int j = 0; j < bmp_h; j++) {
		for (int i = 0; i < bmp_w; i++) {
			lcdDev.FB[(i+x)+(j+y)*800] = lcdbuf[i+(bmp_h-1-j)*bmp_w];
		}
	}
	lcdDev.ret = close(bmpfd);
	if(lcdDev.ret == -1) {
		perror("关闭失败\n");
		return -1;
	}
}

void get_xy(int *x, int *y) {
	*x = -1;
	*y = -1;
	int tsfd = open(EVENT, O_RDONLY);
	if(tsfd == -1) {
		perror("驱动打开失败\n");
		return ;
	}
	struct input_event data;
	while(1) {
		int size = read(tsfd, &data, sizeof(data));
		if(size < 0) {
			perror("读取失败");
			exit(1);
		}
		if(data.type == EV_ABS && data.code == ABS_X) {
			*x = data.value;
		} else if(data.type == EV_ABS && data.code == ABS_Y) {
			*y = data.value;
			if(*x >= 0) {
				break;
			}
		}
	}
	int ret = close(tsfd);
	if(ret == -1) {
		perror("关闭失败");
		return ;
	}
}

void clear_background(unsigned int color) {
	for (int i = 0; i < 800 * 480; i++) {
		lcdDev.FB[i] = color;
	}
}