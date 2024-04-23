#include <driver.h>
#include <sfs_superblock.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "sfs_inode.h"
#include "sfs_dir.h"
#include <math.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

void print_files(sfs_superblock *super);
void print_files_in_inode(sfs_inode_t *inode);
void print_inode_data(sfs_inode_t inode);



void print_inode_data(sfs_inode_t inode) {
    printf("Owner: %d\n", inode.owner);
    printf("Group: %d\n", inode.group);
    printf("Creation Time: %d\n", inode.ctime);
    printf("Modification Time: %d\n", inode.mtime);
    printf("Access Time: %d\n", inode.atime);
    printf("Permissions: %d\n", inode.perm);
    printf("File Type: %d\n", inode.type);
    printf("Reference Count: %d\n", inode.refcount);
    printf("Size: %llu\n", inode.size);
    printf("Direct Blocks:\n");
    for (int j = 0; j < NUM_DIRECT; j++) {
        printf("  %u\n", inode.direct[j]);
    }
}

void print_filenames_in_block(sfs_dirent dirents[], uint64_t num_entries_in_block) {
    for (size_t j = 0; j < num_entries_in_block; j++) {
        const sfs_dirent *entrie = &dirents[j]; // Pointer to the current block
        printf("%s\n", entrie->name);
    }
}

void print_files_in_inode(sfs_inode_t *inode) {
    printf("\nCurrent Inode's Data\n\n");
    print_inode_data(*inode);
    printf("\n\n");


    // calculate the number of valid direct blocks
    uint64_t num_valid_blocks = inode->size / 128;
    if (inode->size % 128 != 0) {
        num_valid_blocks++;
    }

    printf("Inode size: %llu\n", inode->size);
    printf("Number of valid blocks: %llu\n", num_valid_blocks);

    for (size_t i = 0; i < min(num_valid_blocks, 5); i++) {
        uint32_t blk_loc = inode->direct[i];
        sfs_dirent dirents[4];
        driver_read(dirents, blk_loc);

        // calculate the proper number of valid blocks using info from super_block
        char temp_block[128];
        sfs_superblock *curr_block = (sfs_superblock *)temp_block;
        driver_read(curr_block, blk_loc);

        printf("Number of entries in this block: %d\n", curr_block->num_blocks);

        printf("\n\n");

        print_filenames_in_block(dirents, 4);
    }
}

void print_files(sfs_superblock *super) {
    uint32_t inode_table_location = super->inodes;
    uint32_t num_inodes = super->num_inodes;

    printf("Number of inodes in this superblock: %d\n", num_inodes);

    for (size_t i = 0; i < num_inodes/2; i++) {
        sfs_inode_t inodes[2];
        driver_read(inodes, inode_table_location);

        print_files_in_inode(&inodes[0]);
        print_files_in_inode(&inodes[1]);

        inode_table_location += 128;
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

    print_files(super);

    /* close the disk image */
    driver_detach_disk_image();

    return 0;
}


