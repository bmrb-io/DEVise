#include "ContourScheme.hxx"

void main(int argc,char *argv[])
{
       ContourScheme tContourScheme(cin);
       Real fValue;

       cout << "Contour scheme is:" << endl;
       cout << tContourScheme;

       cin >> fValue;
       int iContourLevel = tContourScheme.iGetContourLevel(fValue);
       cout << "Contour level of " << fValue << '\t' << iContourLevel << endl;
}
