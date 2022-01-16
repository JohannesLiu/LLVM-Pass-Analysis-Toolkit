#include <stdio.h>
#include <stdlib.h>



void printCounter();
void counterAdd();

int counter = 0;


void printCounter(){
  printf("counter = %d\n", counter);
}

void counterAdd()
{
  counter = counter + 1;
}