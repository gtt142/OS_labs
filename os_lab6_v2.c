/* The dice */

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <stdio.h>

typedef void (*sighandler)(int);
void brake(int);
void brake2(int);
jmp_buf pos;
static char digit[3];
static char* who[] = { "Man1: ", "Man2: ", "Com1: ","Com2: ",  "Bye. \n" };

static int mj = 0;
char buf[10];

int resMan = 0;
int resCom = 0;

int main(int argc, char** argv) {
int timeout;
int j = 0;
if(argc < 2) {
  write(2, "Usage: dice timeout\n", 20);
  exit(0);
} /* if */ 
if((timeout = atoi(argv[1])) < 1)
  timeout = 1;
digit[0] = '1' + getpid() % 6;
digit[1] = '\b';
digit[2] = '\n';
signal(SIGALRM, (sighandler) brake);
signal(SIGINT, (sighandler) brake2);

j = sigsetjmp(pos, 1);
if(j == 2 || j == 3){
	signal(SIGINT, SIG_IGN);
  alarm(timeout);
}
else{
  alarm(0);
}
write(1, who[j], 6);
while(j < 4) {
  write(1, digit, 2);
  digit[0] = (digit[0] < '6') ? digit[0] + 1 : '1';
  usleep(50000);
} /* while-j */

signal(SIGINT, SIG_DFL);
if (resMan > resCom){
	write(1, "Man win.\n", 9);
}
else if(resCom > resMan){
	write(1, "Com win.\n", 9);
}
else {
	write(1, "Draw.\n", 6);
}
printf("Man - %d\n", resMan);
printf("Com - %d\n", resCom);
exit(0);
}

void brake(int c) {
resCom += digit[0] - '0';
write(1, digit, 3);
signal(SIGALRM, (sighandler) brake);
longjmp(pos, ++mj);
return;
}

void brake2(int c) {
resMan += digit[0] - '0';
write(1, "\b \b\b", 4);	
write(1, digit, 3);
signal(SIGINT, (sighandler) brake2);
longjmp(pos, ++mj);
return;
}