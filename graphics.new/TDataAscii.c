/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <libc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>
#include "Parse.h"
#include "TDataAscii.h"
#include "Exit.h"
#include "Util.h"
#include "Init.h"

static char fileContent[FILE_CONTENT_COMPARE_BYTES];
static char cachedFileContent[FILE_CONTENT_COMPARE_BYTES];

static char *MakeCacheName(char *file) {
    char *fname = StripPath(file);
	int nameLen = strlen(Init::WorkDir())+strlen(fname)+8;
	char *name = new char[nameLen];
	sprintf(name,"%s/%s.cache",Init::WorkDir(),fname);
	assert(strlen(name) < nameLen);
	return name;
}

TDataAscii::TDataAscii(char *name, int recSize): TData(){
    Dispatcher::Current()->Register(this);

	_name = name;
	/*
	_file = fopen(name,"a+");
	*/
	_file = fopen(name,"r");
	_recSize = recSize;
	if (_file ==NULL){
		fprintf(stderr,"TDataAscii: open file '%s' ", name);
		perror("");
		Exit::DoExit(1);
	}
    struct stat sbuf;
	if (fstat(fileno(_file),&sbuf) < 0){
		fprintf(stderr,"UnixRecFile:: can't find stat\n");
		Exit::DoExit(1);
	}

	_bytesFetched = 0;

	_totalRecs = 0;
	_recBuf = new char[recSize];
	_index = new long[INIT_INDEX_SIZE];
	_indexSize = INIT_INDEX_SIZE;
}

TDataAscii::~TDataAscii(){
/*
printf("TDataAscii destructor\n");
*/
	Dispatcher::Current()->Unregister(this);
	fclose(_file);
}

int TDataAscii::Dimensions(int *sizeDimension){
	sizeDimension[0] = _totalRecs;
	return 1;
}

Boolean TDataAscii::HeadID(RecId &recId){
	recId = 0;
	return (_totalRecs > 0);
}

Boolean TDataAscii::LastID(RecId &recId){
	/* check to see if there is more to read */
	if (fseek(_file,0, SEEK_END)< 0){
		perror("TDAtaAscii::LastID");
		Exit::DoExit(1);
	}
	long pos = ftell(_file);
	if (pos < 0){
		perror("TDataAscii::Init: ftel");
		Exit::DoExit(1);
	}

	if (pos > _lastPos){
		BuildIndex();
	}

	recId = _totalRecs-1;
	return (_totalRecs > 0);
}

char *TDataAscii::GetName(){
	return _name;
}

void TDataAscii::InitGetRecs(RecId lowId, RecId highId,RecordOrder order){
/*
printf("TDataAscii::INitGetRecs(%d,%d)\n",lowId, highId);
*/

	if (lowId < 0 || lowId >= _totalRecs ||	
		highId < 0 || highId >= _totalRecs || highId < lowId){
		fprintf(stderr,"TDataAscii::GetRecs: invalid recId %d %d, total %d\n",
			lowId, highId, _totalRecs);
		Exit::DoExit(1);
	}
	_lowId = lowId;
	_highId = highId;
	_nextId = lowId;
	_endId = highId;
}

Boolean TDataAscii::GetRecs(void *buf, int bufSize, 
		RecId &startRid,int &numRecs, int &dataSize, void **recPtrs){

/*
printf("TDataAscii::GetRecs buf = 0x%x\n", buf);
*/

	numRecs = bufSize/_recSize;
	if (numRecs == 0){
		fprintf(stderr,"TDataRec::GetRecs(): not enough buffer space \n");
		fprintf(stderr,"buffer size = %d, rec size= %d\n", bufSize, _recSize);
		Exit::DoExit(1);
	}
	if (_nextId > _endId)
		return false;

	int num= _endId- _nextId+1;
	if (num < numRecs)
		numRecs = num;

/*
printf("TDataAscii bufSize %d,  recSize %d, %d recs left, reading %d recs\n",
	bufSize, _recSize, _endId-_nextId+1, numRecs);
*/

	ReadRec(_nextId, numRecs, buf);

	startRid = _nextId;
	dataSize = numRecs*_recSize;
	_nextId += numRecs;

	_bytesFetched += dataSize;

	return true;
}

void TDataAscii::DoneGetRecs() {}

void TDataAscii::GetRecPointers(RecId startId, int numRecs,
	void *buf, void **recPtrs){

	fprintf(stderr,"TDataRec::GetRecPointers: not implemented\n");
		Exit::DoExit(1);
}

void TDataAscii::GetIndex(RecId id, int *&indices){
	static int index[1];
	index[0] = id;
	indices = index;
}


int TDataAscii::GetModTime(){
    struct stat sbuf;
	if (fstat(fileno(_file),&sbuf) < 0){
		fprintf(stderr,"UnixRecFile:: can't find stat\n");
		Exit::DoExit(1);
	}
	return (long)sbuf.st_mtime;
}

