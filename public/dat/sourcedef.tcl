# Data source types

set "sourceTypes(UNIXFILE)" {{Unix File} ../schema/schema/logical/UNIXFILE}
set "sourceTypes(WWW)" {{World Wide Web} ../schema/schema/logical/WWW}

# Data sources

set "sourceList(DEC Stock)" {UNIXFILE dec.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(HWP Stock)" {UNIXFILE hp.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(IBM Stock)" {UNIXFILE ibm.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(SUN Stock)" {UNIXFILE sun.dat MIT_STOCK ../schema/schema/physical/STOCK {} 100 50 ../dat}
set "sourceList(World Outline)" {UNIXFILE World.dat WORLD ../schema/WORLD {} 100 50 ../dat}
set "sourceList(3d_test.dat)" {UNIXFILE 3d_test.dat Generic-Schema ../schema/3d_test.schema {} 100 50 ../dat}
