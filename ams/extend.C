/*
  $Id$

  $Log$
  Revision 1.3  1995/09/05 20:08:33  jussi
  Updated header

  Revision 1.2  1995/09/05 20:05:24  jussi
  Added VCS header.
*/

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

const int months = 12;
const char *month[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
			"JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const char *comment = "//";

const int time_inc = 1;

int leapyear(int y)
{
  return (y % 4 == 0) && (y % 100 != 0) && (y % 400 == 0);
}

int monthnum(const char *name)
{
  for(int i = 0; i < months; i++)
    if (!strncmp(name, month[i], strlen(month[i])))
      return i + 1;
  assert(0);
  return 0;
}

void next_day(int &d, int &m, int &y)
{
  int dmax = days[m-1];
  if (leapyear(y) && m == 2)
    dmax++;

  if (++d > dmax) {
    d = 1;
    if (++m > months) {
      m = 1;
      if (++y > 1999) {
	cerr << "Year exceeds two digits:" << y << endl;
	exit(1);
      }
    }
  }
}

void next_time(int &t, int &d, int &m, int &y)
{
  t += time_inc;                        // advance clock
  if (t % 100 >= 60)                    // advance to next hour?
    t += 40;                            // so be it

  if (t < 2400)                         // same day (less than 24 hrs)?
    return;

  t = 0;                                // start from new day

  next_day(d, m, y);
}

void parse_line(char *line, int &d, int &m, int &y)
{
  char *comma1 = (line ? strchr(line, ',') : 0);
  char *comma2 = (comma1 ? strchr(comma1 + 1, ',') : 0);
  
  if (!comma1 || !comma2) {
    cerr << "Line does not conform to spec: " << line << endl;
    exit(1);
  }
    
  d = atoi(comma2 + 1);
  m = monthnum(comma2 + 1 + 3);
  y = 1900 + atoi(comma2 + 1 + 3 + 4);
}

void make_line(char *line, int t, int d, int m, int y)
{
  char *comma1 = (line ? strchr(line, ',') : 0);
  char *comma2 = (comma1 ? strchr(comma1 + 1, ',') : 0);
  char *comma3 = (comma2 ? strchr(comma2 + 1, ',') : 0);
  char *comma4 = (comma3 ? strchr(comma3 + 1, ',') : 0);

  if (!comma1 || !comma2 || !comma3 || !comma4) {
    cerr << "Line does not conform to spec: " << line << endl;
    exit(1);
  }
    
  static char temp[255];
  *comma2 = '\0';

  if (t < 0)
    sprintf(temp, "%s,%02d-%3.3s-%02d%s", line,
	    d, month[m - 1], y - 1900, comma3);
  else
    sprintf(temp, "%s,%02d-%3.3s-%02d,%04d%s", line,
	    d, month[m - 1], y - 1900, t, comma4);

  strcpy(line, temp);
}

int main(int argc, char **argv)
{
  if (argc < 3) {
    cerr << "Usage: " << argv[0] << " file nrecs [-c]" << endl;
    cerr << "  produces specified number of records from file in stdout"
         << endl;
    exit(1);
  }

  int cont = 0;
  if (argc > 3)
    cont = (strcmp(argv[3], "-c") == 0);

  ifstream input(argv[1], ios::in);
  if (!input) {
    cerr << "Cannot open file " << argv[1] << endl;
    exit(1);
  }

  const unsigned long numRecs = atol(argv[2]);

  char line[255];
  int t = -time_inc;
  int d = 1;
  int m = 1;
  int y = 1970;
  int nrecs = 0;

  if (cont) {                           // first copy original file?
    // first copy original file to stdout
    while(nrecs < numRecs && input.getline(line, sizeof line)) {
      if (strncmp(line, comment, strlen(comment)) != 0) {
	if (nrecs % (numRecs / 10) == 0)
	  cerr << " " << nrecs;
	parse_line(line, d, m, y);
	nrecs++;
	cout << line << endl;
      }
    }
  }

  // produce the required number of records
  int prev_d = -1;
  while(nrecs < numRecs) {
#if 0
    input.seekg(0, ios::beg);
#else
    input.close();                      // alternate method if seekg fails
    input.open(argv[1], ios::in);
    assert(input);
#endif
    while(nrecs < numRecs && input.getline(line, sizeof line)) {
      if (strncmp(line, comment, strlen(comment)) != 0) {
	if (nrecs % (numRecs / 10) == 0)
	  cerr << " " << nrecs;
	if (!cont) {
	  next_time(t, d, m, y);
	  make_line(line, t, d, m, y);
	} else {
	  int d2, m2, y2;
	  parse_line(line, d2, m2, y2);
	  if (prev_d > 0 && prev_d != d2)
	    next_day(d, m, y);
	  prev_d = d2;
	  make_line(line, -1, d, m, y);
	}
	cout << line << endl;
	nrecs++;
      }
    }
  }

  cerr << endl;

  return 0;
}
