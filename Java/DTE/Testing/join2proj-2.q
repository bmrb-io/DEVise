select Student.sname, Student.major
from   1.3 as Enroll, 1.1 as Student
where  Enroll.dname='math' and Enroll.cno=210 and Enroll.sid=Student.sid