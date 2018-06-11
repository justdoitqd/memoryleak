#include <stdio.h>
#include <stdlib.h>


int main(int argc, void* argv[])
{
	char* c;
	int i = 0;
	while (1)
	//while (i++ < 10)
	{
		sleep(1);
		c = malloc(1024);
		*c = 'a';
		sleep(1);
		free(c);
	}
	return 0;
}
