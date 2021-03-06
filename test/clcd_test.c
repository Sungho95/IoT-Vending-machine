#include    <stdio.h>
#include    <stdlib.h>
#include    <fcntl.h>
#include    <string.h>

#define clcd "/dev/clcd"

int main()
{
    int clcd_d;
    clcd_d = open(clcd, O_RDWR);

    if (clcd_d < 0)
    {
        printf(" no \n");
        return 0;
    }

    write(clcd_d, "Hello World", 12);

    close(clcd_d);
    return 0;
}

/*
static char lcdDev[] = "/dev/clcd";
static int  lcdFd = (-1);

#define     MAXCHR  32

main(int ac, char *av[])
{
    int n;
    char        buf[MAXCHR];

    lcdFd = open( lcdDev, O_RDWR);
    if (lcdFd < 0) {
        fprintf(stderr, "cannot open LCD (%d)", lcdFd);
        exit(2);
    }

    memset(buf, 0, sizeof(buf));
    if (ac > 1) {
        n = strlen(av[1]);

        if (n > MAXCHR)
            n = MAXCHR; //plus the newline
		memcpy(buf, av[1], n);
    }
	
	write(lcdFd, buf, MAXCHR);

}
*/
/* EOF */
