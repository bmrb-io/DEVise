/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.11  1998/08/17 18:52:03  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.10  1996/12/02 18:44:44  wenger
  Fixed problems dealing with DST in dates (including all date composite
  parsers); added more error checking to date composite parsers.

  Revision 1.9  1996/05/11 03:15:52  jussi
  The extraction routine now takes a list of stock symbols, sorts
  them according to file offset on tape, and then extracts them
  in order.

  Revision 1.8  1996/04/16 20:56:38  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.7  1995/11/14 23:00:30  jussi
  Updated interface with Tcl scripts which now pass not only the
  symbol and offset of a company to extract but also the file
  where to cache the data.

  Revision 1.6  1995/09/22 22:36:26  jussi
  Added tape block size parameter.

  Revision 1.5  1995/09/22 15:49:44  jussi
  Added copyright message.

  Revision 1.4  1995/09/11 17:33:15  jussi
  Updated names of tapeToDisk and status to reflect the issm prefix.

  Revision 1.3  1995/09/11  15:23:42  jussi
  Removed .bak files.

  Revision 1.2  1995/09/06 17:23:57  jussi
  Updated names of linked variables.

  Revision 1.1  1995/09/06 15:28:56  jussi
  Initial revision of archive.
*/

#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define X

#ifdef X
#include <tcl.h>
#include <tk.h>

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

#endif

#include "rectape.h"
#include "issmdata.h"
#include "binconv.h"

// define binary or ASCII output for extract command
//#define BINARY_OUT
#define ASCII_OUT

#ifdef BINARY_OUT
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#endif

#ifndef X
static int tapePos = 0;

static int symbolMatch(char *str, int argc, char **argv)
{
  for(argc--; argc >= 0; argc--, argv++) {
    char *pattern = argv[0];
    // see if it's an exact match
    if (!strcmp(str, pattern))
      return 1;

    // see if pattern ends with wildcard, and prefix matches
    if (pattern[strlen(pattern) - 1] == '*'
	&& !strncmp(str, pattern, strlen(pattern) - 1))
      return 1;

    // see if pattern specifies a range in the form A-B
    char *dash = strchr(pattern, '-');
    if (dash) {
      *dash = 0;                        // terminate start of range
      int match = (strcmp(str, pattern) >= 0 && strcmp(str, dash + 1) <= 0);
      *dash = '-';                      // restore dash in range
      if (match)
	return 1;
    }
  }

  return 0;
}

static void getMaxValues(float &price, long &volume)
{
  price = 0;
  volume = 0;

  // Initialize day and denomination values
  int den = 0;
  int bigvol, tvol;
  int denpt = 1;
  int bigpt = 1;
  int conpt = 1;
  char condition = '@';

  for(int i = 0; i < header1.n; i++) {

    // See what the current denominator is
    if (denpt <= header1.ndenom) {
      if (i >= idenom[denpt - 1] - 1) {
	den = denom[denpt - 1];
	denpt++;
      }
    }

    // See if this is a big transaction
    bigvol = 0;
    if (bigpt <= header1.nbig) {
      if (i >= ibig[bigpt - 1] - 1) {
	bigvol = big[bigpt - 1];
	bigpt++;
      }
    }

    // See if there is a special condition
    condition = '@';
    if (conpt <= header1.ncon) {
      if (i >= icon[conpt - 1] - 1) {
	condition = con[conpt - 1];
	conpt++;
      }
    }

    // Only Regular Sale and Sold Last affects the current price.
    // Other conditions affect only Volume, High, and Low.
    if (condition != '@' && condition != 'L')
      continue;

    // Process trade or quote
    if (trdqt[i] == 'T') {            // if this is a trade
      if (priask[i] > 0) {            // ignore negative prices
	if (bigvol > 0) {             // if BIG volume
	  tvol = bigvol;
	} else {
	  tvol = volbid[i];           // normal volume
	}
	float tprice = 1.0 * priask[i] / den;
	if (tprice > price)
	  price = tprice;
	if (tvol > volume)
	  volume = tvol;
      }
    }
  }
}

static void genIndexData()
{
  float maxPrice;
  long maxVolume;
  getMaxValues(maxPrice, maxVolume);
  printf("%s,%d,%d,%.2f,%ld,\"%s\"\n", header1.symbol,
	 tapePos, header1.n, maxPrice, maxVolume,
	 header2.name);
}

static void genListInfo()
{
  printf("%-12.12s %-20.20s %6d %9.2f %9.2f\n",
	 header1.symbol, header2.name, header1.n,
	 header1.avgpri, header1.lowpri);
}
#endif

