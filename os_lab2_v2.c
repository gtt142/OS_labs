#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

int scanfolder(int, char*);
int diag(char*);
int insersort(int);

struct MFile {
    char *name;
    time_t modDate;
};

static struct MFile *infoStruct[512];

int insertsort(int num) {
    int i=0;
    int j;
    struct MFile* p;
    while(++i < num) {
        j = i - 1;
        p = infoStruct[i];
        while(p->modDate < infoStruct[j]->modDate) {
            infoStruct[j+1] = infoStruct[j];
            j--;
        }
    infoStruct[j+1] = p;
    }
    return(0);
}

int diag(char* mes) {
    write(1, mes, strlen(mes));
    write(1, ": ", 2);
    write(1, strerror(errno), strlen(strerror(errno)));
    write(1, "\n", 1);
    exit(errno);
}

int scanfolder(int type, char* suf) {
    DIR* fdir;
    struct dirent* folder;
    struct stat sbuf[1];
    char c = '\n';
    struct MFile fileInfo={&c,0};
    char* start;
    int count=0;
    int len;


    if((fdir = opendir(".")) == NULL)
        return(errno);
    start = sbrk(0);
    infoStruct[count] = sbrk(sizeof(struct MFile));
    *infoStruct[count] = fileInfo;
    count++;
    while((folder = readdir(fdir)) != NULL) { 
        //sbuf->st_mode = 0;
        stat(folder->d_name, sbuf);
        if(((sbuf->st_mode) & S_IFMT) != type) 
            continue;
        len = strlen(folder->d_name);
        fileInfo.modDate = sbuf->st_mtime;
        fileInfo.name = sbrk(len+1);
        infoStruct[count] = sbrk(sizeof(struct MFile));
        memcpy(fileInfo.name, folder->d_name, len);
        *infoStruct[count] = fileInfo;
        count++;
        infoStruct[count] = NULL;
    }    
    closedir(fdir);
    //qsort(start, count, 256, compare);
    insertsort(count);  
    count=1;
    while(infoStruct[count] != NULL) {
        len = strlen(infoStruct[count]->name);
        const time_t *fTime = &infoStruct[count]->modDate;
        char buf[32];
        strcpy(buf, ctime(fTime));
        int len2 = strlen(buf);
        write(1, buf, len2-1);
        write(1, "   ", 3);
        write(1, infoStruct[count]->name, len);
        write(1, suf, 2);
        count++;
    }    
    brk(start);
    return(0);
}

int main(int argc, char* argv[]) {
    if(argc != 2){
        write(1, "Wrong format.\n", 14);
        return 0;
    }
    if(chdir(argv[1]) < 0)
    	return(diag(argv[1]));  
    if(scanfolder(S_IFDIR, "/\n") > 0)
	   return(diag(argv[1]));  
    scanfolder(S_IFREG, "\n");
    return(0);
}

