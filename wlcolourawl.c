#define _POSIX_C_SOURCE 200809L                 
#include <stdio.h>                              
#include <math.h>
#include <stdint.h>
#include "relay.h"

int readCoeffs(FILE *stream, double *coeffs) {
  for(int i=0; i<9; i++) {
    if(fscanf(stream, "%le", &(coeffs[i])) != 1) return 1;
  }

  return 0;
}

int convertCtm(double *coeffs, int64_t* ctm) {
  for (int i = 0; i < 9; i++) {                                        
    double a = coeffs[i];

    int64_t b = (int64_t) (fabs(a) * ((uint64_t) 1L << 32u));   
    if (a < 0) b |= 1ULL << 63u;                                        

    ctm[i] = b;
  }                                                                    

  return 0;
}

int main(int argc, char **argv) {
  printf("got %i args\n", argc);
  for(int i = 0; i < argc; i++) {
    printf("%i: %s\n", i, argv[i]);
  }
  printf("\n");


  double coeffs[9];
  int64_t ctm[9];

  if(readCoeffs(stdin, coeffs)) {
    printf("failed to parse stdin\n");
    return 1;
  }

  if(convertCtm(coeffs, ctm)) {
    printf("error converting to ctm\n");
    return 1;
  }

  if(relayCtm("wayland-0", ctm)) {
    printf("error relaying to compositor\n");
    return 1;
  }
  
  return 0;
}
