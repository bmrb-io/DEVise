/* genAms.c */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ams.h"

#define NUMRECORDS 100
#define outfile "AmsOut"
main(){
int i;
struct AmsData data;
double time = 0.0;
int fd;

	if ((fd=open(outfile,O_CREAT|O_TRUNC|O_WRONLY, 0666))<0){
		perror("open");
		exit(2);
	}
	for (i=0; i < NUMRECORDS; i++){
		data.time = time;
		data.solDn = (double)(((int)i) % 10)+ 10;
		data.windU= 0.0;
		data.windV= 0.0;
		data.windW= 0.0;
		data.tempWet= 0.0;
		data.tempDry= 0.0;
		data.rain = (double)(((int)i)% 10) ;

		if (write(fd,(char *)&data,sizeof(data))!=sizeof(data)){
			perror("write");
			exit(2);
		}

		time += 30.0;
	}
	close(fd);
}

