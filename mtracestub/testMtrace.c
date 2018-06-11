#include <stdio.h>
#include <stdlib.h>

#include <mcheck.h>

int main(int argc ,void **argv)
{
	pid_t childPid = 0;
	char* c = 0;
	mtrace();
#if 1
	childPid = fork();
	if (childPid == 0)
	{
		char* filename = "/home/simon/test/mtracestub/test";
		execv(filename, 0);
		// child will change here.
	}
#endif
	c=malloc(7);

	// Enable mtrace memory leak checking
	//                 //         // Allocate some memory, but
	//                 don't free it
	//                         //
	free(c); 
	while(1);
	//free(c); 
	
	muntrace();
	return 0;
}
