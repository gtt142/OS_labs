#include <unistd.h>
#include <stdio.h>
#include <sys/timeb.h>

/* clear standart text mode window */

void clrscr() {
unsigned char esc[11];

/* obtain set cursor to left-top window corner */

esc[0] = 27; esc[1] = '[';
esc[2] = 'H';
write(1,esc,3);

/* obtain clear escape sequence */

esc[2] = '2'; esc[3] = 'J';
write(1,esc,4);

return;

}

/* position cursor in standart text window */

void gotoxy(int ty, int tx, char c) {
unsigned char esc[17];
static unsigned char ystr[3]; /* vertical cursor location */
static unsigned char xstr[3]; /* horizontal cursor location */
int i;                        /* ESC-sequence current index */
int j;                        /* cursor location current index */

/* convert cursor location to text format */
//tx++;
if((tx > 99) || (ty > 99))
  tx = ty = 99;
if((tx < 1) || (ty < 1))
  tx = ty = 1;
xstr[0] = xstr[1] = xstr[2] = '\0';
ystr[0] = ystr[1] = ystr[2] = '\0';


if(ty > 1) {
	sprintf((char *) xstr,"%d",tx);
	sprintf((char *) ystr,"%d",ty - 1);

	esc[0] = 27; esc[1] = '[';
	i=2; j=0;
	while(ystr[j])
  		esc[i++]=ystr[j++];
	j=0;
	esc[i++]=';';
	while(xstr[j])
  		esc[i++]=xstr[j++];
	esc[i++]='H';
	//esc[i++]='\b';
	esc[i++] = ' ';
	esc[i] = '\0';
	write(1,esc,i);
}

xstr[0] = xstr[1] = xstr[2] = '\0';
ystr[0] = ystr[1] = ystr[2] = '\0';
sprintf((char *) xstr,"%d",tx);
sprintf((char *) ystr,"%d",ty);

/* obtain goto escape sequence */

esc[0] = 27; esc[1] = '[';
i=2; j=0;
while(ystr[j])
  esc[i++]=ystr[j++];
j=0;
esc[i++]=';';
while(xstr[j])
  esc[i++]=xstr[j++];
esc[i++]='H';
//esc[i++]='\b';
esc[i++] = c;
esc[i] = '\0';
write(1,esc,i);

return;
}

int main(int argc, char* argv[]) {
int x = 1;
int status;
int i;
int j = 0;
int PROCNUM = atoi(argv[1]);
int* pid = sbrk((PROCNUM+1)*sizeof(int));
char* lead = sbrk((PROCNUM+1)*sizeof(char));
int dist = atoi(argv[2]);
int p;
char bell = '\007';
struct timeb tp[1];
int jump;

clrscr();
while(j < PROCNUM) {
  if((pid[j] = fork()) == 0) {
    usleep(PROCNUM - j);
    while(x < dist) {
      gotoxy(x, j + 1, 'A' + j);
      ftime(tp);
      if((tp[0].millitm % (j + 'A')) != j)
        continue;
      x++;
     for(i=0; i<1000000; i++);
    }
    exit('A' + j);
  }
  j++;
}

j = 0;
while((p = wait(&status)) != (-1)) {
  for(i = 0; i < PROCNUM; i++)
    if(pid[i] == p)
      lead[j++] = (char) ((status >> 8) & '\377');
  write(1,&bell,1);
}
lead[j] = '\n';
sleep(1);
gotoxy(dist + 1, 1, '\n');
write(1, lead, PROCNUM + 1);
exit(0);
}
