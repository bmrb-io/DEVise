# Data source types

set "sourceTypes(COMMAND)" {{Unix Command Output} ../schema/schema/logical/COMMAND}
set "sourceTypes(COMPUSTAT)" {{Annual and Quarterly Company Financial Data} ../schema/schema/logical/COMPUSTAT compustat.idx}
set "sourceTypes(CRSP)" {{Security Data} ../schema/schema/logical/CRSP crsp_dsm94.idx}
set "sourceTypes(ISSM)" {{Historical Stock Data (Trades and Quotes} ../schema/schema/logical/ISSM-T issm.idx}
set "sourceTypes(NETWORK)" {{Network Server Output} ../schema/schema/logical/NETWORK}
set "sourceTypes(SEQ)" {{SEQ Query Output} ../schema/schema/logical/SEQ}
set "sourceTypes(SQL)" {{SQL Query Output} ../schema/schema/logical/SQL}
set "sourceTypes(UNIXFILE)" {{Unix File} ../schema/schema/logical/UNIXFILE}
set "sourceTypes(WWW)" {{World Wide Web} ../schema/schema/logical/WWW}

# Data sources

set "sourceList(DEC Stock)" {UNIXFILE dec.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(HWP Stock)" {UNIXFILE hp.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(IBM Stock)" {UNIXFILE ibm.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(SUN Stock)" {UNIXFILE sun.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(World Outline)" {UNIXFILE World.dat WORLD ../schema/WORLD {} 100 50 ../dat}
set "sourceList(3d_test.dat)" {UNIXFILE 3d_test.dat Generic-Schema ../schema/3d_test.schema {} 100 50 ../dat}
