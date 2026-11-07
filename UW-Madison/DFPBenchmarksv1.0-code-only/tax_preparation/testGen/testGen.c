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
        //fprintf(testfile, "Title:  US Federal 1040 Tax Form - 2006\n\n");
#ifdef ORIG
        fprintf(testfile2, "Title:  US Federal 1040 Tax Form - 2006\n\n");
#endif
    
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
    
        int_val = rand() % 11;
        fprintf(testfile, "Dependents  %d\n", int_val); //0-10
#ifdef ORIG
        fprintf(testfile2, "Dependents  %d\n", int_val);
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
        fprintf(testfile, "L7          %s\n", value); //gross salary: 0-9,999,999
#ifdef ORIG
        fprintf(testfile2, "L7          %s ;\n", value);
#endif
    
        value_gen(value, 1, 4, 1);
        fprintf(testfile, "L8a         %s\n", value); //interest: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L8a         %s ;\n", value);
#endif

        value_gen(value, 1, 4, 1);
        fprintf(testfile, "L9          %s\n", value); //dividend: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L9          %s ;\n", value);
#endif
    
        value_gen(value, 1, 4, 1);
        fprintf(testfile, "L9b         %s\n", value); //qualified dividend: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L9b         %s ;\n", value);
#endif

        value_gen(value, 1, 3, 1);
        fprintf(testfile, "L10         %s\n", value); //Taxable refunds: 0-999
#ifdef ORIG
        fprintf(testfile2, "L10         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L11         %s\n", value); //Alimony received: 0-999
#ifdef ORIG
        fprintf(testfile2, "L11         %s ;\n", value);
#endif

        int_val = (rand()%2) - 1;
        value_gen_prob5(value, 1, 4, int_val);
        fprintf(testfile, "L12         %s\n", value); //Business income/loss: 0-9,999 (negative/positive)
#ifdef ORIG
        fprintf(testfile2, "L12         %s ;\n", value);
#endif

        int_val = (rand()%2) - 1;
        value_gen_prob5(value, 1, 4, int_val);
        fprintf(testfile, "D4          %s\n", value); //Short term gain from 6252 and short-term gain or loss from Forms 4684, 6781, 8824: 0-9,999 (negative/positive)
#ifdef ORIG
        fprintf(testfile2, "D4          %s ;\n", value);
#endif

        int_val = (rand()%2) - 1;
        value_gen_prob5(value, 1, 4, int_val);
        fprintf(testfile, "D5          %s\n", value); //Net short-term gain or loss from partnerships, S corps, estates, trusts from K-1: 0-9,999 (negative/positive)