static void genExtractData(char *file)
{
  ofstream trade(file, ios::out);
  if (!trade) {
    cerr << "Cannot create trade file " << file << endl;
    cerr << "Skipping firm." << endl;
    return;
  }

#ifdef BINARY_OUT
  // Initialize Unix date processing
  time_t lasttime = 0;
  struct tm laststr;
  laststr.tm_year = 0;
#endif

  // Initialize some values
  int den = 0;
  int bigvol, tvol;
  int daypt = 1;
  int denpt = 1;
  int bigpt = 1;
  int conpt = 1;
  int qpt = 1;
  char condition = '@';

  int day = 1, month = 1, year = 1992;

  int ignored = 0;
  int trades = 0;
  int quotes = 0;

  for(int i = 0; i < header1.n; i++) {

#ifdef X
    if (i % 1000 == 0)
      UPDATE_TCL;
#endif

    // See what date today is
    if (daypt <= header1.ndays) {
      if (i >= idays[daypt - 1] - 1) {
	BinaryConversion::CRSPDate(days[daypt - 1], day, month, year);
	daypt++;
      }
    }

    // See what time it is
    int hh = (cumsec[i] + 32760) / 3600;
    int mm = ((cumsec[i] + 32760) - hh * 3600) / 60;
    int ss = ((cumsec[i] + 32760) - hh * 3600 - mm * 60);
    hh += 8;

#ifdef BINARY_OUT
    time_t now = lasttime;

    // Convert date and time to Unix date and time
    if (year != laststr.tm_year || month != laststr.tm_mon) {
      struct tm timestr;
      timestr.tm_year = year;
      timestr.tm_mon = month;
      timestr.tm_mday = day;
      timestr.tm_hour = hh;
      timestr.tm_min = mm;
      timestr.tm_sec = ss;
      timestr.tm_isdst = -1;
      now = mktime(&timestr);
      laststr = timestr;
      lasttime = now;
    } else {
      now = lasttime +
	    (day - laststr.tm_mday) * 24 * 3600 +
	    (hh - laststr.tm_hour) * 3600 +
	    (mm - laststr.tm_min) * 60 +
	    (ss - laststr.tm_sec);
    }
#endif

    // See what the current denominator is
    if (denpt <= header1.ndenom) {
      if (i >= idenom[denpt - 1] - 1) {
	den = denom[denpt - 1];
	denpt++;
      }
    }

    // See if this is a big transaction
    bigvol = 0;
    if (bigpt <= header1.nbig) {
      if (i >= ibig[bigpt - 1] - 1) {
	bigvol = big[bigpt - 1];
	bigpt++;
      }
    }

    // See if there is a special condition
    condition = '@';
    if (conpt <= header1.ncon) {
      if (i >= icon[conpt - 1] - 1) {
	condition = con[conpt - 1];
	conpt++;
      }
    }

    // Process trade or quote
    if (trdqt[i] == 'T') {            // if this is a trade

      if (priask[i] > 0) {            // ignore negative prices
	trades++;

	// Only Regular Sale and Sold Last affects the current price.
	// Other conditions affect only Volume, High, and Low.
	if (condition != '@' && condition != 'L')
	  continue;

	if (bigvol > 0) {             // if BIG volume
	  tvol = bigvol;
	} else {
	  tvol = volbid[i];           // normal volume
	}
	float tprice = 1.0 * priask[i] / den;
#ifdef BINARY_OUT
	trade.write(&now, sizeof now);
	trade.write(&oexch[i], sizeof oexch[i]);
	trade.write(&tprice, sizeof tprice);
	trade.write(&tvol, sizeof tvol);
	trade.write(&condition, sizeof condition);
#endif
#ifdef ASCII_OUT
	trade << day << "," << month << "," << year
	      << "," << hh << "," << mm << "," << ss
	      << "," << oexch[i] << "," << tprice << "," << tvol
	      << "," << condition << endl;
#endif
      } else {                        // else negative price (canceled trade)
	++ignored;
      }

    } else {                          // else it's a quote
      DOASSERT(trdqt[i] == 'Q', "Invalid record type");
      quotes++;

#if 0
      // Only BBO-eligible quotes are considered.
      if (condition != 'A' && condition != 'B' && condition != 'H'
	  && condition != 'O' && condition != 'R') {
	qpt++;                        // look at next asksiz/bidsiz
	continue;
      }
#endif

#if 0
      // disable quote extraction for now

      float aprice = 1.0 * priask[i] / den;
      float bprice = 1.0 * volbid[i] / den;

#ifdef BINARY_OUT
      quote.write(&now, sizeof now);
      quote.write(&oexch[i], sizeof oexch[i]);
      quote.write(&aprice, sizeof aprice);
      quote.write(&asksiz[qpt], sizeof asksiz[qpt]);
      quote.write(&bprice, sizeof bprice);
      quote.write(&bidsiz[qpt], sizeof bidsiz[qpt]);
      quote.write(&condition, sizeof condition);
#endif
#ifdef ASCII_OUT
      quote << day << "," << month << "," << year
	    << "," << hh << "," << mm << "," << ss
	    << "," << oexch[i]
	    << "," << aprice << "," << asksiz[qpt]
	    << "," << bprice << "," << bidsiz[qpt]
	    << "," << condition << endl;
#endif
#endif

      qpt++;                          // look at next asksiz/bidsiz
    }
  }

#ifndef X
  cout << "  " << trades << " trades, " << quotes << " quotes";
  if (ignored)
    cout << ", " << ignored << " trades with negative prices";
  cout << endl;
#endif

  DOASSERT(trades + ignored + quotes == header1.n, "Inconsistent data");

  trade.close();
}

