/*
  $Id$

  $Log$*/

#ifndef FileMacro_h
#define FileMacro_h

inline void ReadChunk(int fd, char *buf, int toRead){
	while (toRead > 0){
		int len;
		len = read(fd,buf, toRead);
		if (len > 0){
			toRead -= len;
			buf +=  len;
		}
		else if (len < 0){
			if (errno != EINTR){
				perror("ReacChunk");
				Exit::DoExit(2);
			}
		}
		else{
			/* len == 0: trying to read beyond EOF */
			fprintf(stderr,"ReadChunk: read beyond EOF\n");
			Exit::DoExit(2);
		}
	}
}

/* Read chunk returning false if EOF reached (nothing read)*/
inline int ReadChunkStatus(int fd, char *buf, int toRead){
	while (toRead > 0){
		int len;
		len = read(fd,buf, toRead);
		if (len > 0){
			toRead -= len;
			buf +=  len;
		}
		else if (len < 0){
			if (errno != EINTR){
				perror("ReacChunk");
				Exit::DoExit(2);
			}
		}
		else{
			/* len == 0: trying to read beyond EOF */
			return 0;
		}
	}
	return 1;
}

inline void WriteChunk(int fd, char *buf, int toWrite){

	while (toWrite > 0){
		int len;
		len = write(fd,buf,toWrite);
		if (len >= 0){
			toWrite -= len;
			buf += len;
		}
		else {
			if (errno != EINTR){
				perror("WriteChunk:");
				Exit::DoExit(2);
			}
		}
	}
}

#endif
