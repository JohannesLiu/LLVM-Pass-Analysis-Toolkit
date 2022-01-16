#include<stdio.h>
#include<stdlib.h>

void check_flag();
void printCounter();
void counterAdd();
void updateNumberator();
void updateDenominator();


int counter = 0;
int numberator = 0;
int denominator = 0;

int maxResult = 999;
int minResult = -1;

int localMax = 0;
int localMin = 0;

void printCounter(){
    printf("counter = %d\n", counter);
}

void counterAdd()
{
    counter = counter + 1;
}

void printMaxResult(){
    printf("max result = %f\n", maxResult);
}

void printMinResult(){
    printf("min result = %f\n", minResult);
}

void resetLocalResult(){
    localMax = 0;
    localMin = 0;
}

void updateLocalMinResult(){
    localMin += 1;
}

void updateLocalMaxResult(){
    localMax += 1;
}

void updateGlobalMinResult(){
    if(localMin<minResult){
       minResult = localMin;
    }
}

void updateGlobalMaxResult(){
    if(localMax<maxResult){
        maxResult = localMax;
    }
}


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
