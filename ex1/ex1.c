/*************************************
* Lab 2 Exercise 1
* Name:
* Student Id: A????????
* Lab Group: B??
*************************************
Note: Duplicate the above and fill in 
for the 2nd member if  you are on a team
--------------------------------------
Warning: Make sure your code works on
compute cluster node (Linux on x86)
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>   
#include <sys/stat.h>
#include <unistd.h>     //for fork()
#include <sys/wait.h>   //for wait()

int main()
{
    int nChild;

    //Read the number of child
    scanf("%d", &nChild);

    pid_t pids[nChild];
    //Spawn child processes
    for (int i = 0; i < nChild; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            printf("Child %d[%d]: Hello!\n", i, getpid());
            //exit(0);
            return 0;
        }
    }

    int status;
    for (int i = 0; i < nChild; i++) {
        waitpid(pids[i], &status, 0);
        printf("Parent: Child %d[%d] done.\n",i, pids[i]);
    }

    return 0;
}
