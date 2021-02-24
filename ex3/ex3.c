/*************************************
* Lab 2 Exercise 3
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
#include <fcntl.h>      //For stat()
#include <sys/types.h>   
#include <sys/stat.h>
#include <sys/wait.h>   //for waitpid()
#include <unistd.h>     //for fork(), wait()
#include <string.h>     //for string comparison etc
#include <stdlib.h>     //for malloc()


char** split( char* input, char* delimiter, int maxTokenNum, int* readTokenNum )
//Assumptions:
//  - the input line is a string (i.e. with NULL character at the end)
//  - the delimiter is a string of possible delimiters, each delimiter is single chracter
//Behavior:
//  - Split up to and include maxTokenNum from the input string
//Return: Tokenized substrings as array of strings
//        readTokenNum stores the total number of tokens
//Note:
//  - Must use the freeTokenArray to free memory after use!
//  don't need to touch this also
{
    char** tokenStrArr;
    char* tStart;   //start of token
    int i, strSize;      

    //allocate token array, each element is a char*
    tokenStrArr = (char**) malloc(sizeof(char*) * maxTokenNum );

    //Nullify all entries
    for (i = 0; i < maxTokenNum; i++){
        tokenStrArr[i] = NULL;
    }

    //Look for next token as separated by the delimiter
    tStart = strtok(input, delimiter); 

    i = 0;
    while (i < maxTokenNum && tStart){

        strSize = strlen(tStart);

        //Allocate space for token string. +1 for null terminator
        tokenStrArr[i] = (char*) malloc(sizeof(char) * (strSize + 1) );

        strcpy(tokenStrArr[i], tStart);    

        i++;
        tStart = strtok(NULL, delimiter); 
    }

    //return number of token read
    *readTokenNum = i;

    return tokenStrArr;
}

// don't need to touch this
void freeTokenArray(char** strArr, int size)
{
    int i;

    //Free each string first
    for (i = 0; i < size; i++){
        if (strArr[i])
            free( strArr[i] );
        strArr[i] = NULL;
    }

    //Free the entire array
    free( strArr );

    //Note: Caller still need to set the strArr parameter to NULL
    //      afterwards
}

void showPath(char path[]) {
    printf("%s\n", path);
}

void setPath(char path[], char* newPath) {
    strcpy(path, newPath);
}

void runCommandAndWaitResult(char** argv, char* commandName, char* path, int* result) {
    char* wholePath = malloc(strlen(commandName) + strlen(path) + 2);
    strcpy(wholePath, path);
    strcat(wholePath, "/");
    strcat(wholePath, commandName);
    struct stat *statbuf = malloc(sizeof(struct stat));
    if (stat(wholePath, statbuf) != 0) {
        printf("%s not found\n", commandName);
    } else {
        if (fork() == 0) {
            execv(wholePath, argv);
        } else {
            int wstatus;
            wait(&wstatus);
            *result = WEXITSTATUS(wstatus);
        }
    }
}

void runCommandWithoutWait(char** argv, char* commandName, char* path, pid_t pidArr[], int pidIdx) {
    char* wholePath = malloc(strlen(commandName) + strlen(path) + 2);
    strcpy(wholePath, path);
    strcat(wholePath, "/");
    strcat(wholePath, commandName);
    struct stat *statbuf = malloc(sizeof(struct stat));
    if (stat(wholePath, statbuf) != 0) {
        printf("%s not found\n", commandName);
    } else {
        int childId = fork();
        if (childId == 0) {
            execv(wholePath, argv);
        } else {
            pidArr[pidIdx] = childId;
            printf("Child %d in background\n", childId);
        }
    }
}

void waitForChild(int childPid, int pidArr[], int* result) {
    int wstatus;
    waitpid(childPid, &wstatus, 0);
    *result = WEXITSTATUS(wstatus);
    for (int i = 0; i < 10; i++) {
        if (pidArr[i] == childPid) {
            pidArr[i] = 0;
        }
    }
}

void printChildProcess(pid_t pidArr[]) {
    printf("Unwaited Child Processes:\n");
    for (int i = 0; i < 10; i++) {
        if (pidArr[i] != 0) {
            printf("%d\n", pidArr[i]);
        }
    }
}

int main()
{
    char **cmdLineArgs;
    char path[20] = ".";  //default search path
    char userInput[121];
    pid_t pidArr[10];

    int numberOfPids = 0;
    int tokenNum;
    int result;

    //read user input
    printf("YWIMC > ");

    //read up to 120 characters from user input
    fgets(userInput, 120, stdin);

    //split is a helper function to "tokenize" the input
    //see "stringTokenizer.c" program for more sample usage 
    cmdLineArgs = split( userInput, " \n", 7, &tokenNum );

    //At this point you have the user input split neatly into token in cmdLineArg[]
    //
    for (int i = 0; i < 10; i++) {
        pidArr[i] = 0;
    }

    while ( strcmp( cmdLineArgs[0], "quit") != 0 ) {

        //Figure out which command the user want and implement below
        char* command = cmdLineArgs[0];
        if (strcmp(command, "result") == 0) {
            // result
            printf("%d\n", result);
        } else if (strcmp(command, "wait") == 0) {
            // wait
            int childPid = atoi(cmdLineArgs[1]);
            waitForChild(childPid, pidArr, &result);
        } else if (strcmp(command, "pc") == 0) {
            // pc
            printChildProcess(pidArr);
        } else if (strcmp(command, "showpath") == 0) {
            showPath(path);
        } else if (strcmp(command, "setpath") == 0) {
            char* newPath = cmdLineArgs[1];
            setPath(path, newPath);
        } else {
            if (strcmp(cmdLineArgs[tokenNum - 1], "&") == 0) {
                runCommandWithoutWait(cmdLineArgs, command, path, pidArr, numberOfPids);
                numberOfPids++;
            } else {
                runCommandAndWaitResult(cmdLineArgs, command, path, &result);
            }
        }

        //Prepare for next round input

        //Clean up the token array as it is dynamically allocated
        freeTokenArray(cmdLineArgs, tokenNum);

        printf("YWIMC > ");
        fgets(userInput, 120, stdin);
        cmdLineArgs = split( userInput, " \n", 7, &tokenNum );
    }
    

    printf("Goodbye!\n");

    //Clean up the token array as it is dynamically allocated
    freeTokenArray(cmdLineArgs, tokenNum);


    return 0;

}
