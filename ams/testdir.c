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
