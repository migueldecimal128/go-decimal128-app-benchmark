#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

void value_gen_prob5(char *name, int minimum, int range, int sign);
void value_gen(char *name, int minimum, int range, int sign);
void add_cents(char *value);

int main(int argc, char *argv[]){
    int int_val;
    char value[15];
    FILE * testfile;
    int loop;
    int index;
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
        //fprintf(testfile, "Title:  CA State Tax Form 540 - 2006\n\n");
#ifdef ORIG
        fprintf(testfile2, "Title:  CA State Tax Form 540 - 2006\n\n");
#endif

        //Status
        int_val = rand() % 5 + 1;
        fprintf(testfile, "Status      %d\n", int_val); //1-5
#ifdef ORIG
        switch(int_val) {
        case 1: fprintf(testfile2, "Status      Single\n"); break;
        case 2: fprintf(testfile2, "Status      Married/Joint\n"); break;
        case 3: fprintf(testfile2, "Status      Married/Sep\n"); break;
        case 4: fprintf(testfile2, "Status      Head_of_House\n"); break;
        case 5: fprintf(testfile2, "Status      Widow(er)\n"); break;
        }
#endif

        //Are you a dependent?
        int_val = rand() % 2;
        fprintf(testfile, "L6          %d\n", int_val);
#ifdef ORIG
        switch (int_val) {
        case 0: fprintf(testfile2, "L6          no ;\n"); break;
        case 1: fprintf(testfile2, "L6          yes ;\n"); break;
        }
#endif
        
        //Blind
        int_val = rand() % 10;
        if (int_val == 0) int_val = rand() % 3;
        else int_val = 0;
        fprintf(testfile, "L8          %d\n", int_val); //Number of boxes checked: 0-2
#ifdef ORIG
        fprintf(testfile2, "L8          %d ;\n", int_val);
#endif
        
        //Over 65?
        int_val = rand() % 10;
        if (int_val == 0) int_val = rand() % 3;
        else int_val = 0;
        fprintf(testfile, "L9          %d\n", int_val); //Number of boxes checked: 0-2
#ifdef ORIG
        fprintf(testfile2, "L9          %d ;\n", int_val);
#endif

        //Dependents
        int_val = rand() % 11;
        fprintf(testfile, "L10         %d\n", int_val); //0-10
#ifdef ORIG
        fprintf(testfile2, "L10         %d\n", int_val);
#endif
        
        
        int_val = rand() % 10000 + 1;
        if (int_val < 141) {
            strcpy(value, "0");
        }
        else if (int_val < 1024) {
            char c;
            int inc = rand() % 5000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 1942) {
            char c;
            int inc = rand() % 5000 + 5000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 2824) {
            char c;
            int inc = rand() % 5000 + 10000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 3677) {
            char c;
            int inc = rand() % 5000 + 15000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 4411) {
            char c;
            int inc = rand() % 5000 + 20000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 5055) {
            char c;
            int inc = rand() % 5000 + 25000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 6107) {
            char c;
            int inc = rand() % 10000 + 30000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 6906) {
            char c;
            int inc = rand() % 10000 + 40000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 8271) {
            char c;
            int inc = rand() % 25000 + 50000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 9036) {
            char c;
            int inc = rand() % 25000 + 75000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 9772) {
            char c;
            int inc = rand() % 100000 + 100000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 9950) {
            char c;
            int inc = rand() % 300000 + 200000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 9983) {
            char c;
            int inc = rand() % 500000 + 500000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 9991) {
            char c;
            int inc = rand() % 500000 + 1000000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 9994) {
            char c;
            int inc = rand() % 500000 + 1500000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 9999) {
            char c;
            int inc = rand() % 3000000 + 2000000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else if (int_val < 10000) {
            char c;
            int inc = rand() % 5000000 + 5000000;
            sprintf(value, "%d", inc);
            add_cents(value);
        }
        else { //10000
            strcpy(value, "10000000");
        }

        //value_gen(value, 1, 7, 1);
        fprintf(testfile, "L12         %s\n", value); //CA salary: 0-9,999,999
        fprintf(testfile, "L13         %s\n", value); //gross salary: 0-9,999,999
#ifdef ORIG
        fprintf(testfile2, "L12         %s ;\n", value);
        fprintf(testfile2, "L13         %s ;\n", value);
#endif

        //CA adjustments Column B
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L14         %s\n", value); //CA Adjustments Col B: 0-999
#ifdef ORIG
        fprintf(testfile2, "L14         %s ;\n", value);
#endif

        //CA adjustments Column C
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L16         %s\n", value); //CA Adjustments Col C: 0-999
#ifdef ORIG
        fprintf(testfile2, "L16         %s ;\n", value);
#endif

        //Deductions
        value_gen_prob5(value, 1, 4, 1);
        fprintf(testfile, "L18         %s\n", value); //Deductions: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L18         %s ;\n", value);
#endif
        
        //Taxes on distributions
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L23         %s\n", value); //Taxes on distributions: 0-999
#ifdef ORIG
        fprintf(testfile2, "L23         %s ;\n", value);
#endif

        //Special credit
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L25         %s\n", value); //Special credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L25         %s ;\n", value);
#endif

        //Special credit
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L26         %s\n", value); //Special credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L26         %s ;\n", value);
#endif

        //Special credit
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L27         %s\n", value); //Special credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L27         %s ;\n", value);
#endif

        //Nonrefundable renter's credit
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L28         %s\n", value); //Nonrefundable renter's credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L28         %s ;\n", value);
#endif

         //Alternative minimum tax
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L31         %s\n", value); //Alternative minimum tax: 0-999
#ifdef ORIG
        fprintf(testfile2, "L31         %s ;\n", value);
#endif

        //Mental Health Services Tax
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L32         %s\n", value); //Mental Health Services Tax: 0-999
#ifdef ORIG
        fprintf(testfile2, "L32         %s ;\n", value);
#endif

        //Other taxes
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L33         %s\n", value); //Other taxes: 0-999
#ifdef ORIG
        fprintf(testfile2, "L33         %s ;\n", value);
#endif
        
        //CA state income tax withheld
        value_gen(value, 1, 4, 1);
        fprintf(testfile, "L36         %s\n", value); //CA state income tax withheld: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L36         %s ;\n", value);
#endif
        
        //Estimated tax paid
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L37         %s\n", value); //Estimated tax paid: 0-999
#ifdef ORIG
        fprintf(testfile2, "L37         %s ;\n", value);
#endif

        //Realestate withholding
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L38         %s\n", value); //Realestate withholding: 0-999
#ifdef ORIG
        fprintf(testfile2, "L38         %s ;\n", value);
#endif
        
        //Excess SDI
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L39         %s\n", value); //Excess SDI: 0-999
#ifdef ORIG
        fprintf(testfile2, "L39         %s ;\n", value);
#endif
        
        //Child / dependent care credits
        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L43         %s\n", value); //Child / dependent care credits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L43         %s ;\n", value);
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