static int readStock(RecTape &tape)
{
  int status = tape.getrecm((char *)&header1, map1);
  if (!status)
    return 0;
  if (status < 0) {
    cerr << "Cannot read header record" << endl;
    return -1;
  }

  if (header1.nmktmk > bnmktm) {
    cerr << "nmktmk = " << header1.nmktmk << " exceeds bound = "
      << bnmktm << endl;
    return -1;
  }

  if (header1.ndist > bndist) {
    cerr << "ndist = " << header1.ndist << " exceeds bound = "
      << bndist << endl;
    return -1;
  }

  drate = new float [header1.ndist];
  shslst = new float [bnslst];

  shsout = new int [header1.nshs];
  idays = new int [header1.ndays];
  iprint = new int [header1.nprint];
  imktmk = new int [header1.nmktmk];
  icon = new int [header1.ncon];
  iseldy = new int [header1.nseldy];
  ibnk = new int [header1.nbnk];
  istop = new int [header1.nstop];
  idenom = new int [header1.ndenom];
  icor = new int [header1.ncor];
  ibig = new int[header1.nbig];
  big = new int[header1.nbig];

  DOASSERT(drate && shsout && shslst && idays && iprint, "Out of memory");
  DOASSERT(imktmk && icon && iseldy && ibnk && istop, "Out of memory");
  DOASSERT(idenom && icor && ibig && big, "Out of memory");

  volbid = new short [header1.n];
  priask = new short [header1.n];
  cumsec = new short [header1.n];
  days = new short [header1.ndays];
  asksiz = new short [header1.nq];
  bidsiz = new short [header1.nq];
  cor = new short [header1.ncor];
  denom = new short [header1.ndenom];
  exdate = new short [header1.ndist];
  dadate = new short [header1.ndist];
  dpdate = new short [header1.ndist];
  drdate = new short [header1.ndist];
  shsday = new short [header1.nshs];
  seldy = new short [header1.nseldy];

  DOASSERT(volbid && priask && cumsec && days && asksiz, "Out of memory");
  DOASSERT(bidsiz && cor && denom && exdate && dadate, "Out of memory");
  DOASSERT(dpdate && drdate && shsday && seldy, "Out of memory");

  trdqt = new char [header1.n];
  oexch = new char [header1.n];
  print = new char [header1.nprint];
  bnk = new char [header1.nbnk];
  con = new char [header1.ncon];

  DOASSERT(trdqt && oexch && print && bnk && con, "Out of memory");

  memset(&header2, 0, sizeof header2);
  if (header1.hdr2) {
    status = tape.getrecm((char *)&header2, map2);
    DOASSERT(status >= 0, "Invalid data record");
  }

  if (header1.hdr3) {
    tape.getrecr((char *)shslst);
  }

  if (header1.ndays > 0) {
    status = tape.getrec4((char *)idays);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)days);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.ndenom > 0) {
    status = tape.getrec4((char *)idenom);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)denom);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.nbig > 0) {
    status = tape.getrec4((char *)ibig);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec4((char *)big);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.nprint > 0) {
    status = tape.getrec4((char *)iprint);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrecc((char *)print);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.nseldy > 0) {
    status = tape.getrec4((char *)iseldy);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)seldy);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.nstop > 0) {
    status = tape.getrec4((char *)istop);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.ncor > 0) {
    status = tape.getrec4((char *)icor);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)cor);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.ncon > 0) {
    status = tape.getrec4((char *)icon);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrecc((char *)con);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.nbnk > 0) {
    status = tape.getrec4((char *)ibnk);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrecc((char *)bnk);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.nmktmk > 0) {
    char  mktmkr[bnmktm][4];
    status = tape.getrec4((char *)imktmk);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrecc((char *)mktmkr);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.nshs > 0) {
    status = tape.getrec2((char *)shsday);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec4((char *)shsout);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.ndist > 0) {
    status = tape.getrecr((char *)drate);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)exdate);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)drdate);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)dadate);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)dpdate);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrecc((char *)dflag);
    DOASSERT(status >= 0, "Invalid data record");
  }

  if (header1.nq > 0) {
    status = tape.getrec2((char *)asksiz);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)bidsiz);
    DOASSERT(status >= 0, "Invalid data record");
  }
  if (header1.n > 0) {
    status = tape.getrecc((char *)oexch);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrecc((char *)trdqt);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)cumsec);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)priask);
    DOASSERT(status >= 0, "Invalid data record");
    status = tape.getrec2((char *)volbid);
    DOASSERT(status >= 0, "Invalid data record");
  }

  return 1;
}

