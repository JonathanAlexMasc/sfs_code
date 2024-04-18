#include <driver.h>
#include <sfs_superblock.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "sfs_inode.h"
#include "sfs_dir.h"

void print_sfs_dirent(const sfs_dirent *dirent, u_int64_t num_blocks);

void print_sfs_dirent(const sfs_dirent *dirent, u_int64_t num_blocks) {
    for (size_t i = 0; i < num_blocks; ++i) {
        const sfs_dirent *current_block = &dirent[i]; // Pointer to the current block
        printf("%s\n", current_block->name);
    }
}

int main()
{
    /* declare a buffer that is the same size as a filesystem block */
    char raw_superblock[128];

    /* Create a pointer to the buffer so that we can treat it as a
       superblock struct. The superb ;;lock struct is smaller than a block,
       so we have to do it this way so that the buffer that we read
       blocks into is big enough to hold a complete block. Otherwise the
       driver_read function will overwrite something that should not be
       overwritten. */
    sfs_superblock *super = (sfs_superblock *)raw_superblock;

    /* open the disk image and get it ready to read/write blocks */
    driver_attach_disk_image("initrd", 128);

    /* CHANGE THE FOLLOWING CODE SO THAT IT SEARCHES FOR THE SUPERBLOCK */
    uint32_t block_number;
    uint32_t found_superblock = 0;

    /* Search for the superblock in blocks 0 to 10 */
    for (block_number = 0; block_number <= 10; block_number++)
    {
        /* read the block from the disk image */
        driver_read(super, block_number);

        /* Check if it's the filesystem superblock */
        if (super->fsmagic == VMLARIX_SFS_MAGIC &&
            !strcmp(super->fstypestr, VMLARIX_SFS_TYPESTR))
        {
            //printf("Superblock found at block %d!\n", block_number);
            found_superblock = 1;
            break; // Exit loop once superblock is found
        }
    }

    if (!found_superblock)
    {
        printf("Superblock is not found in blocks 0 to 10.\n");
        return 1;
    }

    // super is the superblock that we have found...

    /* Calculate the block number of the first block of the inode table */
    uint32_t inode_table_index = super->inodes;
    uint32_t num_inodes = super->num_inodes;
    //printf("Inode Table Index: %d \n", inode_table_index);
    //printf("Number of inode blocks: %d \n", super->num_inode_blocks);

//    for (int k = 0; k < num_inodes/2; k++) {
//        // get first two inodes
//    }

    /* Read the first block of the inode table */
    char first_inode_block[128];
    sfs_inode_t *inode = (sfs_inode_t *)first_inode_block;
    driver_read(inode, inode_table_index);

    /* Print out the contents of the first inode block
    printf("Printing contents of the first inode block:\n");
    for (int i = 0; i < 2; i++) {
        printf("Inode %d:\n", i);
        printf("Owner: %d\n", inode[i].owner);
        printf("Group: %d\n", inode[i].group);
        printf("Creation Time: %d\n", inode[i].ctime);
        printf("Modification Time: %d\n", inode[i].mtime);
        printf("Access Time: %d\n", inode[i].atime);
        printf("Permissions: %d\n", inode[i].perm);
        printf("File Type: %d\n", inode[i].type);
        printf("Reference Count: %d\n", inode[i].refcount);
        printf("Size: %lu\n", inode[i].size);
        printf("Direct Blocks:\n");
        for (int j = 0; j < NUM_DIRECT; j++) {
            printf("  %u\n", inode[i].direct[j]);
        }
        // Print pointers to direct, indirect, double indirect, and triple indirect blocks as needed
    }
    */

    // get first block of direct array
    uint64_t num_blocks = inode[0].size;
    char first_blk_storage[128];

    // Assuming driver_read function reads the data correctly into first_blk_storage.
    uint32_t direct_arr_first_blk_location = inode[0].direct[0];
    sfs_dirent *dirent = (sfs_dirent *)first_blk_storage;
    driver_read(dirent, direct_arr_first_blk_location);

    for (size_t j = 0; j < 4; ++j) {
        const sfs_dirent *current_block = &dirent[j]; // Pointer to the current block
        printf("%s\n", current_block->name);
    }

    // read in inode2
    sfs_inode_t *inode2 = (sfs_inode_t *)(first_inode_block + sizeof(sfs_inode_t));
    driver_read(inode2, inode_table_index);

    /* Print out the contents of the first inode block
    printf("Printing contents of the first inode block:\n");
    for (int i = 0; i < 2; i++) {
        printf("Inode %d:\n", i);
        printf("Owner: %d\n", inode[i].owner);
        printf("Group: %d\n", inode[i].group);
        printf("Creation Time: %d\n", inode[i].ctime);
        printf("Modification Time: %d\n", inode[i].mtime);
        printf("Access Time: %d\n", inode[i].atime);
        printf("Permissions: %d\n", inode[i].perm);
        printf("File Type: %d\n", inode[i].type);
        printf("Reference Count: %d\n", inode[i].refcount);
        printf("Size: %lu\n", inode[i].size);
        printf("Direct Blocks:\n");
        for (int j = 0; j < NUM_DIRECT; j++) {
            printf("  %u\n", inode[i].direct[j]);
        }
        // Print pointers to direct, indirect, double indirect, and triple indirect blocks as needed
    }
    */

    // get first block of direct array
    num_blocks = inode[1].size;
    char second_blk_storage[128];

    // Assuming driver_read function reads the data correctly into first_blk_storage.
    direct_arr_first_blk_location = inode2[1].direct[0];
    dirent = (sfs_dirent *)second_blk_storage;
    driver_read(dirent, direct_arr_first_blk_location);

    for (size_t j = 0; j < 4; ++j) {
        const sfs_dirent *current_block = &dirent[j]; // Pointer to the current block
        printf("%s\n", current_block->name);
    }


    /* close the disk image */
    driver_detach_disk_image();

    return 0;
}
