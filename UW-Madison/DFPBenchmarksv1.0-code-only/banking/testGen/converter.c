#include <stdio.h>
#include "decimal64.h"
#define MAX_ACCT_NAME_LENGTH 16

typedef struct cmd64_struct{
  char acct_no[MAX_ACCT_NAME_LENGTH];
  decimal64 fund;
  decimal64 str1; //can be used for downPayment in mortgage
  int action;
  //1, 3, or 6 for CD and length for ARM
  //Also the currency type for Credit Card Debt
  int length;
  // 0: Checking, 1: CD, 2: Cash, 3: 15-Year FRM, 4: 30-Year FRM
  // 5: 1-Year ARM, 6: 5/1-Year ARM, 7:Credit Card
  int acct_type;
  //can be "currency unit" for CD and Credit Card,
  //"auto_pay" and currency for mortgage
  //payment/expense type for Credit Card if it's account_pay
  int select;
}cmd64_t;

int main(int argc, char *argv[]){
  int index=0;
  cmd64_t this_cmd;
  char value[30], str1[30];
  int combined_value1, combined_value2;
  decContext set;
  char bin_file[100];
  FILE *ascii_fp, *bin_fp;
  decContextDefault(&set, DEC_INIT_DECIMAL64);
  if (argc!=2) {
    fprintf(stderr, "[cmd] ascii_file\n");
    exit(-1);
  }
  sprintf(bin_file, "%s_bin", argv[1]);
  ascii_fp = fopen(argv[1], "r");
  if (ascii_fp==NULL){
    fprintf(stderr, "open ascii file fail\n");
    exit(-1);
  }
  bin_fp = fopen(bin_file, "wb");
  if (bin_fp==NULL){
    fprintf(stderr, "open bin file fail\n");
    exit(-1);
  }

  while (index<29){
    fscanf(ascii_fp, "%s %d %s %d %d %d %s", &this_cmd.acct_no,
	   &this_cmd.action, value, &this_cmd.acct_type, &this_cmd.length,
	   &this_cmd.select, str1);
    decimal64FromString(&this_cmd.fund, value, &set);
    decimal64FromString(&this_cmd.str1, str1, &set);
    fwrite(&this_cmd, sizeof(cmd64_t), 1, bin_fp);
    if (strcmp(this_cmd.acct_no, "0")==0){
      index++;
    }
  }  
  return 0;
}
