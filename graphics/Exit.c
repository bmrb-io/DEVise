/*
  $Id$

  $Log$*/

#include "Exit.h"
#include <stdio.h>
#include <libc.h>
#include "Init.h"

void Exit::DoExit(int arg){
	if (Init::DoAbort()){
		fflush(stdout);
		fflush(stderr);
		abort();
	}
	else exit(2);
}
