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
/* ............................................................................
module name: ui.c     user interface routines for inquery

Modifications:  
   5/12/93 - created this file to reduce ui.c size and make the functions
     AEG     easier to understand.  Also formats the summary one screen at a
             time, rather than all at once.

............................................................................ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "inerror.h"
#include "docmem.h"
#include "inq_api.h"
#include "summary.h"
#include "ui_utils.h"
#include "ui.h"


static Char_t summary_c_rcsid[]="$RCSfile$, $Revision$, $Date$";


#define MAXLINES (4)		/* maximum lines per author and title */
#define SUMRY_MENU  "page Forward/Back  Line Up/Down  Top/End  Write  #  Query  ?  Refresh  eXit"

/* ADJUST_CUR_SUM_ENT is used to move around a pointer inside the summary */
/* structure. Given a current pointer, a direction and a number of lines to */
/* move in that direction, the pointer will be moved as far as possible */
/* within the summary structure and then returned. */
cur_sum_ent adjust_cur_sum_ent(SUMMARY *cursum, cur_sum_ent current, 
			 direction dir, Int_t lines)
{
  Int_t temp=0;
  Char_t **text=(Char_t **)NULL;
  Int_t hold=lines;

  if ((!cursum) || (!cursum->entries) || (lines <= 0))
    return(current);

  temp = current.entry;
  text = current.line;

  /* We are at the end(top) of the summary...can't go anywhere */
  if (((temp >= cursum->num_ent) && (dir==DOWN))
      || ((temp == 0) && (text==(cursum->entries[0])->text) && (dir == UP)))
    return(current);

  for(;lines>0;lines--)
    {
    if (dir == DOWN) 
      {
      /* If we are not at the end of this entry...go on. */
      if (*text) 
	{
	/* If the next entry is the last one...don't count it as a line. */
	if (!*++text) 
	  lines++;
	/* Otherwise, try to go to the next one. */
      } else
	if (++temp < cursum->num_ent)
	  text = (cursum->entries[temp])->text;
	else {
	  cursum = set_summary(db, (Char_t *)NULL, bl, cursum);
	  if (temp >= cursum->num_ent)
	    break;
	  lines++;		/* this run through doesn't count */
	  temp--;		/* last time it was wrong... */
	  }
    } else /* dir==UP */ {
      /* If we are not at the top of this entry...go up one line. */
      if ((cursum->entries[temp])->text != text)
	text--;
      /* Otherwise, if there is another entry above this one, go to it... */
      else if (temp > 0)
	/* Now find the last real line in this entry. The last one is a */
	/* NULL, to know we are there.  Therefore, go back one more and */
	/* you're where you want to be.  :)  */
	{
	temp--;
	for(text = (cursum->entries[temp])->text; *text; text++);
	text--;
      } else 
	break;
      }
    }

  current.entry = temp;
  current.line = text;
  return(current);
}  

/* DRAW_SUM_ENTRIES is designed to allow the current frame of the visible */
/* summary to be seen or updated. */
cur_sum_ent draw_sum_entries(SUMMARY *cursum, cur_sum_ent current, 
			     Int_t lines) 
{
  if ((!cursum) || (!cursum->entries))
    return(current);

  for(;lines>0;lines--)
    {
    if ((current.entry < cursum->num_ent) && (*current.line))
      printf("%s", *current.line);    
    else break;
    current = adjust_cur_sum_ent(cursum, current, DOWN, 1);
    }

  for(;lines>0;lines--)
    printf("\n");

  return(current);
}

/* ADD_TO_SUM_ENT will make space for a new entry if the space does not */
/* exist.  If a line of text to be added is given, then the attempt is made */
/* to add the needed space and copy in the given text. */
SUM_ENT *add_to_sum_ent(SUM_ENT *cur, Char_t *text)
{
  Int_t textlen, count;
  Char_t **temp;

  if (!cur)
    {
    if (!(vector_make_room_for_n(&cur, 1, sizeof(SUM_ENT))))
      {
      Q_error("Memory allocation error (file %s, line %d)", __FILE__, __LINE__); 
      return((SUM_ENT *)NULL);
      } 
    memset((void*)cur, '\0', sizeof(SUM_ENT));
    }

  /* make room for the line... */
  if (!(vector_make_room_for_n(&cur->text, (cur->num_lines+1), sizeof(Char_t *))))
    {
    Q_error("Memory allocation error (file %s, line %d)", __FILE__, __LINE__); 
    return(cur);
    } 
  for(temp=cur->text,count=0;count<cur->num_lines;count++, temp++);
  *temp=(Char_t *)NULL;
  cur->num_lines++;

  if (text)
    {
    textlen=strlen ((char *)text)+1;
    /* and more room for the text in the line... */
    if (!(vector_make_room_for_n(temp, textlen, sizeof(Char_t))))
      {
      Q_error("Memory allocation error (file %s, line %d)", __FILE__, __LINE__); 
      return(cur);
      }

    memset((void*)*temp, '\0', (size_t)textlen);
    strcpy ((char *)*temp, (char *)text);
    }

  return(cur);
}

