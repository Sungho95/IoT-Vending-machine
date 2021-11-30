#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#define KEY_NUM1 1
#define KEY_NUM2 2
#define KEY_NUM3 3
#define KEY_NUM4 4
#define KEY_NUM5 5
#define KEY_NUM6 6
#define KEY_NUM7 7
#define KEY_NUM8 8
#define KEY_NUM9 9
#define KEY_NUM10 10
#define KEY_NUM11 11
#define KEY_NUM12 12

#define clcd "/dev/clcd"

#define led_dev	"/dev/led"
#define dot_dev "/dev/dot"
#define fnd_dev	"/dev/fnd"
#define dbg(x...) printf(x)

static char tactswDev[] = "/dev/tactsw";
static int tactswFd = (-1);

static char lcdDev[] = "/dev/clcd";
static int  lcdFd = (-1);

unsigned char row[36][8] = {
	{0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},   //0
	{0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C},   //1
	{0x1C, 0x22, 0x02, 0x02, 0x1C, 0x20, 0x20, 0x3E},   //2
	{0x1C, 0x22, 0x02, 0x1C, 0x02, 0x02, 0x22, 0x1C},   //3
	{0x04, 0x0C, 0x14, 0x24, 0x44, 0x3E, 0x04, 0x04},   //4
	{0x1E, 0x20, 0x20, 0x1C, 0x02, 0x02, 0x22, 0x1C},   //5
	{0x3C, 0x40, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C},   //6
	{0x3E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02},   //7
	{0x1C, 0x22, 0x22, 0x1C, 0x22, 0x22, 0x22, 0x1C},   //8
	{0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},   //0
	{0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42},   //A
	{0x1E, 0x22, 0x22, 0x1E, 0x22, 0x22, 0x22, 0x1E},   //B
	{0x1C, 0x22, 0x02, 0x02, 0x02, 0x02, 0x22, 0x1C},   //C
	{0x38, 0x44, 0x42, 0x42, 0x42, 0x42, 0x44, 0x38},   //D
	{0x3E, 0x20, 0x20, 0x3E, 0x20, 0x20, 0x20, 0x3E},   //E
	{0x3E, 0x20, 0x20, 0x3E, 0x20, 0x20, 0x20, 0x20},   //F
	{0x1C, 0x22, 0x42, 0x40, 0x40, 0x47, 0x42, 0x3C},   //G
	{0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42},   //H
	{0x1C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C},   //I
	{0x1C, 0x08, 0x08, 0x08, 0x08, 0x48, 0x48, 0x30},   //J
	{0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x44},   //K
	{0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3E},   //L
	{0x81, 0xC3, 0xA5, 0x99, 0x81, 0x81, 0x81, 0x81},   //M
	{0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42},   //N
	{0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C},   //O
	{0x7C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40},   //P
	{0x38, 0x44, 0x82, 0x82, 0x82, 0x8A, 0x44, 0x3A},   //Q
	{0x7C, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42},   //R
	{0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x3C},   //S
	{0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08},   //T
	{0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},   //U
	{0x81, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18},   //V
	{0x81, 0x99, 0x99, 0x99, 0x99, 0x99, 0x5A, 0x24},   //W
	{0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81},   //X
	{0x81, 0x81, 0x42, 0x42, 0x3C, 0x18, 0x18, 0x18},   //Y
	{0xFF, 0x03, 0x02, 0x04, 0x08, 0x10, 0x60, 0xFF}   //Z
};

unsigned int DOTTest(int val) {

	unsigned char dot_data[8];
	int dot_fd = 0;

	memcpy(dot_data, row[val], 8);

	dot_fd = open(dot_dev, O_RDWR);
	if (dot_fd < 0) {
		printf("Can't Open Device\n");
	}

	write(dot_fd, &dot_data, sizeof(dot_data));

	return 0;
}

void led_down_shift(int* dev) {

	unsigned char data, data2;
	int led_device, count;

	for (count = 0; count < 16; count++) {
		data2 = (~(data >> 7)) & 0x01;
		data = (data << 1) | data2;
		usleep(50000);
		write(*dev, &data, sizeof(unsigned char));
	}
}

