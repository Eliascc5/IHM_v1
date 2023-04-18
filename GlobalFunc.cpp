#include"GlobalFunc.h"
#include <cstdlib>

float* linspace(float x1, float x2, int n) {

 float *x =(float *)malloc(n*sizeof(float));

 float step = (x2 - x1) / (float)(n-1);

 for (int i = 0; i < n; i++) {
     x[i] = x1 + ((float)i * step);
 }

return x;
}
