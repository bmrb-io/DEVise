select Student.sname, Student.sid
from   1.0 as Course, 1.1 as Student, 1.3 as Enroll
where  Course.cname='dbms' and Course.dname=Enroll.dname 
       and Course.cno=Enroll.cno and Enroll.sid=Student.sid