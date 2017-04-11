#include "sfs_api.h"


/******* Setting things up in Memory *******/
SuperBlock SB;

char *fileName = "FileSystem";


INode JNode;   //the root node

unsigned char *fileDesChars;
FileDesc *fdTable;

Block iNodesBlockBuffer[14];

RootDirectory rootDir;

RootDirectoryEntry *rootDirectoryEntries; 

void mkssfs(int fresh){
    if(fresh){

        fileDesChars = calloc(1, 3584);

        fdTable = (FileDesc *)fileDesChars;
        for(int i = 0; i < 224; i++){
            fdTable[i].freeBit = 0;
            fdTable[i].readPointer = 0;
        }

        //Initializing root dir 

        init_fresh_disk(fileName, _BLOCK_SIZE, _NUMBER_OF_BLOCKS);

        unsigned char *entriesBuff = calloc(5, _BLOCK_SIZE);
        rootDirectoryEntries = (RootDirectoryEntry *)entriesBuff;

        //setting up the root dir entries
        for(int i = 0; i < 224; i++){
            rootDirectoryEntries[i].iNodeNumber = -1;
        }

        write_blocks(1, 5, rootDirectoryEntries);

        SB.magic[0] = 0xff;
        SB.magic[1] = 0xff;
        SB.magic[2] = 0xff;
        SB.magic[3] = 0xff;

        SB.blockSize = _BLOCK_SIZE;
        SB.blocksCount = _NUMBER_OF_BLOCKS;
        SB.iNodesCount = _NUMBER_OF_INODES;
        SB.jNode = JNode;

        Block *super = calloc(1, _BLOCK_SIZE);

        memcpy(super, &SB, sizeof(SB));


        Block blocksBuffer[1] = {*super};


        /******* Writing to the disk *******/
        write_blocks(0,1,blocksBuffer);

        int counter = 20;

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
        write_blocks(6,14,iBlocks);
        free(iNode);

    }else{
        fileDesChars = calloc(1, 3584);

        fdTable = (FileDesc *)fileDesChars;

        for(int i = 0; i < 224; i++){
            fdTable[i].freeBit = 0;
            fdTable[i].readPointer =0;
        }
         unsigned char *entriesBuff = calloc(5, _BLOCK_SIZE);
        rootDirectoryEntries = (RootDirectoryEntry *)entriesBuff;

   //setting up the root dir entries
        for(int i = 0; i < 224; i++){
            rootDirectoryEntries[i].iNodeNumber = -1;
        }

        write_blocks(1, 5, rootDirectoryEntries);



        init_disk(fileName, _BLOCK_SIZE, _NUMBER_OF_BLOCKS);
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
    read_blocks(6, 14,iNodesBlocks);

    //reading the root dirs
    read_blocks(1,5,rootDirectoryEntries);

    //checking to see if file exists
    for(int i = 0; i < 224; i++){
        //if we find a file with the name
        if(!strcmp(rootDirectoryEntries[i].fileName, name)){
            printf("file name is : %s\n", rootDirectoryEntries[i].fileName);
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





int index = -1;
    for(int i = 0; i < 224; i++){
        //finding the first free filedesc and setting it equal to the new file inode and fresh pointers
        if (fdTable[i].freeBit == 0){
            fdTable[i].iNodeNumber = iNodeNumber; 
            fdTable[i].readPointer = 0;
            fdTable[i].writePointer = 0;
            fdTable[i].freeBit = 1;
            index = i;
            break;
        }
    }
    write_blocks(1,5,rootDirectoryEntries);
    write_blocks(6,14,iNodesBuff);


    free(iNodesBuff);
    return index;
}
int ssfs_fclose(int fileID){
    if(fileID < 0){
        printf("cant be negative");
        return -1;
    }
    if(fdTable[fileID].freeBit == 1){
        fdTable[fileID].iNodeNumber = -1;
        fdTable[fileID].readPointer = 0;
        fdTable[fileID].writePointer = 0;
        fdTable[fileID].freeBit = 0;
        return 0;
    }
    if(fdTable[fileID].freeBit == 0){
    return -1;
    }
    return -1;
}
int ssfs_frseek(int fileID, int loc){

    if(fileID <0){
        printf("cant be negative");
        return -1;
    }
    if(loc < 0){
        printf("Cannot seek negative location");
        return -1;
    }

    //making room for all inodes
    unsigned char *iNodesBuff = calloc(14, _BLOCK_SIZE);
    INode *iNodesBlocks = (INode *)iNodesBuff;
    /**** Reading the INodes *****/
    read_blocks(6, 14,iNodesBlocks);

    if(fdTable[fileID].freeBit == 0){
        printf("No open file with that ID");
        return -1;
    }


    if((fdTable[fileID].readPointer + loc) > iNodesBlocks[fdTable[fileID].iNodeNumber].size){
        printf("Error, trying to read past end of the file");
        return -1;
    }

    fdTable[fileID].readPointer = loc;

    return 0;

}
int ssfs_fwseek(int fileID, int loc){
    if(fileID <0){
        printf("cant be negative");
        return -1;
    }//making room for all inodes
    unsigned char *iNodesBuff = calloc(14, _BLOCK_SIZE);
    INode *iNodesBlocks = (INode *)iNodesBuff;
    /**** Reading the INodes *****/
    read_blocks(6, 14,iNodesBlocks);

    if(fdTable[fileID].freeBit == 0){
        printf("No open file with that ID");
        return -1;
    }



    if((fdTable[fileID].writePointer + loc) > iNodesBlocks[fdTable[fileID].iNodeNumber].size){
        printf("Error, trying to read past end of the file");
        return -1;
    }


    fdTable[fileID].writePointer = loc;

    return 0;
}
int ssfs_fwrite(int fileID, char *buf, int length){
   if(fileID <0){
        printf("cant be negative");
        return -1;
    }//making room for all inodes
    unsigned char *iNodesBuff = calloc(14, _BLOCK_SIZE);
    INode *iNodesBlocks = (INode *)iNodesBuff;
    /**** Reading the INodes *****/
    read_blocks(6, 14,iNodesBlocks);

    printf("the iNode Number is : %d \n", fdTable[fileID].iNodeNumber);

    unsigned char *blocksBuff = calloc(14, _BLOCK_SIZE);

    //making sure it is open/ it is in the filedesc table
    if(fdTable[fileID].freeBit == 0){
        printf("No open file with that ID");
        return -1;
    }
    int startingBlock = iNodesBlocks[fdTable[fileID].iNodeNumber].direct[0];

    //todo check if read pointer + length is less than buff (idk how to check sizeof of buff cause that function is weird)

    //reading one by one into blockBuff
    read_blocks(startingBlock, 14, blocksBuff);


    if(((int)strlen(buf))<length){
        for(int i = 0; i < (int)strlen(buf); i++){
            blocksBuff[i + fdTable[fileID].writePointer] = buf[i];
        }
    }else{
        for(int i = 0; i < length; i++){
            blocksBuff[i + fdTable[fileID].writePointer] = buf[i];
        }
    }
    for(int i = 0; i<2; i++){
        printf("%02x \n",blocksBuff[i]);  
    }

    iNodesBlocks[fdTable[fileID].iNodeNumber].size = iNodesBlocks[fdTable[fileID].iNodeNumber].size + length;

    write_blocks(6, 14, iNodesBlocks);


    printf("it starts at : %d \n", startingBlock);
    write_blocks(startingBlock, 14, blocksBuff);

    free(iNodesBuff);
    free(blocksBuff);
    return length;

}
int ssfs_fread(int fileID, char *buf, int length){
     if(fileID <0){
        printf("cant be negative");
        return -1;
    }
     //making room for all inodes
    unsigned char *iNodesBuff = calloc(14, _BLOCK_SIZE);
    INode *iNodesBlocks = (INode *)iNodesBuff;
    /**** Reading the INodes *****/
    read_blocks(6, 14,iNodesBlocks);
    
    if(fdTable[fileID].freeBit == 0){
        printf("No open file with that ID");
        return -1;
    }


    INode fileInode = iNodesBlocks[fdTable[fileID].iNodeNumber];

    unsigned char *blocksBuff = calloc(14, _BLOCK_SIZE);


    //making sure it is open/ it is in the filedesc table
       //todo check if read pointer + length is less than buff (idk how to check sizeof of buff cause that function is weird)

    //reading one by one into blockBuff
    read_blocks(fileInode.direct[0], 14,blocksBuff);
    //    if(((int)strlen(buf))<length){
    //      for(int i = 0; i < (int)strlen(buf); i++){
    //         buf[i] = blocksBuff[i + fdTable[fileID].readPointer];
    //  }
    // }else{
    for(int i = 0; i < length; i++){
        buf[i] = blocksBuff[i + fdTable[fileID].readPointer];
    }
    // }

    for(int i = 0; i < 25; i ++){
        printf("%c", buf[i]);
    }

    free(iNodesBuff);
    free(blocksBuff);
    return length;
}
int ssfs_remove(char *file){
    //making room for all inodes
    unsigned char *iNodesBuff = calloc(14, _BLOCK_SIZE);
    INode *iNodesBlocks = (INode *)iNodesBuff;
    /**** Reading the INodes *****/
    read_blocks(6, 14,iNodesBlocks);


    int iNodeNumber = -1;

    read_blocks(1,5,rootDirectoryEntries);

    //checking to see if file exists
    for(int i = 0; i < 224; i++){
        //if we find a file with the name
        if(!strcmp(rootDirectoryEntries[i].fileName, file)){
            iNodeNumber = rootDirectoryEntries[i].iNodeNumber;
            ssfs_fclose(rootDirectoryEntries[i].iNodeNumber);
            //setting all the values of the blocks that the inode poined to, to zero
            for(int i = 0; i < 14; i++){
                Block *emptyBlock = calloc(1, _BLOCK_SIZE);
                write_blocks(iNodesBlocks[iNodeNumber].direct[i], 1, emptyBlock);
            }
            //zeroing the directory entry and writing back changes
            memset(rootDirectoryEntries[i].fileName, 0, 16);
            rootDirectoryEntries[i].iNodeNumber = -1;
            write_blocks(1, 5, rootDirectoryEntries);
            return 0;
            break;
        }
        //if no file with the same name and empy directory entry 
    }


    return 0;
}
