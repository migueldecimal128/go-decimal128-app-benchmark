
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#define COUNTRY 12

void value_gen(char *name, int minimum, int range){
  char c;
  int length;
  int index=0;
#ifdef HAS_EXP
  int exp;
  char *ptr;
#endif
  c = 43;
  name[index++] = c;
  length = rand()%range + minimum;
  for (; index <= length; index++){
    c = (char) (rand() % 10 + 48);
    name[index] = c;
  }
  name[index++] = 'E';
#ifdef HAS_EXP
  ptr = &name[index];
  exp = rand()%767-398;
  sprintf(ptr, "%d", exp);
#else
  if (rand()%2==0){
    c = (char)'0';
    name[index++] = c;
  }
  else {
    c = (char)'-';
    name[index++] = c;
    c = (char)'2';
    name[index++] = c;
  }
  name[index] = '\0';
#endif
}

int main(int argc, char *argv[]){
  char str[30];
  char fileout[255];
  //srand(time(NULL));
  srand(1);
  assert(argc==3);
  FILE *asciiFile;
  int test;
  int index;
  test = atoi(argv[2]);
  sprintf(fileout, "./ascii/%s.txt", argv[1]);
  if ((asciiFile=fopen(fileout,"wb"))==NULL) { // open dec file for writing
    fclose(asciiFile);
    printf("Error: file '%s' could not be opened\n", argv[1]);
    exit(0);
  }
  for (index=0; index<test; index++) {
    int from, to;
    from = rand()%COUNTRY;
    to = rand()%COUNTRY;
    //output_data_dec.from = from;
    //output_data_bid.from = from;
    //output_data_dec.to = to;
    //output_data_bid.to = to;
    value_gen(str, 1, 15);
    fprintf(asciiFile, "%d\n", from);
    fprintf(asciiFile, "%d\n", to);
    fprintf(asciiFile, "%s\n", str);
    //decimal64FromString(&output_data_dec.inp, str, &decRnd);
    //__bid64_from_string(&output_data_bid.inp, str, &bidRnd);
    //fwrite(&output_data_dec, sizeof(data_t_dec), 1, oup_dec);
    //fwrite(&output_data_bid, sizeof(data_t_bid), 1, oup_bid);
  }
  return 0;
}
