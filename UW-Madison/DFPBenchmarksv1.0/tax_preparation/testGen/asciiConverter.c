#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//#define ASCII_TEST "ascii_test.tx"
#define INPUTS "inputs.txt"

#define MAX_ACCT_NAME_LENGTH 24
#define MAX_NAME_LENGTH 24
#if defined(WIDTH64)
 #define MAX_BUF_SIZE 16
#endif
#if defined(WIDTH128)
 #define MAX_BUF_SIZE 34
#endif

#if defined(BID)
 #if defined(WIDTH64)
  #define dec_t BID_UINT64
 #endif
 #if defined(WIDTH128)
  #define dec_t BID_UINT128
 #endif
 #include "../bid_conf.h"
 #include "../bid_functions.h"
 #define context_t _IDEC_round
#else
 #if defined(WIDTH64)
  #include "../decimal64.h"
  #define dec_t decimal64
 #endif
 #if defined(WIDTH128)
  #include "../decimal128.h"
  #define dec_t decimal128
 #endif
 #define context_t decContext
#endif
 
typedef struct line_struct{
   char line_name[MAX_NAME_LENGTH];
   dec_t amt;
}line_t;

int main(int argc, char *argv[]){
    FILE *inp = NULL;
    FILE *oup = NULL;
    char strin[100];
    char strout64[100];
    char msg[100];
    char str1[MAX_BUF_SIZE], str2[MAX_BUF_SIZE];
    FILE *cmp = NULL;
#ifdef DEBUG
    int length;
#endif
    size_t size;
    int ret;
    //decContext set;
    context_t set;
    char filename[100];
    FILE *acct_list_fp;

    line_t readIn;

    line_t line;
    
    printf("***Running asciiConverter***\n\n");
    
    //Initialize the decContext
    #if defined(BID)
     set = BID_ROUNDING_TIES_AWAY;
    #else
     #if defined(WIDTH64)
      decContextDefault(&set, DEC_INIT_DECIMAL64); // initialize
     #endif
     #if defined(WIDTH128)
      decContextDefault(&set, DEC_INIT_DECIMAL128); // initialize
     #endif
    #endif

    if ((acct_list_fp=fopen(INPUTS, "r")) != NULL) {
        while ((ret = fscanf(acct_list_fp, "%s", filename))==1){
	  fprintf(stderr, "Processing %s\n", filename);
            strcpy(strin, "./ascii/");
            strcpy(strout64, "./bin/");
            errno = 0;
            strcat(strin, filename);
            strcat(strout64, filename);
	    #if defined(BID)
             #if defined(WIDTH64)
              strcat(strout64, ".bid64");
             #endif
             #if defined(WIDTH128)
              strcat(strout64, ".bid128");
             #endif
	    #else
             #if defined(WIDTH64)
              strcat(strout64, ".d64");
             #endif
             #if defined(WIDTH128)
              strcat(strout64, ".d128");
             #endif
	    #endif
            if ((inp = fopen(strin, "r"))==NULL){
                sprintf(msg, "%s cannt be open for read\n", strin);
                perror(msg);
                exit(-1);
            }
            if ((oup = fopen(strout64, "wb"))==NULL){
                sprintf(msg, "%s cannt be open for write\n", strout64);
                perror(msg);
                exit(-2);
            }
            while ((ret = fscanf(inp, "%s %s", str1, str2))!=EOF){
                #if defined(BID)
                 #if defined(WIDTH64)
                  __bid64_from_string(&(line.amt), str2, &set);
                 #endif
                 #if defined(WIDTH128)
                  __bid128_from_string(&(line.amt), str2, &set);
                 #endif
                #else 
                 #if defined(WIDTH64)
                  decimal64FromString(&(line.amt), str2, &set);
                 #endif
                 #if defined(WIDTH128)
                  decimal128FromString(&(line.amt), str2, &set);
                 #endif
                #endif
                strcpy(line.line_name, str1);
                size = fwrite(&line, sizeof(line_t), 1, oup);
		if (size!=1){
                    if (size!=0) exit(-1);
                }
            }
            fclose(inp);
            fclose(oup);
        }
    }
    fclose(acct_list_fp);
    return 0;
}
