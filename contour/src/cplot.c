/*
Outputs a contour plot derived from application of a contour scheme to a 
2D matrix of floating point numbers

	INPUT(from standard input):
	1) XDataSize<TAB>YDataSize<RET> (Integers)
	2) bin_filenm<RET>		(String)
	3) XMapSize<TAB>YMapSize<RET>   (Integers)
	4) MinLevel<TAB>NLevels<TAB>Spacing<TAB>SignFlag<RET>(Float,Int,Int,Int)
	where:
		XDataSize : Size of 2D matrix along x dimension
		YDataSize : Size of 2D matrix along y dimension
		bin_filenm : Name of binary file containing matrix data
		XMapSize : Size of contour map along x dimension 
		YMapSize : Size of contour map along y dimension 
		MinLevel : Absolute value of minimum contour level
		NLevels  : Number of contour levels
		Spacing  : Geometric distance between successive contour levels
		SignFlag : Boolean flag indicating 
			TRUE(1) : Show negative contours
			FALSE(0) : Do NOT show negative contours

	OUTPUT( to standard output ):
	1) XMapSize<TAB>YMapSize<RET>
	2) A series of line segments of the following form:
		PX1<TAB>PY1<TAB>PX2<TAB>PY2<TAB>INTENSITY<RET>
	   where:
		PX1 = Start of line segment along x dimension
		PY1 = Start of line segment along y dimension
		PX2 = End of line segment along x dimension
		PY2 = End of line segment along y dimension
		INTENSITY = Contour level associated with line segment
			> 0 : Indicates positive contours
			< 0 : Indicates negative contours

	To image this data just assign colors to the INTENSITY.  Generally
	all positive contours are assigned to black and all negative contours
	are assigned to red

EXAMPLES:
1) Create input 2D matrix
	bin/flx_extract.out < data/extract1.input > data/imatrix1.dat
	extract1.input : Input file which specifies the name of the FELIX
			 matrix and the dimensions of the desired submatrix
			 that will be imaged
	imatrix1.dat : Temporary file which contains name of binary 2D matrix				file and its dimensions 
2) Create line segment contour plot of 2D matrix 
	cat data/imatrix1.dat data/contour1.scheme | bin/cplot.out > data/output1.dat
	imatrix1.dat : See above
	contour1.scheme : Input file which specifies the contour scheme
	output1.dat : Output image file in PPM format

Use the analagous sequence of commands for example plots 2 and 3.
*/

#include "Contour.hxx"

void main(int argc,char *argv[])
{
	/*Read contour map from cin*/
	ContourMap tContourMap(cin);

	/*Initialize a StreamDrawable and designate cout as the 
	  associated ostream */
	StreamDrawable tStreamDrawable(cout);

	/*Draw contour map on tStreamDrawable which sends line
	  segments to cout */
	tContourMap.vDrawContourMap(tStreamDrawable);
}
