/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:19:22  jussi
  Added VCS header
*/

/*
   extendAms.c: extend AMS file and store in TData paged file format
   Usage: extendAms infile outfile [numTimes]
   where numTimes == # of times file should be replicated. 
   Default == 1.
*/

#include <stdio.h>
#include <libc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/uio.h>
#include <unistd.h>
#include "ams.h"

void Usage(char *name){
	printf("Usage: %s infile outfile [numTimes]\n",name);
	exit(1);
}

main(int argc, char **argv){
int numTimes = 1;
char *infile, *outfile;

	if (argc < 3)
		Usage(argv[0]);
	
	infile = argv[1];
	outfile = argv[2];

	if (argc == 4){
		numTimes = atoi(argv[3]);
		if (numTimes < 1){
			fprintf(stderr,"File can't be replicated %d times, < 1\n",numTimes);
			exit(1);
		}
	}

	/* Open output file */
	int outfd;
	if ((outfd=open(outfile,O_WRONLY|O_CREAT|O_TRUNC, 0666))< 0){
		perror("open output file");
		exit(1);
	}

	/* Open input file */
	int fd = open(infile,O_RDONLY,0);
	if (fd < 0){
		fprintf(stderr,"can't open infile %s\n",infile);
		exit(1);
	}

	/* go through the file */
	float firstTime; /* time of 1st record */
	float lastTime; /* time of last record */
	int numRecs= 0; 	/* # of records encountered */
	float timeIncr = 0.0; /* Absolute duration of one file */
	int iter;
	for (iter=0; iter < numTimes; iter++){
		/* seek to beginning of file */
#ifdef SEEK_SET
		if (lseek(fd,0,SEEK_SET)< 0)
#else
		if (lseek(fd,0,L_SET)< 0)
#endif
								{
			fprintf(stderr,"error seeking infile\n");
			exit(1);
		}
		int stat;
		AmsData rec;
		while((stat=read(fd,(char *)&rec,sizeof(AmsData)))== sizeof(AmsData)){
			if (iter == 0){
				if (++numRecs == 1)
					firstTime = rec.time;
				lastTime = rec.time;
			}

			/* adjust time */
			rec.time += iter*timeIncr;

			/* put it through */
			if (write(outfd,(char *)&rec,sizeof(rec))!= sizeof(AmsData)){
				perror("write");
				exit(1);
			}
		}
		 
		if (stat != 0){
			perror("error reading from infile");
			exit(1);
		}

		if (iter == 0){
			/* set timeIncr*/
			timeIncr = lastTime+ (lastTime-firstTime)/((float)numRecs);
		}
	}

	/* done */
	close(fd);
	close(outfd);
}
