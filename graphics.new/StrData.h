/*
  $Id$

  $Log$*/

#ifndef StrData_h
#define StrData_h

const int StrDataNameSize = 20;
struct StrData {
	char name[StrDataNameSize];
	double user;
	double cpu;
	double io;
};

#endif
