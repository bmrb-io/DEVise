// Execute.c

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

// Make sure some condition holds.
#define CHECK_OUT(condition) if (condition) exit (1);

// Make sure system call returns successfully.
#define CHECK_SYSCALL(syscall) CHECK_OUT (syscall < 0)

// Begin a block if some command argument is not empty.
#define BEGIN_OP_ARG(i) if (argv [i][0] != 0) {

// End a block if some command argument is not empty.
#define END_OP_ARG }
 
int main (int argc, char ** argv)
{
  // Make sure there are 4 arguments.
  CHECK_OUT (argc != 5)

  // Redirect stdin.
  BEGIN_OP_ARG (2)
  CHECK_SYSCALL (close (0))
  CHECK_OUT (0 != open (argv [2], O_RDONLY))
  END_OP_ARG
  
  // Redirect stdout.
  BEGIN_OP_ARG (3)
  CHECK_SYSCALL (close (1))
  CHECK_OUT (1 != open (argv [3], O_WRONLY | O_CREAT | O_TRUNC))
  END_OP_ARG

  // Redirect stderr.
  BEGIN_OP_ARG (4)
  CHECK_SYSCALL (close (2))
  CHECK_OUT (2 != open (argv [4], O_WRONLY | O_CREAT | O_TRUNC))
  END_OP_ARG

  // Run the main command.
  CHECK_SYSCALL (system (argv [1]))

  // Set the permission for redirected stdout and stderr.
  BEGIN_OP_ARG (3)
  CHECK_SYSCALL (fchmod (1, 0777))
  END_OP_ARG

  BEGIN_OP_ARG (4)
  CHECK_SYSCALL (fchmod (2, 0777))
  END_OP_ARG

  // Close files.
  BEGIN_OP_ARG (2)
  CHECK_SYSCALL (close (0))
  END_OP_ARG

  BEGIN_OP_ARG (3)
  CHECK_SYSCALL (close (1))
  END_OP_ARG

  BEGIN_OP_ARG (4)
  CHECK_SYSCALL (close (2))
  END_OP_ARG

  return 0;
}


