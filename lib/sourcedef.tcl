# Data source types

set "sourceTypes(COMMAND)" {{Unix Command Output} /p/devise/dat/command.schema}
set "sourceTypes(COMPUSTAT)" {{Annual and Quarterly Company Financial Data} /p/devise/dat/compustat.schema compustat.idx}
set "sourceTypes(CRSP)" {{Security Data} /p/devise/dat/crsp.schema crsp_dsm94.idx}
set "sourceTypes(ISSM)" {{Historical Stock Data (Trades and Quotes} /p/devise/dat/issm-t.schema issm.idx}
set "sourceTypes(NETWORK)" {{Network Server Output} /p/devise/dat/network.schema}
set "sourceTypes(SEQ)" {{SEQ Query Output} /p/devise/dat/seq.schema}
set "sourceTypes(SQL)" {{SQL Query Output} /p/devise/dat/sql.schema}
set "sourceTypes(UNIXFILE)" {{Unix File} /p/devise/dat/unixfile.schema}
set "sourceTypes(WWW)" {{World Wide Web} /p/devise/dat/www.schema}
set "sourceTypes(DQL)" {{DEVise Query Language} /p/devise/dat/dql.schema}

# Data sources

set "sourceList(AAA)" {ISSM AAA ISSM-Trade /p/devise/dat/issm-t.schema /p/devise/dat/ISSM.AAA.dat 100 50 {}}
set "sourceList(AFS performance)" {WWW query AFS-Schema /p/devise/dat/afsperf.schema /p/devise/dat/WWW.query.dat 70 50 http://www.cs.wisc.edu/CSL/afs_stat.dat}
set "sourceList(Airpax Electronics Inc.)" {CRSP 44775 CRSP-Schema /p/devise/dat/crsp.schema /p/devise/dat/CRSP.44775.dat 85 50 {}}
set "sourceList(Allied Signal Inc.)" {CRSP 10145 CRSP-Schema /p/devise/dat/crsp.schema /p/devise/dat/CRSP.10145.dat 85 50 {}}
set "sourceList(Autozone Inc.)" {ISSM AZO ISSM-Trade /p/devise/dat/issm-t.schema /p/devise/dat/ISSM.AZO.dat 100 50 {}}
set "sourceList(Caspen Oil Inc.)" {COMPUSTAT CNO compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.CNO.dat 100 50 {}}
set "sourceList(DEC Corp.)" {COMPUSTAT DEC compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.DEC.dat 100 50 {}}
set "sourceList(Digital Equipment Corp.)" {ISSM DEC ISSM-Trade /p/devise/dat/issm-t.schema /p/devise/dat/ISSM.DEC.dat 90 50 {}}
set "sourceList(Dole Food Company)" {COMPUSTAT DOL compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.DOL.dat 90 50 {}}
set "sourceList(Fluke Corp.)" {COMPUSTAT FLK compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.FLK.dat 100 50 {}}
set "sourceList(HP Corp.)" {COMPUSTAT HWP compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.HWP.dat 60 50 {}}
set "sourceList(Hewlett-Packard Corporation)" {ISSM HWP ISSM-Trade /p/devise/dat/issm-t.schema /p/devise/dat/ISSM.HWP.dat 70 50 {}}
set "sourceList(IBM)" {COMPUSTAT IBM compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.IBM.dat 100 50 {}}
set "sourceList(IBM A)" {CRSP IBM.A CRSP-Schema /p/devise/dat/crsp.schema /p/devise/dat/CRSP.IBM.A.dat 100 50 ""}
set "sourceList(IBM B)" {CRSP IBM.B CRSP-Schema /p/devise/dat/crsp.schema /p/devise/dat/CRSP.IBM.B.dat 100 50 {}}
set "sourceList(IBM C)" {CRSP IBM.C CRSP-Schema /p/devise/dat/crsp.schema /p/devise/dat/CRSP.IBM.C.dat 85 50 {}}
set "sourceList(Informix Performance Data)" {UNIXFILE responsetime.dat RespTime-Schema /p/devise/dat/resptime.schema /dev/null 100 40 /usr/informix/measurement}
set "sourceList(International Business Machines Corp.)" {ISSM IBM.A ISSM-Trade /p/devise/dat/issm-t.schema /p/devise/dat/ISSM.IBM.dat 100 50 ""}
set "sourceList(Marine Harvest Int.)" {COMPUSTAT MHI compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.MHI.dat 100 50 {}}
set "sourceList(Motorola Inc.)" {COMPUSTAT MOT compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.MOT.dat 100 50 {}}
set "sourceList(Nashua Corp.)" {COMPUSTAT NSH compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.NSH.dat 100 50 {}}
set "sourceList(Oracle Performance Data)" {UNIXFILE responsetime.dat RespTime-Schema /p/devise/dat/resptime.schema /dev/null 100 40 /usr/oracle/measurement}
set "sourceList(Oshkosh B'Gosh Inc.)" {COMPUSTAT GOSHA compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.GOSHA.dat 100 50 {}}
set "sourceList(Patient Data)" {UNIXFILE physio.dat Patient-Schema /p/devise/dat/physio.schema /dev/null 100 40 {}}
set "sourceList(SQL Query 1)" {SQL allmanagers SQL-Schema /p/devise/dat/sql.schema /p/devise/dat/SQL.allmanagers.dat 75 30 {select * from r, s where r.deptno = s.deptno}}
set "sourceList(SQL Query 2)" {SQL allemployees SQL-Schema /p/devise/dat/sql.schema /p/devise/dat/SQL.allemployees.dat 75 30 {select * from r, s where r.deptno = s.deptno}}
set "sourceList(SSC)" {COMPUSTAT SSC compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.SSC.dat 75 50 {}}
set "sourceList(Sample Unix Command)" {COMMAND awkcommand Cmd-Schema /p/devise/dat/cmd.schema /p/devise/dat/COMMAND.awkcommand.dat 80 10 {awk -f tabulate.awk < data.txt}}
set "sourceList(Sheffield Watch Corp.)" {CRSP 45188 CRSP-Schema /p/devise/dat/crsp.schema /p/devise/dat/CRSP.45188.dat 100 50 {}}
set "sourceList(Smith Corona Corp.)" {COMPUSTAT SCO compustat-schema /p/devise/dat/compustat.schema /p/devise/dat/COMPUSTAT.SCO.dat 100 50 {}}
set "sourceList(Stock Trend Query)" {SEQ stocktrend SEQ-Schema /p/devise/dat/seq.schema /p/devise/dat/SEQ.stocktrend.dat 100 30 {this is a SEQ query}}
set "sourceList(Sybase Performance Data)" {UNIXFILE responsetime.dat RespTime-Schema /p/devise/dat/resptime.schema /dev/null 60 40 /usr/sybase/measurement}
set "sourceList(Word Count of IBM/Compustat)" {COMMAND wccommand Cmd-Schema /p/devise/dat/cmd.schema /p/devise/dat/COMMAND.wccommand.dat 80 10 {wc /p/devise/dat/COMPUSTAT.IBM.dat}}
