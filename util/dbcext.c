/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char line[255];

void parseJob(FILE *in, int job)
{
  char machine[64];
  char pool[64];
  int pool_id;
  char condor_id[16];
  int restarts;
  int run_time;
  int ftp_get_time;
  int ftp_put_time;
  int exec_time;
  int process_id;
  int exit_status;
  int start_time;
  int stop_time;
  int last_update;

  if (fscanf(in, " statistics { machine %s arch %*s opsys %*s Pool %s",
	     machine, pool) != 2)
    return;
  if (fscanf(in, " condor_id %s restarts %d run_time %d",
	     condor_id, &restarts, &run_time) != 3)
    return;
  if (fscanf(in, " ftp_get_time %d ftp_put_time %d exec_time %d",
	     &ftp_get_time, &ftp_put_time, &exec_time) != 3)
    return;
  if (fscanf(in, " process_id %d exit_status %d start_time %d",
	     &process_id, &exit_status, &start_time) != 3)
    return;
  if (fscanf(in, " stop_time %d last_update %d } }",
	     &stop_time, &last_update) != 2)
    return;

  if (strstr(pool, "poles") || strstr(pool, "wash"))
    pool_id = 2;
  else if (strstr(pool, "bashan"))
    pool_id = 3;
  else if (strstr(pool, "nasa"))
    pool_id = 4;
  else {
    fprintf(stderr, "Unrecognized pool %s\n", pool);
    pool_id = 0;
  }

  printf("%d,%s,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	 job, machine, pool_id, condor_id, restarts, run_time,
	 ftp_get_time, ftp_put_time, exec_time, process_id,
	 exit_status, start_time, stop_time, last_update);
}

void parseLog(FILE *in)
{
  while(fgets(line, sizeof line, in)) {
    int job;
    if (sscanf(line, "job %d { status Done", &job) == 1) {
      parseJob(in, job);
    }
  }
}

int main(int argc, char **argv)
{
  printf("# job,machine,pool_id,condor_id,restarts,run_time,ftp_get_time,\n");
  printf("# ftp_put_time,exec_time,process_id,exit_status,\n");
  printf("# start_time,stop_time,last_update\n\n");

  parseLog(stdin);

  return 0;
}