#ifdef ORIG
        fprintf(testfile2, "D5          %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, -1);
        fprintf(testfile, "D6          %s\n", value); // Carryover short-term loss from 2005: 0-9,999 (negative)
#ifdef ORIG
        fprintf(testfile2, "D6          %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, 1);
        fprintf(testfile, "D11         %s\n", value); //Gain from Form 4797: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "D11         %s ;\n", value);
#endif

        int_val = (rand()%2) - 1;
        value_gen_prob5(value, 1, 4, int_val);
        fprintf(testfile, "D12         %s\n", value); //Partnership net long-term gain or loss: 0-9,999 (negative/positive)
#ifdef ORIG
        fprintf(testfile2, "D12         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, 1);
        fprintf(testfile, "D13         %s\n", value); //Cap Gains Distributions - 1099-DIV col. 2a: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "D13         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, -1);
        fprintf(testfile, "D14         %s\n", value); //Carryover long-term loss from 2003: 0-9,999 (negative)
#ifdef ORIG
        fprintf(testfile2, "D14         %s ;\n", value);
#endif
    
        int_val = rand() % 6;
        fprintf(testfile, "Cap-Gains/Losses %d\n", int_val);
#ifdef ORIG
        fprintf(testfile2, "Cap-Gains/Losses\n");
#endif
        while (int_val) {
            int_val--;
            int day1, month1, year1; //year bought
            int day2, month2, year2; //current year (sold)

            //sell date
            year2 = 6;
            month2 = rand() % 11 + 2;
            day2 = rand() % 28 + 1;

            //buy date
            year1 = rand() % 17 + 90;
            if (year1 >= 100) year1 = year1 - 100;
            do {month1 = rand() % 12 + 1;}
            while (year1 == 6 && month1 >= month2);
            day1 = rand() % 28 + 1;

            value_gen(value, 1, 4, -1);
            fprintf(testfile, "%d-%d-%d   %s\n", month1, day1, year1, value);//buy date and amt: 0-9,999 (negative)
#ifdef ORIG
            fprintf(testfile2, "%s   %d-%d-%d\n", value, month1, day1, year1);
#endif
            value_gen(value, 1, 4, 1);
            fprintf(testfile, "%d-%d-%d   %s\n", month2, day2, year2, value);//sell date and amt: 0-9,999
#ifdef ORIG
            fprintf(testfile2, "%s   %d-%d-%d\n", value, month2, day2, year2);
#endif
        }
    
        int_val = (rand()%2) - 1;
        value_gen_prob5(value, 1, 4, int_val);
        fprintf(testfile, "L14         %s\n", value); //Other gains or losses: 0-9,999 (negative/positive)
#ifdef ORIG
        fprintf(testfile2, ";\nL14         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, 1);
        fprintf(testfile, "L15b        %s\n", value); //Taxable IRA distributions: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L15b        %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, 1);
        fprintf(testfile, "L16b        %s\n", value); //Taxable Pensions/annuities: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L16b        %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, 1);
        fprintf(testfile, "L17         %s\n", value); //Rent realestate, royalties, partnerships, S corp: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L17         %s ;\n", value);
#endif

        int_val = (rand()%2) - 1;
        value_gen_prob5(value, 1, 4, int_val);
        fprintf(testfile, "L18         %s\n", value); //Farm income/loss: 0-9,999 (negative/positive)
#ifdef ORIG
        fprintf(testfile2, "L18         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 4, 1);
        fprintf(testfile, "L19         %s\n", value); //Unemployment compensation: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L19         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L20b        %s\n", value); //Taxable Social security benefits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L20b        %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L21         %s\n", value); //Other income: 0-999
#ifdef ORIG
        fprintf(testfile2, "L21         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L23         %s\n", value); //Educator expenses: 0-999
#ifdef ORIG
        fprintf(testfile2, "L23         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L24         %s\n", value); //Bus. exp.: reservists, artists: 0-999
#ifdef ORIG
        fprintf(testfile2, "L24         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L25         %s\n", value); //Health savings account deduction: 0-999
#ifdef ORIG
        fprintf(testfile2, "L25         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L26         %s\n", value); //Moving expenses: 0-999
#ifdef ORIG
        fprintf(testfile2, "L26         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L27         %s\n", value); //One-half of self-employment tax: 0-999
#ifdef ORIG
        fprintf(testfile2, "L27         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L28         %s\n", value); //Self-employed health insurance deduction: 0-999
#ifdef ORIG
        fprintf(testfile2, "L28         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L29         %s\n", value); //Self-employed SEP, SIMPLE, and qualified plans: 0-999
#ifdef ORIG
        fprintf(testfile2, "L29         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L30         %s\n", value); //Penalty on early withdrawal of savings: 0-999
#ifdef ORIG
        fprintf(testfile2, "L30         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L31a        %s\n", value); //Alimony paid: 0-999
#ifdef ORIG
        fprintf(testfile2, "L31a        %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L32         %s\n", value); //IRA deduction: 0-999
#ifdef ORIG
        fprintf(testfile2, "L32         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L33         %s\n", value); //Student loan interest deduction: 0-999
#ifdef ORIG
        fprintf(testfile2, "L33         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L34         %s\n", value); //Jury Duty pay you gave to your employer: 0-999
#ifdef ORIG
        fprintf(testfile2, "L34         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L35         %s\n", value); //Domestic production activities deduction: 0-999
#ifdef ORIG
        fprintf(testfile2, "L35         %s ;\n", value);
#endif

        int_val = rand() % 10;
        if (int_val == 0) int_val = rand() % 5;
        else int_val = 0;
        fprintf(testfile, "L39         %d\n", int_val); //Number of boxes checked: 0-4
#ifdef ORIG
        fprintf(testfile2, "L39         %d ;\n", int_val);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A1          %s\n", value); //Unreimbursed medical expenses: 0-999
#ifdef ORIG
        fprintf(testfile2, "A1          %s ;\n", value);
#endif

        value_gen(value, 1, 4, 1);
        fprintf(testfile, "A5          %s\n", value); //State and local income taxes: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "A5          %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A6          %s\n", value); //Real estate taxes: 0-999
#ifdef ORIG
        fprintf(testfile2, "A6          %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A7          %s\n", value); //Personal property taxes: 0-999
#ifdef ORIG
        fprintf(testfile2, "A7          %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A8          %s\n", value); //Other taxes: 0-999
#ifdef ORIG
        fprintf(testfile2, "A8          %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A10         %s\n", value); //Home mortgage interest and points: 0-999
#ifdef ORIG
        fprintf(testfile2, "A10         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A11         %s\n", value); //Home mortgage interest not reported to you: 0-999
#ifdef ORIG
        fprintf(testfile2, "A11         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 2, 1);
        fprintf(testfile, "A12         %s\n", value); //Points not reported to you: 0-99
#ifdef ORIG
        fprintf(testfile2, "A12         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A13         %s\n", value); //Investment interest: 0-999
#ifdef ORIG
        fprintf(testfile2, "A13         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A15         %s\n", value); //Charity contributions: 0-999
#ifdef ORIG
        fprintf(testfile2, "A15         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A16         %s\n", value); //Contributions other than cash or check: 0-999
#ifdef ORIG
        fprintf(testfile2, "A16         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A17         %s\n", value); //Carryover from prior year: 0-999
#ifdef ORIG
        fprintf(testfile2, "A17         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A19         %s\n", value); //Casualty or theft loss(es): 0-999
#ifdef ORIG
        fprintf(testfile2, "A19         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A20         %s\n", value); //Unreimbursed employee expenses: 0-999
#ifdef ORIG
        fprintf(testfile2, "A20         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A21         %s\n", value); //Tax preparation fees: 0-999
#ifdef ORIG
        fprintf(testfile2, "A21         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A22         %s\n", value); //Other expenses: 0-999
#ifdef ORIG
        fprintf(testfile2, "A22         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "A27         %s\n", value); //Other deductions: 0-999
#ifdef ORIG
        fprintf(testfile2, "A27         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L47         %s\n", value); //Foreign tax credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L47         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L48         %s\n", value); //Child / dependent care expense credits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L48         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L49         %s\n", value); //Credit for the elderly or the disabled: 0-999
#ifdef ORIG
        fprintf(testfile2, "L49         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L50         %s\n", value); //Education credits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L50         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L51         %s\n", value); //Retirement savings contributions credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L51         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L52         %s\n", value); //Residential energy credits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L52         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L53         %s\n", value); //Child tax credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L53         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L54         %s\n", value); //Credits from Forms 8396 8859 8839: 0-999
#ifdef ORIG
        fprintf(testfile2, "L54         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L55         %s\n", value); //Other credits: 0-999
#ifdef ORIG
        fprintf(testfile2, "L55         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L58         %s\n", value); //Self-employment tax: 0-999
#ifdef ORIG
        fprintf(testfile2, "L58         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 2, 1);
        fprintf(testfile, "L59         %s\n", value); //Social Sec/Medicare tax on tips: 0-99
#ifdef ORIG
        fprintf(testfile2, "L59         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L60         %s\n", value); //Tax on tax-favored accounts: 0-999
#ifdef ORIG
        fprintf(testfile2, "L60         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L61         %s\n", value); //Advance earned income credit payments from W-2: 0-999
#ifdef ORIG
        fprintf(testfile2, "L61         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L62         %s\n", value); //Household employment taxes: 0-999
#ifdef ORIG
        fprintf(testfile2, "L62         %s ;\n", value);
#endif

        value_gen(value, 1, 4, 1);
        fprintf(testfile, "L64         %s\n", value); //Federal income tax withheld, Forms W-2, 1099: 0-9,999
#ifdef ORIG
        fprintf(testfile2, "L64         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L65         %s\n", value); //'06 estimated payments + amnt applied from '05: 0-999
#ifdef ORIG
        fprintf(testfile2, "L65         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L66a        %s\n", value); //Earned income credit (EIC): 0-999
#ifdef ORIG
        fprintf(testfile2, "L66a        %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L67         %s\n", value); //Excess soc. sec. + tier 1 RRTA tax withheld: 0-999
#ifdef ORIG
        fprintf(testfile2, "L67         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 3, 1);
        fprintf(testfile, "L68         %s\n", value); //Additional child tax credit: 0-999
#ifdef ORIG
        fprintf(testfile2, "L68         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 2, 1);
        fprintf(testfile, "L69         %s\n", value); //Amnt paid in filing extension req.: 0-99
#ifdef ORIG
        fprintf(testfile2, "L69         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 2, 1);
        fprintf(testfile, "L70         %s\n", value); //Other payments forms 2439, 4136, 8885.: 0-99
#ifdef ORIG
        fprintf(testfile2, "L70         %s ;\n", value);
#endif

        value_gen_prob5(value, 1, 2, 1);
        fprintf(testfile, "L71         %s\n", value); //Credit for telephone excise tax paid: 0-99
#ifdef ORIG
        fprintf(testfile2, "L71         %s ;\n", value);
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