static void freeStockSpace()
{
  delete [] drate;
  delete [] shsout;
  delete [] shslst;
  delete [] idays;
  delete [] iprint;
  delete [] imktmk;
  delete [] icon;
  delete [] iseldy;
  delete [] ibnk;
  delete [] istop;
  delete [] idenom;
  delete [] icor;
  delete [] ibig;
  delete [] big;

  delete [] volbid;
  delete [] priask;
  delete [] cumsec;
  delete [] days;
  delete [] asksiz;
  delete [] bidsiz;
  delete [] cor;
  delete [] denom;
  delete [] exdate;
  delete [] dadate;
  delete [] dpdate;
  delete [] drdate;
  delete [] shsday;
  delete [] seldy;

  delete [] trdqt;
  delete [] oexch;
  delete [] print;
  delete [] bnk;
  delete [] con;
}

#ifndef X
int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " command" << endl;
    cerr << "where command is one of the following:" << endl;
    cerr << "  index           builds an index of tape contents" << endl;
    cerr << "  list symspec    lists companies matching symspec" << endl;
    cerr << "  extract symspec fetches companies matching symspec" << endl;
    cerr << endl;
    cerr << "Examples of symspec (symbol specification):" << endl;
    cerr << "  DEC IBM HWP     exact matches of DEC, IBM, and HWP" << endl;
    cerr << "  D* G-HZZZZ      symbols starting with D and symbols" << endl;
    cerr << "                  in the range G through H, inclusive" << endl;
    return 1;
  }

  int indexCmd = 0;
  int listCmd = 0;
  int extractCmd = 0;

  if (!strncmp(argv[1], "index", strlen(argv[1]))) {
    if (argc > 2) {
      cerr << "Too many parameters specified: " << argv[2] << endl;
      return 1;
    }
    indexCmd = 1;
  } else if (!strncmp(argv[1], "list", strlen(argv[1]))) {
    if (argc < 3) {
      cerr << "Incorrect number of parameters: symbol list expected"
	   << endl;
      return 1;
    }
    listCmd = 1;
  } else if (!strncmp(argv[1], "extract", strlen(argv[1]))) {
    if (argc < 3) {
      cerr << "Incorrect number of parameters: symbol list expected"
	   << endl;
      return 1;
    }
    extractCmd = 1;
  } else if (!strncmp(argv[1], "help", strlen(argv[1]))) {
    cout << "Help:" << endl;
    return 0;
  } else {
    cerr << "Invalid command: " << argv[1] << endl;
    return 1;
  }

  char *tapeDrive = "/dev/nrst11";
  int tapeFile = 4;
  int tapeBsize = 32768;

  RecTape tape(tapeDrive, "r", tapeFile, tapeBsize);
  if (!tape) {
    cerr << "Cannot open " << tapeDrive << " for reading." << endl;
    return 1;
  }

  for(int firm = 0;; firm++) {
    int status = readStock(tape);
    if (!status)
      break;
    if (status < 0)
      return 1;
      
    if (indexCmd)
      genIndexData();
    else if (listCmd && symbolMatch(header1.symbol, argc - 2, &argv[2]))
      genListInfo();
    else if (extractCmd && symbolMatch(header1.symbol, argc - 2, &argv[2])) {
      cout << "Extracting " << header1.symbol << " ("
	   << header1.n << " trades/quotes)" << endl;
      ostrstream fileName;
      fileName << header1.symbol << ".dat" << ends;
      char *file = fileName.str();
      genExtractData(file);
      delete file;
    }
    
    freeStockSpace();

    tapePos = tape.tell();
  }
  
  return 0;
}
#endif

