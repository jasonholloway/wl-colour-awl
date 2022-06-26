#include <stdint.h>
#include <stdio.h>

int readCoeffs(FILE *stream, double *coeffs) {
  for(int i=0; i<9; i++) {
    if(fscanf(stream, "%le", &(coeffs[i])) != 1) return 1;
  }

  return 0;
}

int convertCtm(double *coeffs, int64_t* ctm) {
  for (int i = 0; i < 9; i++) {                                        
    double a = coeffs[i];
    int64_t b = (int64_t) (a * ((uint64_t) 1L << 32u));   

    if (a < 0) {                                            
      b = -b;
      b |= 1ULL << 63u;                                        
    }

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

  printf("hullo %le %le %le\n", coeffs[0], coeffs[1], coeffs[2]);

  FILE *dump = fopen("dump", "w");
  fwrite(ctm, sizeof(int64_t), 9, dump);
  fflush(dump);
  fclose(dump);
  
  return 0;
}
