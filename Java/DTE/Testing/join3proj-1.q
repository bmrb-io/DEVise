select Course.dname, Course.cno, Course.cname
from   1.0 as Course, 1.1 as Student, 1.3 as Enroll
where  Student.sname='jamme' and Student.sid=Enroll.sid 
       and Enroll.dname=Course.dname and Enroll.cno=Course.cno