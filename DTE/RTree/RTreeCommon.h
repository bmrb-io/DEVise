#ifndef RTREE_COMMON_H
#define RTREE_COMMON_H

const int VolumeSize = 1*1024*1024;
static const char* VolumeName = "RTREE";
extern int RTreeFile;

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize);

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize);

#endif
