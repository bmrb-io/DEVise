/*
  $Id$

  $Log$
  Revision 1.3  1995/09/05 20:08:33  jussi
  Updated header

  Revision 1.2  1995/09/05 20:05:24  jussi
  Added VCS header.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>

main(int argc, char **argv){
DIR *dirp;
struct dirent *dp;
char *dir;

	if (argc != 2){
		printf("usage: testdir dir\n");
		exit(1);
	}
	dir = argv[1];
    printf("directory %s\n",dir);
	dirp = opendir(dir);
	if (dirp != NULL){
		for (dp = readdir(dirp); dp != (struct dirent *)NULL; dp = readdir(dirp)){
				printf("file %s\n",dp->d_name);
																						}
		closedir(dirp);
	}

}
