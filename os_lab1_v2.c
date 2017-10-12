#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int diag(char*, char*, int);
unsigned char* recode(int);
static unsigned char buf[BUFSIZ];

int main(int argc, char* argv[]) {
	int fds = 0;
	int fdt = 1;
	int num = 0;

	while((num = read(fds, buf, BUFSIZ)) > 0)
		write(fdt, recode(num), num);
	exit(0);
}

int diag(char* name, char* mes, int code) {
	int len;
	strcpy(buf, name);
	strncat(buf, ": ", 2);
	strcat(buf, mes);
	len = strlen(buf);
	buf[len] = '\n';
	write(2, buf, len + 1);
	return(code);
}

unsigned char* recode(int n) {
	unsigned char c;
	int i = 0;
	static char tab[32] = {
		30, 0, 1, 22, 4, 5, 20, 3, 21, 8, 9, 10, 11, 12, 13, 14, 15, 
		31, 16, 17, 18, 19, 6, 2, 28, 27, 7, 24, 29, 25, 23, 26
	};
	for(i = 0; i < n; i++) {
		c = buf[i];
		if(c < 192) {
			continue;
		}
		if(c < 224) {
			buf[i] = tab[c - 192] + 224;
		} else {
			buf[i] = tab[c - 224] + 192;
		}
	}
	return(buf);
}