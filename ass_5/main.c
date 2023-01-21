#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "structs.h"

FILE* f;
int fsfd;


void read_sect(uint sec, void *buf, size_t buf_size) {
    lseek(fsfd, sec * BSIZE, 0);
    read(fsfd, buf, BSIZE);
}


void read_inode(uint sec, void *buf, size_t buf_size, uint inum) {
    fseek(f, sec * BSIZE + inum * buf_size, SEEK_SET);
    fread(buf, 1, buf_size, f);
}

void read_struct(uint sec, void *buf, size_t buf_size)
{
    fseek(f, sec * BSIZE, SEEK_SET);
    fread(buf, 1, buf_size, f);
}


void ls(struct dinode *inodes, int inum) {
    char buf[BSIZE];
    struct dinode din;
    struct dirent de;
    int offst, file_type;
    int dir_inode;
    char *dir_name;

    int size = inodes[inum].size;
    int n = size / BSIZE;
    printf(".\n");
    printf("..\n");

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
                printf("%s\t\t%d\t%d\t%d\n", dir_name, file_type, dir_inode, inodes[dir_inode].size);
            }
        }
    }
}


void cp(struct dinode *inodes, int inum, char *xv6file, char *linux_dir) {
    struct dinode din;
    char buf[BSIZE], addr_buf[BSIZE];
    int k = 0;
    int inode_num, linux_fd, file_size, n;

    linux_fd = open(linux_dir, O_WRONLY | O_CREAT, 0666);
    if (linux_fd < 0){
        printf("Could not open file %s under linux system", linux_dir);
        exit(1);
    }

    read_sect(inodes[inum].addrs[0], buf, sizeof(buf));

    for (int j = 0; j < BSIZE; j += 16) 
    {
        if (strcmp(buf + j + 2, xv6file) == 0)
        {
            din = inodes[inode_num];
            inode_num = buf[j];
            file_size = din.size;
            n = file_size / BSIZE;
            
            for (int j = 0; j < n; j++) 
            {
                if (j < NDIRECT)
                {
                    read_sect(din.addrs[j], buf, sizeof(buf));
                    write(linux_fd, buf, BSIZE);
                }
            }

            if (n > NDIRECT)
            {
                read_sect(din.addrs[NDIRECT], addr_buf, sizeof(buf));
                for (int k = 0; k < n - NDIRECT; k++) 
                {
                    read_sect(addr_buf[k], buf, sizeof(buf));
                    write(linux_fd, buf, BSIZE);
                }
            }
            
            if (file_size % BSIZE != 0) 
            {
                if (n < NDIRECT) 
                {
                    read_sect(din.addrs[n], buf, sizeof(buf));
                }
                else 
                {
                    read_sect(din.addrs[NDIRECT], addr_buf, sizeof(buf));
                    read_sect(addr_buf[n - NDIRECT], buf, sizeof(buf));
                }
                write(linux_fd, buf, file_size % BSIZE);
            }

            close(linux_fd);
            return;
        }
    }
    printf("File %s does not exist in the root directory\n", xv6file);
    exit(1);
}


void get_superblock(struct superblock* sb) {
    read_struct(1, sb, sizeof(struct superblock));
}


int main(int argc, char *argv[]) {
    struct superblock sb;
    uint inode_count;
    // struct dinode *inodes;

    char* file_name = "./fs.img";
    char* func_type = "ls";

    fsfd = open(argv[1], O_RDONLY, 0444);

    f = fopen(file_name, "r");

    if (NULL == f) {
        printf("Error: Unable to open fs.img file: %s\n", file_name);
        exit(1);
    }

    get_superblock(&sb);

    inode_count = sb.ninodes;
    printf("%d inodes\n", inode_count);
    struct dinode inodes[sb.ninodes];

    for (int i = 0; i < inode_count; i++) {
        read_inode(sb.inodestart, &inodes[i], sizeof(struct dinode), i);
    }

    if (strcmp(func_type, "ls") == 0)  // Print the contents of the root directory
    {
        ls(inodes, 1);
    }

    if (strcmp(func_type, "cp") == 0)  // Copy file to Linux
    {
        cp(inodes, ROOTINO, argv[3], argv[4]);
    }

    fclose(f);  // Close the image file
}