void led_up_shift(int* dev) {

	unsigned char data, data2;
	int led_device, count;

	for (count = 0; count < 16; count++) {
		data2 = (~(data << 7)) & 0x80;
		data = (data >> 1) | data2;
		usleep(50000);
		write(*dev, &data, sizeof(unsigned char));
	}

}
void P9(int* dev) {
	unsigned char data;
	int led_device, count;

	for (count = 0; count < 16; count++) {
		if (count % 2) {
			data = 0xAA;
			write(*dev, &data, sizeof(unsigned char));
		}
		else {
			data = 0x55;
			write(*dev, &data, sizeof(unsigned char));
		}
		usleep(100000);
	}
}

void led_all(int* dev) {
	unsigned char data;
	int led_device, count;

	for (count = 0; count < 16; count++) {
		if (count % 2) {
			data = 0xff;
			write(*dev, &data, sizeof(unsigned char));
		}
		else {
			data = 0x00;
			write(*dev, &data, sizeof(unsigned char));
		}
		usleep(100000);
	}
}

int FNDTest(char a, char b, char c, char d) {

	unsigned char FND_DATA_TBL[] = {
			0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,
			0x83,0xC6,0xA1,0x86,0x8E,0xC0,0xF9,0xA4,0xB0,0x99,0x89
	};

	int fnd_fd = 0;

	unsigned char fnd_num[4];

	fnd_num[0] = FND_DATA_TBL[a];
	fnd_num[1] = FND_DATA_TBL[b];
	fnd_num[2] = FND_DATA_TBL[c];
	fnd_num[3] = FND_DATA_TBL[d];

	fnd_fd = open(fnd_dev, O_RDWR);

	if (fnd_fd < 0) {
		printf("Can't Open Device\n");
	}
	write(fnd_fd, &fnd_num, sizeof(fnd_num));
}

unsigned char tactsw_get(int tmo)
{
	unsigned char b;

	if (tmo){
		if (tmo < 0)
			tmo = ~tmo * 1000;
		else
			tmo *= 1000000;

		while (tmo > 0) {
			usleep(10000);
		read(tactswFd, &b, sizeof(b));
		if (b) return(b);
				tmo -= 10000;
		}
		return(-1);
	}
	else {
		read(tactswFd, &b, sizeof(b));
		return(b);
	}
}

int main()
{
	unsigned char c;
	int dev;
	int clcd_d;

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0)
	{
		printf("no device.\n");
		return 0;
	}

	dev = open(led_dev, O_RDWR);
	if (dev < 0) {
		fprintf(stderr, "cannot open LED Device (%d)", dev);
		exit(2);
	}

	if((tactswFd = open(tactswDev, O_RDONLY)) < 0){
		perror("open faile /dev/key");
		exit(-1);
	}
	
	while(1){
		c = tactsw_get(10);
		switch(c) {
			case KEY_NUM1: write(clcd_d, "Coke", 5); FNDTest(1, 5, 0, 0); break;
			case KEY_NUM2: write(clcd_d, "Cider", 6); FNDTest(1, 5, 0, 0); break;
			case KEY_NUM3: write(clcd_d, "Fanta", 6); FNDTest(1, 3, 0, 0); break;
			case KEY_NUM4: write(clcd_d, "Grape", 6); FNDTest(1, 6, 0, 0); break;
			case KEY_NUM5: write(clcd_d, "Orange", 7); FNDTest(1, 6, 0, 0); break;
			case KEY_NUM6: write(clcd_d, "Getorade", 9); FNDTest(1, 0, 0, 0); break;
			case KEY_NUM7: write(clcd_d, "Powerade", 9); FNDTest(1, 1, 0, 0); break;
			case KEY_NUM8: write(clcd_d, "Redbull", 8); FNDTest(2, 0, 0, 0); break;
			case KEY_NUM9: write(clcd_d, "Hotsix", 7); FNDTest(1, 8, 0, 0); break;
			case KEY_NUM10: write(clcd_d, "Bandage", 8); FNDTest(3, 0, 0, 0); break;
			case KEY_NUM11: write(clcd_d, "Tissue", 7); FNDTest(1, 0, 0, 0); break;
			case KEY_NUM12: write(clcd_d, "EXIT", 5); break;
			default: dbg("end"); sleep(1); break;
		}
		led_all(&dev);
		sleep(1);
	}
	
	return 0;
}
