
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#if defined(BID) 
  #include "../bid_conf.h"
  #include "../bid_functions.h"
  #if defined(WIDTH64) 
    typedef struct data_bid{
      int from;
      int to;
      BID_UINT64 inp;
    } data_t_bid;
  #else
    typedef struct data_bid{
      int from;
      int to;
      BID_UINT128 inp;
    } data_t_bid;
  #endif

#else
  #if defined(WIDTH64)
    #include "../decimal64.h"
    typedef struct data_dec{
      int from;
      int to;
      decimal64 inp;
    } data_t_dec;
  #else
    #include "../decimal128.h"
    typedef struct data_dec{
      int from;
      int to;
      decimal128 inp;
    } data_t_dec;
  #endif
#endif

#define COUNTRY 12
#define INPUTS "inputs.txt"


int main(int argc, char *argv[]){
  #if defined(BID) 
    data_t_bid output_data;
    _IDEC_round rnd;
    rnd = BID_ROUNDING_TO_NEAREST;
  #else
    #if defined(WIDTH64)
      decContext rnd;
      decContextDefault(&rnd, DEC_INIT_DECIMAL64); // initialize
    #else
      decContext rnd;
      decContextDefault(&rnd, DEC_INIT_DECIMAL128); // initialize
    #endif
    data_t_dec output_data;
  #endif
  char str[30];
  srand(time(NULL));
  FILE *list;
  int ret;
  int i = 0;
  char filename[255];          // input filename
  char strin[255];
  char strout64[255];

  if ((list=fopen(INPUTS, "r")) != NULL) {
    while ((ret = fscanf(list, "%s", filename))==1){
      fprintf(stderr, "Processing %s\n", filename);
      strcpy(strin, "./ascii/");
      strcpy(strout64, "./bin/");
      strcat(strin, filename);
      strcat(strout64, filename);
      #if defined(BID)
        #if defined(WIDTH64)
          strcat(strout64, ".bid64");
        #else
          strcat(strout64, ".bid128");
        #endif
      #else
        #if defined(WIDTH64)
          strcat(strout64, ".d64");
        #else
          strcat(strout64, ".d128");
        #endif
      #endif
      FILE *input = fopen(strin, "r");
      FILE *output = fopen(strout64, "wb");
      while((fscanf(input, "%d\n", &(output_data.from)) == 1) &&
            (fscanf(input, "%d\n", &(output_data.to)) == 1) &&
            (fscanf(input, "%s\n", str) == 1)) {
        fwrite(&(output_data.from), sizeof(int), 1, output);
	fwrite(&(output_data.to), sizeof(int), 1, output);

        #if defined(BID)
          #if defined(WIDTH64)
            __bid64_from_string(&(output_data.inp), str, &rnd);
            fwrite(&(output_data.inp), sizeof(BID_UINT64), 1, output);
          #else
            __bid128_from_string(&(output_data.inp), str, &rnd);
            fwrite(&(output_data.inp), sizeof(BID_UINT128), 1, output);
          #endif
        #else
          #if defined(WIDTH64)
            decimal64FromString(&(output_data.inp), str, &rnd);
            fwrite(&(output_data.inp), sizeof(decimal64), 1, output);
          #else
            decimal128FromString(&(output_data.inp), str, &rnd);
            fwrite(&(output_data.inp), sizeof(decimal128), 1, output);
          #endif
        #endif
        i++;
      }
      fclose(input);
      fclose(output);
    }
  }
  printf("Done. Processed %d numbers.\n", i);
  return 0;
}
