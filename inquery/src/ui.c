
/**************************************************************************/
/*                                                                        */
/*                    Copyright (c) 1990 - 1995 by the                    */
/*  Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM).  */
/*            Do not distribute without written permission.               */
/*                          All Rights Reserved.                          */
/*                                                                        */
/*                 INQUERY Software was provided by the                   */
/*           Center for Intelligent Information Retrieval (CIIR)          */
/*         University of Massachusetts Computer Science Department        */
/*                     Amherst, Massachusetts  01003                      */
/*                                                                        */
/*           Contact ACSIOM at (413)545-6311 for more information         */
/*                                                                        */
/**************************************************************************/

/* module name: ui.c     user interface routines for inquery        */

/* ------------------------------- Inclusions ------------------------------ */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <memory.h>		/* for memset and memcpy */

#include "inerror.h"
#include "docmem.h"
#include "inq_api.h"
#include "ui_utils.h"
#include "summary.h"
#include "ui.h"

/*
#if defined(NETWORK)
#include "inq_feedback.h"
#endif
*/

/* -------------------- Macros/Global Constants ---------------------------- */

#define DEBUG
/* #define STDOUT (1) */

#ifdef TRUE
  #undef TRUE
  #define TRUE (1)
#endif

#ifdef FALSE
  #undef FALSE
  #define FALSE (0)
#endif

#define UP (0)
#define DOWN (1)
#define INHELP (2)
#define COL80 (2)		/* to hardwire width to 80 characters */
#define MAXPAGE (100)		/* max lines per page on a screen */
#define ESCSIZE (4)		/* size of each escape character in a line */
#define ESCCHAR '\x1b'		/* escape character sequence */

#define STR_REV_ON       "\033[7m"
#define STR_REV_OFF      "\033[0m"
#define REV_VIDEO_ON     printf("\033[7m")
#define REV_VIDEO_OFF    printf("\033[0m")

#define BROWSE_MENU1 "page Forward/Back  line Up/Down  doc Next/Prev  Top/End  Write  #  Query  ?  >"
#define BROWSE_MENU2 "<  nextKeyword/Allkeywords  Change settings  Summary  ?  Refresh"
#define HELP_MENU "page Forward/Back  line Up/Down  Top/End  Refresh  eXit help"
#define DATABASE_PROMPT "\nEnter the number of the database to use (<return> \
for current; \nor an entire path and database name): \n"
#define MAXDOC_PROMPT "\nWhat is the maximum number of documents you want \
retrieved for a single query?\nEnter a number %s: "
#define QUERYPROMPT "Enter a query (end with a blank line) or '?' for help: "

/* ---------------------- Structured Declarations ---------------------------*/

/* For screen flow in ui() 
 * Note that ui_states are negative.  This is because browse(), which returns
 * a ui_state, returns a positive integer instead of browse_doc, which is
 * used as the rank of the next doc to browse. 
 */
typedef enum {get_query = -11, browse_summary, browse_help, new_settings, \
	      browse_doc, modify_query, modify_parsed_query, parsed_query, quit_prog, \
	      go_to_last, show_ndx_table} ui_state;

typedef enum {allkeys = -27, back, change, down, end, fwd, help, \
	      nextkey, mod_qry, mod_prs_qry, next, prev, prs_qry, query,\
	      refresh, summary, top, up, writef, bye, left, right, none, \
	      rel, nonrel, ndx, eval_query_feedback} cmd;

typedef enum {doc, sum, hlp} disp_type;
typedef enum {no, yes} response_t;

typedef struct line 		/* structure used to hold the documents */
{          			/* and summaries for ease in browsing. */
  Char_t   *text;
  Int_t    num;			/* document line numbering */
  Int_t    keynum;		/* keyword line numbering */
  struct line *next; 
  struct line *prev; 
} LINE;

typedef struct partdoc
{
  LINE *head;			/* beginning of formatted document */
  LINE *tail;			/* end of formatted portion of document */
  Int_t  lines;			/* number of formatted lines */

  LINE *keyhead;		/* first formatted line with a keyword */
  LINE *keytail;		/* last formatted line with a keyword */
  Int_t  keylines;		/* number of keyword lines */

  Char_t *title;
  Int_t   id;
  Int_t   rank;
  Char_t *rest;			/* remaining unformatted part of document */
  Int_t  doc_offset;            /* offset of unformatted part in document */
  Int_t  num_matches;           /* num query term matches for highlighting */
  matchinfo_t *matches;         /* match info for highlighting.            */
} PARTDOC;

typedef struct basic {
  LINE *head;
  LINE *tail;
} BASIC;

/* ------------------------- Function Declarations --------------------------*/

void   banner();	       		/* set_up procedures */
void   init_ui(void);
Int_t  settings(void);
void   exit_ui(void);
				/* Main interface procedure and help*/
Char_t *addtohelp(Char_t *temp);
Int_t   set_help(void);
void    ui(void);
				/* structure/list manipulation procedures */
void     release_text(disp_type menu);
PARTDOC *gen_new_doc(void);
PARTDOC *set_up_text(PARTDOC *buff, Int_t indent, Int_t dokeys, Int_t width,
		     Int_t check_left); 
LINE    *gen_new(void);
				/* display procedures */
cmd       get_comm(void);
Int_t     checknum(Char_t *str);
ui_state  disp_help(void);
ui_state  disp_doc(void);
ui_state  disp_summ(SUMMARY *cursum);
BASIC    *draw_menu(Char_t *topline, BASIC *cur, Char_t *options, 
		    disp_type menu); 
BASIC    *move(BASIC *start, Int_t lines, Int_t dir);
BASIC    *next_doc_key(BASIC *cur, Int_t holddoc);
LINE     *show_lines(Int_t lines, BASIC *start, disp_type menu);
				/* ioctl screen size functions */
int      performance(void *info);

				/* text manipluation procedures */
Char_t   *unhighlight(Char_t *str);
Int_t     highlight_query_words(void);
				/* file procedure definitions */
static BoolInt_t mark_docs (SUMMARY *cursum, Int_t new, disp_type menu);
static BoolInt_t open_out_file(void);
static BoolInt_t store_docs_in_file (Char_t *qfirst, disp_type menu);
				/* query functions */
Char_t  *read_query(Char_t *prompt);
ui_state run_query(Char_t *newq, Int_t *qdone);
				/* document section retrieval */
Int_t   new_doc(Int_t newrank);

static Char_t *tableStr;

/* library functions that do not have prototypes in any system header:*/
#if (!defined INQ_ARCH_rs6000 && !defined (__alpha) && (!defined INQ_ARCH_solaris) && (!defined INQ_ARCH_sgi) && (!defined INQ_ARCH_dgux))
int  getopt(int argc, char **argv, char *);
#endif

/* The PC is very sensitive to argc and argv being ints and chars, so 
 * keep it that way.
 */
/*Int_t  getopt(int argc, char *argv[], Char_t *); */

void error_msg(Char_t *str);

/*
void ioctl();

extern int isatty();     This has a prototype in unistd.h, but also contains
                         write() which conflicts with enumerated type, so 
                         define our own prototype here 
*/

extern int tty_reset();
extern int tty_cbreak();

/*
 * These functions have been moved to DOS_SPECIFIC.C or UNIX_SPECIFIC.C as
 * required by the type of ui being built.
 */
 extern Int_t screen_width(void);
 extern Int_t screen_length(void);

/* ---------------------- Global Variable Definitions -----------------------*/

/* The following line is used by RCS to tell about the differences between
   versions in the system. */
static Char_t rcsid[]="$RCSfile$, $Revision$, $Date$";


Int_t maxdocs = -1;	/* does not change during runtime after it
				   gets a value.  Therefore global. Requested
				   number of documents to be summarized. */

extern int optind, opterr;	/* these are required by getopt() */
extern char *optarg;

Char_t msg_str[MAXLEN];		/* error message for user, displayed by menu
				   then erased. */


static Char_t    params[MAXLEN];
static PARTDOC   *helpmenu = (PARTDOC *)NULL;
static SUMMARY   *headsum = (SUMMARY *)NULL;
static PARTDOC   *front = (PARTDOC *)NULL;


/* -------------------------- Function Definitions --------------------------*/
/*void main(Int_t argc, Char_t **argv) */
void main(int argc, char *argv[]) 
{
  banner();
  /* db = process_commandline ((Int_t)argc, (Char_t **)argv); */
  db = process_commandline ((Int_t)argc, argv);
  if(!db)
    {
     exit(EXIT_FAILURE);
     }
  
  ui();
  inq_closedb(db);

  inq_term_inquery(db);

  inq_free_dbinfo(&db);
  
  inq_free_belief_list(bl);
  exit(EXIT_SUCCESS);
}

/* ============================ Set-up Procedures ========================== */

void banner()
{
  CLEAR;
  fflush(stdout);
  Q_inform("\n		      Welcome to INQUERY v3.1\n\n\
		 Copyright 1993, 1994, 1995, 1996 by the\n\
		  Information Retrieval Laboratory,\n\
		    Department of Computer Science\n\
		     University of Massachusetts,\n\
			Amherst MA. 01003, USA\n\
			 All Rights Reserved.\n\
	    Do not distribute without written permission.\n\n\n");
}

/* 
 * Do initialization for the user interface. 
 */
void init_ui()
{
  int i;
  settings();

  if (*logname) 
    {
    logfile = fopen((char *)logname, "w");
    if (logfile == (FILE *)NULL) 
      {
      printf("***Couldn't open %s ***\n", (char *) logname);
      exit(1);
      }  
    }

  front = gen_new_doc();
  helpmenu = gen_new_doc();
  memset((void *)rel_docs, (int)'\0', 
         (size_t)(MAX_DOCS_JUDGED*(sizeof(doc_rel_t))));
  rel_docs_size = 0;
}

/*
 * Check to see if the given string is a positive number.  Return either the 
 * number it is or a -1 if it is not a positive number.
 */
Int_t checknum(Char_t *str)
{
  Char_t *p;			/* pointer through string */

  if (*str=='\0')
    return(-1);
  for (p = &str[0]; p && isspace(*p); p++);
  for (; p && isdigit(*p); p++);
  if (*p == '\0')
    return(atoi((char *)str));
  else
    return(-1);
}