/* ADD_TO_SUMMARY will check to see if a summary currently exists.  If not, */
/* it will make one to fill in.  If an entry is given, then it will attempt */
/* to add the entry to the current summary.  The return is the new summary. */
SUMMARY *add_to_summary(SUMMARY *cursum, SUM_ENT *new_entry, Char_t *text)

{
  Int_t qlen=(text)?strlen ((char *)text)+1:0;
  Int_t count;
  SUM_ENT **temp;

  if (!cursum) 
    {
    if (!(vector_make_room_for_n(&cursum, 1, sizeof(SUMMARY))))
      {
      Q_error("Memory allocation error (file %s, line %d)", __FILE__, __LINE__); 
      return((SUMMARY *)NULL);
      }
    memset((void*)cursum, '\0', sizeof(SUMMARY));
    }
  
  if (!(vector_make_room_for_n(&cursum->entries, (cursum->num_ent+1),
			       sizeof(SUM_ENT *))))
    {
    Q_error("Memory allocation error (file %s, line %d)", __FILE__, __LINE__);
    return(cursum);
    }
  for(temp=cursum->entries,count=0;count<cursum->num_ent;count++, temp++);
  if(new_entry) 
    *temp=new_entry;
  else (*temp)=(SUM_ENT *)NULL;
  cursum->num_ent++;

  if (text)
    {
    if ((!cursum->query) && (!(vector_make_room_for_n(&cursum->query,
						     qlen, sizeof(Char_t)))))
      {
      Q_error("Memory allocation error (file %s, line %d)", __FILE__,
	    __LINE__);
      return(cursum);
      }
    memset((void*)cursum->query, '\0', (size_t)qlen);
    strcpy ((char *)(cursum->query), (char *)text);
    }

  return(cursum);
}

/* RELEASE_SUMMARY goes systematically through the structures and releases */
/* the associated memory. */
void release_summary(SUMMARY **current)
{
    SUM_ENT **temp=(SUM_ENT **)NULL;
    Char_t **text=(Char_t **)NULL; 
    Int_t i;
  
    if (current && *current && ((*current)->num_ent > 0))
    {
	for (i=1,temp=(*current)->entries; i<= (*current)->num_ent; i++)
	{
	    if (temp && *temp) 
	    {
		for(text=(*temp)->text; text && *text; text++) 
		{
		    vector_free((void **)text);
		}
		vector_free((void **)&((*temp)->text));
		vector_free((void **)temp);
	     
	    }
	    if (i<(*current)->num_ent)
		temp++;
	}
	vector_free((void **)&(*current)->entries);
	vector_free((void **)&(*current)->query);
	vector_free((void **)current);
    }

    current = (SUMMARY **)NULL;
}

static Char_t *skip_blanks(Char_t *s) 
{     
  for(;s && (*s==' ');s++);
  return(s);
}

/* DRAW_SUMMARY is used to display the summary structure on the screen.  As */
/* many of the elements are displayed as possible.  The pointer to the end */
/* of the screen is returned to the calling procedure so it can keep track */
/* of the current position. */
cur_sum_ent draw_summary(SUMMARY *cursum, Char_t *topline,  
			 Int_t width, cur_sum_ent current)
{
  CLEAR;
  printf("%s\n", highlight(topline, width));
  current = draw_sum_entries(cursum, current, screen_length()-3);
  printf ("%s\n", highlight((Char_t *)SUMRY_MENU, width));
  
  if (*msg_str)
    {
    BEEP;
    printf("%s", msg_str);
    memset ((void *)msg_str, (int)'\0', (size_t)strlen ((char *)msg_str));
    }
  return(current);
}

