select S.sname, S.sid
from   1.0 as C, 1.1 as S, 1.3 as E
where  C.cname='dbms' and C.dname=E.dname and C.cno=E.cno and E.sid=S.sid