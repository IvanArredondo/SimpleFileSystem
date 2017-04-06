#include "structures.h"
#include "SFS/disk_emu.c"


 /******* Setting things up in Memory *******/
SuperBlock SB;

unsigned char* magic = "7777";

INode JNode;   //the root node
    
FileDesc FDTable[10];

Block iNodesBlockBuffer[14];

RootDirectory *rootDir;

int main(int argc, char*argv[]){
char *fileName = "FileSystem";

int i = init_fresh_disk(fileName, _BLOCK_SIZE, _NUMBER_OF_BLOCKS);

mkssfs(1);

int res = ssfs_fopen("hello");

return 0;
}


void mkssfs(int fresh){

  //Initializing root dir 

    

    strncpy(SB.magic, magic, 4);

    SB.blockSize = _BLOCK_SIZE;
    SB.blocksCount = _NUMBER_OF_BLOCKS;
    SB.iNodesCount = _NUMBER_OF_INODES;
    SB.jNode = JNode;

    Block *super = calloc(1, _BLOCK_SIZE);

    printf("%lu\n", sizeof(*super));
    memcpy(super, &SB, sizeof(SB));

    Block blocksBuffer[1] = {*super};

    
    /******* Writing to the disk *******/
    int ret = write_blocks(0,1,blocksBuffer);


    for(int i = 0; i < 14; i++){
       Block *block = calloc(1, _BLOCK_SIZE);
       INode *iNode = (INode *)block;

        for(int j = 0; j < 16 ; j++){
                iNode[j].size = -1;
                iNode[j].direct[0] = 3;
               //printf("In the condition\n");

                       // block++;//this is the issue, since block is 1024 bytes, it increments by 1024 
        }
        int *p = memcpy(block, iNode, sizeof(iNode));
        free(iNode);
        iNodesBlockBuffer[i] = *block;
    }
    ret = write_blocks(1,14,iNodesBlockBuffer);
}

int ssfs_fopen(char *name){

    Block *buff = calloc(1, _BLOCK_SIZE*14);
    INode *iNodesBlocks = (INode *)buff;
    int result = read_blocks(1, 14,iNodesBlocks);
   
    printf("the tests \n");
    printf("size of the list is : %lu\n", sizeof(*buff));
    
    INode freeINode;
    printf("number of iNodes is : %lu\n", sizeof(&iNodesBlocks)/sizeof(&iNodesBlocks[0]));

    for(int i = 0; i < 224; i++){
        if(iNodesBlocks[i].size == -1){
            freeINode = iNodesBlocks[i];
            printf("The First free I-Node is : %d\n", i);
            break;
        }

    }

    printf("%d\n", freeINode.direct[0]);
    printf("%d\n", iNodesBlocks[1].size);
    printf("%d\n", iNodesBlocks[13].size);



    // int inodeSize = iNodesBlockBuffer[1].bytes[4];
   // printf("%d\n", inodeSize);
   // while(0){
      // for(int i = 0; i < iNodesBlockBuffer.length; i++){
           // for(int i = 0; i< 14; i++){
               // if(iNodesBlockBuffer
           // }
        
      // }
   // }
}

int ssfs_write(int fileID, char *buf, int length){


}







