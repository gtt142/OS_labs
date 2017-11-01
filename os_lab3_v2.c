//  Клавиатурный тренажер
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <time.h>

int textmode(int);
int randline();
int getch();

static char pattern[80];

int textmode(int mode) {
    static struct termios con[2];
    if(mode > 0)
        return(tcsetattr(0, TCSAFLUSH, &con[1]));
    tcgetattr(0, &con[0]);
    tcgetattr(0, &con[1]);
    con[0].c_lflag &= ~(ICANON | ECHO | ISIG);
    con[0].c_iflag &= ~(ISTRIP | IXOFF | IXANY | IXON);
    con[0].c_oflag |= CS8;
    con[0].c_cc[VMIN] = 2;
    con[0].c_cc[VTIME] = 1;
    return(tcsetattr(0, TCSAFLUSH, &con[0]));
}

int getch() {
    unsigned char c[2];
    static int len=0;
/*if(len > 1) {
  c[0] = len;
  len = 0;
  return(c[0]);
}
*/

    c[0] = c[1] = 0;
    if((len = read(0, c, 2)) < 2){
    //	printf("+++\n");
        return(c[0]);
    }
    if(c[0] == 27){
    //	printf("__!!!__\n");
        c[0] = 0;
    }
    /* len = c[1]; */
    //	printf("~~~\n");
    ungetc(c[1], stdin);
    return(c[0]);
}

int randline(int fd, int len) {
    int i=0;
    int r;
    char c;
    srand(time(NULL));
    
    r = rand() % len;
    while (i < r){
        read(fd, &c, 1);
        if(c == '\n')
            i++;;
    }
    i = 0;
    while(read(fd, &c, 1) && c != '\n') {
        pattern[i++] = c;
    }
    pattern[i] = '\0';
    return(0);
}

int main(int argc, char* argv[]) {
    int len;
    int i=0;
    int err=0;
    unsigned char c;
    int charCount = 0;
    time_t start_time = time(NULL);
    len = atoi(argv[1]);
    char buf[256];

    int fd;
    if(argc != 2){
        write(1, "Wrong format.\n", 14);
        return 0;
    }
    if((fd = open(argv[1], 0)) < 0){
        int len;
        strcpy(buf, argv[1]);
        strncat(buf, ": ", 2);
        strcat(buf, strerror(errno));
        len = strlen(buf);
        buf[len] = '\n';
        write(2, buf, len + 1);
        return 0;
    }

    c = '\0';
    int strNum = 0;
    int qw = 0;
    while (read(fd, &c, 1) > 0) {
        if(c == '\n')
            strNum++;
    }
    while(1){
        lseek (fd, 0, SEEK_SET);
        randline(fd, strNum);
        len = strlen(pattern);
        write(1, pattern, len);
        write(1, "\n", 1);
        textmode(0);
        i = 0;
        while(i < len) {
            switch(c = getch()) {
                case 0:  c = '\007';
                    switch(getch()) {
                        case 67: c = pattern[i];
                            break;
                        case 68: if(i == 0)
                              break;
                            i--;
                            write(1, "\b", 1);
                            continue;
                        default: break;
                    }
                    break;
                case 27: i = len;
                    break;
                default: if(c != pattern[i])
                    c = '\007';
                    break;
            }
            if(c != 27) {
                (c == '\007') ? err++ : (i++, charCount++);
                write(1, &c, 1);
            }
            else
                break;

        }
        write(1, "\n", 1);
        textmode(1);
        if(c == 27)
            break;
    }
    close(fd);
    int wrkTm = (int)(time(NULL) - start_time);
    printf("Work time: %d\n", wrkTm);
    printf("Error counts: %d\n", err);
    printf("Avg Speed: %f\n", ((float)(charCount)/wrkTm));
    return(err);
}
