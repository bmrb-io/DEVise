#ifdef RCSID
static char rcsid[] = "$Header$";
#endif

/* Copyright (c) 1991, 1990, 1984 - Gerard Salton, Chris Buckley. 

   Permission is granted for use of this file in unmodified form for
   research purposes. Please contact the SMART project to obtain 
   permission for other uses.
*/


int
INIT_EMPTY (unused1, unused2)
char *unused1;
char *unused2;
{
    return (0);
}


int 
EMPTY(unused1, unused2, inst)
char *unused1;
char *unused2;
int inst;
{ 
    return (0);
}


int 
CLOSE_EMPTY(inst)
int inst;
{ 
    return (0);
}