/* GET_YESNO takes a prompt and a default answer.  The default is placed in */
/* brackets and added to the prompt.  The answer returned is the default, */
/* unless an explicit yes or no was entered. */
response_t get_yesno(char *prompt, response_t yn_default)
{
  char str[1];

  printf("%s (%s/%s)? ", prompt, (yn_default==yes)?"[y]":"y",
	 (yn_default==yes)?"n":"[n]");
  gets(str);
  if (!*str)
    return(yn_default);
  if ((*str == 'Y') || (*str == 'y'))
    return(yes);
  if ((*str == 'N') || (*str == 'n'))
    return(no);
  return(yn_default);
}
/* 
 * Get preferences ("settings") from user, e.g., # of docs to retrieve for
 * each query.  Added the possibility of changing databases to this routine.
 */
Int_t settings()
{
  Char_t buff[MAXLEN];		/* buffer to hold user input */
  Int_t  n, i;			/* number input by user */
  Int_t  changed = FALSE;
  Char_t **cur_dbs, **temp;
  Char_t *path = g_getenv((Char_t *)"INQDATA");
  Int_t status = TRUE;
  
  if ((!db->name[0]) || (maxdocs > 0))
    {
    memset(buff, '\0', MAXLEN);
    cur_dbs = inq_get_database_list(db, path);

    /* if no databases are found, then ask for input. */
    if (!cur_dbs)
      {
      printf("No listing of available databases on path:\n%s.\n", 
	     path ? (char *)path : "./");
      printf("Enter a complete path and database name or <return> for");
      printf(" current database:\n");
      gets((char *)buff);
      if ((*buff) && (check_db(buff) == TRUE) &&
	  ( (status = change_dbs(buff)) != FALSE))
	 changed = TRUE;
    } 

    /* If using MSDOS, don't execute the following statements since it makes
     * use of unix specific directory functions.  The MSDOS executable is a
     * networking program, and can no as yet get listings of available data
     * bases.
     */
    #if !defined(_MSDOS) /*  && !defined(NETWORK) */
    else {

      printf("\nAvailable databases:\n");

      /* otherwise list available databases and choose from those. */
      for (i=0; cur_dbs && cur_dbs[i]; i++)
	printf("%2ld.) %-s\n", i+1, (char *) cur_dbs[i]); 
    
      printf(DATABASE_PROMPT);
      gets((char *)buff);

      /* if they enter a number given, then check that database. */
      n = checknum(buff);
      if ((n > 0) && (n <= i))
	{
	n--;			/* keeps the 0 <= i < n */
	for (i=0, temp=cur_dbs;*temp && (i<n);temp++, i++);
	if ( (status = change_dbs(*temp))!= FALSE)
	    changed = TRUE;
        }
      
      /* if a non-number was entered pretend it is a path and use it. */
      else if (n == -1) {
	if ((*buff) && (check_db(buff) == TRUE) && 
	    ((status = change_dbs(buff)) != FALSE))
	  changed = TRUE;
        }
      if (!db->name[0] || !status) {	/* nothing is open */
/*	error_msg((Char_t *)"No valid database given.  Exiting.");
	exit(-1);
*/
	db->name[0] = '\0';
	settings();
	return(0);
      }
    }
    #endif   /* _MSDOS */

    inq_free_database_list(cur_dbs);
    }

  /* change number of documents to view */
  printf(MAXDOC_PROMPT, (maxdocs > 0) ? "(<return> for same)" : "");
  while (1)
    {
    memset((void *)buff, (int)'\0', (size_t)MAXLEN);
    gets((char *)buff);
      
    /* make sure a number is entered */
    n = checknum(buff);

    /* if maxdocs is already assigned, leave it as is if no number was */
    /* entered; otherwise, maxdocs must be assigned still so insure a number */
    /* is entered. */
    if ((n < 0) && (maxdocs > 0))
      {
      printf("\nMaximum number of documents still at: %d\n", (int) maxdocs);
      break;
    } else if (n > 0) {
      maxdocs = n;
      break;
      }
    printf("\nPlease enter a number\n: ");
    }
  return(changed);
}    

/*
 * Close the database file and clear the screen for quitting.
 */
void exit_ui(void)
{
    int i;
    
    if (logfile)
	fclose(logfile);
    if (doc_out_file_ptr)
	fclose(doc_out_file_ptr);
    release_text(doc);
    release_summary(&headsum);
    release_text(hlp);
    vector_free((void **)&front);
    vector_free((void **)&helpmenu);
    CLEAR;
}

/* used in case the format for displaying error messages ever changes. */
void error_msg(Char_t *str)
{
  printf("%s\n", (char *) str);
}

/* QUERY_FREE takes a pointer to a vector_make_room query pointer, frees the */
/* associated memory and sets the pointer to NULL. */
Boolean_t query_free(Char_t **query)
{
  if (!*query)
    return(TRUE);

  vector_free((void **)query);
  (*query) = (Char_t *)NULL;
  return(TRUE);
}

/* QUERY_COPY returns a copy of "query" in "copy".  If "copy" exists, it */
/* first frees the memory and makes it null, then creates new room for it */
/* and fills it in. */
Boolean_t query_copy(Char_t *query, Char_t **copy)
{
  Int_t len=0;

  if (!query)
    return(FALSE);

  query_free(copy);
  len = strlen((char *)query);

  if (!(vector_make_room_for_large_n(copy, len+1, sizeof(Char_t))))
    Q_error("Memory allocation error (file %s, line %d)", __FILE__, __LINE__); 
  memset(*copy, '\0', (size_t)(len+1));
  strcpy((char *)*copy, (char *)query);
  return(TRUE);
}

/* 
 * User interface top level.  This function handles screen flow.  The 
 * user interface block diagram has 4 nodes, corresponding to the 3 ui_states
 * and one more.  They are query, browse_summary, browse_doc_n, and exit.  We
 * always proceed to browse_summary after doing a query.  The function browse
 * returns the next state to enter, or the number of the next doc to browse.
 * Doc numbers are distinguished from other ui_states by being negative.
 */
void ui()
{
  Int_t iterations = 0;
  ui_state last_s = get_query;	/* last state visited for returning */
  Char_t *newq=(Char_t *)NULL;		/* temporarily holds the query results */
  Int_t  qdone = FALSE;		/* was a good query previously performed? */
  Char_t *qfirst = (Char_t *)NULL;	/* hold first 2 lines for header line */
  ui_state s;			/* where to go next */
  static Char_t *goodq=(Char_t *)NULL;	/* to rerun the last query again */
  Char_t *qlstr;
  belieflst *old_bl;
  
  init_ui();			
				/* start by asking for a query */
  for (s = get_query; TRUE; ) 
    { 
    switch(s) 
      {
      case new_settings:
	if (settings() == FALSE)
	  {
	  printf("\nRecreating the summary...\n");
	  if (headsum)
	    release_summary(&headsum);
	  if (maxdocs <= bl->doc_cnt)
	    {
	    headsum = set_summary(db, newq, bl, headsum);
	    s = browse_summary;
	    if (s != go_to_last)
	      last_s = new_settings;
	  } else {
	    query_copy(goodq, &newq);

	    s = run_query(newq, &qdone);
	    if (s == browse_summary)
	      qdone = TRUE;
	    if (s != go_to_last)
	      last_s = new_settings;
	    }
	} else {
	  s = get_query;
	  last_s = new_settings;
	  }
	break;

      case get_query:
	/* Start with a blank screen... */
	if (last_s && (last_s != new_settings) && !*msg_str)
	  CLEAR;

	/* If a message is given, then display the message. */
	if (msg_str[0]) 
	  {
	  BEEP;
	  printf("%s\n", (char *) msg_str);
	  memset((void *)msg_str, (int)'\0', (size_t)strlen((char *)msg_str));
	  }

	/* free the last query backup, then read in the new query. */
	query_free(&newq);
	newq = read_query((Char_t *)QUERYPROMPT);

	/* If nothing was returned go back where we came from... */
	if (!newq)
	  s = go_to_last;
	else if (!strcmp((char *)newq, (char *)"quit")) {
	  if (get_yesno("Exit INQUERY", no) == yes)
	    s = quit_prog;
	  else s = go_to_last;
	  vector_free((void **)&newq);
	} else {
	  /* Make a backup copy for possible use later. */
	  query_copy(newq, &goodq);

	  /* reset relevant docs to empty */
	  iterations = 0;
	  memset(rel_docs,'\0',
		 MAX_DOCS_JUDGED*(sizeof(doc_rel_t)));
	  rel_docs_size = 0;

	  /* Used for the title on the summary screens... */
	  query_copy(newq, &qfirst);

	  s = run_query(newq, &qdone);
	  if (s == browse_summary)
	    qdone = TRUE;
	  if (s != go_to_last)
	    last_s = get_query;
	  }
	break;

      case browse_help:
	if (!helpmenu->head)
	  set_help();
	s = disp_help();
	break;

      case browse_summary:
	if (bl->doc_cnt > 0 ) 
	  {
	  if(!headsum)
	    {
	    printf("Summarizing...\n");
	    headsum = set_summary(db, qfirst, bl, headsum);
	    }
	  s = disp_summ(headsum);
	  if (s != go_to_last)
	    last_s = browse_summary;
	  } 
	else {
/*	  printf("\nNo documents retrieved for last query.\n"); */
           if (s == last_s)
           s = get_query;
          else
	   s = last_s;
	  }
	break;

      case browse_doc:
	s = disp_doc();
	if (s != go_to_last)
	  last_s = browse_doc;
	break;

      case modify_query:
	old_bl = bl;
	rel_docs[rel_docs_size].id = INQ_END_RF_LIST;
	query_copy(goodq, &newq);

	bl = (belieflst *)inq_re_eval_query_top_n(db, newq, iterations++, rel_docs, performance,maxdocs);

	if(!bl) /* It failed */
	  {
	  Q_warning("inq_re_eval_query failed for some reason");
	  qlstr=(Char_t *)malloc(80);
	  fprintf(stdout,"\npress return to continue: ");
	  gets((char *)qlstr);
	  free(qlstr);
	  s = browse_summary;
	  bl = old_bl;
	  break;
	  }
	else
	   inq_free_belief_list(old_bl);
	
 	CLEAR;
	if (switch_remove_duplicate_docs)
	  {
	  printf("Removing duplicates...\n");
	  remove_duplicate_docs (maxdocs);
	  }

	/* reset the header of the query. */
	query_copy(goodq, &qfirst);

	release_summary(&headsum);
	s = browse_summary;
	break;

      case modify_parsed_query:
        s=last_s;
	rel_docs[rel_docs_size].id = INQ_END_RF_LIST;
	query_copy(goodq, &newq);
	qlstr = inq_get_modified_query(db, newq, iterations++, rel_docs);
	if (qlstr) 
	{
	   CLEAR;
	   fprintf(stdout,"Modified query:\n  %s\n", (char *) qlstr);
	   inq_free_parsed_query(qlstr);
	}	   

	qlstr=(Char_t *)malloc(80);
	fprintf(stdout,"\npress return to continue: ");
        gets((char *)qlstr);
	free(qlstr);
        break;

      case parsed_query:
        s=last_s;
	qlstr=inq_get_parsed_query(db,goodq);
	if (qlstr) 
	{
	   CLEAR;
	   fprintf(stdout,"\n  %s\nevaluates to:\n  %s\n", (char *) goodq,
		   (char *) qlstr);
	   inq_free_parsed_query(qlstr);
	}
	else 
	   fprintf(stdout,"\n\ninq_get_parsed_query encountered a problem "
		   "with the Query:\n  %s\n", (char *) goodq);
	
	qlstr=(Char_t *)malloc(80);
	fprintf(stdout,"\npress return to continue: ");
        gets((char *)qlstr);
	free(qlstr);
        break;

      case go_to_last:
	s = last_s;
	break;

      case quit_prog:
	/* free local variables declared with */
	/* vector_make_room_for_large_n() before quitting the procedure. */
	query_free(&qfirst);
	query_free(&goodq);
	query_free(&newq);
	exit_ui();
	return;

      case show_ndx_table:
        /* show the current field index table - this is a quick hack */
        CLEAR;
        inq_get_index_table (db, &tableStr);
        printf ("%s", (char *) tableStr);
        printf ("Hit Return to Continue");
	inq_free_index_table(tableStr);
        get_comm ();
        s = go_to_last;
        break;        

      default:
	break;
      }	    
  }  
}

