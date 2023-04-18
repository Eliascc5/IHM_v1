#ifndef DOUBLESPROFILE_H
#define DOUBLESPROFILE_H

#include<iostream>
#include<cmath>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


//typedef int bool;
#define true 1
#define false 0




//float get_Straj(float t);
float * update_ScurveTraj(float qf ,float qi, float vi,float vf ,float vmax,float amax,float jmax);
float* get_Straj(float t);

#endif // DOUBLESPROFILE_H
