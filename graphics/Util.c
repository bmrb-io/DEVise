/*
  $Id$

  $Log$*/

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#ifdef PENTIUM
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <unistd.h>
#include "Util.h"
#include "Exit.h"

long ModTime(char *fname){

	struct stat sbuf;
	if (stat(fname,&sbuf)<0){
		fprintf(stderr,"ModTime:: error\n");
		Exit::DoExit(2);
	}
	return (long)sbuf.st_mtime;
}

char *CopyString(char *str){
	int len = strlen(str)+1;
	char *result = new char[len];
	strcpy(result, str);
	return result;
}

static char dateBuf[21];
char *DateString(time_t tm){
	int i;
	char *dateStr =ctime(&tm);
	for (i=0; i < 7; i++)
		dateBuf[i] = dateStr[i+4];

	for (i=7; i < 11; i++)
		dateBuf[i] = dateStr[i+13];

	dateBuf[11] = ' ';

	for (i=12; i < 20; i++)
		dateBuf[i] = dateStr[i-1];
	dateBuf[21] = '\0';

	return dateBuf;
}

void ClearDir(char *dir){
	/* clear directory */
/*
	printf("ClearDir %s\n",dir);
*/
	DIR *dirp = opendir(dir);
	if (dirp != NULL){
#ifdef PENTIUM
		struct dirent *dp;
#else
		struct direct *dp;
#endif
		for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)){
#if defined(IBM_POWER_STATION)||defined(PENTIUM)
			struct dirent *realdp = (struct dirent *)dp;
#else
			struct direct *realdp = dp;
#endif
			if (strcmp(realdp->d_name,".") != 0 &&
				strcmp(realdp->d_name,"..") != 0 ){
				char buf[512];
				sprintf(buf,"%s/%s",dir,realdp->d_name);
				/*
				printf("unlinking %s\n", buf);
				*/
				unlink(buf);
			}
		}
		closedir(dirp);
	}
}

/* Check if directory exists. Make directory if not already exists
Clear directory if clear == true*/
void CheckAndMakeDirectory(char *dir, int clear ){
	struct stat sbuf;
	int ret;
	ret = stat(dir,&sbuf);
    if (ret >=  0 ) {
		if (!(sbuf.st_mode & S_IFDIR)){
			fprintf(stderr,"Init:: %s not a directory\n");
			Exit::DoExit(1);
		}
        if (clear){
			ClearDir(dir);
		}
	} else {
		/* make new directory */
		int code = mkdir(dir,0755);
		if (code < 0 ){
			printf("Init::can't make directory %s\n",dir);
			perror("");
			Exit::DoExit(1);
		}
	}
}

