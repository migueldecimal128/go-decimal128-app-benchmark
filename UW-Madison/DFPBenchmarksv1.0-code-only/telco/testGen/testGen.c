#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define MINIMUM 0
#define RANGE 100

int value_gen(int minimum, int range);

int main(int argc, char *argv[]){

    int tempNum;
    FILE * asciiFile;
    int loopMax;
    int index;

    assert(argc==3);
    loopMax = atoi(argv[2]);
    
    srand( (unsigned int) time( NULL ));

    if ((asciiFile = fopen(argv[1], "w"))==NULL) {
        fprintf(stderr, "%s cannot be opened for write\n", argv[1]);
        exit(-2);
    }

    for (index=0; index<loopMax; index++){
        tempNum = value_gen(MINIMUM, RANGE);
        fprintf(asciiFile, "%d\n", tempNum); // print number to file
    }
    fclose(asciiFile);
    return 0;
}

int value_gen(int minimum, int range){
    // Pick a number between minimum and (minimum + range)
    return ((rand() % range) + minimum);
}

void add_cents(char *value){
    int len;
    char c;

    len = strlen(value);
    value[len++] = '.';
    c = (char) (rand() % 10 + 48);
    value[len++] = c;
    c = (char) (rand() % 10 + 48);
    value[len++] = c;
    value[len] = '\0';
}
