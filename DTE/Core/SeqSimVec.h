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

/* This file defines the class SeqSimVec which is a new type added to the 
 * system. It stores the Similarity Vector for the 1-dimensional sub-sequence
 * for similarity searching. 
 */

#define NFA 3		// The number of coefficents from the DFT which will be 
					// used for getting the fingerprint of a sub-seqeunce
#include <math.h>
class SeqSimVec {
// Store the summary vector here
public:
double sum, sumsqr, alphak, sigmak;
int omega;						// The length of the window for the similarity
								// vector
double DFT_pts[2*NFA+3];		// This is the actual summary vector for the
								// sub-sequence
	SeqSimVec(){
		sum = 0;
		sumsqr = 0;
		alphak = 0;
		sigmak = 0;
		for (int m=0; m < 2*NFA+2; m++)
			DFT_pts[m] = 0;
	}

	~SeqSimVec(){}

	// This function is called when there is a query 
	// '...where vec1 similar vec2 ...'
	bool similar(const SeqSimVec* arg) {	
		double sum = 0;
		for (int i=0; i < 2*NFA+2; i++)
			sum += pow((DFT_pts[i] - arg->DFT_pts[i]),2);
		double epsilon = 0.01;
		if (sum <= epsilon)
			return true;
		else        
			return false;
	}

	// This function is called to update the fingerprint function for the
	// sub-sequence. It updates it to add a new value for the sub-sequence
	SeqSimVec* addTup(double* arg) {
		double prev[2*NFA+3];
		for (int i=0; i < 2*NFA+2; i++)
			prev[i] = DFT_pts[i];
		sum += (*arg);
		sumsqr += (*arg) * (*arg);
		alphak = sum / (double)omega;
		sigmak = sqrt(sumsqr/(double)omega - alphak*alphak);
		double a,b,c = (*arg)/sqrt(omega);
		double _sin_, _cos_;
		for (int m=1; m <= NFA;m++)
		{
			a = prev[2*m]*prev[1];
			b = prev[2*m+1]*prev[1];
			_sin_ = sin((2*M_PI*m)/(double)omega);
			_cos_ = cos((2*M_PI*m)/(double)omega);
			DFT_pts[2*m] = (a+c)*_cos_ - b*_sin_;
			DFT_pts[2*m+1] = b*_cos_ + (a+c)*_sin_;
		}

		// calculate mean and std 
		DFT_pts[0] = alphak;
		DFT_pts[1] = sigmak;

		// calculate Fourier amplitudes of normalized sequences
		for (int m=1; m <=NFA; m++)
		{
			DFT_pts[2*m] /= DFT_pts[1];
			DFT_pts[2*m+1] /= DFT_pts[1];
		}
		return this;
	}

	// This function is called to update the fingerprint function for the
	// sub-sequence. It updates it to delete a value from the sub-sequence
	// So addTup and subTup combined gets the DFT of a new sub-sequence 
	// given the DFT of the sub-sequence just before it.
	SeqSimVec* subTup(double* arg) {
		double prev[2*NFA+3];
		for (int i=0; i < 2*NFA+2; i++)
			prev[i] = DFT_pts[i];
		sum -= *(arg);
		sumsqr -= (*arg) * (*arg);
		alphak = sum / (double)omega;
		sigmak = sqrt(sumsqr/(double)omega - alphak*alphak);
		double a,b,c = -(*arg)/sqrt(omega);
		double _sin_, _cos_;

		for (int m=1; m <= NFA;m++)
		{
			a = prev[2*m]*prev[1];
			b = prev[2*m+1]*prev[1];
			_sin_ = sin((2*M_PI*m)/(double)omega);
			_cos_ = cos((2*M_PI*m)/(double)omega);
			DFT_pts[2*m] = (a+c);
			DFT_pts[2*m+1] = b;
		}

		// calculate mean and std 
		DFT_pts[0] = alphak;
		DFT_pts[1] = sigmak;

		// calculate Fourier amplitudes of normalized sequences
		for (int m=1; m <=NFA; m++)
		{
			DFT_pts[2*m] /= DFT_pts[1];
			DFT_pts[2*m+1] /= DFT_pts[1];
		}
		return this;
	}
};
