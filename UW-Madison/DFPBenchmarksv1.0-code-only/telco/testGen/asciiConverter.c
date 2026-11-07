#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STRMAX 1024
#define INPUTS "inputs.txt"

#if defined(BID) 
  #include "../bid_conf.h"
  #include "../bid_functions.h"
#else
 #if defined(WIDTH64)
   #include "../decimal64.h"
 #else
   #include "../decimal128.h"
 #endif 
#endif

int main(int argc, char *argv[]) {
        #if defined(BID) 
          #if defined(WIDTH64)
	    BID_UINT64 val;
          #else
	    BID_UINT128 val;
          #endif
	#else
          #if defined(WIDTH64)
	    decimal64 val;
          #else
	    decimal128 val;
          #endif
	#endif
	//char strin[STRMAX];
	char * teststr = malloc(sizeof(char) * STRMAX);
	char * strin = malloc(sizeof(char) * STRMAX);
	char * strout_bin = malloc(sizeof(char) * STRMAX);
	char * filename = malloc(sizeof(char) * STRMAX);
	FILE * list;
	int ret;
        #if defined(BID) 
	  _IDEC_round rnd = BID_ROUNDING_DOWN;
	#else
	  decContext rnd;
	  rnd.round = DEC_ROUND_FLOOR;
	#endif
	int i = 0;

        if ((list=fopen(INPUTS, "r")) != NULL) {
          while ((ret = fscanf(list, "%s", filename))==1){
	    fprintf(stderr, "Processing %s\n", filename);
            strcpy(strin, "./ascii/");
            strcpy(strout_bin, "./bin/");
            strcat(strin, filename);
            strcat(strout_bin, filename);
	    #if defined(BID)
              #if defined(WIDTH64)
                strcat(strout_bin, ".bid64");
              #else
                strcat(strout_bin, ".bid128");
              #endif 
	    #else
              #if defined(WIDTH64)
                strcat(strout_bin, ".d64");
              #else
                strcat(strout_bin, ".d128");
              #endif 
	    #endif
	    FILE *input = fopen(strin, "r");
	    FILE *output = fopen(strout_bin, "wb");
	    while(fscanf(input, "%s\n", strin) == 1) {
	      #if defined(BID)
                #if defined(WIDTH64)
	          __bid64_from_string(&val, strin, &rnd);
	          fwrite(&val, sizeof(BID_UINT64), 1, output);
                #else
	          __bid128_from_string(&val, strin, &rnd);
	          fwrite(&val, sizeof(BID_UINT128), 1, output);
                #endif
	      #else
                #if defined(WIDTH64)
	          decimal64FromString(&val, strin, &rnd);
	          fwrite(&val, sizeof(decimal64), 1, output);
                #else
	          decimal128FromString(&val, strin, &rnd);
	          fwrite(&val, sizeof(decimal128), 1, output);
                #endif
	      #endif
	      i++;
	    }
	  }
	}

	printf("Done. Processed %d numbers.\n", i);

}