/* This crashes hard but I've just NULLed out the calls to it for now */

int performance(void *info)
{
  static UInt_t  max = 0;
  static Float_t unit = (Float_t)0.0;
  Int_t i;
  Int_t show=0, done=0;		/* # of units to show, % finished */
  Int_t oldmax = 0;
  Int_t current;

  if (info == (Int_t *)NULL)
    return(0);
  else
    current = inq_get_feedback_doc_cnt(info);

 
  if (current == -1)
  {
      printf("\nSorting...\n");
      max = 0;
      return(0);
  }

  if ((max == 0) && (current!=0))
  { /* starting new meter reading */
    printf("\n");
    max = current;
    unit = (Float_t)(25.0/(Float_t)max);
  } 
  else if (current == 0) {
    show = 25;
    done = 100;
    oldmax = max;
    max = current;
  }
  else {
    show = (Int_t)(unit*(Float_t)(max-current));
    done = (Int_t)(((Float_t)(max-current)/(Float_t)max)*100.0);
  }

  printf("     ");		/* leading blanks */
  for(i=0;i<show;i++)		/* fill done in with o's */
    printf("o");
  for(i=0;i<(25-show);i++)	/* fill remainder in with blanks */
    printf(".");
				/* print staying on same line */
  printf(" %3i%c / %li%c", (int) done, '%', (max) ? max : oldmax, 
	 (current) ? 13 : '\n'); 
  fflush (stdout);
  return(0);
}

ui_state run_query(Char_t *newq, Int_t *qdone)
{
  static count = 0;
  Int_t newqlen;		/* length of new query; reduce strlen calls */
  int i;

  if (newq == (Char_t *)NULL || *newq == '\0') {  /* Did they enter anything? */
    if ((*qdone) == TRUE)	/* is there something from before? */
      return(browse_summary);
    else {
      if (!count) {
	sprintf((char *)msg_str, 
		"\nNo query entered.\nPress return once more to exit.\n");
	count++;
	return(get_query);
      }
      else 
	return(quit_prog);
    }
  }		
  count = 0;
				/* Do they want help? */
  if (*newq == '?') 
    return(browse_help);

  printf("Processing query...\n");
  *qdone = FALSE;

  if (inq_get_spell_check_flag(db)) {
    Char_t aline[80];
    Int_t num;
    query_info_t *result;
    Char_t *saveq;
    Int_t i,reenter_flag=false;

    saveq = (Char_t *) malloc (1+strlen((char *)newq));
    strcpy((char *)saveq,(char *)newq);
    result = inq_check_query(db, newq);
    if (!result) Q_error("Error in inq_check_query");
    switch(result->error_code) {
    case NO_ERROR:
      if (result->num_not_found > 0) {

	if (result->xformed_queries) {
	  printf("\nHere's what the transformers did to your query:\n");
	  for(i=0;result->xformed_queries[i].name != NULL;i++) 
	    printf("  The \"%s\" transformer returned:\n    \"%s\"\n",
		   (char *) result->xformed_queries[i].name,
		   (char *) result->xformed_queries[i].q_str);
	}
	if (result->parsed_query) 
	  printf("\nThe parsed query string generated is:\n%s\n",
		 (char *) result->parsed_query);
	if (result->num_xformers > 0) {
	  printf("\nThere were (%d) transformations in this query:\n",
		 (int) result->num_xformers);
	  for (num=1,i=0;i<result->num_terms || num == result->num_xformers;
	       i++) {
	    if (result->term_list[i].code == TRANSFORMER)
	      printf("\t%2d: %s -> %s\n", (int) num++,
		     (char *) result->term_list[i].list[0],
		     (char *) result->term_list[i].list[1]);
	  }
	}
	  
	printf("\nYour query had the following (%d) spelling errors:\n",
	       (int) result->num_not_found);
	printf("\t    Original       Stemmed\n");
	for (num=1,i=0;i<result->num_terms && num <= result->num_not_found;i++) {
	  if (result->term_list[i].code == TERM_NOT_FOUND)
	    printf("\t%2d: %-15s%s\n", (int) num++,
		   (char *) result->term_list[i].list[0],
		   (char *) result->term_list[i].list[1]);
	}
	printf("\nWould you like to reenter the query? (y/[n])?");
	fgets((char *)aline, 80, stdin);
	if (tolower(aline[0]) == 'y')
	  reenter_flag = true;
      }
      break;
    case SYNTAX_ERROR:
      sprintf((char *)msg_str, 
	      "\nThere was a syntax error:\n%s\n%*s^\nPlease reenter...\n",
	      (char *) saveq, (int) result->error_offset - 2," ");
      reenter_flag = true;
      break;
    case NO_INDEXED_TERMS:
      sprintf((char *)msg_str, 
	      "\nNone of the terms in your query were found in\nin the current database, please reenter...\n");
      reenter_flag = true;
      break;
    case EVALUATION_ERROR:
      if (result->num_not_found) {
	printf("\nYour query had the following (%d) spelling errors:\n",
	       (int) result->num_not_found);
	printf("\t    Original       Stemmed\n");
	for (num=1,i=0;i<result->num_terms && num <= result->num_not_found;i++) {
	  if (result->term_list[i].code == TERM_NOT_FOUND)
	    printf("\t%2d: %-15s%s\n", (int) num++,
		   (char *) result->term_list[i].list[0],
		   (char *) result->term_list[i].list[1]);
	}
      }
      sprintf((char *)msg_str, 
	      "\nNo documents matched your query, please reenter...\n");
      reenter_flag = true;
      break;
    default:
      Q_error("Bad error code returned from inq_check_query\n");
    }
    inq_free_query_info(&result);
    free(saveq);
    if (reenter_flag)
      return(get_query);
  }

  inq_free_belief_list(bl);
  
  /* Added tty_cbreak for query evaluation -- RCC 12/5/95 */
#if defined(NETWORK)
 /*
  tty_cbreak(0);

  bl = inq_eval_query_top_n(db, newq, inq_feedback, maxdocs);

  tty_reset(0); 
  */
  bl = inq_eval_query_top_n(db, newq, performance, maxdocs);

#else
  bl = inq_eval_query_top_n(db, newq, performance, maxdocs);
#endif

  if (!bl || bl->doc_cnt < 0 )
  {
      sprintf((char *)msg_str, "\nNone of the query terms exists in database\n ");
      return(get_query);
  }

  if (bl->doc_cnt == 0 ) 
  {
      sprintf((char *)msg_str, "\nNo documents retrieved for the last query.\n");
      return(get_query);
  }
  

  if (switch_remove_duplicate_docs)
    {
    printf("Removing duplicates...\n");
    remove_duplicate_docs (maxdocs);
    }

  if (*logname) {
    /* disable logging on MSDOS */
    #if defined(NETWORK)
      Q_warning ("Logging of top n docs is disabled");
    #else
      printf("Writing to file...\n");
      write_top_n_docs (logfile, bl, maxdocs);
    #endif
  }

  release_text(doc);
  if ((tokenize_query(newq)) == 0)
      return(quit_prog);
  release_summary(&headsum);

  return(browse_summary);
}

/* 
 * Create some space for a new node of the linked list.
 * NOTE: No space is malloced for temp->text other than the pointer.
 *       therefore it must be done before it can be assigned something.
 */
LINE *gen_new()
{
  LINE *temp=(LINE *)NULL;		/* go through list */
  
  if (!(vector_make_room_for_n(&temp, 1, sizeof(LINE))))
    {
    Q_warning("Memory allocation error in gen_new().");
    exit(-4);
    }
  memset((void *)temp, (int)'\0', (size_t)sizeof(LINE));
  temp->prev = (struct line *)NULL;
  temp->next = (struct line *)NULL;
  return(temp);
}

/*
 * Create space for the document/summary screen by screen information.
 */
PARTDOC *gen_new_doc()
{
  PARTDOC *temp=(PARTDOC *)NULL;

  if (!(vector_make_room_for_n(&temp, 1, sizeof(PARTDOC))))
    {
    Q_warning("Memory allocation error in gen_new_doc().");
    exit(-4);
    }
  memset(temp, '\0', sizeof(PARTDOC));
  return(temp);
}

/* 
 * This procedure takes the input document (or summary) and structures
 * it using struct LINE to be able to do scrolling and paging 
 * efficiently and similarly each time through. 
 *
 * dokeys = 0 if no key words are looked for
 *        = 1 if key words are looked for
 *        = 2 if no key words are looked for and screen_width is 
 *          "hardwired" as 80 for the 'w' command.
 * check_left = 0 if no check is made for length of the last line
 *            = 1 if the last line is checked for being width characters
 * used to insure the lack of breaks in the middle of paragraphs and tables.
 */
