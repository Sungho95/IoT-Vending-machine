#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <asm/ioctls.h>

#define KEY_NUM1	1
#define KEY_NUM2    2
#define KEY_NUM3    3
#define KEY_NUM4    4
#define KEY_NUM5    5
#define KEY_NUM6    6
#define KEY_NUM7    7
#define KEY_NUM8    8
#define KEY_NUM9    9
#define KEY_NUM10   10
#define KEY_NUM11   11
#define KEY_NUM12   12

#define MAXCHR	32
#define led_dev	"/dev/led"
#define dot "/dev/dot"
#define fnd_dev	"/dev/fnd"

#define dbg(x...)       printf(x)

static char tactswDev[] = "/dev/tactsw";
static int  tactswFd = (-1);
static char lcdDev[] = "/dev/clcd";
static int  lcdFd = (-1);

void dot_matrix(int x) {
	int dot_fd;
	unsigned char matrix[12][8] = {
		{0x04, 0x0a, 0x11, 0x3a, 0x5c, 0x88, 0x50, 0x20, }, //Bandage
		{0x7e, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x7e, }, //Hamburger
		{0x06, 0x0c, 0x18, 0xff, 0xdb, 0xe7, 0xff, 0x00, }, //Tissue
		{0x3c, 0x24, 0x3c, 0x24, 0x24, 0x24, 0x24, 0x3c, }, //Water
		{0x06, 0x1c, 0x30, 0xfc, 0x87, 0x85, 0xcf, 0x78, }, //Coffee
		{0x1f, 0x11, 0x39, 0x3d, 0x7f, 0x7c, 0xf0, 0xc0, }, //Icecream
		{0x24, 0x7e, 0xff, 0xbd, 0x3c, 0x3c, 0x3c, 0x3c, }, //Clothes
		{0x7e, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, }, //Pants
		{0x00, 0x18, 0xf8, 0xf8, 0x03, 0x1f, 0x1f, 0x00, }, //Shoes
		{0x00, 0x3c, 0x7e, 0x7e, 0xff, 0xff, 0x00, 0x00, }, //Hat
		{0x00, 0x00, 0x3f, 0xe1, 0x3e, 0x00, 0x00, 0x00, }, //Knife
		{0xe5, 0x82, 0xe5, 0x80, 0xe7, 0x12, 0x12, 0x12, }  //EXIT
	};

	dot_fd = open(dot, O_RDWR);
	if (dot_fd < 0) {
		printf("Open Error\n");
	}

	write(dot_fd, &matrix[x], sizeof(matrix));
	sleep(2);

	close(dot_fd);

}

