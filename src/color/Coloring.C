//******************************************************************************
// class Coloring
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: Coloring.C
// Last modified: Fri Dec  5 10:13:43 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970429 [weaver]: Original file.
// 970505 [weaver]: Updated to new Color Manager.
// 970513 [weaver]: Updated to standard C++ class String.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971118 [weaver]: Changed from ColorID to PColorID for palette operations.
// 971203 [weaver]: Added RMSDistance() methods.
//
//******************************************************************************

#include "Coloring.h"

//******************************************************************************
// Libraries
//******************************************************************************

#include <cmath>

//******************************************************************************
// Static Member Definitions
//******************************************************************************

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

//******************************************************************************
// Getters and Setters
//******************************************************************************

void	Coloring::SetColors(PColorID fgid, PColorID bgid)
{
	fore = fgid;
	back = bgid;
}

//******************************************************************************
// Utility Functions (Data Colors)
//******************************************************************************

void	Coloring::AddDataColor(PColorID pcid)
{
	data.push_back(pcid);
}

//******************************************************************************
// Utility Functions (Color Heuristics)
//******************************************************************************

double	Coloring::ForeBackRMSDistance(void)
{
	RGB		rgbFore, rgbBack;

	PM_GetRGB(fore, rgbFore);
	PM_GetRGB(back, rgbBack);

	return rgbFore.Distance(rgbBack);
}

double	Coloring::DataRMSDistance(void)
{
	RGBList		rgbList = PM_GetRGBList();
	double		distance = 0;

	for (ulong i=0; i<data.size(); i++)
		for (ulong j=0; j<data.size(); j++)
			if (i != j)
				distance += rgbList[data[i]].Distance(rgbList[data[j]]);

	return distance;
}

double	Coloring::RMSDistance(void)
{
	RGB			rgbFore, rgbBack;
	RGBList		rgbList = PM_GetRGBList();
	double		distance = 0;

	PM_GetRGB(fore, rgbFore);
	PM_GetRGB(back, rgbBack);

	for (ulong i=0; i<data.size(); i++)
	{
		distance += rgbFore.Distance(rgbList[data[i]]);
		distance += rgbBack.Distance(rgbList[data[i]]);

		for (ulong j=0; j<data.size(); j++)
			if (i != j)
				distance += rgbList[data[i]].Distance(rgbList[data[j]]);
	}

	return distance + rgbFore.Distance(rgbBack);
}

double	Coloring::Entropy(IntVector count)
{
	cout << "count.size() = " << count.size() << endl;
	cout << "data.size() = " << data.size() << endl;

	if (count.size() != data.size())
		return 0.0;

	DoubleVector	entropy(count.size(), 0.0);
	RGBList			rgbList = PM_GetRGBList();
	int				total = 0;
	double			totalEntropy;

	for (ulong i=0; i<count.size(); i++)
		total += count[i];

	cout << "total = " << total << endl;

	for (ulong i=0; i<count.size(); i++)
	{
		double	ratio = (double)count[i] / total;

		if (ratio > 0.0)
			entropy[i] = ratio * log(ratio);

		cout << "ratio[" << i << "] = " << ratio << endl;
		cout << "count[" << i << "] = " << count[i] << endl;
		cout << "entropy[" << i << "] = " << entropy[i] << endl;
	}

	for (ulong i=0; i<data.size(); i++)
	{
		for (ulong j=0; j<i; j++)
		{
			double	distance = rgbList[data[i]].Error(rgbList[data[j]]);

			totalEntropy += distance * entropy[i] * entropy[j];

//			cout << "distance[" << i << ", " << j << "] = " << distance << endl;
		}
	}

	return totalEntropy / (0.5 * log(0.5));
}

//******************************************************************************
// Conversions and Operators
//******************************************************************************
// Total ordering including data PColorIDs should be added eventually.

// Two colorings are the same if the fore and back PColorIDs are the same).
bool	Coloring::operator==(const Coloring& t) const
{
	return ((fore == t.fore) && (back == t.back));
}

// Total ordering on PColorIDs, in precedence fore > back.
bool	Coloring::operator<(const Coloring& t) const
{
	return ((fore < t.fore) || ((fore == t.fore) && (back < t.back)));
}

//******************************************************************************
// Output Functions
//******************************************************************************

string	Coloring::ToString(void) const
{
	return string();
}

bool	Coloring::FromString(const string& /*s*/)
{
	return true;
}

//******************************************************************************
// Static Functions
//******************************************************************************

//******************************************************************************
// Private Functions
//******************************************************************************

//******************************************************************************
