/***************************************************************************
*                              Copyright (c)                               *
*  Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*                              1990 - 1996.                                *
*                                                                          *
*                          All Rights Reserved.                            *
*                                                                          *
* The INQUERY SOFTWARE was developed at the University of Massachusetts    *
* at Amherst Computer Science Department within the National Center for    *
* Intelligent Information Retrieval (CIIR). For more information, contact  *
* 413-545-0463 or http://ciir.cs.umass.edu.                                *
*                                                                          *
* This SOFTWARE was developed in significant part with private funding.    *
* The INQUERY SOFTWARE is proprietary to ACSIOM and may not be distributed *
* in whole or in part or used in any way without authorized permission in  *
* writing from ACSIOM.  Possession, use, duplication or dissemination of   *
* the INQUERY SOFTWARE and media is authorized only pursuant to a valid    *
* written license from ACSIOM.                                             *
*                                                                          *
* RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.   *
* Government is subject to restrictions as set forth in subparagraph       *
* (c)(l)(ii) of the Rights in Technical Data and Computer Software clause  *
* at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.               *
*                                                                          *
*        Contact ACSIOM at (413)545-6311 for more information              *
***************************************************************************/ 

/*
 * "FLEXBUFF_C" is a C file to contain functions which allow flex to
 * work from an in-memory buffer rather than reading-in or copying its
 * data.  This file is meant to be included into scanners running flex
 * 2.4.6 or later.  This file must be used with FLEXBUFF.H which defines 
 * the corresponding flex macros.
 */

/*
 *  RCS identifier for module:
 */
static char flexbuff_c_rcsid[]="$RCSfile$, $Revision$, $Date$";

#ifdef FLEXBUFF

#include "inerror.h"

/* FILL_FLEX_BUFF
 *
 * Fill the flex internal buffer by assigning it to the given buffer 'buff'.
 * The rest of the YY_BUFFER_STATE is filled in just as flex would do
 * it internally, except that yy_fill_buffer is set to 0 so that flex
 * will report an EOF on end of buffer rather than trying to refill it.
 */
static void
fill_flex_buff(Char_t *buff, Int_t num_in_buff)
{
    static int first_time = 1;

    new_buff = (YY_BUFFER_STATE)malloc(sizeof(struct yy_buffer_state));


    if(new_buff == (YY_BUFFER_STATE)NULL)
	Q_error("Failed to allocate %d bytes in fill_flex_buff.",
		sizeof(struct yy_buffer_state));

    /* This is a cop-out way of making sure that the whole structure
     * is initialized since we don't have real control over the
     * fields in the structure.
     */
    memset((char *)new_buff, 0, sizeof(struct yy_buffer_state));
    
    new_buff->yy_buf_size = num_in_buff;
    new_buff->yy_ch_buf = (unsigned char *)buff;
    new_buff->yy_input_file = (FILE *)NULL;
    new_buff->yy_n_chars = num_in_buff;
    save_1 = new_buff->yy_ch_buf[num_in_buff];
    save_2 = new_buff->yy_ch_buf[num_in_buff + 1];
    new_buff->yy_ch_buf[num_in_buff] = YY_END_OF_BUFFER_CHAR;
    new_buff->yy_ch_buf[num_in_buff+1] = YY_END_OF_BUFFER_CHAR;
    new_buff->yy_buf_pos = new_buff->yy_ch_buf + 1;
    new_buff->yy_is_interactive = 0;
    new_buff->yy_fill_buffer = 0;

    yy_switch_to_buffer(new_buff);

    if(!yy_start)
	yy_start = 1;
    yy_load_buffer_state();
    
    yy_init = 0;
}

/* FLUSH_FLEX_BUFF
 *
 * Replace the holding character and free the current buffer.  This 
 * should be called before returning from a scanner using these functions.
 */
static void
flush_flex_buff(void)
{
    int num_in_buff = yy_current_buffer->yy_buf_size;
    
    *yy_c_buf_p = yy_hold_char;

    yy_current_buffer->yy_ch_buf[num_in_buff] = save_1;
    yy_current_buffer->yy_ch_buf[num_in_buff + 1] = save_2;
    free(yy_current_buffer);
    yy_current_buffer = NULL;
    
    return;
}

#endif /* FLEXBUFF */
