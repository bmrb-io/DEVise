#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Image.h"

main(int argc, char *argv[])
{
  int i, totalnum;
  Image TheImage, DecImage;
  int nplanes, corig, cdec;

  double error, signal, SNR, PSNR;

  error = 0.0;
  signal = 0.0;

  if (argc != 3) {
    printf("Usage: computeSNR original_file decoded_file\n");
    exit(0);
  }
  
  InitImage(&TheImage);
  TheImage.Silent = 1;
  if (!SetImageFile(&TheImage, argv[1], 0)) exit(1);
  PeekImage(&TheImage);
  if (TheImage.ImKind == IM_UNKNOWN) exit(1);
  if (TheImage.NumComponents == 1) {
    nplanes = 1;
  }
  else if (TheImage.NumComponents == 3) {
    nplanes = 3;
    TheImage.ColorConvKind = RGBtoYCC;
  }
  else exit(1);


  totalnum = TheImage.NumCols * TheImage.NumRows;

  ReadImg(&TheImage);

  InitImage(&DecImage);
  if (!SetImageFile(&DecImage, argv[2], 0)) exit(1);
  DecImage.Silent = 1;
  PeekImage(&DecImage);
  if (DecImage.ImKind == IM_UNKNOWN) exit(1);
  if (DecImage.NumComponents == 1) {
    nplanes = 1;
  }
  else if (DecImage.NumComponents == 3) {
    nplanes = 3;
    DecImage.ColorConvKind = RGBtoYCC;
  }
  else exit(1);


  if (totalnum != (DecImage.NumCols * DecImage.NumRows)) {
    fprintf(stderr,"Size mismatch\n");
    exit(1);
  }

  ReadImg(&DecImage);


  for (i=0; i<totalnum; i++) {
    corig = (int) TheImage.Im[0][i];
    cdec = (int) DecImage.Im[0][i];
    signal += ((double) (corig*corig));
    error += ((double) ((corig-cdec)*(corig-cdec)));
  }
  

  SNR = log10(signal/error)*10.0;
  PSNR = log10(((double) totalnum)*((double) (255*255))/error)*10.0;

  printf("SNR = %lf, PSNR = %lf\n",SNR,PSNR);

}