SUMMARY *set_summary(dbinfo *db, Char_t *query, belieflst *bl, SUMMARY *cursum)
{
  Int_t  entries;		/* Number of documents being summarized */
  Int_t  i, j, ind;		/* loop control variable                */
  Int_t  new=0;			/* new counting method                   */

  /* These variables used in for loop only */
  Int_t  relevant;		/* check relevance marking of doc       */
  Int_t  count;			/* maxlines per author and title each   */
  Char_t *str;			/* pointer to middle of current line    */
  Char_t s[MAXLEN];		/* holds temporary line until finished  */
  Char_t *curr_id;              /* Current doc id string */
  int     id_len;
  Char_t *text=(Char_t *)NULL, *tmp; /* points to author/title of document   */
  Int_t  width = screen_width(); /* how wide is the current screen?     */
  Int_t  tmplen;		/* length of the current text line      */
  Int_t  per_scr = 1+((screen_length()-3)/4); /* how many entries can fit on */
					    /* the screen at most. */

  /* The following has been added to put document information lookup down a */
  /* level. */
  Char_t **all_summary;		/* information about every document */
  Int_t curfield;		/* the current field (0-num_fields) for the */
				/* current document id. */
  static Int_t *doc_ids=(Int_t *)NULL;	/* a complete list of the ids to use. */
  static Int_t num_ids=0;	/* number of doc_ids used last time */
  Int_t num_fields=3;		/* how many fields in the following */
				/* structure? */
  Int_t fields[3] = {		/* which fields to lookup for each doc id */
    INQ_DOC_FIELD_ID, 
    INQ_DOC_FIELD_TITLE,
    INQ_DOC_FIELD_SOURCE, 
  };
  SUM_ENT *current_sum_entry=(SUM_ENT *)NULL; /* fills in the summary */

  Char_t lmargin[MAXINDENT];	/* filled with spaces */
  Int_t indent=5;		/* how far to indent each line (not first) */

  if (bl->doc_cnt < 1)		/* were any found? */
    return(0);
				/*  do one screen at most each time. */
  if (cursum)
    {
    entries = MIN(per_scr+cursum->num_ent, MIN(maxdocs, bl->doc_cnt));
    /* check to see if we are already done. */
    if (entries <= cursum->num_ent)
      return(cursum);
  } else
    entries = MIN(per_scr, MIN(maxdocs, bl->doc_cnt));

  /* set up the indentation of the lines -- currently at 5 spaces */
  memset((void*)lmargin, '\0', (size_t)MAXINDENT);
  memset((void*)lmargin, ' ', (size_t)indent);

  /* (return this only the first time) were less requested than found? */
  if (!cursum && (maxdocs > bl->doc_cnt))
    sprintf ((char *)msg_str, "Only %d documents returned. ", bl->doc_cnt);


  /* Dynamically allocate space for doc_ids.  Otherwise need to preallocate */
  /* MAXDOCS, which on average is way too large and a memory hog.  Keep */
  /* track of the number allocated and either reallocate if the next needed */
  /* is larger, or just reuse if the number is the same or smaller. */
  
  if (num_ids < entries+1)
    num_ids = entries+1;
  if (!(vector_make_room_for_n(&doc_ids, num_ids, (sizeof(Int_t)))))
    {
    Q_error("Memory allocation error (file %s, line %d)", __FILE__,
	  __LINE__);
    return(cursum);
  } else memset((void*)doc_ids, '\0', (size_t)num_ids);

  ind = (cursum)?entries - cursum->num_ent:entries;
  for(i=0;i<ind;i++)
    if (!cursum)
      doc_ids[i] = bl->list[i].doc_id;
    else doc_ids[i] = bl->list[i+cursum->num_ent].doc_id;
  
  all_summary = 
    inq_get_doc_list_fields(db, i, doc_ids, num_fields, fields);

  ind = (cursum)?entries - cursum->num_ent:entries;
  new = (cursum)?cursum->num_ent+1:1;
  for (i=0; i<ind; i++) 
    {
    curfield=0;			/* which field are we currently using? */
    current_sum_entry = (SUM_ENT *)NULL; /* start with a new entry each time... */
    memset((void*)s, '\0', (size_t)MAXLEN);

    /* first print out the relevance of the document. */
    if ((relevant = find_rel_doc(doc_ids[i])) != -1)
      sprintf ((char *)s, "%c", (rel_docs[relevant].relevant==TRUE)?'+':'-');
    else sprintf ((char *)s, " ");

    /* Remove excess white space from the id string */
    curr_id = all_summary[i*num_fields+curfield];
    while(isspace((int)*curr_id))
	curr_id++;
    id_len = strlen(curr_id);
    while(id_len > 0 && isspace((int)curr_id[id_len - 1]))
	id_len--;

    if(id_len < 80)
	curr_id[id_len] = '\0';
    else
	curr_id[80] = '\0';

    
    sprintf ((char *)(s+1), " %3d. %.3f %s(%d)\n", (int)(i+new),
	    (float)bl->list[i+new-1].belief, (char *)curr_id,
	    (int)doc_ids[i]);

    /* Now insert the header... */
    current_sum_entry = add_to_sum_ent(current_sum_entry, s);
    curfield++;

    /* Now insert the title...then the source */
    for(j=0;j<2;j++)
      {
      text = rmv_blanks(cr_to_sp(all_summary[i*num_fields+curfield],
				 FALSE));
      for(count=0;text && (count<MAXLINES); count++) 
	{
	if ((tmp = chopline(text, width-(indent+1), TRUE)))
	  {
	  tmplen = strlen ((char *)tmp);
	  memset ((void *)s, (int)'\0', (size_t)(MAXLEN*sizeof(Char_t)));
	  sprintf ((char *)s, "%s%s\n", lmargin, tmp);
	  current_sum_entry = add_to_sum_ent(current_sum_entry, s);
	  text = skip_blanks(text);
	  text += tmplen;
	} else if (count==0) {
	  sprintf ((char *)s, "%s%s missing\n", lmargin, (j==0)?"title":"source");
	  current_sum_entry = add_to_sum_ent(current_sum_entry, s);
	  break;
	  } 
        }
      curfield++;
      }
    /* Add a blank line to be between each summary entry... */
    current_sum_entry = add_to_sum_ent(current_sum_entry, (Char_t*)"\n");

    /* Add a null to know the end of the text lines... */
    cursum = add_to_summary(cursum, add_to_sum_ent(current_sum_entry,
						   (Char_t *)NULL), query);
    }
  if(all_summary != (Char_t **)NULL)
      inq_free_doc_list_fields(all_summary);
  
  return(cursum);
}
