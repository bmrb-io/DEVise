select C.dname, C.cno, C.cname
from   1.0 as C, 1.1 as S, 1.3 as E
where  S.sname='jamme' and S.sid=E.sid and E.dname=C.dname and E.cno=C.cno