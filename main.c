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
            if(parseStatus == 1 && param[0] != NULL){
                int status;
                status = allocate(param[0]);
                printf("%d\n", status);
                int i;
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
    if(toalloc <= 0 || toalloc > HEAPSIZE-HEADERSIZE){    //converts input to integer and stores as such (be careful about checking/printing these, cast headers as ints
        printf("unrecognized input, please reenter\n");
        return -2;
    }
    if(heap[0] == 0){      //if first allocation, just allocate it without setting up loops
        sizeptr = heap;
        *sizeptr = (short)(toalloc+HEADERSIZE);
        highBlock++;
        heap[HEADERSIZE-1] = highBlock;
        if(HEAPSIZE-toalloc-HEADERSIZE <= HEADERSIZE){
            *sizeptr = HEAPSIZE;
        }
        sizeptr = heap+toalloc+HEADERSIZE;
        *sizeptr = (HEAPSIZE-toalloc-HEADERSIZE)*-1;    //FIX THIS SO IT DOESN'T ALLOCATE BLOCK 3 AT END OF LIST
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
                if(*walker<=(toalloc*-1)-HEADERSIZE){         //if space available in this block (or rest of list if at end of chain) is less than block size you're attempting to allocate, assign to that block
                    walkto = *walker;
                    *walker = toalloc+HEADERSIZE;
                    blockNum = (char*)walker;
                    blockNum+=2;
                    highBlock++;
                    *blockNum = highBlock;
                    walker = heap+i+toalloc+HEADERSIZE;
                    if(-walkto+ i == HEAPSIZE){ //if this is the last block in the chain (so far), write to end of chain
                        if(-walkto-(toalloc+HEADERSIZE) <= HEADERSIZE){
                            short *intermed = walker;
                            intermed = heap+i;
                            *intermed = -walkto;
                        }
                        else{
                            *(walker) = walkto+(toalloc+HEADERSIZE);
                        }
                    }
                    else{   //otherwise, fill block and split if needed
                        int temp = -walkto;
                        short *intermed = walker;
                        if(temp-(toalloc+HEADERSIZE) == 0){
                            //if allocating something with same block size, do nothing
                        }
                        else if(temp-(toalloc+HEADERSIZE) <= HEADERSIZE){
                            intermed = heap+i;
                            *intermed = temp;
                        }
                        else{
                            *intermed = -(temp-(toalloc+HEADERSIZE));
                        }
                    }
                    allocated = 1;
                    return highBlock;
                }
                else if(*walker>(toalloc*-1)-HEADERSIZE){    //if found unallocated block, but it's smaller
                    if(-*walker+ i == HEAPSIZE) return -1;   //if allocating same size block, change nothing
                    walkto = *walker*-1;        //continue around loop using the unallocated space as indicator
                }
            }
            else if(*walker > 0){
                walkto = *walker;
            }
            else if(*walker == 0){
                return -1;
            }
            i+= walkto;
            while(heap[i] == 0){
                i++;
            }
            if(i > HEAPSIZE) return -1;
            walker = heap+i;
        }

    }
}
void freeBlock(char *blockNum){ //since first block is supposed to be block 1, atoi is fine
    short *blockPoint;
    char *num;
    short block = (short)atoi(blockNum);
    printf("block = %d\n", block);
    int i = 0;
    while(i < 400-HEADERSIZE){
        blockPoint = heap+i;
        num = blockPoint;
        num += 2;
        if(*num == block && *blockPoint>0){
            *blockPoint = *blockPoint*-1;
            return;
        }
        else{
            if(*blockPoint>0){
                i+=*blockPoint;
            }
            else{
                i+=*blockPoint*-1;
            }
        }
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
            printf("%d\t%s\t\t0x%p\t0x%p\n", *blockSize*-1, "no", blockSize, pointerSize-1);
            i += (*blockSize*-1);
        }
        else{   //otherwise if block allocated, same thing without negation
            pointerSize = blockSize;
            pointerSize+= *blockSize;
            printf("%d\t%s\t\t0x%p\t0x%p\n", *blockSize, "yes", blockSize, pointerSize-1);
            i += (*blockSize);
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
        if(*blockSize == 0){
            printf("No Blocks Allocated \n");
            return;
        }

        while(i < HEAPSIZE) {
            blockSize = heap+i;
            //blockNum = heap+(i+2);
            //blockNum = (*blockNum);
            int blockNum = heap[i+2];
            //printf("BLOCK#%d, SIZE:%d\n", *blockNum, *blockSize);
            if(*blockSize > 0){ //if block is allocated
                if(blockNum == block) {
                    if(number > ((*blockSize) - 3) ) { printf("ERROR: Cannot write. Not enough blocks allocated.\n"); return; }
                    int c;
                    for(c=0; c<number;c++) {
                        heap[i+c+3] = writer;
                    } for(c=number; c < ((*blockSize) - 3); c++) {
                        heap[i+c+3] = 0;
                    }
                    return;
                }

                i += (*blockSize);
                /*while(heap[i] == 0){    //override code, ensures that if block end does not line up with block beginning, block will not print
                    i++;
                }*/
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
    int i=0;
    short *blockSize;
    blockSize = heap;

    if(block == 0 || number == 0){
        printf("handle this error\n");  //should probably change the text for this error message
    }
    else if (block > highBlock) {
        printf("ERROR: Block not yet allocated.\n");
    }
    else{
        if(*blockSize == 0){
            printf("No Blocks Allocated \n");
            return;
        }

        while(i < HEAPSIZE) {
            blockSize = heap+i;
            int blockNum = heap[i+2];

            if(*blockSize > 0){ //if block is allocated
                if(blockNum == block) {
                    //if(number > ((*blockSize) - 3) ) { printf("ERROR: Cannot read. There are not %d bytes in block %d.\n", number, blockNum); return; }
                    int c;
                    for(c=0; c<number;c++) {
                        char *fill = heap+i+3+c;
                        if(*fill <= 31 && *fill >= 0) printf("[%d]", *fill);    //if characters are special chars, just fill in actual value instead of ascii value
                        else printf("%c", *fill);  //added pipes around characters since we'll be printing null chars
                        //printf("|%d|", *fill);
                    }
                    printf("\n");
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
