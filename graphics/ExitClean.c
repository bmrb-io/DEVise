/*
  $Id$

  $Log$*/

#include "Exit.h"
#include <stdio.h>
#include <libc.h>

void Exit::DoExit(int arg){
	abort();
	/*
	exit(2);
	*/
}