#ifdef X
static int extract(RecTape &tape, char *stock, long offset, char *file)
{
  ostrstream note;
  note << "Reading " << stock << " from tape..." << ends;
  char *notestr = note.str();
  Tcl_SetVar(globalInterp, "issm_status", notestr, TCL_GLOBAL_ONLY);
  delete notestr;
  UPDATE_TCL;

  if (tape.seek(offset) != offset) {
    cerr << "Cannot seek to offset " << offset << endl;
    return -1;
  }

  int status = readStock(tape);
  if (status <= 0)
    return status;

  if (strcmp(header1.symbol, stock)) {
    cerr << "Stock symbol mismatch: " << header1.symbol
         << " vs. " << stock << endl;
    return -1;
  }

  ostrstream note2;
  note2 << "Writing " << stock << " to disk..." << ends;
  notestr = note2.str();
  Tcl_SetVar(globalInterp, "issm_status", notestr, TCL_GLOBAL_ONLY);
  delete notestr;
  UPDATE_TCL;

  genExtractData(file);

  freeStockSpace();

  return 1;
}

// Searches through the index file and finds the entry for the given 
// key. Then, returns the tape offset for this security.

static long int find_issm_offset(FILE *idxfile, char *key)
{
  unsigned long int offval;
  char symbol[12];

  while(fscanf(idxfile, "%[^,],%lu,%*[^\n]\n", symbol, &offval) == 2) {
    if (!strcmp(symbol, key))
      return offval;
  }

  printf("ERROR: could not find key %s in the index file\n", key);
  return -1;
}

static int issm_create(char *tapeDrive, char *tapeFile, char *tapeOff,
		       char *tapeBsize, char *idxFile, char **argv, int argc)
{
  DOASSERT(argc % 2 == 0, "Invalid parameters");
  int num = argc / 2;

  // Get the index file pointer
  FILE *idxfile;
  if (!(idxfile = fopen(idxFile, "r"))) {
    printf("Error: could not open index file: %s\n", idxFile);
    return TCL_ERROR;
  }
  
  // We will retrieve num offsets and sort them
  long int *offset_arr = (long int *)
                         malloc(num * sizeof(long int));
  int *spos_arr = (int *)malloc(num * sizeof(int));
  DOASSERT(offset_arr && spos_arr, "Out of memory");

  int i;
  for(i = 0; i < num; i++) {
    spos_arr[i] = i * 2;
    // find offset for this security by looking through the index file
    offset_arr[i] = find_issm_offset(idxfile, argv[i * 2]);
    // add offset of beginning of tape file
    offset_arr[i] += atol(tapeOff);
    rewind(idxfile);
  }

  fclose(idxfile);

  // Now sort offset_arr - bubble sort for now
  for(i = 0; i < num; i++) {
    for(int j = i + 1; j < num; j++) {
      if (offset_arr[i] > offset_arr[j]) {
	unsigned long int tmp = offset_arr[i];
	offset_arr[i] = offset_arr[j];
	offset_arr[j] = tmp;
	tmp = spos_arr[i];
	spos_arr[i] = spos_arr[j];
	spos_arr[j] = tmp;
      }
    }
  }

  RecTape tape(tapeDrive, "r", atoi(tapeFile), atoi(tapeBsize));
  if (!tape) {
    fprintf(stderr, "Error: could not open tape device %s\n", tapeDrive);
    return TCL_ERROR;
  }

  int errors = 0;
  for(i = 0; i < num; i++) {
    if (offset_arr[i] < atol(tapeOff))
      continue;
    if (extract(tape, argv[spos_arr[i]], (unsigned long int)offset_arr[i],
		argv[spos_arr[i] + 1]) <= 0)
      ++errors;
  }

  free(offset_arr);
  free(spos_arr);

  if (errors)
    return TCL_ERROR;

  return TCL_OK;
}

int extractStocksCmd(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  // Allow other functions to UPDATE_TCL

  globalInterp = interp;

  DOASSERT(argc >= 8, "Invalid parameters");

  // Get parameter values from TCL script

  char *tapeDrive = argv[1];
  char *tapeFile = argv[2];
  char *tapeOff = argv[3];
  char *tapeBsize = argv[4];
  char *idxFile = argv[5];

  printf("Reading from %s:%s:%s (%s)\n",
	 tapeDrive, tapeFile, tapeOff, tapeBsize);

  // pass pairs of <key, cachefilename> to extraction routine

  return issm_create(tapeDrive, tapeFile, tapeOff, tapeBsize, idxFile,
		     &argv[6], argc - 6);
}
#endif
