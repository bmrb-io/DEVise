// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include <stdio.h>
#include <math.h>             // We need to get to it before perl does

#define  explicit explic      // A C++ keyword?
extern "C" {
#    include <EXTERN.h>
#    include <perl.h>
}
#undef   explicit

static PerlInterpreter *my_perl;

#include "PerlFrag.h"

int main(int ac, char **av, char **env)
{
    my_perl = perl_alloc();
    perl_construct(my_perl);

/* */
    perl_parse(my_perl,NULL,ac,av,env);
    perl_run(my_perl);
/* 

    char **mine = (char **) new char[2000];
    int n = matches("  23, 45 ", "([^,]+),([0-9]+)", &mine);

    printf("Got %d matches, 1st: %s   2nd one is %s", n, mine[0], mine[1]);
*/

    perl_destruct(my_perl);
    perl_free(my_perl);

    return(0);
}

