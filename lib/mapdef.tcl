#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1995
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#  $Id$

#  $Log$

############################################################

# This file contains several pieces of information that are relevant while
# generating the mapping table and while using them for visualization of
# data

# Format of the index files as a list of pairs of the form ...
# <field name, type> where the type can be int or string (CORAL types)

set idxFormat(COMPUSTAT) {{OFFSET int} {DNUM int} {CUSIP string} {CIC string} \
	{NAME string} {SMBL string} {YEAR int} {STATE int} {COUNTY int}}
set idxFormat(CRSP) {{OFFSET int} {STARTREC int} {PERMNO int} {CUSIP string} \
	{HEXCD int} {HSICCD int} {NAME string} {BEGDAT int} {ENDDAT int}}


# Programs to convert index files into coral loader programs

set LoadPgm(COMPUSTAT) compcreate
set LoadPgm(CRSP) crspcreate


# Mapping tables for different pairs of data types
# The info for a data type A consists of a list of entries each of which is:
# <Data type to which it is mapped B, mapping file name, index of A, index B>

set "MapTable(COMPUSTAT)" {{CRSP $datadir/comp_crsp.table 0 1}}
set "MapTable(CRSP)" {{COMPUSTAT $datadir/comp_crsp.table 1 0}}