PARTDOC *set_up_text(PARTDOC *buff, Int_t indent, Int_t dokeys, Int_t width,
		     Int_t check_left) 
{
  Int_t  curpos;			/* pointer to current line in temp */
  Char_t *current;		/* line being added to the list */
  Char_t *hold = buff->rest;	/* check if there is a newline */
  Char_t ind[MAXLEN];		/* add in the indent */
  LINE *key;			/* list of lines containing keywords */
  LINE *lastkey = buff->keytail;
  LINE *last = buff->tail;	/* placeholder in list */
  Int_t  lcount = -1;		/* ato add to number of lines formatted */
  Int_t  lines = screen_length();
  Int_t  length = FALSE;		/* use indent this line? */
  Char_t *pos = buff->rest;	/* pointer to full text */
  LINE *temp;			/* placeholder through document */
  Char_t *tempos;			/* hold pointer value */
  static Int_t  lastn = FALSE;	/* remove beginning blanks? */

  memset((void *)ind, (int)'\0', (size_t)MAXLEN);
  if (indent > 0)
    memset((void *)ind, (int)' ', (size_t)indent);
				/* start new document */
  if (buff->lines == 0)
    lastn = FALSE;
				/* as long as more exists, convert 1 screen */
  while (pos && (*pos != '\0') && (lines > ++lcount))
    {		
    current = chopline(pos, (length) ? width-indent : width, lastn); 
    if (!current || !*current)
      {
      buff->rest = (Char_t *)NULL;
      break;
      }
    if (current && (tempos = (Char_t *)strstr((char *)pos, (char *)current)) != (Char_t *)NULL)
      pos = tempos;
    curpos = strlen((char *)current);

    /* This is used to max out the line lengths as much as possible by
     * returning a partial line if more text is to be dealt with later.
     * check for newline:
     * if no newline, remove following blanks
     *   if checking line length, 
     *   and the line is shorter than it could be
     *   and no more text is currently present return the line.
     */
    pos += curpos; 
    if (!strchr((char *)current, '\n'))
      {
      for (;*pos && ((*pos == ' ') || (*pos == '\t')); pos++); 
      if (check_left && (strlen((char *)current) < width) && (*pos == '\0'))
	break;
      }
				/* give it some space to live */
    temp = gen_new();
    /* necessary since lines are variable lengths, prevents mallocing 
       unused space */
    if (!vector_make_room_for_n(&temp->text, (length) ?
				curpos+indent+2 : curpos+2,
				sizeof(Char_t)))
      {
      Q_warning("Memory allocation error in set_up_text()");
      exit(-4);
      }
    memset((void *)temp->text, (int)'\0', 
           length ? (size_t)(curpos+indent+2) : (size_t)(curpos+2));
    if (length)
      sprintf((char *)temp->text,"%s%s", (char *) ind, (char *) current);
    else
      strcpy((char *)temp->text, (char *)current);

				/* if no newlines exist give it one */
    if ((hold = (Char_t *)strchr((char *)(temp->text),'\n')) == 0)
      {
      temp->text[(length) ? curpos+indent : curpos] = '\n';
      lastn = TRUE;
      }
    else lastn = FALSE;
				/* pointer manipulation for structs */
    if (!buff->head) 
      {
      buff->head = temp;
      buff->head->num = 1;	/* start line numbering at 1 */
      }
    if (last)
      {
      last->next = temp;
      temp->prev = last;
      temp->num = last->num + 1;
      }
    last = temp;

				/* if keywords in this line point to it */
    if (strchr((char *)(temp->text), ESCCHAR))
      {
      key = (LINE *)gen_new();
      key->text = temp->text;
      if (!buff->keyhead)
	{
	buff->keyhead = key;
	buff->keyhead->keynum = 1;
	buff->keyhead->num = last->num;
	}
      if (lastkey)
	{
	lastkey->next = key;
	key->prev = lastkey;
	key->keynum = lastkey->keynum + 1;
	key->num = last->num;
	}
      lastkey = key;
      }
    else			/* only count keyword lines? */
      if (dokeys)
	lcount--;
				/* does the next line get indented? */
    if (indent > 0 && !hold)
      length = TRUE;
    else
      length = FALSE;
    if (*pos == '\0')
      pos = (Char_t *)NULL;
  }
  
  if (lastkey)
    {
    buff->keytail = lastkey;
    buff->keylines = lastkey->keynum;
    }
  buff->tail = last;
  if (last)
    buff->lines = last->num;

  if (buff->rest && *buff->rest)
    if ( (pos != (Char_t *)NULL) && (*pos == '\0') )
      buff->rest = (Char_t *)NULL;
    else
      buff->rest = pos;

  return(buff);			/* give back the head of the list */
}

/*
 * release globally static structures.
 */
void release_text(disp_type menu)
{
  LINE *temp, *next;

  switch(menu)
    {
    case doc:
      if (front)
	{
	/* Free up key list */
	if(front->keyhead)
	{
	    for(temp = front->keyhead;
		temp != (LINE *)NULL ; temp = next)
	    {
		next = temp->next;
		vector_free((void **)&temp);
	    }
	}
	for(temp = front->head; temp && temp->next;)
	  {
	  next = temp->next;
	  if (temp->text)
	    vector_free((void **)&temp->text);
	  vector_free((void **)&temp);
	  temp = next;
	  }
	if (temp)
	  {
	  if (temp->text)
	    vector_free((void **)&temp->text);
	  vector_free((void **)&temp);
	  }
	if (front->title)
	  vector_free((void **)&front->title);
	memset(front, '\0', sizeof(PARTDOC));
      }
      break;

    case hlp:
      if (helpmenu)
	{
	for(temp = helpmenu->head; temp && temp->next;)
	  if (temp)
	    {
	    next = temp->next;
	    if(temp->text)
	      vector_free((void **)&temp->text);
	    vector_free((void **)&temp);
	    temp = next;
	    }
	if (temp)
	  {
	  if(temp->text)
	    vector_free((void **)&temp->text);
	  vector_free((void **)&temp);
	  }
	memset(helpmenu, '\0', sizeof(PARTDOC));
      }
      break;

    default:
      break;
    }
}

Char_t *addtohelp(Char_t *temp)
{
  static Char_t *helpbuff=(Char_t *)NULL; /* hold help menu */
  static Int_t  pos = 0;		    /* hold value of current position */
  Int_t    tempos = strlen((char *)temp);   /* only have to calculate it once */

  if(!vector_make_room_for_large_n(&helpbuff,(pos+tempos+1),sizeof(Char_t)))
    {
    Q_warning("Not enough room for the help menu buffer.");
    exit(-4);
    }
  
  strcpy((char *)(helpbuff+pos), (char *)temp);
  pos += tempos;
  helpbuff[pos] = '\0';
  return(helpbuff);
}

/* This procedure is the help menu for the current inquery.  Should be
 * updated to include any new operators or functions. Preferrably will be
 * set up as a separate help.c and help.h file for ease in update and
 * to keep this file from growing extraneosuly large. 
 */