void TDataAscii::Initialize() {
	/* Read file contents into buffer */
	struct stat cacheSbuf;
	_cacheFileName = MakeCacheName(_name);
	Boolean fileOpened = false;
	int cacheFd;
	if ((cacheFd=open(_cacheFileName,O_RDONLY,0666)) <0)
		goto error;
	fileOpened = true;


	/* cache file exists. See if we are still working on the same
	file, and if we are, reinitialize */
	if (fseek(_file,0,SEEK_SET) < 0){
			goto error;
	}

	if (read(cacheFd,cachedFileContent, FILE_CONTENT_COMPARE_BYTES)
			!= FILE_CONTENT_COMPARE_BYTES  ||
		fread(fileContent, FILE_CONTENT_COMPARE_BYTES, 1,_file) != 1 ||
		bcmp(cachedFileContent, fileContent, FILE_CONTENT_COMPARE_BYTES) != 0)
		goto error;

	/* Assume identical file */
	/* Read last file position */
	if (lseek(cacheFd,LAST_POS_OFFSET,SEEK_SET) != LAST_POS_OFFSET)
		goto error;
	if (read(cacheFd, &_lastPos, sizeof(_lastPos))!= sizeof(_lastPos))
		goto error;

	/* Read number of records */
	if (lseek(cacheFd,TOTAL_RECS_OFFSET,SEEK_SET) != TOTAL_RECS_OFFSET)
		goto error;
	if (read(cacheFd,&_totalRecs, sizeof(_totalRecs))!= 
		sizeof(_totalRecs)){
		goto error;
	}
	if (_totalRecs >= _indexSize){
		delete _index;
		_index = new long[_totalRecs+INDEX_ALLOC_INCREMENT];
		_indexSize = _totalRecs+INDEX_ALLOC_INCREMENT;
	}
	/* read the index */
	if (read(cacheFd,_index, _totalRecs*sizeof(long))
			!= _totalRecs*sizeof(long)){
		goto error;
	}

	/* Let subclass read cache */
	if (lseek(cacheFd,SUBCLASS_OFFSET,SEEK_SET) != SUBCLASS_OFFSET)
		goto error;

	if (!ReadCache(cacheFd)) {
		/* can't read cache */
		goto error;
	}

	close(cacheFd);

	_initTotalRecs = _totalRecs;
	_initLastPos  = _lastPos;
	/* continue to build index */
	BuildIndex();
	return;
error:
	/* recover frome error by building index from scratch  */
	if (fileOpened)
		close(cacheFd);
	_initTotalRecs = _totalRecs = 0;
	_initLastPos = _lastPos = 0;
	BuildIndex();
	return;
}

/* Do a checkpoint */
void TDataAscii::Checkpoint(){
	printf("Checkpointing %s, %d existing, %d additional records\n", _name,
		_initTotalRecs, _totalRecs-_initTotalRecs);

	if (_lastPos == _initLastPos && _totalRecs == _initTotalRecs)
		/* no need to checkpoint */
		return;

	Boolean fileOpened = false;
	int cacheFd;
	if ((cacheFd=open(_cacheFileName,O_CREAT| O_RDWR,0666)) <0)
		goto error;
	fileOpened = true;

	if (fseek(_file,0,SEEK_SET) < 0){
		perror("TDataAscii::CheckPoint");
		goto error;
	}
	if (fread(fileContent,FILE_CONTENT_COMPARE_BYTES, 1,_file) != 1){
		goto error;
	}

	/* write contents of file to be compared later */
	if (write(cacheFd,fileContent,FILE_CONTENT_COMPARE_BYTES)!=
		FILE_CONTENT_COMPARE_BYTES)
		goto error;
	
	/* subclass should write its contents */
	if (lseek(cacheFd, SUBCLASS_OFFSET,SEEK_SET) != SUBCLASS_OFFSET){
		perror("TDataAScii::ChecPoint");
		goto error;
	}
	/* let subclass write */
	if (!WriteCache(cacheFd))
		goto error;

	/* write last position in the file */
	if (_lastPos > _initLastPos){
		if (lseek(cacheFd, LAST_POS_OFFSET,SEEK_SET) != LAST_POS_OFFSET){
			perror("TDataAScii::ChecPoint");
			goto error;
		}
		if (write(cacheFd, &_lastPos, sizeof(_lastPos))!= sizeof(_lastPos))
			goto error;
	}
	
	/* write # of records */
	if (_totalRecs > _initTotalRecs){
		if (lseek(cacheFd, TOTAL_RECS_OFFSET,SEEK_SET) != TOTAL_RECS_OFFSET){
			perror("TDataAScii::ChecPoint");
			goto error;
		}
		if (write(cacheFd, &_totalRecs, sizeof(_totalRecs))!=sizeof(_totalRecs))
			goto error;

		/* write indices */
		int indexOffset = INDEX_OFFSET + _initTotalRecs*sizeof(long);
		if (lseek(cacheFd,indexOffset, SEEK_SET) != indexOffset)
			goto error;
		int bytes = (_totalRecs-_initTotalRecs)*sizeof(long);
		if (write(cacheFd, &_index[_initTotalRecs], bytes) != bytes)
			goto error;
	}
	close(cacheFd);
	return;

error:
	if (fileOpened)
		close(cacheFd);
	(void)unlink(_cacheFileName);
}

