#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "structs.h"

int fsfd;


void read_sect(uint sec, void *buf)
{
  lseek(fsfd, sec * BSIZE, SEEK_SET);
  read(fsfd, buf, BSIZE);
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

    for (int i = 0; i < n+1; i++)
    {
        read_sect(inodes[inum].addrs[i], buf);
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

    read_sect(inodes[inum].addrs[0], buf);

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
                    read_sect(din.addrs[j], buf);
                    write(linux_fd, buf, BSIZE);
                }
            }

            if (n > NDIRECT)
            {
                read_sect(din.addrs[NDIRECT], addr_buf);
                for (int k = 0; k < n - NDIRECT; k++) 
                {
                    read_sect(addr_buf[k], buf);
                    write(linux_fd, buf, BSIZE);
                }
            }
            
            if (file_size % BSIZE != 0) 
            {
                if (n < NDIRECT) 
                {
                    read_sect(din.addrs[n], buf);
                }
                else 
                {
                    read_sect(din.addrs[NDIRECT], addr_buf);
                    read_sect(addr_buf[n - NDIRECT], buf);
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


void read_inode(struct dinode* inode, int inode_start, int num) {
    unsigned char buf[BSIZE];
    int sector = inode_start + num * IPB;

    memset(buf, 0, sizeof(buf));
    read_sect(sector, buf);
    memmove(inode, buf + num % IPB * sizeof(*inode), sizeof(*inode));
}


void get_inodes(struct dinodes **inodes, struct superblock* sb,  int inode_count) {
    for (int i = 0; i < inode_count; i++) {
        read_inode(inodes[i], sb->inodestart, i);
    }
}


void get_superblock(int fsfd, struct superblock* sb) {
    unsigned char buf[BSIZE];

    memset(buf, 0, sizeof(buf));
    read_sect(1, buf);
    memmove(sb, buf, sizeof(*sb));
}


int main(int argc, char *argv[]) {
    int fsfd;
    struct superblock sb;
    uint inode_count;
    struct dinode *inodes;

    char* file_name = "/home/ubuntu/repos/xv6-public/fs.img";
    char* func_type = "ls";

    if ((fsfd = open(file_name, O_RDONLY, 0444)) < 0) {
        printf("Error: Unable to open fs.img file: %s\n", file_name);
        exit(1);
    }

    get_superblock(fsfd, &sb);

    inode_count = sb.ninodes;
    printf("%d inodes\n", inode_count);
    inodes = (struct dinode *) malloc(sizeof(struct dinode) * inode_count);

    get_inodes(&inodes, &sb, inode_count);

    if (strcmp(func_type, "ls") == 0)  // Print the contents of the root directory
    {
        ls(inodes, ROOTINO);
    }

    if (strcmp(func_type, "cp") == 0)  // Copy file to Linux
    {
        cp(inodes, ROOTINO, argv[3], argv[4]);
    }

    close(fsfd);  // Close the image file
    free(inodes);
}