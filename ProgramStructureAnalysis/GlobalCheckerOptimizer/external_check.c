#include <stdio.h>
#include <stdlib.h>
void global_add(long long instruction1,long long instruction2);
void global_check();
long long global_var1 = 0, global_var2 = 0 ;

void global_add(long long instruction1,long long instruction2)
{
	global_var1 = global_var1 + instruction1 ;
	global_var2 = global_var2 + instruction2 ;
}

void global_check()
{
	if(global_var1!=global_var2)
	{
		exit(99);
	}
}
