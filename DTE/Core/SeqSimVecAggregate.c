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

/* This file implements some of the functions for the classes for calculating 
 * the Similarity Vectors as a moving aggregate for all the sub-sequences from 
 * the stream of the Sequences of values.
*/

#include "SeqSimVecAggregate.h"

// This function is called to remove values from the sub-sequence as we advance
// the window for calculating the moving aggregate SeqSimVec
void ExecMovSeqSimVec::dequeue(int n){
	for(int i = 0; i < n; i++){
		assert(!tupLoad->empty());
		const Tuple* front = tupLoad->front();
		subPtr(seqVec,front[0],seqVec);
		tupLoad->pop_front();
	}
}

// This function is for calculating the MBR from trail of points. This is not
// used by our implementation.

/*
void ExecMovSeqSimVec::CreateMBR()
{
	int omega = ((SeqSimVec*) seqVec)->omega;
	if ( ((SeqSimVec*) seqVec)->count < ((SeqSimVec*) seqVec)->omega)	
		return;
	if (first_time == 1)
	{
		    for(int j=0; j<4*NFA+4; j++)
			{
				old_mbr[j]=((SeqSimVec*)seqVec)->DFT_pts[j%(2*NFA+2)];
			}
			first_time = 0;
			temp_count = 1;
			starting_offset = 1;
			//ending_offset = ((SeqSimVec*) seqVec)->omega;
	}
	else
	{
			for(int j=0; j<2*NFA+2; j++)
			{   
				if (old_mbr[j]> ((SeqSimVec*)seqVec)->DFT_pts[j])
					new_mbr[j]= ((SeqSimVec*)seqVec)->DFT_pts[j];
				else new_mbr[j]=old_mbr[j];
			}

			for(int j=2*NFA+2; j<4*NFA+4; j++)
			{   
				if (old_mbr[j]< ((SeqSimVec*)seqVec)->DFT_pts[j-2*NFA-2])
					new_mbr[j]=((SeqSimVec*)seqVec)->DFT_pts[j-2*NFA-2];
				else new_mbr[j]=old_mbr[j];
			}
			v_new=1;
			for(int j=2; j<2*NFA+2; j++)
				v_new *= (new_mbr[j+2*NFA+2]-new_mbr[j]+par);
			v_new /= temp_count+1;

			if (v_new > v_old)
			{
				ending_offset = starting_offset + omega +temp_count;
				// ***** Insert the old_mbr in the RTree with start and end pts

				for(int j=0; j < 4*NFA+4; j++)
					old_mbr[j]=((SeqSimVec*)seqVec)->DFT_pts[j%(2*NFA+2)];
				starting_offset += temp_count + 1; 
				temp_count = 1;
				v_old = pow(par,2*NFA);
			}
			else
			{
				for(int j=0; j < 4*NFA+4; j++)
					old_mbr[j]=new_mbr[j];
				temp_count++;
				v_old = v_new;
			}
	}

}
*/
