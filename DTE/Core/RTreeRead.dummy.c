
int NextEmptyPage; // global variable used by genrtree.C
char *FileArr;     // global variable used by genrtree.C
int RTreeFile;	    // mapped file pointer

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize)
{
}

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize)
{
}

char *temp_file_name="bulk_temp";
int using_sort=1;
int num_of_clip_iter=1;

