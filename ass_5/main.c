#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "structs.h"

FILE* fs_img_stream;

void read_sect(uint sec, void *buf, size_t buf_size) {
    fseek(fs_img_stream, sec * BSIZE, SEEK_SET);
    fread(buf, 1, buf_size, fs_img_stream);
}

void read_inode(uint sec, void *buf, size_t buf_size, uint inum) {
    fseek(fs_img_stream, sec * BSIZE + inum * buf_size, SEEK_SET);
    fread(buf, 1, buf_size, fs_img_stream);
}

void read_struct(uint sec, void *buf, size_t buf_size)
{
    fseek(fs_img_stream, sec * BSIZE, SEEK_SET);
    fread(buf, 1, buf_size, fs_img_stream);
}


void ls(struct dinode *inodes, int inum) {
    char buf[BSIZE];
    struct dinode din;
    int offst, file_type;
    int dir_inode;
    char *dir_name;

    int size = inodes[inum].size;
    int n = size / BSIZE;

    for (int i = 0; i < n+1; i++)
    {
        read_sect(inodes[inum].addrs[i], buf, sizeof(buf));
        for(int j = 0; j < BSIZE; j += 16)
        {
            if (buf[j] > 0)
            {
                offst++;
                dir_name = buf + j + 2;
                dir_inode = buf[j];
                file_type = (int) inodes[dir_inode].type;
                printf("%-15s\t%d\t%d\t%d\n", dir_name, file_type, dir_inode, inodes[dir_inode].size);
            }
        }
    }
}


void cp(struct dinode *inodes, int inum, char *xv6file, char *linux_path) {
    char buf[BSIZE], addr_buf[BSIZE];

    FILE *linuxfile = fopen(linux_path, "w");
    if (NULL == linuxfile){
        printf("Could not open file %s under linux system", linux_path);
        exit(1);
    }

    read_sect(inodes[inum].addrs[0], buf, sizeof(buf));

    int wanted_file_inode_num = -1;
    for (int j=0; j < BSIZE; j += 16) {
        if (strcmp(buf + j + 2, xv6file) == 0) {
            wanted_file_inode_num = *(ushort*)(buf+j);
            break;
        }
    }

    // file wasn't found in root directory
    if (wanted_file_inode_num == -1) {
        printf("File %s does not exist in the root directory\n", xv6file);
        exit(1);
    }
        
    struct dinode *wanted_file_inode = &inodes[wanted_file_inode_num];

    int file_size = wanted_file_inode->size;
    int num_blocks = file_size / BSIZE;
    int trailing_bytes = file_size % BSIZE;
        
    for (int j = 0; j < num_blocks-1; j++)
    {
        if (j == NDIRECT) // read indirect block addresses
        {
            read_sect(wanted_file_inode->addrs[NDIRECT], addr_buf, sizeof(addr_buf));
        }
        if (j < NDIRECT) // read direct block
            read_sect(wanted_file_inode->addrs[j], buf, sizeof(buf));
        else // read indirect block
            read_sect(*(uint*)(addr_buf + (j - NDIRECT) * 4), buf, sizeof(buf));

        // write to file
        fwrite(buf, BSIZE, 1, linuxfile);
    }

    // handle trailing bytes
    if (trailing_bytes > 0)
    {
        if (num_blocks == NDIRECT) // read indirect block addresses
        {
            read_sect(wanted_file_inode->addrs[NDIRECT], addr_buf, sizeof(addr_buf));
        }
        
        if (num_blocks < NDIRECT) // read direct block
            read_sect(wanted_file_inode->addrs[num_blocks], buf, sizeof(buf));
        else // read indirect block
            read_sect(*(uint*)(addr_buf + (num_blocks - NDIRECT) * 4), buf, sizeof(buf));

        // write to file
        fwrite(buf, trailing_bytes, 1, linuxfile);
    }

    fclose(linuxfile);
    return;
}


void get_superblock(struct superblock* sb) {
    read_struct(1, sb, sizeof(struct superblock));
}


int main(int argc, char *argv[]) {
    struct superblock sb;
    uint inode_count;

    if (argc < 2) {
        printf("Error: No path to fs.img specified\n");
        exit(1);
    } else if (argc < 3) {
        printf("Error: No function type specified (ls or cp)\n");
        exit(1);
    }

    char *fs_img_path = argv[1];
    char *func_type = argv[2];

    fs_img_stream = fopen(fs_img_path, "r");

    if (NULL == fs_img_stream) {
        printf("Error: Unable to open fs.img file: %s\n", fs_img_path);
        exit(1);
    }

    get_superblock(&sb);

    inode_count = sb.ninodes;
    struct dinode inodes[sb.ninodes];

    for (int i = 0; i < inode_count; i++) {
        read_inode(sb.inodestart, &inodes[i], sizeof(struct dinode), i);
    }

    if (strcmp(func_type, "ls") == 0)  // print the contents of the root directory
    {
        ls(inodes, 1);
    }

    if (strcmp(func_type, "cp") == 0)  // copy file to Linux
    {
        if (argc < 5) {
            printf("Error: No file name specified\n");
            exit(1);
        }
        cp(inodes, ROOTINO, argv[3], argv[4]);
    }

    fclose(fs_img_stream);  // close the image file
}
