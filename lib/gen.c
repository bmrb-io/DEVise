/* generate a file of sin(x) and cos(x) */
#include <stdio.h>
#include <math.h>

#define PI 3.1415
main(){
	double time;
	double increment;
	int cycles;

	increment = 2*PI/360.0;
	cycles = 2;
	printf("#time\tsin\tcos\n");
	for (time=0.0; time < 2*PI*cycles; time += increment){
		printf("%f.2 %f.2 %f.2\n", time*10, cos(time*10), sin(time*10));
	}
}
