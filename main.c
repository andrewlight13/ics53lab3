//Light, Andrew: 11712029
//Roke, Adam: 48266987
//Lab 3 - Heap
//Last Modified: 5/9/2016
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

int parseInput(char *input);

int main(int argc, char *argv[])
{
    int exit_check = 1;
    int parseStatus;

    while(exit_check != 0) {
        char *input;
    	input = malloc(sizeof(char)*100);

        printf("> ");
        fgets(input, 100, stdin);
        parseStatus = parseInput(input);
        if(parseStatus != -1) {
            //run function according to parseStatus number
            //i.e. if 1: run allocate(int bytes)
        } else { exit(0); }
    }

    return 0;
}

int parseInput(char *input){
    //determines the command specified by the user
    char *split_input;

    split_input = strtok(input, " \n\t\r");
    if(split_input != NULL) {
        if(strcasecmp(split_input, "allocate") == 0) {
            return 1;
        } else if(strcasecmp(split_input, "free") == 0) {
            return 2;
        } else if(strcasecmp(split_input, "blocklist") == 0) {
            return 3;
        } else if(strcasecmp(split_input, "writeheap") == 0) {
            return 4;
        } else if(strcasecmp(split_input, "printheap") == 0) {
            return 5;
        } else if(strcasecmp(split_input, "quit") == 0) {
            return -1;
        }
    } else {
        return 0; //no command given. exit
    }
    return 0;
}
