/****************************************************************************\
*	            Copyright (c) 1990-1995 by the                           *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#ifndef _WATCOM
/*
#include <memory.h>
*/
#endif

#include "inq_api.h"
#include "inqmdb_api.h"
#include "inq_err.h"
#include "ui_utils.h"
#include "docmem.h"
#include "inerror.h"

/* should be removed, and maybe use a configuration file, but for   */
/* now, put it in so we can get it into the RCS Library before more */
/* changes are added - RCC 5/5/93                                   */
#if defined(NETWORK) && defined(USE_DECNET)
#define CONN_SERVER_OBJECT_NUMBER "132"
#endif

#if defined(NETWORK) && defined(USE_TCPIP)
#define CONN_SERVER_PORT_NUMBER "1234"
#endif

/*=== Local Definitions ===*/

static char ui_utils_c_rcsid[]="$RCSfile$, $Revision$, $Date$";


static Doc_t *current_doc=(Doc_t *) NULL;	/* Document being displayed. */
static Int_t num_matches = 0;
static matchinfo_t *matches = NULL;
static Int_t memory_usage=0;

/*=== Local Function Definitions ===*/
static void initialize_vars(void);

/*=== Global Definitions ===*/
belieflst *bl = (belieflst *)NULL;	/* used by ui.c and xinquery files. */
dbinfo *db = (dbinfo *)NULL;
void   *session_handle = (void *)NULL;  /* the session information */
location_t locate;

#define DEFAULT_STOPLIST "default.stp"

/* should be removed, and maybe use a configuration file, but for   */
/* now, put it in so we can get it into the RCS Library before more */
/* changes are added - RCC 5/5/93                                   */
/* Please don't use these long #define macros!! WATCOM chokes. - FKL 5/21/93 */
#if defined(NETWORK) 
#if defined(USE_DECNET)
/* Note that USAGE is a printf control string that expects 1 string argument */
#define USAGE "\n\
usage:  %s database [options]\n\
\n\
        database    is the full pathname of the database with server name to search\n\
                    i.e. foobar:[portnum]:/foo/bar/database\n\
\n\
    OPTIONS\n\
\n\
        -h          	  Help - This usage message\n\
        -nostem     	  Queries are already stemmed.  Do not stem (or stop) further.\n\
        -recog       	  Turn ON all query recognizers.\n\
        -norecog    	  Turn OFF all query recognizers (DEFAULT).\n\
        -use_recog  name  Turn ON query recognizers named <name>.\n\
        -skip_recog name  Turn OFF query recognizers named <name>.\n\
        -w file     	  write retrieved docs and scores to file\n\
        -objnum val 	  DECnet Object number (from 128 to 255, default=132)\n"
#endif
#if defined(USE_TCPIP)
#define USAGE "\n\
usage:  %s database [options]\n\
\n\
        database    is the full pathname of the database with server name to search\n\
                    i.e. foobar::/foo/bar/database\n\
\n\
    OPTIONS\n\
\n\
        -h           	  Help - This usage message\n\
        -nostem     	  Queries are already stemmed.  Do not stem (or stop) further.\n\
        -recog       	  Turn ON all query recognizers.\n\
        -norecog	  Turn OFF all query recognizers (DEFAULT).\n\
        -use_recog  name  Turn ON query recognizers named <name>.\n\
        -skip_recog name  Turn OFF query recognizers named <name>.\n\
        -w file      	  write retrieved docs and scores to file\n\
        -o dir       	  specify directory for output files; Default: ./\n"
