// Environment.c

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

#include <stdlib.h>
#include "Environment.h"

// Return the system environment variable value.
JNIEXPORT jstring JNICALL Java_Environment_getSystemEnv (JNIEnv * env, jclass, jstring js)
{
  char * ret;

  if ((ret = getenv (env -> GetStringUTFChars (js, 0))) == NULL)
    ret = "";
  
  return env -> NewStringUTF (ret);
}



