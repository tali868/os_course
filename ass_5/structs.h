#ifndef STRUCTS_H 
#define STRUCTS_H

#include <sys/types.h>

#define ROOTINO  1   // root i-number
#define BSIZE 1024  // block size

#define FSMAGIC 0x10203040

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Special device

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB           (BSIZE*8)

typedef struct superblock {
  uint magic;        // Must be FSMAGIC
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
} superblock;

// Inode structure
typedef struct dinode {
    short type;    // File type
    short major;   // Major device number (T_DEV only)
    short minor;   // Minor device number (T_DEV only)
    short nlink;   // Number of links to inode in file system
    uint size; // Size of file (bytes)
    uint addrs[NDIRECT+1];  // Data block addresses
} dinode;

// Directory entry structure
typedef struct dirent {
    ushort inum;          // Inode number
    char name[DIRSIZ];    // File name
} dirent;

#endif