/* Build index for the file. This code should work when file size
is extended dynamically. Before calling this function, position
should be at the last place where file was scanned. */
void TDataAscii::BuildIndex(){
char buf[LINESIZE];
	int oldTotal = _totalRecs;


	int numArgs;
	char **args;
	/* File has been appended, extend index */
	if (fseek(_file,_lastPos, SEEK_SET) < 0){
		perror("TDataAscii::BuildIndex");
		Exit::DoExit(1);
	}
	long pos = _lastPos;

	while(fgets(buf,LINESIZE, _file)!= NULL){
		/* get rid of '\n' */
		int len = strlen(buf);
		if (len > 0 && buf[len-1] == '\n'){
			buf[len-1] = '\0';
		}

		if (Decode(_totalRecs, _recBuf, buf)){
			/* got it */
			if (_totalRecs >= _indexSize)
				/* index buffer too small, extend it */
				ExtendIndex();

			_index[_totalRecs++] = pos;
		}
		pos = ftell(_file);
		if (pos < 0){
			perror("TDataAscii::Init: ftell");
			Exit::DoExit(1);
		}
	}
	_lastPos = ftell(_file);

	printf("Extend Index for %s, %d records already built, now has %d recs\n", 
		_name, oldTotal, _totalRecs);
	/*
	PrintIndices();
	*/
}

void TDataAscii::ReadRec(RecId id, int numRecs, void *buf){
char line[LINESIZE];
/*
printf("TDataAscii::ReadRec %d,%d,0x%x\n",id,numRecs,buf);
*/

	int i;
	char *ptr = (char *)buf;
	for (i=0; i < numRecs; i++){
		if (fseek(_file,_index[id+i],SEEK_SET) < 0){
			fprintf(stderr,"TDataTWeety::ReadRec(%d,%d,0x%x) seek\n",
				id,numRecs, buf);
			perror("");
			PrintIndices();
			Exit::DoExit(1);
		}
		if (fgets(line,LINESIZE, _file)== NULL){
			fprintf(stderr,"TDataTWeety::ReadRec(%d,%d,0x%x) fgets pos %d\n",
				id,numRecs, buf, _index[id+i]);
			perror("");
			/*
			PrintIndices();
			*/
			Exit::DoExit(1);
		}
		int len = strlen(line);
		if (len > 0 ){
			if (line[len-1] != '\n'){
				fprintf(stderr,"TDataTWeety::ReadRec: line too long\n");
				Exit::DoExit(1);
			}
			line[len-1] = '\0';
		}
		/*
		printf("%s", buf);
		*/
		(void)Decode(id+i, ptr, line);

		ptr += _recSize;
	}
}

int TDataAscii::RecSize() { return _recSize; }

int TDataAscii::PageSize() { return -1; }

Boolean TDataAscii::HasDeletion(){ return false; }

Boolean TDataAscii::HasAppend() { return true;}

void TDataAscii::ExtendIndex(){
	long *newIndex= new long[_indexSize+INDEX_ALLOC_INCREMENT];
	int i;
	for (i=0; i < _totalRecs; i++)
		newIndex[i] = _index[i];
	delete _index;
	_index = newIndex;
	_indexSize += INDEX_ALLOC_INCREMENT;

}

void TDataAscii::WriteRecs(RecId startRid, int numRecs, void *buf){
	if (_totalRecs >= _indexSize)
		/* index buffer too small, extend it */
		ExtendIndex();
	if (fseek(_file,_lastPos, SEEK_SET) < 0){
		perror("TDataAscii::WriteRecs");
		Exit::DoExit(1);
	}
	_index[_totalRecs++] = _lastPos;
	if (fwrite(buf,strlen((char *)buf),1,_file) != 1){
		perror("TDataAScii::WriteRecs");
		Exit::DoExit(1);
	}
	_lastPos = ftell(_file);
}

void TDataAscii::WriteLine(void *line){
	if (fseek(_file,_lastPos, SEEK_SET) < 0){
		perror("TDataAscii::WriteLine");
		Exit::DoExit(1);
	}
	if (fwrite(line,strlen((char *)line),1,_file) != 1){
		perror("TDataAScii::WriteRecs");
		Exit::DoExit(1);
	}
	_lastPos = ftell(_file);
}

void TDataAscii::Cleanup(){
	Checkpoint();
}

void TDataAscii::PrintIndices(){
	int i;
	int cnt = 0;
	for (i=0; i < _totalRecs; i++){
		printf("%d ", _index[i]);
		if (cnt++ == 10){
			printf("\n");
			cnt=0;
		}
	}
	printf("\n");
}
