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
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define X

#ifdef X
#include <tcl.h>
#include <tk.h>

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

static char *tradePath = 0;
static char *quotePath = 0;

#else

static char *tapeDrive = "/dev/nrst1";
static int tapeFile = 5;
static int tapeBsize = 32768;
static char *tradePath = "/tmp/nyam1";
static char *quotePath = "/tmp/nyam2";
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

static void genExtractData()
{
  char tradeFile[64];
  sprintf(tradeFile, "%s/%s-t.dat", tradePath, header1.symbol);

  ofstream trade(tradeFile, ios::out);
  if (!trade) {
    cerr << "Cannot create trade file " << tradeFile << endl;
    cerr << "Skipping firm." << endl;
    return;
  }

  char quoteFile[64];
  sprintf(quoteFile, "%s/%s-q.dat", quotePath, header1.symbol);

  ofstream quote(quoteFile, ios::out);
  if (!quote) {
    cerr << "Cannot create quote file " << quoteFile << endl;
    cerr << "Skipping firm." << endl;
    trade.close();
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
      assert(trdqt[i] == 'Q');
      quotes++;

#if 0
      // Only BBO-eligible quotes are considered.
      if (condition != 'A' && condition != 'B' && condition != 'H'
	  && condition != 'O' && condition != 'R') {
	qpt++;                        // look at next asksiz/bidsiz
	continue;
      }
#endif

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
      qpt++;                          // look at next asksiz/bidsiz
    }
  }

#ifndef X
  cout << "  " << trades << " trades, " << quotes << " quotes";
  if (ignored)
    cout << ", " << ignored << " trades with negative prices";
  cout << endl;
#endif

  assert(trades + ignored + quotes == header1.n);

  trade.close();
  quote.close();
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

  assert(drate && shsout && shslst && idays && iprint);
  assert(imktmk && icon && iseldy && ibnk && istop && idenom);
  assert(icor && ibig && big);

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

  assert(volbid && priask && cumsec && days && asksiz && bidsiz);
  assert(cor && denom && exdate && dadate && dpdate);
  assert(drdate && shsday && seldy);

  trdqt = new char [header1.n];
  oexch = new char [header1.n];
  print = new char [header1.nprint];
  bnk = new char [header1.nbnk];
  con = new char [header1.ncon];

  assert(trdqt && oexch && print && bnk && con);

  memset(&header2, 0, sizeof header2);
  if (header1.hdr2) {
    status = tape.getrecm((char *)&header2, map2);
    assert(status >= 0);
  }

  if (header1.hdr3) {
    tape.getrecr((char *)shslst);
  }

  if (header1.ndays > 0) {
    status = tape.getrec4((char *)idays);
    assert(status >= 0);
    status = tape.getrec2((char *)days);
    assert(status >= 0);
  }
  if (header1.ndenom > 0) {
    status = tape.getrec4((char *)idenom);
    assert(status >= 0);
    status = tape.getrec2((char *)denom);
    assert(status >= 0);
  }
  if (header1.nbig > 0) {
    status = tape.getrec4((char *)ibig);
    assert(status >= 0);
    status = tape.getrec4((char *)big);
    assert(status >= 0);
  }
  if (header1.nprint > 0) {
    status = tape.getrec4((char *)iprint);
    assert(status >= 0);
    status = tape.getrecc((char *)print);
    assert(status >= 0);
  }
  if (header1.nseldy > 0) {
    status = tape.getrec4((char *)iseldy);
    assert(status >= 0);
    status = tape.getrec2((char *)seldy);
    assert(status >= 0);
  }
  if (header1.nstop > 0) {
    status = tape.getrec4((char *)istop);
    assert(status >= 0);
  }
  if (header1.ncor > 0) {
    status = tape.getrec4((char *)icor);
    assert(status >= 0);
    status = tape.getrec2((char *)cor);
    assert(status >= 0);
  }
  if (header1.ncon > 0) {
    status = tape.getrec4((char *)icon);
    assert(status >= 0);
    status = tape.getrecc((char *)con);
    assert(status >= 0);
  }
  if (header1.nbnk > 0) {
    status = tape.getrec4((char *)ibnk);
    assert(status >= 0);
    status = tape.getrecc((char *)bnk);
    assert(status >= 0);
  }
  if (header1.nmktmk > 0) {
    status = tape.getrec4((char *)imktmk);
    assert(status >= 0);
    status = tape.getrecc((char *)mktmkr);
    assert(status >= 0);
  }
  if (header1.nshs > 0) {
    status = tape.getrec2((char *)shsday);
    assert(status >= 0);
    status = tape.getrec4((char *)shsout);
    assert(status >= 0);
  }
  if (header1.ndist > 0) {
    status = tape.getrecr((char *)drate);
    assert(status >= 0);
    status = tape.getrec2((char *)exdate);
    assert(status >= 0);
    status = tape.getrec2((char *)drdate);
    assert(status >= 0);
    status = tape.getrec2((char *)dadate);
    assert(status >= 0);
    status = tape.getrec2((char *)dpdate);
    assert(status >= 0);
    status = tape.getrecc((char *)dflag);
    assert(status >= 0);
  }

  if (header1.nq > 0) {
    status = tape.getrec2((char *)asksiz);
    assert(status >= 0);
    status = tape.getrec2((char *)bidsiz);
    assert(status >= 0);
  }
  if (header1.n > 0) {
    status = tape.getrecc((char *)oexch);
    assert(status >= 0);
    status = tape.getrecc((char *)trdqt);
    assert(status >= 0);
    status = tape.getrec2((char *)cumsec);
    assert(status >= 0);
    status = tape.getrec2((char *)priask);
    assert(status >= 0);
    status = tape.getrec2((char *)volbid);
    assert(status >= 0);
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
      genExtractData();
    }
    
    freeStockSpace();

    tapePos = tape.tell();
  }
  
  return 0;
}
#endif

