
# Mapping tables for different pairs of data types
# The info for a data type A consists of a list of entries each of which is:
# <Data type to which it is mapped B, mapping file name, index of A, index B>

set "MapTable(COMPUSTAT)" {{CRSP /p/devise/dat/comp_crsp.table 0 1}}
set "MapTable(CRSP)" {{COMPUSTAT /p/devise/dat/comp_crsp.table 1 0}}

