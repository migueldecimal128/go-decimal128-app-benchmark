#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

void value_gen_prob5(char *name, int minimum, int range, int sign);
void value_gen(char *name, int minimum, int range, int sign);
void add_cents(char *value);

int main(int argc, char *argv[]){
    int int_val, status, exemp, itemized, limited, limited_l6;
    float income, perc;
    char value[15], value2[15], AGI[15];
    FILE * testfile;
    int loop, index;
#ifdef ORIG
    char filename[100];
    FILE * testfile2;
#endif
    
    assert(argc==3);
    loop = atoi(argv[2]);

    srand( (unsigned int) time( NULL ));

    char filename_str[100];
    sprintf(filename_str, "ascii/%s.txt", argv[1]);
    
    if ((testfile = fopen(filename_str, "w"))==NULL) {
        fprintf(stderr, "%s cannot be opened for write\n", argv[1]);
        exit(-2);
    }

#ifdef ORIG
    strcpy(filename, "orig_");
    strcat(filename, argv[1]);
    if ((testfile2 = fopen(filename, "w"))==NULL) {
        fprintf(stderr, "%s cannot be opened for write\n", filename);
        exit(-2);
    }
#endif

    for (index=0; index<loop; index++){
        //fprintf(testfile, "Title:  US Federal 1040 Tax Form - 2006\n\n");
#ifdef ORIG
        fprintf(testfile2, "Title:  US Federal 1040 Tax Form - 2006\n\n");
#endif
    
        status = rand() % 5 + 1;
        fprintf(testfile, "Status      %d\n", status); //1-5
#ifdef ORIG
        switch(status) {
        case 1: fprintf(testfile2, "Status      Single\n"); break;
        case 2: fprintf(testfile2, "Status      Married/Joint\n"); break;
        case 3: fprintf(testfile2, "Status      Married/Sep\n"); break;
        case 4: fprintf(testfile2, "Status      Head_of_House\n"); break;
        case 5: fprintf(testfile2, "Status      Widow(er)\n"); break;
        }
#endif
    
        fprintf(testfile, "Exception   0\n"); //0
#ifdef ORIG
        fprintf(testfile2, "Exception   0\n");
#endif
        
        int_val = rand() % 10;
        if (int_val == 0) {
            switch(status) {
            case 1:
            case 4: exemp = rand() % 3; break;
            case 2:
            case 3:
            case 5: exemp = rand() % 5; break;
            }
        }
        else exemp = 0;
        fprintf(testfile, "Exemptions  %d\n", exemp); //Number of boxes checked: 0-2
#ifdef ORIG
        fprintf(testfile2, "Exemptions  %d ;\n", exemp);
#endif

        int_val = rand() % 5;
        if (int_val == 0) itemized = rand() % 2;
        else              itemized = 0;
        fprintf(testfile, "Itemized    %d\n", itemized); //Itemized
#ifdef ORIG
        fprintf(testfile2, "Itemized    %d ;\n", itemized);
#endif

        //Calculating Adjusted Gross Income
        int_val = rand() % 10000 + 1;
        if (int_val < 141) {
            strcpy(AGI, "0");
        }
        else if (int_val < 1024) {
            char c;
            int inc = rand() % 5000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 1942) {
            char c;
            int inc = rand() % 5000 + 5000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 2824) {
            char c;
            int inc = rand() % 5000 + 10000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 3677) {
            char c;
            int inc = rand() % 5000 + 15000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 4411) {
            char c;
            int inc = rand() % 5000 + 20000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 5055) {
            char c;
            int inc = rand() % 5000 + 25000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 6107) {
            char c;
            int inc = rand() % 10000 + 30000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 6906) {
            char c;
            int inc = rand() % 10000 + 40000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 8271) {
            char c;
            int inc = rand() % 25000 + 50000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 9036) {
            char c;
            int inc = rand() % 25000 + 75000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 9772) {
            char c;
            int inc = rand() % 100000 + 100000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 9950) {
            char c;
            int inc = rand() % 300000 + 200000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 9983) {
            char c;
            int inc = rand() % 500000 + 500000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 9991) {
            char c;
            int inc = rand() % 500000 + 1000000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 9994) {
            char c;
            int inc = rand() % 500000 + 1500000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 9999) {
            char c;
            int inc = rand() % 3000000 + 2000000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else if (int_val < 10000) {
            char c;
            int inc = rand() % 5000000 + 5000000;
            sprintf(AGI, "%d", inc);
            add_cents(AGI);
        }
        else { //10000
            strcpy(AGI, "10000000");
        }

        income = (float) atof(AGI);

        if ((income > 150500.0 && status != 3) ||
            (income > 75250.0 && status == 3)) {
            int_val = rand() % 5;
            if (int_val == 0) limited = 1;
            else              limited = 0;
        }
        else                  limited = 0;

        if (income > 112875.0) {
            int_val = rand() % 5;
            if (int_val == 0) limited_l6 = 1;
            else              limited_l6 = 0;
        }
        else                  limited_l6 = 0;
        
        fprintf(testfile, "Limited     %d\n", limited); //0/1
        fprintf(testfile, "LimitedL6   %d\n", limited_l6); //0/1
        fprintf(testfile, "WS2         %d\n", 3300.0 * exemp);
#ifdef ORIG
        fprintf(testfile2, "Limited     %d\n", limited); //0/1
        fprintf(testfile2, "LimitedL6   %d\n", limited_l6); //0/1
        fprintf(testfile2, "WS2         %d\n", 3300.0 * exemp);
#endif

        
        if (limited_l6 == 0) {
            value_gen(value, 1, 5, 1);
            fprintf(testfile, "WS10        %s\n", value); //interest: 0-99,999
#ifdef ORIG
            fprintf(testfile2, "WS10        %s ;\n", value);
#endif
        }
        else {
            fprintf(testfile, "WS10        %d\n", 3300.0 * exemp);
#ifdef ORIG
            fprintf(testfile2, "WS10        %d\n", 3300.0 * exemp);
#endif
        }
        
        fprintf(testfile, "AGI         %s\n", AGI); //gross salary: 0-9,999,999
#ifdef ORIG
        fprintf(testfile2, "AGI         %s ;\n", AGI);
#endif
        
        value_gen(value, 1, 7, 1);  //9,999,999
        while (((float) atof(value)) > income)
            value_gen(value, 1, 7, 1);  //9,999,999
        fprintf(testfile, "L6          %s\n", value); // Taxable income from Fed1040 Line 43.
#ifdef ORIG
        fprintf(testfile2, "L6          %s ;\n", value);
#endif

        int_val = rand() % 3;
        if (int_val == 0) {
            int_val = rand() % 100;
            perc = ((float) int_val) / 100.0;
        }
        else {
            perc = 1.0;
        }
        fprintf(testfile, "L12         %2.2f\n", perc); //0-10
#ifdef ORIG
        fprintf(testfile2, "L12         %2.2f\n", perc);
#endif
        
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L15         %s\n", value); //tax credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L15         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L17         %s\n", value); //consumer use tax: 0-999
#ifdef ORIG
        fprintf(testfile2, "L17         %s ;\n", value);
#endif

        value_gen(value, 1, 4, 1);
        fprintf(testfile, "L19         %s\n", value); //income tax withheld, Forms W-2, 1099: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L19         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L20         %s\n", value); //other tax payments: 0-999
#ifdef ORIG
        fprintf(testfile2, "L20         %s ;\n", value);
#endif

        {
            float temp;
            value_gen(value, 1, 4, 1);
            value_gen(value2, 1, 4, 1);
            temp = (float) (atof(value) + atof(value2));
            fprintf(testfile, "L29        %6.2f\n", temp); //itemized deductions: 0-9,999
#ifdef ORIG
            fprintf(testfile2, "L29        %6.2f ;\n", temp);
#endif
        }

        value_gen(value, 1, 4, 1);
        fprintf(testfile, "L32         %s\n", value); //State and local income taxes: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L32         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L35         %s\n", value); //interest income: 0-999
#ifdef ORIG
        fprintf(testfile2, "L35         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L36         %s\n", value); //additional 1st yr...: 0-999
#ifdef ORIG
        fprintf(testfile2, "L36         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L37         %s\n", value); //other additions to income: 0-999
#ifdef ORIG
        fprintf(testfile2, "L37         %s ;\n", value);
#endif

        value_gen(value, 1, 3, 1);
        fprintf(testfile, "L39         %s\n", value); //Taxable refunds: 0-999
#ifdef ORIG
        fprintf(testfile2, "L39         %s ;\n", value);
#endif
        
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L40         %s\n", value); //Interest from US or NC obligations: 0-999
#ifdef ORIG
        fprintf(testfile2, "L40         %s ;\n", value);
#endif
        
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L41         %s\n", value); //Taxable soc sec and RR benefits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L41         %s ;\n", value);
#endif
        
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L42         %s\n", value); //Retirement benefits (Bailey): 0-999
#ifdef ORIG
        fprintf(testfile2, "L42         %s ;\n", value);
#endif
        
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L43         %s\n", value); //Other retirement benefits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L43         %s ;\n", value);
#endif
        
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L44         %s\n", value); //Severance wages: 0-999
#ifdef ORIG
        fprintf(testfile2, "L44         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L45         %s\n", value); //Adjust additional 1st yr....: 0-999
#ifdef ORIG
        fprintf(testfile2, "L45         %s ;\n", value);
#endif
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L46         %s\n", value); //Other deductions: 0-999
#ifdef ORIG
        fprintf(testfile2, "L46         %s ;\n", value);
#endif

    }
    fclose(testfile);
#ifdef ORIG
    fclose(testfile2);
#endif

    return 0;
}

void value_gen_prob5(char *name, int minimum, int range, int sign) {
    int int_val;
    int_val = rand() % 5;
    if (int_val == 0) value_gen(name, minimum, range, sign);
    else strcpy(name, "0");   
}

void value_gen(char *name, int minimum, int range, int sign){
    char c;
    int length=0;
    int index=0;

    if (sign < 0) {
        c = 45;
        name[index++] = c;
        length = 1;
    }
    
    length += rand()%range + minimum;
    for (; index < length; index++){
        c = (char) (rand() % 10 + 48);
        name[index] = c;
    }
    name[index] = '\0';
    add_cents(name);
    /*
    name[index++] = '.';
    c = (char) (rand() % 10 + 48);
    name[index++] = c;
    c = (char) (rand() % 10 + 48);
    name[index++] = c;
    name[index] = '\0';
    */
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