Int_t set_help()
{
  Char_t s[MAXLEN];		/* hold first line of help menu */
  Char_t temp[MAXLEN];		/* hold temporary string */
  Int_t  width = screen_width();

  sprintf((char *)s, "%-*s\n", (int)screen_width(), "Help Menu");

  strcpy((char *)temp, "The following commands are accessed by typing the capitalized letter.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Parameters directly follow the single capital letter.\n\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "?                     - Get to this help screen.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "page Forward/Back     - Go a page at a time through a document. Wraps 2 lines from current screen.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "line Up/Down          - Go line by line through a document.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "doc Next/Prev         - View the Next or Previous document in order of the belief listing.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Top/End               - Jump to the first screen or the last screen of the current document or summary.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Key                   - Starting from the fourth line of the current screen, the next line of the document containing a keyword is looked for.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "All                   - List of all lines in document containing keywords. Toggle between document and keyword summary.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Query                 - Enter a new query.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Change settings       - Change how many documents are being summarized.\n");
  strcpy((char *)temp, "Refresh               - Used to redraw the current screen in case of outside screen interference.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "eXit                  - Quit the program or the Help Menu.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Summary               - Get to the summary screen.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#                     - The position in the document list of the document you wish to view.  \"1\" for the highest ranked document retrieved.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "+ i                   - Mark the document number i as relevant.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Modify                - Modify the query given the evidence of the marked \n                        relevant documents.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Z                     - View the parsed query.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Y                     - View the modified query.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "Write i [ - j ]       - Write documents ranked i through j (if j is specified) to docs.out.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp,"\nQueries can be expressed in either English or a structured query language.\nThe structured query language includes the following set of operators:\n\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#and (t_1, ..., t_n)    Documents should contain all terms t_1, ..., t_n.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#or  (t_1, ..., t_n)    Documents should contain at least 1 of terms t_1,..., t_n.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#not (t_1)              Documents should not contain term t_1.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#sum (t_1, ..., t_n)    Documents should contain all terms t_1, ..., t_n. The document belief is the sum of the beliefs in terms t_1, ..., t_n.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#wsum (node_weight, w_1 term_1 ... w_n term_n)  all w_i, 0 <= w_i <= 1.0; and node_weight, 0 <= node_weight <= 1.0.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#max (t_1, ..., t_n)    Documents should contain all terms t_1, ..., t_n. The document belief is the maximum of the beliefs for terms t_1, ..., t_n.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#n   (t_1, ..., t_n)    Documents should contain terms t_1, t_2, ..., and t_n.  The distance in the document between any adjacent query terms should be <= n.\n");
  strcpy((char *)temp, "#uwn (t_1, ..., t_n)    (Unordered Window n).  Documents should contain terms t_1, t_2, ..., t_n, in any order.  Distance between the first and last term should be <= n.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#phrase (t_1, ..., t_n) Documents should be about the phrase t_1, ..., t_n.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#passageN (t_1, ...,t_n) Documents should contain a passage of N words about terms (or nodes) t_1, ..., t_n.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#syn (t_1, ..., t_n)    Terms t_1, ..., t_n should be considered synonymous.\n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "#field (FIELDNAME #FOP t_1, ..., t_n) \n");
  if(addtohelp(temp) == 0)  return(0);
  strcpy((char *)temp, "                        The document field FIELDNAME should contain terms t_1, ..., t_n as modified by the field operator (#FOP).\n");

  if((helpmenu->rest = addtohelp(temp)) == 0)  
    return(0);

				/* set up the whole thing at once */
  while (helpmenu->rest)
    helpmenu = set_up_text(helpmenu, 24, FALSE, width, FALSE); 

  
  return(1);
}

/* 
 * Truncate the string to maxlen, and remove trailing blanks.
 * Note: buffer is transient, must copy from it on return 
 * Even though maxlen changes in the loop potentially, no need to return
 * the final value as the strlen of the returned string is used.
 * 
 * do_space = 0 if the leading blanks are not to be removed
 *          = 1 if they are to be removed
 */
Char_t *chopline(Char_t *s, Int_t length, Int_t do_space)
{
  static Char_t *buff = (Char_t *)NULL; /* buffer to return */
  Int_t  here = 0;		/* hold place of last space or comma */
  Int_t  maxlen = length;	/* length of current line for printing */
  Int_t  odd = 0;		/* do we want even/odd num characters */
  Int_t  sp = 0;		/* goes through s */
  Int_t  slen;

  /* remove beginning blanks */
  if (do_space) {
    for (sp = 0; s[sp] && (s[sp] == ' '); sp++);
    s += sp;
  }

  if(buff)
    vector_free((void **)&buff);

  if (s[0] == '\0')
    return((Char_t *)NULL);

  slen = strlen((char *)s);

  for (sp = 0; sp < slen && sp < maxlen; sp++)
    {
      /* don't count ESC chars in line length */
      if (s[sp] == ESCCHAR)
	{
	  maxlen += ESCSIZE;
	  sp += ESCSIZE-1;
	  continue;
	}
      /* mark last breaking point reached */
      if ((s[sp] == ' ') || (s[sp] == ','))
	here = sp;
      /* chop at newline? */
      if (s[sp] == '\n')
	break;
    }
  if (!s[sp])
    maxlen = sp;
  else if ((sp == maxlen) && s[sp+1] && (s[sp+1] == ',') && (here > 0))
    maxlen = here+1;
  else if ((sp < maxlen) && (s[sp] == '\n'))
    maxlen = sp+1;
  else if (sp == maxlen && here > 0)
    maxlen = here+1;
  if (s[maxlen-1] == ' ')
    maxlen--;
  if (!vector_make_room_for_n(&buff, (maxlen+1), sizeof(Char_t)))
    {
      Q_warning("Memory allocation error in chopline()");
      exit(-4);
    }
  memset((void *)buff, (int)'\0', (size_t)(maxlen+1)); 
  strncpy((char *)buff, (char *)s, (size_t)maxlen);
  return(buff);			/* return the line */
}

/* 
 * Buffer is transient.
 */
Char_t *read_query(Char_t *prompt)
{
  static Char_t buff[MAXLEN];	/* need room for user input */
  UChar_t *buf2 = (UChar_t *)NULL; /* start empty add stuff on */
  Int_t  pos = 0,pos2 = 0, i;	/* positions of strings input */


  memset((void *)buff, (int)'\0', (size_t)MAXLEN);

  printf("%s\n", (char *) prompt);
    
  do{
    if (!vector_make_room_for_large_n(&buf2, pos2 + pos + 3, sizeof(char))) 
      {
	Q_warning("Not enough memory for entire query.");
	return((Char_t *)NULL);
      }

    if (pos2 > 0) 
      {
        /* make sure there's a newline between query components */
	/*	strcpy((char *)(buf2+pos2), (char *)" ");  */
	strcpy((char *)(buf2+pos2), (char *)"\n"); 
	pos2++;
      }

    if (pos > 0)
      {
        /* add line to query buff */
	strcpy((char *)(buf2+pos2), (char *)buff); 

        /* update position in string */
        pos2 += pos;		
      }

    strcpy((char *)(buf2+pos2), (char *)"\0");
    gets((char *)buff);

    for (i=0; buff[i] && isspace(buff[i]); i++);

    /* If the user enters "quit" to signal the query is void, check to see */
    /* if they would like to exit INQUERY.  If not, return a null query */
    /* and go back to where they came from, else exit completely. */
    if (!strcmp ((char *)&buff[i], (char *)"quit") || 
	!strcmp ((char *)&buff[i], (char *)"exit"))
      {
	vector_free((void **)&buf2);
	buf2 = (UChar_t *)NULL;
	if (!(vector_make_room_for_n(&buf2, 5, sizeof(char))))
	  {
	    Q_warning("Not enough memory for entire query.");
	    return((Char_t *)NULL);
	  }
	strcpy((char *)buf2, (char *)"quit");
	return(buf2);
      }

    for(i=0; buff[i]; i++)	/* remove tabs from query and replace with */
      if (buff[i] == '\t')	/* spaces; for printing purposes */
	buff[i] = ' ';
	
    if (*buff == '?')		/* goto help */
      {
	vector_free((void **)&buf2);
	return(buff);
      }

  }while ((pos = strlen((char *)buff)) > 0); 

  /* 
   * Make sure the query ends with a newline to keep hanging comments
   * from being ignored by the query parser.
   */
  if (buf2[pos2-1] != '\n') {
    buf2[pos2++] = '\n';
    buf2[pos2] = '\0';
  }

  if (blank(buf2))		/* check for alphanumeric input only */
    {
      vector_free((void **)&buf2);
      return((Char_t *)NULL);
    }
  return(buf2);
}

Int_t new_doc(Int_t newrank)
{
  Int_t  id;			/* id of current document */
  Int_t  restlength;		/* length left on the line to use */
  Char_t buff[MAXLEN];		/* temp buffer to hold title */
  Char_t *s = buff;		/* used to go through buff */
  Char_t *tmp;
  Char_t *id_str;
  Int_t  width = screen_width(); /* width of screen for printing */
  Int_t  length;		/* temp variable reducing strlen() calls */
  Int_t  relevant;		/* TRUE if doc is relevant, else FALSE */

  memset((void *)buff, (int)'\0', (size_t)MAXLEN);

  if ((newrank > bl->doc_cnt) || (newrank < 1))
    {
    sprintf((char *)msg_str, "Valid rankings are 1 - %i.", (int) bl->doc_cnt);
    return(-1);
    }
  if (front->head)
    release_text(doc);

  id = bl->list[newrank-1].doc_id;
  if ((relevant = find_rel_doc(id)) != -1)
    sprintf((char *)s, "%c", (rel_docs[relevant].relevant == TRUE)?'+':'-');
  else sprintf((char *)s, " ");
  s++;

  ui_get_doc (db, id, ALL);

  id_str = ui_lookup_doc_field (INQ_DOC_FIELD_ID);
  if(strlen(id_str) > 30)
  {
      id_str[27] = '.';
      id_str[28] = '.';
      id_str[29] = '.';
      id_str[30] = '\0';
  }
  

  sprintf((char *)s, " %3d. %.3f %s ", (int)newrank, (float)bl->list[newrank-1].belief,
	  (char *)id_str);
  length = strlen((char *)buff);
  restlength = width - length;
  s += length - 1;
  tmp = (char *)chopline(rmv_blanks(cr_to_sp(ui_lookup_doc_field (INQ_DOC_FIELD_TITLE), FALSE)), restlength, TRUE);
  if (tmp) sprintf((char *)s, "%-*s", (int)restlength, tmp);

  length = strlen((char *)buff);
  if (!vector_make_room_for_n(&front->title, length+1, sizeof(Char_t)))
    {
    Q_warning("Memory allocation error in new_doc()");
    exit(-4);
    }
  memset((void *)front->title, (int)'\0', (size_t)(length+1));
  strncpy((char *)front->title, (char *)buff, (size_t)length);
  front->id = id;
  front->rank = newrank;
  front->rest = ui_lookup_doc_field (INQ_DOC_FIELD_TEXT);
  front->num_matches = ui_get_doc_nummatches();
  front->matches = ui_get_doc_matches();
  front->doc_offset = 0;
  highlight_query_words();
  return(1);
}

Char_t *highlight(Char_t *instr, Int_t width)
{
  static Char_t outstr[MAXLEN];

  memset((void *)outstr, (int)'\0', (size_t)MAXLEN);

  if (!instr)
    sprintf((char *)outstr, "%s%-*s%s", STR_REV_ON, (int)width, " ", STR_REV_OFF);
  else
    sprintf((char *)outstr, "%s%-*s%s", STR_REV_ON, (int)width, instr, STR_REV_OFF);

  return(outstr);
}

/* DRAW_MENU draws out the basic screen pattern:         */
/*    query line, text, menu line, entry/message line.   */
BASIC *draw_menu(Char_t *topline, BASIC *cur, Char_t *options, disp_type menu)
{
  LINE *temp = (LINE *)NULL;

  /* start with a clean screen. */
  CLEAR;

  /* put the top line (highlighted) on first */
  printf("%s\n", (char *) highlight(topline, screen_width()));

  /* now put the body of whatever is requested, in normal print */
  temp = show_lines(screen_length()-3, cur, menu);

  /* now highlight the menu options */
  printf("%s\n", (char *) highlight(options, screen_width()));

  /* if a message needs to be printed, then beep and print it last. */
  if (*msg_str)
    {
    BEEP;
    printf("%s", (char *) msg_str);
    memset((void *)msg_str, (int)'\0', (size_t)strlen((char *)msg_str));
    }

  cur->tail = temp;
  return(cur);
}

ui_state disp_summ(SUMMARY *cursum)
{
  Int_t width = screen_width();
  Int_t height = screen_length()-3; /* number of printable lines per screen */
  Int_t do_sum = TRUE;
  cmd  next_comm = none, hold_comm = bye;
  Char_t *s;
  Char_t str[MAXLEN];
  cur_sum_ent first_ent, last_ent, hold_ent;

  first_ent.entry = last_ent.entry = 0;
  first_ent.line = last_ent.line = (cursum->entries[0])->text;

  memset(str, '\0', MAXLEN);
  strcpy((char *)str, (char *)"Query: ");
  strcpy((char *)(str+strlen((char *)str)), 
         (char *)chopline(cursum->query, width-8, TRUE));

  while(strlen((char *)str) && str[strlen((char *)str)-1] == '\n')
      str[strlen((char *)str)-1] = '\0';

  for(;;)
    {
    last_ent = draw_summary(cursum, str, width, first_ent);
    if (hold_comm == refresh && hold_comm != next_comm)
      next_comm = refresh;
    else
      next_comm = get_comm();
    hold_comm = next_comm;
				/* check for doc number entered */
    if (next_comm > 0)
      {
      new_doc(next_comm);
      return(browse_doc);
      }
				/* Now do the actual code... */
    switch(next_comm)
      {
      case fwd:
	/* make sure enough summary exists to do this... */
	adjust_cur_sum_ent(cursum, last_ent, DOWN, screen_length());
	if (last_ent.entry >= cursum->num_ent) 
	  strcpy((char *)msg_str, (char *)"No more screens. ");
	else
	  first_ent = adjust_cur_sum_ent(cursum, last_ent, UP, 2);
	break;
      case back:
	if ((first_ent.entry == 0) && 
	    (first_ent.line == (cursum->entries[0])->text))
	  strcpy((char *)msg_str, "No previous screens. ");
	else 
	  first_ent = adjust_cur_sum_ent(cursum, first_ent, UP, height-1);
	break;
      case up:
	if ((first_ent.entry == 0) && 
	    (first_ent.line == (cursum->entries[0])->text))
	  strcpy((char *)msg_str, "No previous lines. ");
	else
	  first_ent = adjust_cur_sum_ent(cursum, first_ent, UP, 1);
	break;
      case down:
	/* make sure enough summary is configured to do this... */
	adjust_cur_sum_ent(cursum, last_ent, DOWN, 1);
	if (last_ent.entry >= cursum->num_ent)
	  strcpy((char *)msg_str, "No more lines. ");
	else 
	  first_ent = adjust_cur_sum_ent(cursum, first_ent, DOWN, 1);
	break;
      case top:
	if (cursum->entries)
	  {
	  first_ent.entry = 0;
	  first_ent.line = (cursum->entries[0])->text;
	  }
	break;
      case end:
	/* make sure the summary is entirely configured... */
	for(;last_ent.entry < cursum->num_ent;)
	  last_ent = adjust_cur_sum_ent(cursum, last_ent, DOWN,
					screen_length()*2);

	first_ent.entry = cursum->num_ent-1;
	first_ent.line=(cursum->entries[first_ent.entry])->text;
        for(;*(first_ent.line+1);first_ent.line++);
	first_ent = adjust_cur_sum_ent(cursum, first_ent, UP, height-1);
	break;
      case help:
	return(browse_help);
      case ndx:
        return(show_ndx_table);
      case writef:		/* write some docs to a file */
      case rel:			/* or mark relevance */
	if (params[1])
	  for(s = &params[1]; *s; s++)
	    if (isdigit(*s))
	      do_sum = FALSE;
	if (next_comm == writef)
	  store_docs_in_file(cursum->query, (do_sum) ? sum : doc);
	if (next_comm == rel)
	  hold_comm = mark_docs (cursum, TRUE, (do_sum) ? sum : doc);
	do_sum = TRUE;
	break;
      case nonrel:
	/* hold_comm = mark_docs (cursum, FALSE, (do_sum) ? sum : doc); */
	strcpy((char *)msg_str, "Non-Relevance not yet implemented.");
	break;
      case mod_qry:
	return(modify_query);
      case mod_prs_qry:
	return(modify_parsed_query);
      case prs_qry:
	return(parsed_query);
      case query:		/* get a new query */
	return(get_query);
      case refresh:		/* do nothing for refresh */
	break;
      case change:		/* change default user settings */
	return(new_settings);
      case bye:			/* get out of inquery */
	return(quit_prog);
      case none:
	break;
      default:			/* report invalid command */
	sprintf((char *)msg_str, "Not a valid function for Summary menu. ");
	break;
      }
    }
}

ui_state disp_help()
{
  static BASIC *cur=(BASIC *)NULL; /* point start/end of current screen */
  cmd  next_comm;		/* next user entered command */

  if (!helpmenu->head)
    {
    strcpy((char *)msg_str, "Error creating help structure.");
    return(go_to_last);
    }

  if (cur)
    vector_free((void **)&cur);
  cur=(BASIC *)NULL;
  if (!(vector_make_room_for_n(&cur, 1, sizeof(BASIC))))
    {
    Q_warning("Memory allocation error in disp_help().");
    exit(-4);
    }  
  memset(cur, '\0', sizeof(BASIC));
  cur->head = helpmenu->head; 

  for (;;)
    {
    cur = draw_menu((Char_t *)"Help Menu", cur, (Char_t *)HELP_MENU, hlp);
    next_comm = get_comm();
    switch(next_comm)
      {				/* do nothing for help/refresh */
      case fwd:			/* move forward one page */
	if (!cur->tail)
	  strcpy((char *)msg_str, "No more screens beyond the current screen.");
	else
	  cur = move(cur, screen_length()-5, DOWN);
	break;
      case back:			/* go back a single page */
	if (!cur->head->prev)
	  strcpy((char *)msg_str, "No more screens before the current screen.");
	else
	  cur = move(cur, screen_length()-5, UP);
	break;
      case up:			/* go up one line */
	if (!cur->head->prev)
	  strcpy((char *)msg_str, "No more lines before the current screen.");
	else
	  cur = move(cur, 1, UP);
	break;
      case down:			/* go down one line */
	if (!cur->tail)
	  strcpy((char *)msg_str, "No more lines after the current screen.");
	else
	  cur = move(cur, 1, DOWN);
	break;
      case top:			/* go to top of info */
	cur->head = helpmenu->head; 
	break;
      case end:			/* go to bottom of info */
	cur->head = helpmenu->tail; 
	cur = move(cur, screen_length()-4, UP);
	break;
      case help:
	break;
      case refresh:
	break;
      case bye:			/* exit help only */
	return(go_to_last);
      case none:
	break;
      default:			/* report invalid command */
	sprintf((char *)msg_str, "Not a valid function for Help menu");
	break;
      }
    }
}

ui_state disp_doc()
{
  cmd    next_comm=none, hold_comm=bye;	/* next command the user enters */
  Int_t  holdrank = 0;		/* holds rank of current document       */
  Int_t  view = FALSE;		/* TRUE if viewing allkeys              */
  static BASIC *cur=(BASIC *)NULL; /* point to start/end of current screen */
  BASIC *nextp;
  Char_t  *menu = (Char_t *)BROWSE_MENU1;	/* what menu to start with */
  char   tmp_str[80];
  

  if (!cur)
    if (!(vector_make_room_for_n(&cur, 1, sizeof(BASIC))))
      {
      Q_warning("Memory allocation error in disp_doc().");
      exit(-4);
      }  
  memset(cur, '\0', sizeof(BASIC));
  cur->head = front->head; 

  for (;;)
    {
    nextp = draw_menu(front->title, cur, menu, doc);
    if (hold_comm == refresh && next_comm != hold_comm)
      next_comm = refresh;
    else
      next_comm = get_comm();
    hold_comm = next_comm;
    
				/* check for doc number entered */
    if (next_comm > 0)
      {
      new_doc(next_comm);
      cur->head = front->head;
      cur->tail = (LINE *)NULL;
      continue;
      }

    switch(next_comm)
      {
      case next:		/* go to next doc in list */
	if (front->rank == bl->doc_cnt)
	  {
	  strcpy((char *)msg_str, "No following documents to view.");
	  break;
	  }
	new_doc(front->rank+1);
	cur->head = front->head;
	view = FALSE;
	break;
      case prev:		/* go to prev doc in list */
	if (front->rank == 1)
	  {
	  strcpy((char *)msg_str, "No previous documents to view.");
	  break;
	  }
	new_doc(front->rank-1);
	cur->head = front->head;
	view = FALSE;
	break;
      case down:		/* go down one line */
	if (!cur->tail)
	  if (front->rest)
	    {
	    highlight_query_words();
	    cur = move(cur, 1, DOWN);
	    }
	  else
	    strcpy((char *)msg_str, "No more lines after the current screen.");
	else cur = move(cur, 1, DOWN);
	break;
      case top:
	if (!view)
	  cur->head = front->head;
	else
	  cur->head = front->keyhead;
	break;
      case end:
	while (front->rest)
	  highlight_query_words();
	/* check if we are viewing regular document or all*/
	if (!view)
	  cur->head = front->tail; 
	else
	  cur->head = front->keytail;
	cur = move(cur, screen_length()-4, UP);
	break;
      case left:		/* go to first menu */
	menu = (Char_t *)BROWSE_MENU1;
	break;
      case right:		/* go to second menu */
	menu = (Char_t *)BROWSE_MENU2;
	break;
      case allkeys:		/* top of keyword listing */
	if (!view)
	  {
	  while (front->rest && (!front->keyhead))
	    highlight_query_words();

	  if (!front->keyhead)
	    sprintf((char *)msg_str, "No keywords to view in document %i",
		    (int) front->rank);
	  else {
	    view = TRUE;
	    cur->head = front->keyhead;
	    }
	  }
	else {
	  view = FALSE;
	  cur->head = front->head;
	  }
	break;
      case nextkey:		/* get next line of doc with a keyword */
	cur = next_doc_key(cur, view); 
	break;
      case summary:		/* display the summary */
	return(browse_summary);
      case writef:		/* write docs to a file */
	holdrank = front->rank;
	store_docs_in_file(headsum->query, doc);
	new_doc(holdrank);
	cur->head = front->head;
	break;
      case rel:
	hold_comm = mark_docs(headsum, TRUE, doc);
	break;
      case nonrel:
	/* hold_comm = mark_docs(headsum, FALSE, doc); */
	strcpy((char *)msg_str, "Non-Relevance not yet implemented.");
	break;
      case mod_prs_qry:
	return(modify_parsed_query);
      case prs_qry:
	return(parsed_query);
      case query:		/* get a new query */
	return(get_query);		
      case refresh:		/* do nothing for refresh */
	break;
      case change:		/* change default user settings */
	return(new_settings);
      case bye:			/* get out of inquery */
	return(quit_prog);
      case help:
	return(browse_help);
      case fwd:			/* move forward one page */
	if (!cur->tail)
	  if (front->rest)
	    {
	    highlight_query_words();
	    cur = move(cur, screen_length()-5, DOWN);
	    }
	  else
	    strcpy((char *)msg_str, "No more screens beyond the current screen.");
	else cur = move(cur, screen_length()-5, DOWN);
	break;
      case back:			/* go back a single page */
	if (!cur->head->prev)
	  strcpy((char *)msg_str, "No more screens before the current screen.");
	else
	  cur = move(cur, screen_length()-5, UP);
	break;
      case up:			/* go up one line */
	if (!cur->head->prev)
	  strcpy((char *)msg_str, "No more lines before the current screen.");
	else
	  cur = move(cur, 1, UP);
	break;
      case ndx:
        return (show_ndx_table);
        break;
      case eval_query_feedback:
        {
	    void *query_contrib;
	    int i, num_part;
	    float contrib;
	    char *query_part;
	    
	    CLEAR;
	    if(!inq_get_eval_feedback_on_doc(db, headsum->query,
					     bl->list[front->rank - 1].doc_id,
					     &query_contrib))
	    {
		fprintf(stdout,"\n\nError returned by inq_get_eval_feedback_on_doc.\n");
	    }
	    else
	    {
		inq_query_contrib_num_parts(query_contrib, &num_part);
		for(i=0; i < num_part; i++)
		{
		    if(!inq_query_contrib_ith_part(query_contrib, i, &query_part)
		       || !inq_query_contrib_ith_value(query_contrib, i, &contrib))
		    {
			printf("Failed to get values for query element #%d.\n",
				i);
			
		    }else
			printf("%.3f  %s\n\n", contrib, query_part);
		}
		
	    }
	    
	    fprintf(stdout,"\npress return to continue: ");
	    gets((char *)tmp_str);
	    inq_free_query_contrib(query_contrib);
	}
	
        break;

      case none:
	break;
      default:			/* report invalid command */
	sprintf((char *)msg_str, "Not a valid function for Document menu"); 
	break;
      }
    }
}

/* get the next user input, make sure it is valid */
cmd get_comm()
{
  Int_t  rank = -1;		/* if positive, document number */

  gets((char *)params);
  
  if ((rank = checknum(params)) > 0)
    return((cmd)rank);

  switch(tolower(params[0]))
    {
    case 'a': return(allkeys);
    case 'b': return(back);
    case 'c': return(change);
    case 'd': return(down);
    case 'e': return(end);
    case 'f': return(fwd);
    case 'h': return(help);
    case 'i': return(ndx);
    case 'k': return(nextkey);
    case 'm': return(mod_qry);
    case 'n': return(next);
    case 'p': return(prev);
    case 'q': return(query);
    case 'r': return(refresh);
    case 's': return(summary);
    case 't': return(top);
    case 'u': return(up);
    case 'w': return(writef);
    case 'v': return(eval_query_feedback);
    case 'x': return(bye);
    case 'y': return(mod_prs_qry);
    case 'z': return(prs_qry);
    case '<': return(left);
    case '>': return(right);
    case '?': return(help);
    case '+': return(rel);
    case '-': return(nonrel);
    default: 
      strcpy((char *)msg_str, "No valid command entered.");
      return((cmd)none);
    }
}

/* SHOW_LINES will display on the screen a given number of lines of the */
/* current info of type menu. */ 
LINE *show_lines(Int_t lines, BASIC *start, disp_type menu)
{
  Int_t lcnt;			/* lines printed so far    */
  LINE *temp, *temp2=start->head; /* pointer to current line */

  for(lcnt = 0, temp = start->head; lcnt < lines; lcnt++)
    {
    /* If there is something given to print, print it. */
    /* Otherwise, if something is not given, but more can be found, then */
    /* find the extra and print it.  If nothing more can be found, print */
    /* blank lines until the end of the current screen section is reached. */
    if (temp)
      {
      printf("%s", (char *) temp->text);
      temp2 = temp;
      temp = temp->next;
    } else if ((menu == doc) && front->rest) 
      {
      highlight_query_words();
      temp = temp2->next;
      lcnt--;
    } else printf("\n");
    }
  
  /* return the current line to the calling procedure. */
  return(temp);
}

/* Move the current pointer through the info up or down x lines */
BASIC *move(BASIC *start, Int_t lines, Int_t dir)
{
  Int_t lcnt;			/* lines done so far            */
  LINE *temp;			/* pointer through document     */

  switch(dir)
    {
    case UP:
      for (lcnt = 1, temp = start->head; temp && temp->prev && lcnt <= lines; 
	   lcnt++, temp = temp->prev);
      start->head = temp;
      return(start);
    case DOWN:
      temp = start->head;
      do{
	for (lcnt = 1; temp && lcnt <= lines; lcnt++, temp = temp->next);
	if (lcnt < lines && !temp && front->rest)
	  {
	  temp = front->tail;
	  highlight_query_words();
	  } 
      }while(lcnt < lines);
      if (temp)
	start->head = temp;
      else
	start->head = temp;
      return(start);
    }
  return(start);
}

BASIC *next_doc_key(BASIC *cur, Int_t holddoc)
{
  static BASIC *holdcur = (BASIC *)NULL; /* start and end of current screen      */
  Int_t lcnt = cur->head->num;	/* current line number at top of screen */
  LINE *temp = (LINE *)NULL;	/* pointer through document             */

  if (holdcur)
    memset(holdcur, '\0', sizeof(BASIC));
  else if (!(vector_make_room_for_large_n(&holdcur, 1, sizeof(BASIC))))
    {
    Q_warning("Memory allocation error in next_doc_key().");
    exit(-4);
    }

  /*  memcpy(holdcur, cur, sizeof(BASIC)); */
  holdcur->head = cur->head;
  holdcur->tail = cur->tail;
  holdcur = move(holdcur, 3, DOWN);

  if ((!holdcur->head) || (holdcur->head->num == cur->head->num))
    {
    strcpy((char *)msg_str, "No more keywords to view in current document.");
    return(cur);
    }

  while (front->rest && (!front->keyhead || 
			 (!holddoc && front->keytail->num <= 
			  holdcur->head->num) ||
			 (front->keytail->keynum <= holdcur->head->keynum)))
    highlight_query_words();

  if ( (!front->keyhead) || (!front->keytail) ||
      (!holddoc && front->keytail->num < holdcur->head->num) || 
      front->keytail->keynum < holdcur->head->keynum)
    {
    strcpy((char *)msg_str,"No more keywords to view in current document.");
    return(cur);
    }

  if ((!holddoc && front->keytail->num > holdcur->head->num) ||
      front->keytail->keynum > holdcur->head->keynum)
    {
    for (temp = front->keyhead; temp && ((!holddoc && temp->num < 
					  holdcur->head->num) ||
					 temp->keynum < holdcur->head->keynum);
	 temp = temp->next);
    if (!holddoc)
      lcnt = temp->num - holdcur->head->num;
    else
      lcnt = temp->keynum - holdcur->head->keynum;
    holdcur = move(holdcur, lcnt, DOWN);
    }
  holdcur = move(holdcur, 2, UP);
  cur->head = holdcur->head; 
  return(cur);
}


static BoolInt_t open_out_file()
{
  /*
   *  The file is opened here so that no file is created if the user
   *  doesn't save any documents.
   */
  if (doc_out_file_ptr == (FILE *)NULL)
    {
    doc_out_file_ptr = fopen ((char *)doc_out_file_name, "a");

    if (doc_out_file_ptr == (FILE *)NULL)
      {
      Q_warning ("Can't open file %s for output", doc_out_file_name);
      return (FALSE);
      };
    };
  return(TRUE);
}

/*
 * Parses the global parameter variable "params" into something of the
 * form "number - number" or "number : number". Ignoring all previous
 * string characters.
 */
static BoolInt_t parse(Int_t *first, Int_t *last, Char_t *usage)   
{
  Char_t *str;
  Char_t *token_ptr;

  str = &params[0];
  *first = strtol (str+1, (char **) &token_ptr, 10);
      
  if (token_ptr == (str+1))
    {
    strcpy ((char *)msg_str, (char *)usage);
    return (FALSE);
    };
      
  str = token_ptr;
  while ((*str == ' ') || (*str == '\n') || (*str == '\t'))
    str ++;
      
  if ((*str == '-') || (*str == ':'))
    {
    *last = strtol (str+1, (char **) &token_ptr, 10);
    
    if (token_ptr == (str+1))
      {
      strcpy ((char *)msg_str, (char *)usage);
      return (FALSE);
      }
    } 
  else
    *last = *first;

  return(TRUE);
}

/*
 *  STORE_DOCS_IN_FILE stores one or more documents to a file.  The str is
 *  the user-string indicating which documents to store.  The bl is the
 *  belief list containing the ranked document ids.
 */
static BoolInt_t store_docs_in_file(Char_t *qfirst, disp_type menu)
{

  Int_t  first_doc, last_doc;	/* document numbers start and end */
  LINE *temp = (LINE *)NULL;	/* pointer through document       */
  Int_t  i;			/* loop variable                  */
  Char_t *str;			/* pointer through parameters     */
  Int_t  width = 80;		/* fixed for line printers */
  Char_t usage[]=
    "Usage:  W i [ - j ]    (Note:  1 <= i <= j <= num docs retrieved)";

  /*
   *  Parse the command, responding (relatively) gracefully to user errors.
   */

  switch(menu)
    {
    case sum:
      first_doc = 1;
      last_doc = MIN(maxdocs, bl->doc_cnt);
      break;

    case doc:
				/* remove beginning blanks */
      for (str = &params[1]; *str && isspace(*str); str++);
                 /* if no number entered, use current ranking as default */
      if (!*str)
	first_doc = last_doc = front->rank;
				/* otherwise use the user input rankings */
      else {
	if (!parse(&first_doc, &last_doc, usage))
	  return(FALSE);

	if ((first_doc < 1) || (last_doc  < 1) ||
	    (first_doc > maxdocs) || (last_doc > maxdocs) ||
	    (first_doc > bl->doc_cnt) || (last_doc > bl->doc_cnt) ||
	    (first_doc > last_doc))
	  {
	  strcpy ((char *)msg_str, (char *)usage);
	  return (FALSE);
          };
        }
      break;
    default:
      break;
    }

    if (!open_out_file())
      return(FALSE);
 
  /*
   *  The buffer is flushed for user convenience (they can be immediately 
   *   viewed via an editor in another window, for example).
   */
				/* print separator and query */
  for(i = 0; i < width; i += 2)
    fprintf(doc_out_file_ptr,"- ");
  fprintf (doc_out_file_ptr, "\nQuery: %s\n", (char *) qfirst);
  for (i=first_doc-1; i<last_doc; i++)
    {
    ui_get_doc (db, bl->list[i].doc_id, (menu == doc) ? ALL : HEADING);
    fprintf (doc_out_file_ptr,
	     "DOC:  %ld. %f (Internal Id:  %d) (Source Id:  %s)\n%%",
	     i+1, bl->list[i].belief, (int) bl->list[i].doc_id,
	     (char *) ui_lookup_doc_field (INQ_DOC_FIELD_ID));
    fprintf (doc_out_file_ptr, "TITLE:  %s\n%%", 
	     (char *) ui_lookup_doc_field (INQ_DOC_FIELD_TITLE));
    fprintf (doc_out_file_ptr, "AUTHOR:  %s\n%%", 
	     (char *) ui_lookup_doc_field (INQ_DOC_FIELD_SOURCE));

    /* only print the document body when requested. */
    if (menu == doc)
      {
      fprintf (doc_out_file_ptr, "TEXT:\n"); 

      release_text(doc);
      front->rest = ui_lookup_doc_field (INQ_DOC_FIELD_TEXT);
				/* set up whole document w/o highlighting*/
      while(front->rest)
	set_up_text(front, FALSE, FALSE, width, TRUE);
      
				/* put in output file */
      for (temp = front->head; temp; temp = temp->next)
	fprintf(doc_out_file_ptr, "%s", (char *) temp->text);

      release_text(doc);
      }

    fprintf(doc_out_file_ptr, "\n\n%%");

    };
  fflush (doc_out_file_ptr);
  
  return (TRUE);
} /* end of store_docs_in_file */

static BoolInt_t mark_docs (SUMMARY *cursum, Int_t new, disp_type menu)
{
  Int_t first_doc = 0, last_doc;	/* document numbers to mark */
  Int_t i, j, id;		/* loop variable            */
  LINE *temp = (LINE *)NULL;	/* pointer through summary  */
  Char_t *str, str_id[10];	/* pointer through params   */
  SUM_ENT *sum_ent;
  Char_t **text;
  Char_t usage[]=
    "Usage:  R/N i [ - j ]    (Note:  1 <= i <= j <= num docs retrieved)";

  if (menu == doc)
    {
				/* remove beginning blanks */
      for (str = &params[1]; *str && isspace(*str); str++);
	       /* if no number entered, use current ranking as default */
      if (!*str)
	first_doc = last_doc = front->rank;
    }

  if (first_doc == 0)
    /* make sure the input string is valid */
    if (!parse(&first_doc, &last_doc, usage))
      return(FALSE);

  /* make sure that the given rankings are valid */
  if ((first_doc < 1) || (last_doc  < 1) ||
      (first_doc > maxdocs) || (last_doc > maxdocs) ||
      (first_doc > bl->doc_cnt) || (last_doc > bl->doc_cnt) ||
      (first_doc > last_doc))
    {
    strcpy ((char *)msg_str, (char *)usage);
    return (FALSE);
    };

  for (i = first_doc; i <= last_doc; i++)
    {
    id = bl->list[i-1].doc_id;
    add_rel_doc(id, new);
    sprintf ((char *)str_id, "(%i)", (int) id);

    /* Look for the id in the summary...it can only appear on the first line */
    /* of any summary entry. */
    for(j=0; j < cursum->num_ent; j++)
    {
	sum_ent = cursum->entries[j];
	if (strstr((char *)(sum_ent->text[0]), (char *)str_id))
	    break;
    }
    
	
    if ( j < cursum->num_ent )
      (*(sum_ent)->text)[0] = (new==TRUE)?'+':'-';

    }

  /* refresh the screen to update the markings of relevance */
  if (menu == sum)
    return(refresh);
  
  if (menu == doc && first_doc == last_doc && first_doc == front->rank)
    front->title[0] = (new==TRUE)?'+':'-';
  return(TRUE);
}

Char_t *unhighlight(Char_t *str)
{
  Char_t *s;
  Int_t j = 0;
  Int_t holdlen = strlen((char *)str);	/* original length of input string */

  for (s = str; (s = (Char_t *)strchr((char *)s, ESCCHAR));) {
    strcpy((char *)s, (char *)(s+ESCSIZE));
    j++;			/* number of ESC chars removed */
  }
  if (j > 0)
    str[holdlen-(j*ESCSIZE)] = '\0';
  return(str);
}

Int_t highlight_query_words()
{
    Int_t  holdj=0, j=0, k=0, l=0, oldnum = front->lines; 
    Int_t  curlen;		/* where you are in query array */
    Int_t  length = screen_length(); /* how many lines to process */
    Char_t newline[MAXLEN*MAXPAGE]; /* current line being added to    */
    Char_t newtoken[MAXLEN];	/* next word from the file           */
    Char_t newtoken_work[MAXLEN]; /* tokenized version of the word     */
    Int_t  newtoklen = 0;	/* length of current token           */
    Int_t  numlines = 0;	/* lines completed so far            */
    Char_t savechar[MAXLEN];	/* white space, punct between words  */
    Int_t  savecharlen = 0;	/* length of save characters         */
    Char_t *text = front->rest;	/* pointer to remaining undone text  */
    Int_t doc_offset = front->doc_offset;
    Int_t  tlen;		/* length of remaining text          */
    Int_t  pos=0;		/* length of current line so far     */
    Int_t  wordexist = FALSE;	/* is the current token in the query */
    Int_t  width = screen_width();
    Int_t  maxlen = width;	/* max length current line can be    */
    Int_t  maxchar = width*length+100; /* approx 1 screen of chars. */
    Int_t  holdline;
    match_t *curr_match;

    rewind_match_list(front->matches);
    curr_match = get_next_match(front->matches);

    if (front->tail)
	holdline = front->tail->num;
    else
	holdline = 0;
    memset(newline, '\0', MAXLEN);

    tlen = strlen((char *)text);
    while (j < tlen)
    {

      holdj = j;

      for (k=0;
	   (!ispunct(text[j]) && !isspace(text[j]) && (j < tlen));
	   k++, j++)
	{
	  newtoken[k] = text[j];
	  newtoken_work[k] = tolower(text[j]);
	}
      /* null terminate */
      newtoken[k] = '\0'; 
      newtoken_work[k] = '\0';

      for (k=0; (ispunct(text[j]) || isspace(text[j])) && (j < tlen);
	   k++, j++)
	savechar[k] = text[j];
      savechar[k] = '\0';	/* null terminate */
	  
      /* Find out if this token is a query match by consulting the
       * sorted matchinfo list.
       */
      if (newtoken_work[0] == '\0' || 
	  curr_match == (match_t *)NULL ||
	  (j + doc_offset) < get_match_offset(curr_match))
	wordexist = FALSE;
      else 
	{
	  while( curr_match != (match_t *)NULL &&
		(holdj + doc_offset) > 
		(get_match_offset(curr_match) + 
		 get_match_length(curr_match))
		)
	    {
	      /* The current match is finished, move on to the next */
	      curr_match = get_next_match(front->matches);
	      if( curr_match != (match_t *)NULL &&
		 (holdj + doc_offset) >= 
		 (get_match_offset(curr_match)))
		{
		  /* This is a match! */
		  wordexist = TRUE;
		}
	    }
	}
      
	/* Find out if this token is a query match by consulting the
	 * sorted matchinfo list.
	 */
	if (newtoken_work[0] == '\0' || 
	    curr_match == (match_t *)NULL ||
	    (j + doc_offset) < get_match_offset(curr_match))
	  wordexist = FALSE;
	else 
	  {
	    while( curr_match != (match_t *)NULL &&
		  (holdj + doc_offset) > 
		  (get_match_offset(curr_match) + 
		   get_match_length(curr_match))
		  )
	      {
		/* The current match is finished, move on to the next */
		curr_match = get_next_match(front->matches);
	      }
	    if( curr_match != (match_t *)NULL &&
	       (holdj + doc_offset) >= 
	       (get_match_offset(curr_match)))
	      {
		/* This is a match! */
		wordexist = TRUE;
	      }
	  }
      
      newtoklen = strlen ((char *)newtoken);
      savecharlen = strlen ((char *)savechar);

      /* make sure there are few enough characters to fit on the */
      /* currentline.  If too many, leave off here and format the current */
      /* line. */
      if (newtoklen+savecharlen+pos > maxchar)
	{
	  front->rest = cr_to_sp(newline, TRUE);
	  front = set_up_text(front, FALSE, TRUE, width, TRUE);
	  memset(newline, (char)'\0', MAXLEN*MAXPAGE);

	  /* if still more exists, set up for the next section. */
	  if (front->rest)
	    {
	      strcpy ((char *)newline, (char *)(front->rest));
	      pos = strlen ((char *)newline);
	      newline[pos] = '\0';
	    }
	  else 
	    pos = 0;
	  maxlen = width;
	  /* how many NEW lines got added? */
	  numlines = front->lines-oldnum;
	}
	      

      /* check if the number of lines formatted this time is at least a */
      /* screen length.  If it is, make sure that something was put into the */
      /* last added line.  Assuming something was added, we need to find out */
      /* where to put the front->rest pointer for the rest of the */
      /* information.  Compare the last formatted line with the document */
      /* text and keep what was not used. */
      if (numlines >= length) 
	{  
	  if (newline[0]) {
	    strcpy ((char *)newline, (char *)unhighlight(newline));
	    for(k = (3*(holdj/4)); *(text+k) && (k < (2*holdj)) && 
		(l = strncmp((char *)(text+k), (char *)newline, (size_t)pos)); 
		k++);
	    if (!l)
	      {
		front->rest = text+k;
		front->doc_offset += k;
	      }
	  } else
	    {
	    
	      front->rest = text+holdj;
	      front->doc_offset += holdj;
	    }
	
	  return(1);
	}
	      

      /* if the last token is something, and the word did not exist in the */
      /* query, then just add it in.  If the word was in the query, we must */
      /* highlight the word before putting it out to the document. */
      if (*newtoken)
	{
	  if (!wordexist)
	    {
	      strcpy ((char *)(newline+pos), (char *)newtoken);
	      pos += newtoklen;
	    }
	  else {
	    strcpy ((char *)(newline+pos), (char *)highlight(newtoken, newtoklen));
	    pos += newtoklen + 2*ESCSIZE;
	    maxlen += 2*ESCSIZE;
	  }
	}
      
      /* reset the existence check for the next word, add in all of the */
      /* whitespace and unchecked characters to the line. */
      wordexist = FALSE;  
      if (*savechar)
	{
	  strcpy ((char *)(newline+pos), (char *)savechar);
	  pos += savecharlen;
	}

    }
    
    /* If we are at the end of the text, format the last line, otherwise, */
    /* ignore this and just move on. */
    if (j >= tlen)
    {
	front->rest = newline;
	front = set_up_text(front, FALSE, TRUE, width, FALSE);
	front->rest = (Char_t *)NULL;
    } 
    return(1);

}
