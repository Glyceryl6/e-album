#ifndef _LCD_H
#define _LCD_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <dirent.h>

#define EVENT "/dev/ubuntu_event"
#define LCD "/dev/ubuntu_lcd"
#define SIZE 800*480*4

struct lcd {
    int lcdfd;
    unsigned int* FB;
    int ret;
};

int lcd_open();

int lcd_close();

int show_bmp(int x, int y, const char* pathname);

void get_xy(int* x, int* y);

void clear_background(unsigned int color);

#endif