#ifdef X
static int extract(RecTape &tape, char *stock, long offset)
{
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

  genExtractData();

  freeStockSpace();

  return 1;
}

int extractStocksCmd(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  // Allow other functions to UPDATE_TCL

  globalInterp = interp;

  // Get parameter values from TCL script

  char *tapeDrive = Tcl_GetVar(interp, "issm_tapeDrive", TCL_GLOBAL_ONLY);
  char *tapeFile = Tcl_GetVar(interp, "issm_tapeFile", TCL_GLOBAL_ONLY);
  char *tapeBsize = Tcl_GetVar(interp, "issm_tapeBsize", TCL_GLOBAL_ONLY);
  tradePath = Tcl_GetVar(interp, "issm_tradePath", TCL_GLOBAL_ONLY);
  quotePath = Tcl_GetVar(interp, "issm_quotePath", TCL_GLOBAL_ONLY);

  if (!tapeDrive || !tapeFile || !tapeBsize || !tradePath || !quotePath) {
    cerr << "One of issm_tapeDrive, issm_tapeFile, issm_tapeBsize," << endl;
    cerr << "issm_tradePath, or issm_quotePath is undefined." << endl;
    cerr << "Define these values in $(DEVISE_LIB)/issm.tk." << endl;
    return TCL_ERROR;
  }

  cout << "Reading from " << tapeDrive << ":" << tapeFile
       << " (" << tapeBsize << ") to " << tradePath << endl;

  RecTape tape(tapeDrive, "r", atoi(tapeFile), atoi(tapeBsize));
  if (!tape) {
    cerr << "Cannot open " << tapeDrive << " for reading." << endl;
    return TCL_ERROR;
  }

  for(int i = 1; i < argc; i += 2) {
    char *stock = argv[i];
    long offset = atol(argv[i + 1]);

    ostrstream note;
    note << "Extracting " << stock << " from tape..." << ends;
    char *notestr = note.str();
    Tcl_SetVar(interp, "issm_status", notestr, TCL_GLOBAL_ONLY);
    delete notestr;
    UPDATE_TCL;

    int status = extract(tape, stock, offset);
    if (status <= 0)
      return TCL_ERROR;

    ostrstream note2;
    note2 << "issm_tapeToDisk " << stock << ends;
    notestr = note2.str();
    status = Tcl_Eval(interp, notestr);
    if (status != TCL_OK)
      cerr << "Error: " << interp->result << endl;
    delete notestr;
    UPDATE_TCL;
  }

  return TCL_OK;
}
#endif
