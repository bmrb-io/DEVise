/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of Session class (for opening and saving sessions).
 */

/*
  $Id$

  $Log$
 */

#ifndef _Session_h_
#define _Session_h_


#include <sys/types.h>

#include "DevStatus.h"

class Session {
public:
  static DevStatus Open(char *filename);
  static DevStatus Save(char *filename, Boolean asTemplate, Boolean asExport,
      Boolean withData);

private:
  static int DEViseCmd(ClientData clientData, Tcl_Interp *interp,
      int argc, char *argv[]);

  static DevStatus SaveSchemas(FILE *fp);
  static DevStatus SaveTDatas(FILE *fp);

  static DevStatus SaveCategory(FILE *fp, char *category);
  static DevStatus SaveClass(FILE *fp, char *category, char *devClass);
  static DevStatus SaveInstance(FILE *fp, char *category, char *devClass,
      char *instance);

  typedef DevStatus (*InstanceFuncP)(char *category, char *devClass,
      char *instance, void *clientData);
  static DevStatus ForEachInstance(char *category, InstanceFuncP function,
      void *clientData);

  static DevStatus SaveView(char *category, char *devClass, char *instance,
      void *clientData);
  static DevStatus SaveInterpMapping(char *category, char *devClass,
      char *instance, void *clientData);
  static DevStatus SaveGData(char *category, char *devClass, char *instance,
      void *clientData);
  static DevStatus SaveWindowLayout(char *category, char *devClass,
      char *instance, void *clientData);
  static DevStatus SaveViewAxisLabels(char *category, char *devClass,
      char *instance, void *clientData);
  static DevStatus SaveViewLinks(char *category, char *devClass,
      char *instance, void *clientData);
  static DevStatus SaveViewMappings(char *category, char *devClass,
      char *instance, void *clientData);

  static DevStatus SaveParams(FILE *fp, char *getCommand, char *setCommand,
      char *arg0, char *arg1 = NULL, char *arg2 = NULL);

  static void PrintArgs(FILE *fp, int argc, char **argv);
};

#endif /* _Session_h_ */

/*============================================================================*/
