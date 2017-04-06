#include "structures.h"
#include "SFS/disk_emu.c"


 /******* Setting things up in Memory *******/
SuperBlock SB;

unsigned char* magic = "7777";

INode JNode;   //the root node
    
FileDesc FDTable[10];

Block iNodesBlockBuffer[14];

RootDirectory rootDir;

int main(int argc, char*argv[]){
char *fileName = "FileSystem";

int i = init_fresh_disk(fileName, _BLOCK_SIZE, _NUMBER_OF_BLOCKS);

mkssfs(1);

int res = ssfs_fopen("hello");

return 0;
}


void mkssfs(int fresh){

  //Initializing root dir 

    unsigned char* rootDirs = calloc(5, _BLOCK_SIZE);

    

    strncpy(SB.magic, magic, 4);

    SB.blockSize = _BLOCK_SIZE;
    SB.blocksCount = _NUMBER_OF_BLOCKS;
    SB.iNodesCount = _NUMBER_OF_INODES;
    SB.jNode = JNode;

    Block *super = calloc(1, _BLOCK_SIZE);

    memcpy(super, &SB, sizeof(SB));
    memcpy(rootDirs, &rootDir, sizeof(rootDir));


    Block blocksBuffer[1] = {*super};

    
    /******* Writing to the disk *******/
    int ret = write_blocks(0,1,blocksBuffer);
    int rdirs = write_blocks(1, 5, rootDirs);

    int counter = 19;

    for(int i = 0; i < 14; i++){
       Block *block = calloc(1, _BLOCK_SIZE);
       INode *iNode = (INode *)block;

        for(int j = 0; j < 16 ; j++){
                iNode[j].size = -1;
                for(int k = 0; k < 14; k++){
                    iNode[j].direct[k] = counter;
                    counter++;
               //printf("In the condition\n");
                }
                       // block++;//this is the issue, since block is 1024 bytes, it increments by 1024 
        }
        int *p = memcpy(block, iNode, sizeof(iNode));
        free(iNode);
        iNodesBlockBuffer[i] = *block;
    }
    ret = write_blocks(6,14,iNodesBlockBuffer);
}

int ssfs_fopen(char *name){

    unsigned char *buff = calloc(14, _BLOCK_SIZE);
    INode *iNodesBlocks = (INode *)buff;
    int iNodeNumber;

    /**** Reading the INodes *****/
    int result = read_blocks(6, 14,iNodesBlocks);
   
    printf("the tests \n");
    
    for(int i = 0; i < 224; i++){
        if(iNodesBlocks[i].size == -1){
            iNodesBlocks[i].size = 0;
            iNodeNumber = i;
            printf("The First free I-Node is : %d\n", iNodeNumber);
            break;
        }

    }

    write_blocks(6,14,buff);
    free(iNodesBlocks);

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







