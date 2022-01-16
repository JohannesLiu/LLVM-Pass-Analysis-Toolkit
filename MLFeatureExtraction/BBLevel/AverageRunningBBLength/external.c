#include<stdio.h>
#include<stdlib.h>

void check_flag();
void updateNumberator();
void updateDenominator();

int numberator = 0;
int denominator = 0;

void printResult(){
    printf("counter = %f\n", numberator/denominator);
}

void updateNumberator()
{
  numberator = numberator + 1;
}

void updateDenominator()
{
  denominator = denominator + 1;
}
