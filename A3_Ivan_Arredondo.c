#include "structures.h"
#include "SFS/disk_emu.c"


/******* Setting things up in Memory *******/
SuperBlock SB;

char *fileName = "FileSystem";

unsigned char* magic = "7777";

INode JNode;   //the root node

FileDesc *fdTable;

Block iNodesBlockBuffer[14];

RootDirectory rootDir;

RootDirectoryEntry *rootDirectoryEntries; 

int main(int argc, char*argv[]){

    //intializing the file descriptor table
    fdTable = calloc(_NUMBER_OF_INODES, sizeof(FileDesc));

    for(int i = 0; i < 224; i++){
        fdTable[i].freeBit = 0;
    }


    mkssfs(1);

    for(int i = 0; i <50; i++){
        ssfs_fopen("hello world" + i);
    }

    ssfs_fclose(1);
ssfs_fclose(0);
ssfs_fclose(4);

for(int i = 0; i <10; i++){
    printf("fd table: %d \n", fdTable[i].freeBit);
}

    return 0;
}


void mkssfs(int fresh){

    if(fresh){
        //Initializing root dir 

        int i = init_disk(fileName, _BLOCK_SIZE, _NUMBER_OF_BLOCKS);

        unsigned char *entriesBuff = calloc(5, _BLOCK_SIZE);
        rootDirectoryEntries = (RootDirectoryEntry *)entriesBuff;

        //setting up the root dir entries
        for(int i = 0; i < 224; i++){
            rootDirectoryEntries[i].iNodeNumber = -1;
        }

        int rootDirsWrite = write_blocks(1, 5, rootDirectoryEntries);

        strncpy(SB.magic, magic, 4);

        SB.blockSize = _BLOCK_SIZE;
        SB.blocksCount = _NUMBER_OF_BLOCKS;
        SB.iNodesCount = _NUMBER_OF_INODES;
        SB.jNode = JNode;

        Block *super = calloc(1, _BLOCK_SIZE);

        memcpy(super, &SB, sizeof(SB));


        Block blocksBuffer[1] = {*super};


        /******* Writing to the disk *******/
        int ret = write_blocks(0,1,blocksBuffer);

        int counter = 19;

        unsigned char *iBlocks = calloc(14, _BLOCK_SIZE);
        INode *iNode = (INode *)iBlocks;

        for(int j = 0; j < 224 ; j++){
            iNode[j].size = -1;
            for(int k = 0; k < 14; k++){
                iNode[j].direct[k] = counter;
                counter++;
            }
            //printf("In the condition\n");
            // block++;//this is the issue, since block is 1024 bytes, it increments by 1024 
        }
        ret = write_blocks(6,14,iBlocks);
        free(iNode);

    }else{
        int i = init_disk(fileName, _BLOCK_SIZE, _NUMBER_OF_BLOCKS);
    }
}

int ssfs_fopen(char *name){

    //making room for all inodes
    unsigned char *iNodesBuff = calloc(14, _BLOCK_SIZE);
    INode *iNodesBlocks = (INode *)iNodesBuff;

    //setting up vars that we'll use
    int iNodeNumber = -1;
    int writePointer = 0;
    int readPointer = 0;


    /**** Reading the INodes *****/
    int iNodeResult = read_blocks(6, 14,iNodesBlocks);

    //reading the root dirs
    int rootDirsResult = read_blocks(1,5,rootDirectoryEntries);

    //checking to see if file exists
    for(int i = 0; i < 224; i++){
        //if we find a file with the name
        if(!strcmp(rootDirectoryEntries[i].fileName, name)){
            iNodeNumber = rootDirectoryEntries[i].iNodeNumber;
            writePointer = iNodesBlocks[iNodeNumber].size;
            break;
        }
        //if no file with the same name and empy directory entry 
    }

    //if we didnt find an existing entry
    if(iNodeNumber == -1){
        //finding the first free inode
        for(int i = 0; i < 224; i++){
            if(iNodesBlocks[i].size == -1){
                iNodesBlocks[i].size = 0;
                iNodeNumber = i;
                //find a free location in root dirs
                for(int j = 0; j < 224; j++){
                    if(rootDirectoryEntries[j].iNodeNumber == -1){
                        rootDirectoryEntries[j].iNodeNumber = iNodeNumber;
                        strcpy(rootDirectoryEntries[j].fileName, name); 
                        break;
                    }         
                }
                break;
            }

        }

    }

    write_blocks(1,5,rootDirectoryEntries);
    write_blocks(6,14,iNodesBuff);


    



    for(int i = 0; i < 224; i++){
        //finding the first free filedesc and setting it equal to the new file inode and fresh pointers
        if (fdTable[i].freeBit == 0){
            fdTable[i].iNodeNumber = iNodeNumber; 
            fdTable[i].readPointer = 0;
            fdTable[i].writePointer = 0;
            fdTable[i].freeBit = 1;
            printf("added to fdtable");
            break;
        }
    }
    
    free(iNodesBuff);

    return 0;
}



// int inodeSize = iNodesBlockBuffer[1].bytes[4];
// printf("%d\n", inodeSize);
// while(0){
// for(int i = 0; i < iNodesBlockBuffer.length; i++){
// for(int i = 0; i< 14; i++){
// if(iNodesBlockBuffer
// }

// }
// }
//


int ssfs_fclose(int fileID){

    for(int i = 0; i < 224; i++){
        if(fdTable[i].freeBit == 1){
            if(fdTable[i].iNodeNumber == fileID){
                fdTable[i].iNodeNumber = -1;
                fdTable[i].readPointer = 0;
                fdTable[i].writePointer = 0;
                fdTable[i].freeBit = 0;
                for(int j = 0; j < 224; j++){
                    if(rootDirectoryEntries[j].iNodeNumber == fileID){ 
                        printf("we close: %s\n", rootDirectoryEntries[j].fileName);
                        break;
                    }
                }

            }
        }
    }
    return -1;
}

int ssfs_write(int fileID, char *buf, int length){


}







