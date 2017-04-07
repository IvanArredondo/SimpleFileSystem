#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SFS/disk_emu.h"

#define _BLOCK_SIZE 1024
#define _INODE_SIZE 16
#define _NUMBER_OF_BLOCKS 2850
#define _NUMBER_OF_INODES 224

typedef struct blocks{
    unsigned char bytes[_BLOCK_SIZE];
//      unsigned char byte;
}Block;

typedef struct iNodes{
    int size;
    unsigned int direct[14];
    unsigned int indirect;

}INode;

typedef struct rootDirsEntries{
    char fileName[16];
    int iNodeNumber;

}RootDirectoryEntry;

typedef struct rootdirs{
    RootDirectoryEntry lookUpTable[224];
}RootDirectory;

typedef struct superBlocks{
    unsigned char magic[4];
    unsigned int blockSize;
    unsigned int blocksCount;
    unsigned int freeBlocks;
    unsigned int iNodesCount;
    INode jNode;
}SuperBlock;


typedef struct fileDescs{
    unsigned int  iNodeNumber;
    unsigned int readPointer;
    unsigned int writePointer;
    unsigned int freeBit;
}FileDesc;

typedef struct fileDescTables{
    FileDesc table[200];
}FileDescTable;

//Functions you should implement. 
//Return -1 for error besides mkssfs
void mkssfs(int fresh);
int ssfs_fopen(char *name);
int ssfs_fclose(int fileID);
int ssfs_frseek(int fileID, int loc);
int ssfs_fwseek(int fileID, int loc);
int ssfs_fwrite(int fileID, char *buf, int length);
int ssfs_fread(int fileID, char *buf, int length);
int ssfs_remove(char *file);
int ssfs_commit();
int ssfs_restore(int cnum);



