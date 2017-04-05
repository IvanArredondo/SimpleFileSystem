#include "structures.h"
#include "SFS/disk_emu.c"

int main(int argc, char*argv[]){
char *fileName = "FileSystem";

int i = init_fresh_disk(fileName, _BLOCK_SIZE, _NUMBER_OF_BLOCKS);

mkssfs(1);

return 0;
}


void mkssfs(int fresh){

    SuperBlock SB;

    unsigned char* magic = "7777";

    INode JNode;

    strncpy(SB.magic, magic, 4);

    SB.blockSize = _BLOCK_SIZE;
    SB.blocksCount = _NUMBER_OF_BLOCKS;
    SB.iNodesCount = _NUMBER_OF_INODES;
    SB.jNode = JNode;

    Block *super = calloc(1, _BLOCK_SIZE);

    printf("%lu\n", sizeof(*super));
    memcpy(super, &SB, sizeof(SB));

    Block blocksBuffer[1] = {*super};

    Block iNodesBlockBuffer[14];
    int ret = write_blocks(0,1,blocksBuffer);


    for(int i = 0; i < 14; i++){
            printf("%d\n", i);
       Block *block = calloc(1, _BLOCK_SIZE);
       printf("%d\n", i);
        for(int j = 0; j < 11 ; j++){
            INode *iNode = (INode *)block;
            iNode[j].size = _INODE_SIZE;
            iNode[j].direct[0] = 5;
            int *p = memcpy(block, &iNode, sizeof(iNode));
           // block++;//this is the issue, since block is 1024 bytes, it increments by 1024 
            printf("%d\n", *p);
        }
        iNodesBlockBuffer[i] = *block;
    }
    ret = write_blocks(1,14,iNodesBlockBuffer);
}
