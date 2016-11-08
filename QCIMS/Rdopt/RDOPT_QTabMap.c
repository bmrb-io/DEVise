#include "RDOPT.h"


extern long RD_QToQentry(long q, RD_QMapper qmfunc, boolean roundup)
{
  long lowqe, lowq, highqe, highq, midqe, midq; 
  midqe = lowqe = 1;
  midq = lowq = qmfunc(lowqe); 
  highqe = RD_QENTRY_MAX;
  highq = qmfunc(highqe); 
  while ((q > lowq) && (q < highq)) {
    midqe = (lowqe + highqe)/2;
    if (midqe == lowqe) break; 
    midq = qmfunc(midqe); 
    if (q < midq) {
      highqe = midqe;
      highq = midq;
    } else { 
      lowqe = midqe;
      lowq = midq;
    } 
  }
  if (q <= lowq) {
    midq = lowq; midqe = lowqe; 
  } else if (q >= highq) { 
    midq = highq; midqe = highqe; 
  } else /* midqe == lowqe && highqe == lowqe+1 */ if (roundup) {
    midqe = highqe;
    midq = highq; 
  } 
  while ((midqe > 1) && (qmfunc(midqe-1) == midq)) midqe--;
  return midqe; 

}

extern long RD_QMapperId(long qe)
{
  return qe; 
}

extern long RD_QMapper8fast1(long qe)
{
  if (qe <= 16) return qe; 
  else if (qe < 128) return (2*qe-16);
  else return 255L; 
}

extern long RD_QMapper8fast2(long qe)
{
  if (qe <= 16) return qe;
  else if (qe <= 64) return (2*qe-16);
  else if (qe <= 99) return (4*qe-144); 
  else return 255L; 
}

extern long RD_QMapper8fast3(long qe)
{
  if (qe <= 16) return (qe*2);
  else if (qe <= 32) return (4*qe-32);
  else if (qe < 52) return (8*qe-160);
  else return 255L; 
}

extern long RD_QMapper8fast4(long qe)
{
  if (qe <= 8) return (qe*2);
  else if (qe <= 16) return (4*qe-16);
  else if (qe <= 32) return (8*qe-80);
  else if (qe < 37) return (16*qe-336);
  else return 255L; 
}

extern long RD_QMapper8fast5(long qe)
{
  if (qe <= 8) return (qe*2);
  else if (qe <= 14) return (4*qe-16);
  else if (qe <= 20) return (8*qe-72);
  else if (qe <= 24) return (16*qe-232);
  else if (qe <  28) return (32*qe-616);
  else return 255L; 
}

extern long RD_QMapper8fast6(long qe)
{
  if (qe <= 8) return (qe*2);
  else if (qe <= 14) return (4*qe-16);
  else if (qe == 15) return (64L);
  else if (qe == 16) return (96L);
  else if (qe == 17) return (128L);
  else if (qe == 18) return (176L);
  else return 255L; 
}

extern long RD_QMapper8fast7(long qe)
{
  if (qe <= 5) return (qe*2);
  else if (qe <= 8) return (4*qe-10);
  else if (qe <= 11) return (8*qe-42); 
  else if (qe == 12) return (96L);
  else if (qe == 13) return (128L);
  else return 255L; 
}

extern long RD_QMapper8fastest(long qe)
{
  if (qe<=1) return 2L;
  else if (qe==2) return 6L;
  else if (qe==3) return 10L;
  else if (qe==4) return 14L;
  else if (qe==5) return 18L;
  else if (qe==6) return 32L;
  else if (qe==7) return 128L;
  else return 255L; 
}

extern long RD_QMapper10(long qe)
{
  if (qe <= 16) return qe; 
  else if (qe <= 64) return (qe*2); 
  else if (qe <= 128) return (qe*3); 
  else if (qe < 255) return (qe*4); 
  else return 1023L; 
}

extern long RD_QMapper10fast1(long qe)
{
  if (qe < 8) return qe; 
  else if (qe <= 16) return (qe*2); 
  else if (qe <= 64) return (qe*3); 
  else if (qe <= 128) return (qe*4); 
  else if (qe <= 204) return (qe*5); 
  else return 1023L; 
}

extern long RD_QMapper10fast2(long qe)
{
  if (qe <= 2) return qe; 
  else if (qe <= 4) return (qe*2); 
  else if (qe <= 8) return (qe*3); 
  else if (qe <= 16) return (qe*4); 
  else if (qe <= 64) return (qe*5); 
  else if (qe <= 128) return (qe*6); 
  else if (qe <= 146) return (qe*7); 
  else return 1023L; 
}

extern long RD_QMapper10fast3(long qe)
{
  if (qe <= 4) return (qe*2); 
  else if (qe <= 8) return (qe*4); 
  else if (qe <= 16) return (qe*8); 
  else if (qe < 64) return (qe*16); 
  else return 1023L; 
}

extern long RD_QMapper10fast4(long qe)
{
  if (qe <= 4) return (qe*4); 
  else if (qe <= 8) return (qe*8); 
  else if (qe <= 16) return (qe*16); 
  else if (qe < 32) return (qe*32); 
  else return 1023L; 
}

extern long RD_QMapper10fastest(long qe)
{
  if (qe < 10) return (1L << (qe-1)); 
  else return 1023L; 
}

extern long RD_QMapper12(long qe)
{
  if (qe <= 8) return (qe);
  else if (qe <= 16) return (qe*4);
  else if (qe <= 32) return (qe*6);
  else if (qe <= 64) return (qe*4);
  else if (qe <= 128) return (qe*8);
  else if (qe < 255) return (qe*16); 
  else return 4095L; 
}

extern long RD_QMapper12fastest(long qe)
{
  if (qe < 12) return (1L << (qe-1)); 
  else return 4095L; 
}

extern long RD_QMapper16(long qe)
{
  if (qe <= 1) return (qe);
  else if (qe <= 2) return (qe*2);
  else if (qe <= 4) return (qe*4);
  else if (qe <= 8) return (qe*8);
  else if (qe <= 16) return (qe*16);
  else if (qe <= 32) return (qe*32);
  else if (qe <= 64) return (qe*64);
  else if (qe <= 128) return (qe*128);
  else if (qe < 255) return (qe*256); 
  else return 65535L; 
}


extern long RD_QMapper16fastest(long qe)
{
  if (qe < 16) return (1L << (qe-1)); 
  else return 65535L; 
}


