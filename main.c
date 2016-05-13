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

#define HEAPSIZE 400
#define HEADERSIZE 3    //size of the header in bytes

int parseInput(char *input);
int allocate(char *size);
void freeBlock(char *blockNum);
void blocklist();
void writeheap(char *blockNum, char *writechar, char *amount);
void printheap(char *blockNum, char *amount);

char* heap; //we're supposed to operate on bytes, so making it an array of characters makes sense
int highBlock = 0; //highest block allocated thus far, will never decrement

int main(int argc, char *argv[])
{
    heap = malloc(sizeof(char)*HEAPSIZE);   //heap allocated HEAPSIZE bytes (heapsize defaults to 400 since that's what Prof. specified
    int i;
    for(i=0;i<HEAPSIZE;i++){
        heap[i] = 0;  //just setting everything to an arbitrary value for testing purposes
    }
    printf("start of heap: 0x%p, end of heap: 0x%p\n", heap, heap+HEAPSIZE);    //here's how to print addresses
    int exit_check = 1;
    int parseStatus;

    while(exit_check != 0) {
        char *input;
        char *param[3];
    	input = malloc(sizeof(char)*100);

        printf("> ");
        fgets(input, 100, stdin);
        parseStatus = parseInput(input);
        if(parseStatus != -1) {
            param[0] = strtok(NULL, " \n\t\r");
            param[1] = strtok(NULL, " \n\t\r");
            param[2] = strtok(NULL, " \n\t\r"); //maximum of 3 parameters required (for writeheap)
            printf("|%s|\n", param[0]);
            printf("|%s|\n", param[1]);
            printf("|%s|\n", param[2]);
            if(parseStatus == 1 && param[0] != NULL){
                int status;
                status = allocate(param[0]);
                printf("~allocate has exited, with return status = %d\n", status);
                int i;
                for(i=0; i < 30; i++) {
                    printf("%d\n", heap[i]);
                }
            }
            else if(parseStatus == 2 && param[0] != NULL){
                freeBlock(param[0]);
            }
            else if(parseStatus == 3){
                blocklist();
            }
            else if(parseStatus == 4 && param[2] != NULL){  //this SHOULD work, since all 3 params must be filled for param2 to not be null
                writeheap(param[0],param[1],param[2]);
            }
            else if(parseStatus == 5 && param[1] != NULL){
                printheap(param[0], param[1]);
            }
            //run function according to parseStatus number
            //i.e. if 1: run allocate(int bytes)
        } else { exit_check = 0; }
    }
    free(heap);
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
int allocate(char *size){   //TEST CASE FOR THIS FUNCTION: allocate 7, allocate 5, allocate 3, free 1, allocate 3, allocate 1
    if(size == NULL) {
        printf("No parameter given. Please reenter.\n");
        return -1;
    }
    int toalloc = atoi(size);
    int i, allocated;
    short *sizeptr, walkto, *walker;
    char *blockNum;
    if(toalloc <= 0 || toalloc > HEAPSIZE){    //converts input to integer and stores as such (be careful about checking/printing these, cast headers as ints
        printf("unrecognized input, please reenter\n");
        return -2;  //i'll probably restructure this later
    }
    if(heap[0] == 0){      //if first allocation, just allocate it without setting up loops
        sizeptr = heap;
        *sizeptr = (short)(toalloc+HEADERSIZE);
        //printf("sizeptr = %d\n", *sizeptr);
        highBlock++;
        heap[HEADERSIZE-1] = highBlock;

        sizeptr = heap+2;
        *sizeptr = (short)highBlock++;
        sizeptr = heap+toalloc+HEADERSIZE;
        *sizeptr = (HEAPSIZE-toalloc-HEADERSIZE)*-1;
        printf("sizeptr = %d\n", *sizeptr);
        return highBlock;
        //heap[toalloc+2] = (HEAPSIZE-toalloc)*-1;    //negative numbers represent unallocated blocks
    }
    else{
        //walk up list, looking for space to allocate
        i=0;
        walker = heap;
        walkto = *walker;
        allocated = 0;
        while(allocated == 0){
            if(*walker < 0){
                printf("wait what\n");
                if(*walker<=(toalloc*-1)-HEADERSIZE){         //if space available in this block (or rest of list if at end of chain) is less than block size you're attempting to allocate, assign to that block
                    printf("negative allocation entered, heap=%d, toalloc=%d\n", heap[i], toalloc);
                    walkto = *walker;
                    *walker = toalloc+HEADERSIZE;
                    blockNum = (char*)walker;
                    blockNum+=2;
                    highBlock++;
                    *blockNum = highBlock;
                    printf("WALKER = 0x%p\n", walker);
                    walker = heap+i+toalloc+HEADERSIZE;
                    printf("walkto = %d\n", walkto);
                    printf("i = %d\n", i);
                    if(-walkto+ i == HEAPSIZE){ //if this is the last block in the chain (so far), write to end of chain
                        *(walker) = walkto+(toalloc+HEADERSIZE);
                        printf("walker = %d\n", *walker);
                    }
                    else{   //otherwise, fill block and split if needed
                        int temp = -walkto;
                        short *intermed = walker;
                        printf("YUP ITS WORKING %d\n", temp);
                        printf("OK WHAT %d\n", temp-(toalloc+HEADERSIZE));
                        if(temp-(toalloc+HEADERSIZE) == 0){
                            //if allocating something with same block size, do nothing
                            printf("THIS WAS ENTERED 0x%p, 0x%p\n",walkto, intermed);
                        }
                        else if(temp-(toalloc+HEADERSIZE) <= HEADERSIZE){
                            intermed = heap+i;
                            printf("INTERMED NOW = 0x%p\n", intermed);
                            *intermed = temp;
                        }
                        else{
                            printf("temp= %d\n", temp);
                            printf("temp-toalloc+header = %d\n", temp-(toalloc+HEADERSIZE));
                            *intermed = -(temp-(toalloc+HEADERSIZE));
                        }
                    }
                    allocated = 1;
                    return highBlock;
                }
                else if(*walker>(toalloc*-1)-HEADERSIZE){    //if found unallocated block, but it's smaller
                    if(-*walker+ i == HEAPSIZE) return -1;      //alright, that should fix the lingering issues with this function
                    printf("%WALKTOIN:%d\n", walkto);
                    printf("WALKER:%d\n", *walker);
                    walkto = *walker*-1;        //continue around loop using the unallocated space as indicator
                    printf("WALKTO:%d\n",walkto);
                }
            }
            else if(*walker > 0){
                printf("entered here\n");
                walkto = *walker;
            }
            else if(*walker == 0){
                printf("ok something went wrong, you shouldn't be here, i'll just return for now\n");
                return -1;
            }
            printf("walker = %d, i = %d, walkto = %d\n", *walker, i, walkto);
            i+= walkto;
            while(heap[i] == 0){
                i++;
            }
            if(i > HEAPSIZE) return -1;
            walker = heap+i;
            printf("walker = %d, i = %d, walkto = %d\n", *walker, i, walkto);
        }

    }
}
void freeBlock(char *blockNum){ //since first block is supposed to be block 1, atoi is fine
    printf("%s\n", blockNum);
    int i = atoi(blockNum); //this function should be pretty simple, just loop through til you find a block with this number, then negate the allocation size
    printf("%d\n", i);
    short *lol; //test code for allocate, delete
    lol = heap;
    *lol = -10;
    for(i = 0; i < 20; i++){
        printf("%d\n", heap[i]);
    }
}
void blocklist(){
    int i=0;
    short *blockSize;
    char *pointerSize;
    blockSize = heap;
    if(*blockSize == 0){
        printf("No Blocks Allocated \n");
        return;
    }
    printf("%s\t%s\t%s\t\t%s\n", "Size", "Allocated", "Start", "End");
    while(i < HEAPSIZE){    //while still in heap
        blockSize = heap+i;
        if(*blockSize < 0){ //if block is unallocated
            pointerSize = blockSize;
            pointerSize += (*blockSize*-1);
            //printf("pointerSize = %p\n", pointerSize);
            printf("%d\t%s\t\t0x%p\t0x%p\n", *blockSize*-1, "no", blockSize, pointerSize-1);
            i += (*blockSize*-1);
            //printf("i is now %d\n", i);
        }
        else{   //otherwise if block allocated, same thing without negation
            pointerSize = blockSize;
            pointerSize+= *blockSize;
            //printf("pointerSize = %p\n", pointerSize);
            printf("%d\t%s\t\t0x%p\t0x%p\n", *blockSize, "yes", blockSize, pointerSize-1);
            i += (*blockSize);
             while(heap[i] == 0){
                i++;
            }
            //printf("i is now %d\n", i);
        }
    }
}


void writeheap(char *blockNum, char *writechar, char *amount){
    int block = atoi(blockNum), number = atoi(amount);
    char writer = writechar[0];
    int i=0;
    short *blockSize;
    blockSize = heap;

    if(block == 0 || number == 0){
        printf("ERROR: INVALID WRITE REQUEST.\n");
    }
    else if (block > highBlock) {
        printf("ERROR: Block has not been allocated yet.\n");
    }
    else{
        //printf("you will be writing %c to block %d, %d times\n", writer, block, number);
        if(*blockSize == 0){
            printf("No Blocks Allocated \n");
            return;
        }
        short *blockNum;
        while(i < HEAPSIZE) {
            blockSize = heap+i;
            blockNum = heap+(i+2);
            //blockNum = (*blockNum);
            printf("BLOCK#%d, SIZE:%d\n", *blockNum, *blockSize);
            if(*blockSize > 0){ //if block is allocated
                if(*blockNum == block) {
                    if(number > ((*blockSize) - 3) ) { printf("ERROR: Cannot write. Not enough blocks allocated.\n"); return; }
                    int c;
                    for(c=0; c<number;c++) {
                        short *fill = heap+i+3+c;
                        *(fill) = writer;
                    }
                    return;
                }

                i += (*blockSize);
                while(heap[i] == 0){    //override code, ensures that if block end does not line up with block beginning, block will not print
                    i++;
                }
            }
            else if(*blockSize == 0) {
                break;
            }
            else{
                i += (*blockSize*-1);
                while(heap[i] == 0){
                    i++;
                }
            }
        }

        if(block <= highBlock) {
            printf("ERROR: Requested block no longer allocated.\n");
        }
    }
}
void printheap(char *blockNum, char *amount){
    int block = atoi(blockNum), number = atoi(amount);
    if(block == 0 || number == 0){
        printf("handle this error\n");
    }
    else{
        printf("you will be printing %d bytes from block %d\n", number, block);
    }
}