int FND_Out(char a, char b, char c, char d) {

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
	sleep(1);
	close(fnd_fd);
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

void print_lcd(char* av) {

	int n;
	char        buf[MAXCHR];

	lcdFd = open(lcdDev, O_RDWR);
	if (lcdFd < 0) {
		fprintf(stderr, "cannot open LCD (%d)", lcdFd);
		exit(2);
	}

	memset(buf, 0, sizeof(buf));

	n = strlen(av);

	memcpy(buf, av, n);

	write(lcdFd, buf, MAXCHR);

	close(lcdFd);

}

void print_coin(void) {

	printf("\n");
	printf("************* Please inssert money *************\n");
	printf("*1. 1,000 won    2. 5,000 won    3. 10,000 won *\n");
	printf("************************************************\n");
	printf("\n");

}

void print_menu(void) {

	printf("\n");
	printf("******************* Menu ********************\n");
	printf("*  1. Bandage   2. Hamburger   3. Tissue    *\n");
	printf("*  4. Water     5. Coffee      6. Icecream  *\n");
	printf("*  7. Clothes   8. Pants       9. Shoes     *\n");
	printf("* 10. Hat      11. Knife      12. EXIT      *\n");
	printf("*********************************************\n");
	printf("\n");
	printf("***************** Price(won) ****************\n");
	printf("*  1. 1,000     2. 3,000       3. 2,000     *\n");
	printf("*  4. 500       5. 2,500       6. 1,000     *\n");
	printf("*  7. 5,000     8. 6,000       9. 9,000     *\n");
	printf("* 10. 8,000    11. 4,000      12. EXIT      *\n");
	printf("*********************************************\n");
	printf("\n");

}

unsigned char tactsw_get(int tmo)
{
	unsigned char b;

	if (tmo) {
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
	unsigned char d;
	int dev;
	int a = 1;
	int b = 1;
	int money = 0;

	if ((tactswFd = open(tactswDev, O_RDONLY)) < 0) {         // KEY open
		perror("open faile /dev/key");
		exit(-1);
	}
	printf("tact opend");

	dev = open(led_dev, O_RDWR);
	if (dev < 0) {
		fprintf(stderr, "cannot open LED Device (%d)", dev);
		exit(2);
	}
	
	print_coin();

	while (a) {
		d = tactsw_get(10);
		switch (d) {
			case KEY_NUM1:
				print_lcd("+1,000 won");
				led_all(&dev);
				money = money + 1000;
				break;
			case KEY_NUM2:
				print_lcd("+5,000 won");
				led_all(&dev);
				money = money + 5000;
				break;
			case KEY_NUM3:
				print_lcd("+10,000 won");
				led_all(&dev);
				money = money + 10000;
				break;
			case KEY_NUM4:
				printf("Money : %d won", money);
				a = 0;
				break;
		}
		sleep(1);
	}

	printf("\nMoney : %d \n", money);
	print_menu();

	while (b) {
		c = tactsw_get(10);
		switch (c) {
			case KEY_NUM1:
				if (money >= 1000) {
					print_lcd("Bandage");
					led_all(&dev);
					FND_Out(1, 0, 0, 0);
					money = money - 1000;
					printf("Money : %d won\n", money);
					dot_matrix(0);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM2:
				if (money >= 3000) {
					print_lcd("Hamburger");
					led_all(&dev);
					FND_Out(3, 0, 0, 0);
					money = money - 3000;
					printf("Money : %d won\n", money);
					dot_matrix(1);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM3:
				if (money >= 2000) {
					print_lcd("Tissue");
					led_all(&dev);
					FND_Out(2, 0, 0, 0);
					money = money - 2000;
					printf("Money : %d won\n", money);
					dot_matrix(2);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM4:
				if (money >= 500) {
					print_lcd("Water");
					led_all(&dev);
					FND_Out(0, 5, 0, 0);
					money = money - 500;
					printf("Money : %d won\n", money);
					dot_matrix(3);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM5:
				if (money >= 2500) {
					print_lcd("Coffee");
					led_all(&dev);
					FND_Out(2, 5, 0, 0);
					money = money - 2500;
					printf("Money : %d won\n", money);
					dot_matrix(4);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM6:
				if (money >= 1000) {
					print_lcd("Icecream");
					led_all(&dev);
					FND_Out(1, 0, 0, 0);
					money = money - 1000;
					printf("Money : %d won\n", money);
					dot_matrix(5);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM7:
				if (money >= 5000) {
					print_lcd("Clothes");
					led_all(&dev);
					FND_Out(5, 0, 0, 0);
					money = money - 5000;
					printf("Money : %d won\n", money);
					dot_matrix(6);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM8:
				if (money >= 6000) {
					print_lcd("Pants");
					led_all(&dev);
					FND_Out(6, 0, 0, 0);
					money = money - 6000;
					printf("Money : %d won\n", money);
					dot_matrix(7);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				
				break;
			case KEY_NUM9:
				if (money >= 9000) {
					print_lcd("Shoes");
					led_all(&dev);
					FND_Out(9, 0, 0, 0);
					money = money - 9000;
					printf("Money : %d won\n", money);
					dot_matrix(8);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM10:
				if (money >= 8000) {
					print_lcd("Hat");
					led_all(&dev);
					FND_Out(8, 0, 0, 0);
					money = money - 8000;
					printf("Money : %d won\n", money);
					dot_matrix(9);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM11:
				if (money >= 4000) {
					print_lcd("Knife");
					led_all(&dev);
					FND_Out(4, 0, 0, 0);
					money = money - 4000;
					printf("Money : %d won\n", money);
					dot_matrix(10);
				}
				else {
					print_lcd("Insufficient balance.");
				}
				break;
			case KEY_NUM12:
				print_lcd("EXIT : Good bye");
				led_all(&dev);
				printf("The remaining money is %d won.\n", money);
				dot_matrix(11);
				b = 0;
				break;
			default:
				break;
		}
		sleep(1);
		if (money == 0) {
			b = 0;
			print_lcd("You've spent all your money.");
		}
	}

	return 0;
}
