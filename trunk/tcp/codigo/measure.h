#include <iostream>
#include <sys/time.h>

static timeval t1,t2;

void tic(){
    gettimeofday(&t1, NULL);    
}

double toc(){
   gettimeofday(&t2, NULL);
   double elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
   elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
   return elapsedTime;
}