#endif
#else
#define USAGE "\
usage:  %s <options> [database] [stopwords]\n\
        -h          	  Help - This usage message\n\
        -memory <num>     Maximum memory usage\n\
        -nostem     	  Queries are already stemmed.  Do not \n\
                          stem (or stop) further.\n\
        -recog      	  Turn ON all query recognizers.\n\
        -norecog    	  Turn OFF all query recognizers (DEFAULT).\n\
        -use_recog  name  Turn ON query recognizers named <name>.\n\
        -skip_recog name  Turn OFF query recognizers named <name>.\n\
        -cc_on            Use concurrency control when accessing databases.\n\
        -cc_off           Do not use concurrency control (DEFAULT).\n\
        -w file     	  write retrieved docs and scores to file\n\
        -o dir      	  specify directory for output files; Default: ./\n\
        -dups       	  allow duplicate documents to be retrieved (default)\n\
        -nodups     	  prevent duplicate documents from being retrieved\n\
        -spelling         Use spell checking information \n\
        -top_docs_only    Use the top-docs only optimization (default is off)\n\
        -num_top_docs <int> Max number of top-docs to use (0 = off, \n\
                            default = 1000\n\
        database    	  is the name of the database to search\n\
        stopwords   	  is the name of a file containing stopwords\n\n"
#endif

doc_rel_t rel_docs[MAX_DOCS_JUDGED];
Int_t rel_docs_size;

Char_t logname[128];
static Char_t local_dir[]=local_directory;  /* defined in sys_specific.h */ 
BoolInt_t switch_remove_duplicate_docs=FALSE;

FILE *logfile;
Char_t *newquery = (Char_t *)NULL;	/* pointer to current tokenized query */

/* These are currently only used in ui.c */
Char_t doc_out_file_name[128];
FILE *doc_out_file_ptr=(FILE *)NULL;


/*====================== External Functions ===============================*/

/* SAFE_OPTARG function
 * checks to see if there is another arg available, (don't read beyond array end), 
 * increments the ipointer and returns the next arg
 * ELSE exits with a nice error message.
 */
static char *safe_optarg(Int_t *remaining, char *argv_var[], Int_t *ip_var, char *arg)
{
   if (*remaining)
   {
      --(*remaining);
      ++(*ip_var);
      return(argv_var[*ip_var]);
   }
   else
   {
      Q_error("The %s option requires a following argument.",arg);
      return (char *)NULL; /* for the compiler */
   }
}

/*
 *  PROCESS_COMMANDLINE reads and responds to arguments specified on the
 *  command line.  It returns  a partially-initialized dbinfo data structure.
 */
dbinfo *process_commandline (int argc, char **argv)
  {
     Char_t *arg;
     Int_t args_left;
     
  dbinfo *db;
  Int_t    i;
  Int_t    positional_arg;
  Char_t   *stopdir;
  
  double strtod();		/* stdlib.h on the SUN doesn't declare this */
  
  initialize_vars();

  /*
   *  If the required arguments aren't present, be helpful before quitting.
  if (argc < 2)
    {
    printf (USAGE, argv[0]);
    exit(1);
    }
   */

     db = inq_new_db( NULL);
     

  /*
   *  Fill in defaults for the optional arguments.  This is done before
   *  argument processing, so that the values specified on the command
   *  line will override the defaults.
   */
  stopdir = g_getenv ((Char_t *)"DOCSTOPS"); 
  if (stopdir == ((Char_t *)NULL))
    stopdir = local_dir;
  add_file((Char_t *)(db->stopname), stopdir, (Char_t *)DEFAULT_STOPLIST);

  db->boolean_retrieval =  FALSE;
  *logname = '\0';
  add_file(doc_out_file_name, local_dir, (Char_t *)"docs"); 
  set_extension(doc_out_file_name, (Char_t *)"out");
  
  
  /*
   *  Process command line arguments.  Switches can occur anywhere on the
   *  command line, in any order.  (Let's be user-friendly.)
   */

  for (i=1, positional_arg=0, args_left=argc-i-1; i<argc; i++,args_left--)
  {
     Char_t *hostname_p;
     Char_t *portnum_p;
     Char_t *database_p;
     int tmp_length;

     arg=(Char_t *)argv[i];
     if (arg[0] == '-')
     {
	if (!strcmp ((char *)"-use_recog", (char *)arg))
	{
	   arg = (Char_t *)safe_optarg(&args_left,argv,&i,(char *)arg);
#if !defined(NETWORK)
	   if (init_qtransformer((char *)arg, 1))
	      Q_error(">>Transformer initialization failed.  Exiting");
	}
	else if (!strcmp ("-skip_recog", (char *)arg))
	{
	   arg = (Char_t *)safe_optarg(&args_left,argv,&i,(char *)arg);
	   if (init_qtransformer((char *)arg, 0))
	      Q_error(">>Transformer initialization failed.  Exiting");
	}
	else if (!strcmp ("-recog", (char *)arg))
	   init_all_qtransformers(1);
	else if (!strcmp ("-norecog", (char *)arg))
	   init_all_qtransformers(0);
	else if (!strcmp (arg, "-top_docs_only"))
	    inq_set_top_docs_only(db,TRUE);
	else if (!strcmp (arg, "-num_top_docs"))
	  {
	    /* get the number of top docs to use */
	    int num_top_docs;

	    num_top_docs = (Int_t)atoi(safe_optarg(&args_left,argv,&i,
						    (char *)arg));
	    inq_set_num_top_docs(db,num_top_docs);
	  }
	else if (!strcmp ("-spelling", (char *)arg))
	  inq_set_spell_check_flag(db, true);
	else if (!strcmp ("-qbs", (char *)arg))
	  inq_set_db_qbs_flag(db, TRUE);
	else if (!strcmp ("-noqbs", (char *)arg))
	  inq_set_db_qbs_flag(db, FALSE);
	else if (!strcmp ((char *)arg, "-nostem"))
	{
	   init_all_qtransformers(0);
	   set_query_stemming(FALSE);
#endif
	}
	else if(!strcmp((char *)arg, "-memory"))
	  memory_usage = atoi(safe_optarg(&args_left, argv, &i, (char *)arg));

	else
	   switch (arg[1])
	   {
	   case 'b':
	      if (arg[2] == 'e')
	      {
		 db->bel = (Float_t)strtod (safe_optarg(&args_left,argv,&i,(char *)arg), (char **)NULL);
	      }
	      else
		 if (arg[2] == 'o')
		    db->boolean_retrieval = true;
		 else
		    printf ("Switch %s not recognized.\n", (char *) arg);
	      break;
	      
	   case 'c':
	      if (!strcmp ((char *)arg, "-cc_on"))
		  inq_set_db_concurrency(db, TRUE);
	      else if (!strcmp ((char *)arg, "-cc_off"))
		  inq_set_db_concurrency(db, FALSE);
	      else
		  printf ("Switch %s not recognized.\n", (char *) arg);
	      break;
	      
	   case 'd':
	      if (!strcmp ((char *)arg, "-dups"))
		 switch_remove_duplicate_docs = FALSE;
	      else
		 printf ("Switch %s not recognized.\n", (char *) arg);
	      break;
	      
	   case 'h':
	      printf (USAGE, argv[0]);
	      exit(1);
	      break;
	      
	   case 'n':
	      if (!strcmp ((char *)arg, "-nodups"))
		 switch_remove_duplicate_docs = TRUE;
	      else
		 printf ("Switch %s not recognized.\n", (char *) arg);
	      break;
	      
     /* should be removed, and maybe use a configuration file, but for   */
     /* now, put it in so we can get it into the RCS Library before more */
     /* changes are added - RCC 5/5/93                                   */
#if defined(NETWORK) && defined(USE_DECNET)
	   case 'o':
	     if (!strcmp ((char *)arg, "-objnum")) 
	     {
	       /* need to assign db->conn_id to this argument - RCC 12/4/95 */
		if ((atoi(safe_optarg(&args_left,argv,&i,(char *)arg)) < 128) || 
		    (atoi(safe_optarg(&args_left,argv,&i,(char *)arg)) > 255))
		{
		   Q_error("DECnet Object number must be in the range 128-255");
		}
	     }
#endif
	     
#if defined(NETWORK) && defined(USE_TCPIP)
	  case 'p':
	     if (!strcmp ((char *)arg, "-portnum")) 
	     {
	       int tmp_size = sizeof(Char_t) * 5;
	       assert(tmp_size>0);
	       assert(strlen(safe_optarg(&args_left,argv,&i,(char *)arg)) <= (tmp_size - 1));
	       if ((db->conn_id = (Char_t *)malloc(tmp_size)) == (Char_t *)NULL)
		 Q_error("Can't Allocate %d bytes to store Connection ID", tmp_size);
	       strncpy(db->conn_id,safe_optarg(&args_left,argv,&i,(char *)arg), tmp_size - 1);
	       db->conn_id[tmp_size]='\0';
	     }
	     break;
#endif
	  case 't':
	      if( (arg[2] == '\0') || (!strcmp((char *)arg, "-tf")))
	      {
		  
		  db->tf = (Float_t)strtod (safe_optarg(&args_left,argv,&i,(char *)arg), (char **)NULL);
	      } else
		  printf ("Switch %s not recognized.\n", (char *) arg);
	     break;
	     
	  case 'w':
	     strcpy ((char *)logname, safe_optarg(&args_left,argv,&i,(char *)arg));
	     if (*logname) 
	     {
		logfile = fopen((char *)logname, "w");
		if (logfile == (FILE *)NULL) 
		{
		   printf("***Couldn't open %s ***\n", (char *) logname);
		   exit(1);
		}   
	     }
	     break;
	     
	  default:
	     printf ("Switch %s not recognized.\n", (char *) arg);
	     break;
	  }
    }
    else
       switch (positional_arg++)
       {
       case 0:
#ifdef NETWORK
          hostname_p = arg;
          portnum_p = (Char_t *)strchr((char *)arg, ':');
          if (portnum_p == (Char_t *)NULL)
             {
               Q_warning("Database syntax incorrect (%s)\n, must be \"hostname:conn_id:database\"", arg);
               fprintf(stderr, USAGE, argv[0]);
               return((dbinfo *)NULL);
             }

          tmp_length = portnum_p - arg;
          hostname_p[tmp_length] = '\0';        
          db->hostname = (Char_t *)malloc(strlen(hostname_p) + 1);
	  strcpy((char *)db->hostname, hostname_p);
          portnum_p++;
          
          database_p = (Char_t *)strchr((char *)portnum_p, ':');
	  if(database_p)
	    tmp_length = database_p - portnum_p;
	  else
	    tmp_length = strlen((char *)portnum_p);
#ifdef USE_DECNET
	  if (tmp_length > 0)
	  {
	    if ((atoi((char *)portnum_p) < 128) || 
		(atoi((char *)portnum_p) > 255)){
	      Q_warning("DECnet Object number must be in the range 128-255");
	      fprintf(stderr, USAGE, argv[0]);
	      return((dbinfo *)NULL);
	  }
#endif
          if (tmp_length == 0)
          {
#ifdef USE_DECNET

	    tmp_length = strlen(CONN_SERVER_OBJECT_NUMBER);
	    portnum_p=CONN_SERVER_OBJECT_NUMBER;
#endif
#ifdef USE_TCPIP
	    tmp_length = strlen(CONN_SERVER_PORT_NUMBER);
	    portnum_p=CONN_SERVER_PORT_NUMBER;
#endif
	  }
	  if ((db->conn_id = (Char_t *)malloc((size_t)((sizeof (Char_t) * tmp_length) + 1))) ==
	      (Char_t *)NULL)
	    Q_error("Can't Allocate %d bytes to store the Connection ID", 
		    (sizeof(Char_t) * tmp_length) +1);

	  strncpy ((char *)db->conn_id, (char *)portnum_p, (size_t)tmp_length);
	  db->conn_id[tmp_length] = '\0';



          if(database_p && (strlen((char *)database_p) > 1))
            {
              database_p++;
              strcpy(db->name, (char *)database_p);
            }
#else
	  strcpy(db->name, (char *)arg);
#endif
	  break;
       case 1:
	  strcpy (db->stopname, (char *)arg);
	  break;
       default:
	  printf ("Argument %s not recognized.\n", (char *) arg);
	  break;
       };
 }
     
     locate.hostname = db->hostname;
     locate.connection_id = db->conn_id;
     locate.protocol = -1;

  if(inq_init_system(&session_handle, &locate, NULL) == INQ_SUCCESS)
  {
     if(memory_usage > 0){
       if(inq_mdb_set_system_resource(session_handle, Q_BUFFER_QRY_MGR, memory_usage) != INQ_SUCCESS){
	 inq_term_system(&session_handle, NULL);
	 return((dbinfo *)NULL);
       }
     }
    if (db->name[0])
    {
	if( inq_opendb(db) == FALSE)
	{
	    fprintf(stderr, "Failed to open database %s.\n", db->name);
	    return((dbinfo *)NULL);
	}
	
    }
    return (db);
  }

  inq_free_dbinfo(&db);
  printf(USAGE, argv[0]);
  return((dbinfo *)NULL);

} /* end of process_commandline */

/*
 *  REMOVE_DUPLICATE_DOCS tries to identify and remove duplicate documents
 *  from a belief list.  This function can remove duplicate documents from
 *  the entire list, or can remove just enough duplicates to ensure that the
 *  top n documents are unique.  The minimum_unique parameter determines how
 *  much of the list is checked.  The ability to focus on making the first n
 *  documents unique is important because checking for duplicates is expensive
 *  (in terms of I/O, and therefore time).
 */
void remove_duplicate_docs (Int_t minimum_unique)
{

  register Int32_t i,j;
  register Int32_t num_unique;

  UChar_t *doc_i_id=(UChar_t *)NULL;
  UChar_t *doc_i_title=(UChar_t *)NULL;
  UChar_t *doc_i_line1=(UChar_t *)NULL;

  /*
   *  Make sure the arguments makes sense.
   */

  if (minimum_unique > bl->doc_cnt)
    minimum_unique = bl->doc_cnt;

  /*
   *  Start by marking the duplicate documents.  The i'th document is
   *  always unique.  The j'th document may or may not be unique.  Once
   *  a document is known to be a duplicate, it is "marked" by having its
   *  belief set to 0.0.  The only documents that are even potentially
   *  duplicates are those with identical belief values.  Note that unique
   *  documents do not necessarily occur adjacent in the belief list.
   *  Several documents may have the same belief value (especially for
   *  short queries), so duplicates could be separated by one or more
   *  documents.
   */
  num_unique = 0;

  for (i=0; i<bl->doc_cnt; i++)
    if (bl->list[i].belief > 0.0)
      {
      num_unique ++;

      if (num_unique >= minimum_unique)
	break;

      for (j=i+1; j<bl->doc_cnt; j++)
	if (bl->list[j].belief > 0.0)
	  if (bl->list[i].belief != bl->list[j].belief)
	    break;
	  else
	    {
	    if (i == (j-1))
	      {
	      ui_get_doc (db, bl->list[i].doc_id, ALL);

	      vector_make_room_for_n
		(&doc_i_id,
		 (strlen ((char *)ui_lookup_doc_field (INQ_DOC_FIELD_ID)) + 1),
		 sizeof(Char_t));
	      vector_make_room_for_n
		(&doc_i_title,
		 (strlen ((char *)ui_lookup_doc_field (INQ_DOC_FIELD_TITLE)) + 1),
		 sizeof(Char_t));
	      vector_make_room_for_n (&doc_i_line1, 512, sizeof(Char_t));

	      strcpy  ((char *)doc_i_id, (char *)ui_lookup_doc_field (INQ_DOC_FIELD_ID));
	      strcpy  ((char *)doc_i_title, (char *)ui_lookup_doc_field (INQ_DOC_FIELD_TITLE));
	      strncpy ((char *)doc_i_line1, 
                       (char *)ui_lookup_doc_field (INQ_DOC_FIELD_TEXT), 
                       (size_t)512);
	      }

	    ui_get_doc (db, bl->list[j].doc_id, ALL);
	
	    /*
	     *  This is a heuristic test.  We  trust the source document ids.
	     *  Titles are unreliable, because they are sometimes the name of
	     *  regular columns (e.g. "Heard on the Street").  However, if the
	     *  title matches *and* the first line or so of the text matches,
	     *  then it is probably a duplicate.
	     */
	    if (!strcmp   ((char *)doc_i_id,
			   (char *)ui_lookup_doc_field (INQ_DOC_FIELD_ID)) ||
		(! strcmp  ((char *)doc_i_title,
			    (char *)ui_lookup_doc_field (INQ_DOC_FIELD_TITLE)) &&
		 ! strncmp ((char *)doc_i_line1,
			    (char *)ui_lookup_doc_field (INQ_DOC_FIELD_TEXT),  512)))
	      bl->list[j].belief = (Float_t)0.0;
	    }
      }

  /*
   *  Remove the duplicate entries from the list.  This is done last, so
   *  that the list is only copied once (because it can be quite long).
   *  The loop is optimized as much as possible, because belief lists can
   *  be quite long.
   */
  if (i >= num_unique)
    {

    /*
     *  Fast-forward to the first "marked" entry.
     */
    for (i=0; bl->list[i].belief > 0.0; i++)
      ;

    /*
     *  Remove the duplicates by "promoting" the unique entries.  This
     *  loop could do the entire list, but the "if" is useless after the
     *  last "marked" duplicate is seen.
     */
    for (j=i+1; j<bl->doc_cnt; j++)
      if (bl->list[j].belief > 0.0)
	{
	bl->list[i].belief = bl->list[j].belief;
	bl->list[i].doc_id = bl->list[j].doc_id;
	i++;

	if (i >= num_unique)
	  break;
	}

    /*
     *  Promote the rest of the documents.
     */
    for (j=j+1; j<bl->doc_cnt; j++,i++)
      {
      bl->list[i].belief = bl->list[j].belief;
      bl->list[i].doc_id = bl->list[j].doc_id;
      }

    bl->doc_cnt = (Int_t)i;
    }

  /*
   *  Free the dynamic memory occupied by the character buffers.
   */
  vector_free ((void **) &doc_i_id);
  vector_free ((void **) &doc_i_title);
  vector_free ((void **) &doc_i_line1);

  } /* end of remove_duplicate_docs */

/* BLANK checks to make sure that alphanumeric characters exists in */
/* the query before accepting it. This ensures a non-blank query will */
/* be refused. */
Int_t blank(Char_t *s)
{
  Int_t all_blank = TRUE;	/* assume no alphanum chars to start */
    
  while (*s && all_blank)
    all_blank &= !isalnum(*s++);
  return(all_blank);
}

/* TOKENIZE_QUERY takes a string and turns it into tokens the */
/* highlight_query_words() function can check against.  The tokenized */
/* version is saved in a file-global vector with nulls as word */
/* separators, and the number of words saved is returned. */
Int_t tokenize_query(Char_t *qstring)
{
    if( (!qstring) || (qstring[0] == '\0') )
	return 0;
    
    if (!(vector_make_room_for_large_n(&newquery, strlen ((char *)qstring) + 1,
					 sizeof(Char_t))))
    {
	Q_warning("Not enough memory to save query.");
	return(0);
    }
    strcpy((char *)newquery, (char *)qstring);
    return(1);
}

/* RMV_BLANKS makes all multiple blanks into single blanks.  This is */
/* useful in the summaries of the documents. It is not used in the */
/* documents themselves because of tables and soforth. */
Char_t *rmv_blanks(Char_t *s)
{
    static Char_t *buff = (Char_t *)NULL;  /* buffer to return */
    Int_t  did = 0;		           /* flag for if a space was kept last char */
    Int_t  length = strlen((char *)s);	   /* length of s */
    Int_t  sp = 0;		           /* position in s */
    Char_t *tp;			           /* pointer through buff */

    /* remove old buffer */
    if (buff)
      vector_free((void **)&buff);
    if (!vector_make_room_for_n(&buff, (length+1), sizeof(Char_t)))
      {
      Q_warning("Memory allocation error in rmv_blanks()");
      exit(-4);
      }
    memset((void *)buff, (int)'\0', (size_t)(length+1));
    tp = buff;
				/* while still characters in s */
    while (sp<length) 
      {
      if(s[sp]==' ') 		/* if it is a space, */
	{
	if(did == 0) 		/* and none has been done yet, */
	  {
	  did = 1;		/* keep it */
	  *tp = ' ';
	  tp++;
	  sp++; 
	  } 
	else 
	  sp++;			/* otherwise skip it */
        } 
      else 			/* if not a space, keep it */
	{
	did = 0;
	*tp = s[sp]; 
	tp++;
	sp++; 
        }
      }
    buff[length] = '\0';	/* null terminate the string */
    return(buff);		/* before return */
}

/*
 *  UI_GET_DOC converts from the old way of loading the document buffer
 *  (supported by get_doc) to the new way (supported by inq_get_doc).
 */
void ui_get_doc (dbinfo *db, Int_t doc_id, Int_t mode)
{
    Char_t *tmpqry;

    if (current_doc)
	inq_free_doc (current_doc);


    tmpqry = inq_get_parsed_query(db, newquery);
    current_doc = inq_get_doc (db, doc_id, mode, tmpqry);
    inq_free_parsed_query(tmpqry);
      


}				/* end of ui_get_doc */


/*
 *  UI_LOOKUP_DOC_FIELD converts from the old way of looking up document
 *  fields (supported by did, dsource, dtext, and dtitle) to the new
 *  way (supported by inq_get_doc_field).
 */
Char_t *ui_lookup_doc_field (Int_t field)
  {

  Char_t *field_text = inq_get_doc_field (current_doc, field, 
					  &num_matches, &matches);

  if (num_matches > 0)
  {
      sort_matches_by_position(matches);
  }

  if (field_text)
    if (strlen ((char *)field_text) > 0)
      return (field_text);

  switch (field)
    {
    case INQ_DOC_FIELD_ID:
      field_text = (Char_t *)"id missing.";
      break;

    case INQ_DOC_FIELD_SOURCE:
      field_text = (Char_t *)"source missing.";
      break;

    case INQ_DOC_FIELD_TEXT:
      field_text = (Char_t *)"text missing.";
      break;

    case INQ_DOC_FIELD_TITLE:
      field_text = (Char_t *)"title missing.";
      break;

    default:
      field_text = (Char_t *)"field missing.";
      break;
    };

  return (field_text);
  } /* end of ui_lookup_doc_field */

/* UI_GET_DOC_NUMMATCHES
 *
 * Return the number of query-word matches found in the current
 * document.
 */
Int_t
ui_get_doc_nummatches(void)
{
    return(num_matches);
}

/* UI_GET_DOC_MATCHES
 *
 * Return the query-word matches found in the current
 * document.
 */
matchinfo_t *
ui_get_doc_matches(void)
{
    return(matches);
}


/* CR_TO_SP removes all "\n"'s and replaces them with spaces to make */
/* formatting a lot easier.  Returns the new string. */
Char_t *cr_to_sp(Char_t *s, Int_t check_space)
{
    static Char_t *buff = (Char_t *)NULL;  /* buffer to return */
    Int_t  length = strlen ((char *)s);
    Char_t *p;			           /* pointer for going through s */

    if (buff)
      vector_free((void **)&buff);
    if (!vector_make_room_for_n(&buff, (length+1), sizeof(Char_t)))
      {
      Q_warning("Memory allocation error in cr_to_sp()");
      exit(-4);
      }
    memset((void *)buff, (int)'\0', (size_t)(length+1));
    strcpy ((char *)buff, (char *)s);
    p = buff;

    while ((p = (Char_t *)strchr ((char *)p, '\n')))
      if (!check_space || (check_space && ((*(p+1) == '\n') || 
					   !isspace(*(p+1)) || (!*(p-1)))))
	*p = ' ';
      else p++;

    if (!check_space)
      if (p && *(p+1) == '\0')	/* put back the last one */
        *p = '\n';

    return(buff);
}    

Boolean_t change_dbs(Char_t *new_db_name)
{
  extern void error_msg(Char_t *str);
  Char_t str[MAXLEN];
  dbinfo *newdb;

  /* if ((check_db(new_db_name) == TRUE) && */
  if(strcmp ((char *)(db->name), (char *)new_db_name) != 0)
  {
    if (db->name[0])
      {
      sprintf ((char *)str, "Closing database: %s\n", db->name);
      error_msg(str);
      inq_closedb(db);

      /* get a new db structure */
      newdb = inq_new_db(new_db_name);

      /* Copy user preferences forward */
      newdb->spell_check_flag = db->spell_check_flag;
      newdb->highlighting_fix = db->highlighting_fix;
      newdb->boolean_retrieval = db->boolean_retrieval;

      /*  Move location information forward */
      newdb->hostname = strdup1(db->hostname);
      newdb->conn_id = strdup1(db->conn_id);

      /* Copy comm_link forward */
      newdb->comm_link = db->comm_link;

      inq_free_dbinfo(&db);
      db = newdb;
      }
    strcpy ((char *)db->name, (char *)new_db_name);
    sprintf ((char *)str, "Opening database: %s\n", db->name);
    error_msg(str);
    if(inq_opendb(db)== FALSE)
    {
	sprintf((char *)str, "Failed to open database %s.\n", db->name);
	error_msg(str);
	return(FALSE);
    }
  } 
  else {
    sprintf ((char *)str, "Requested database already open.\nCurrent \
database: %s\n", db->name);
    error_msg(str);
    return(FALSE);
  }

  return(TRUE);			/* everything worked out ok. */
}

/* Check to see if the database desired can really be used. */
Int_t check_db(Char_t *new_db)
{
  Char_t lookup[200];
  FILE *temp;

  memset ((void *)lookup, (int)'\0', sizeof(Char_t)*200);
  sprintf ((char *)lookup, "%s%s", (char *) new_db, Q_DOTINF); 
  
/* omit this check when the PC is server, it won't work */
   /* (should we be doing this at all ??) */

  if(lookup[1] == ':')
    return(TRUE);

  if ((temp = fopen ((char *)lookup, "r")))
    {
    fclose(temp);
    return(TRUE);
    }
  Q_warning("Couldn't open the database (inf file \"%s\")",lookup);
  return(FALSE);
}

/*====================== Relevance Functions ==============================*/
/* Add a document id to the list of relevant documents.  Then sort the list. */
/* Return FALSE if the document fails to be added.  TRUE otherwise. */
Int_t add_rel_doc(Int_t doc_id, Int_t relevant)
{
  Int_t i,j;

  /* found in current list */
  if (find_rel_doc(doc_id) > -1)
    return(FALSE);

  if ((++rel_docs_size) == MAX_DOCS_JUDGED)
    {
    rel_docs_size--;
    return(FALSE);
    }
  for(i=0;(i<rel_docs_size-1) && (rel_docs[i].id < doc_id);i++);
  for(j=rel_docs_size-1;(j>0) && (j>i);j--)
    {
    rel_docs[j].id = rel_docs[j-1].id;
    rel_docs[j].relevant = rel_docs[j-1].relevant;
    }
  rel_docs[i].id = doc_id;
  rel_docs[i].relevant = relevant;
  return(TRUE);
}

/* remove the relevant document from the list, leave the space and */
/* resort the list so that the valid docid's are in the front. */
Int_t remove_rel_doc(Int_t doc_id)
{
  Int_t i;
  
  if ((i = find_rel_doc(doc_id)) > -1)
    {
    for(;i<rel_docs_size-1;i++)
      {
      rel_docs[i].id = rel_docs[i+1].id;
      rel_docs[i].relevant = rel_docs[i+1].relevant;
      }
    rel_docs[rel_docs_size-1].id = 0;
    rel_docs_size--;
    return(1);
    }

  /* not found */
  return(0);
}

/* Do a binary search of the relevant document list for the given id. */
/* Return entry if found, -1 otherwise.  THE LIST MUST BE SORTED BY DOCID. */
Int_t find_rel_doc(Int_t doc_id)
{
  Int_t mid, high=rel_docs_size-1, low=0;

  while(low<=high)
    {
    mid = low+(high-low)/2;
    if (rel_docs[mid].id == doc_id)
      return(mid);
    else if (rel_docs[mid].id < doc_id)
      low = ++mid;
    else high = --mid;
    }
  return(-1);
}

/*====================== Internal Functions ===============================*/

static void initialize_vars()
{
  memset((void *)rel_docs, (int)'\0', sizeof(doc_rel_t)*MAX_DOCS_JUDGED);
  rel_docs_size=0;
}
