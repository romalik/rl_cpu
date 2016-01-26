#include "rlfs3.h"
#include <blk.h>
#include <stdio.h>
#include <sched.h>

FILE openFiles[MAX_FILES];
struct fs_node fs_root;

/* RLFS3 filesystem
 *
 * Block0: superblock (not impl)
 * Block1-33: free block bitmap
 * Block34: root node
 *
 */

/* Fs node header:
 *  Flags (16):
 *    File
 *    Dir
 *    CharDevice
 *    BlockDevice
 *    Pipe
 *    Link
 *    rwxrwxrwx
 */

/* File/dir node:
 *  Flags
 *  SizeL(16)
 *  SizeH(16)
 *  150 indexes
 *  100 index-indexes
 *  1 triple-index
 */

/* Dir contents:
 * 0: [31 words - name, node idx]
 * 32: ...total 8...
 */

/* Device/pipe node:
 *  dev/pipe id
 */

void fs_mkfs() {
    struct Block *b;
    int i;
    size_t n;
    b = bread(0, 0);

    memcpy(b->data, (void *)"RLFS filesystem", 16);
    b->flags = BLOCK_MODIFIED;
    bfree(b);

    for (i = 1; i < 35; i++) {
        b = bread(0, i);
        for (n = 0; n < 256; n++) {
            b->data[n] = 0;
        }
        b->flags = BLOCK_MODIFIED;
        bfree(b);
    }
    b = bread(0, 1);
    b->data[0] = 0xffff;
    b->data[1] = 0xffff;
    b->data[2] = 0x0007;
    b->flags = BLOCK_MODIFIED;
    bfree(b);

    b = bread(0, 34);
    b->data[0] = FS_DIR;
    b->data[1] = 0x0000;
    b->data[2] = 0x0000;
    for (n = 3; n < 256; n++) {
        b->data[n] = 0;
    }
    b->flags = BLOCK_MODIFIED;
    bfree(b);

    block_sync();
}

void fs_init() {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        openFiles[i].mode = FS_MODE_NONE;
    }
    fs_root.idx = 34;
}

int fs_get_free_fd() {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        if (openFiles[i].mode == FS_MODE_NONE) {
            return i;
        }
    }
    return -1;
}

blk_t fs_findFreeSector() {
    int i;
    int j;
    int cBitmap = 0;
    struct Block *b;
    // for (cBitmap = 0; cBitmap < 32; cBitmap++) {
    b = bread(0, cBitmap + 1);
    for (i = 0; i < 256; i++) {
        if (b->data[i] != 0xffff) {
            for (j = 0; j < 16; j++) {
                if ((b->data[i] & (1 << j)) == 0) {
                    bfree(b);
/*
                    printf("Found free sector %d mask %04X i %d j %d\n",
                           (i << 4) + j, b->data[i], i, j);
                           */
                    return /*(cBitmap << 12) +*/ (i << 4) + j;
                }
            }
        }
    }
    bfree(b);
    //}
    return 0;
}

void fs_markSector(blk_t sect, int val) {
    int idx;
    int pos;
    int cBitmap;
    struct Block *b;

//    printf("Mark sector %d %d\n", sect, val);

    cBitmap = 0; // sect >> 12;
    sect = sect & 0xfff;
    idx = sect >> 4;
    pos = sect & 0x0f;
    b = bread(0, cBitmap + 1);
    if (val) {
        b->data[idx] = (b->data[idx] | (1 << pos));
    } else {
        b->data[idx] = (b->data[idx] & ~(1 << pos));
    }
    b->flags = BLOCK_MODIFIED;
    bfree(b);
}

blk_t fs_allocBlock() {
    blk_t newBlock;
    newBlock = fs_findFreeSector();
    fs_markSector(newBlock, 1);
//    printf("Alloc block %d\n", newBlock);
    return newBlock;
}

#define fs_freeBlock(b) fs_markSector((b), 0)

stat_t fs_stat(fs_node_t node) {
    struct Block *b;
    stat_t s;
    b = bread(0, node.idx);
    s.flags = b->data[0];
    s.size = b->data[1];
    // s.size |= (b->data[2]<<16);
    bfree(b);

//    printf("FS stat node %d flags %04x size %d\n", node.idx, s.flags, s.size);

    return s;
}

fs_node_t fs_create(fs_node_t where, unsigned int *name, unsigned int flags) {
    stat_t s;
    fs_node_t node;
    s = fs_stat(where);
    printf("FS create at node %d stat %04x name %s\n", where.idx, s.flags, name);
    if ((s.flags & 0xff) == FS_DIR) {
//        printf("is a dir\n");
        node = fs_finddir(where, name);
//        printf("Finddir result: %d\n", node.idx);
        if (node.idx != 0) {
            node.idx = 0;
        } else {
            int i;
            struct Block *b;
            blk_t newBlock = fs_allocBlock();

            printf("Dirent: at inode %d off %d write block %d\n", where.idx,
                   s.size, newBlock);
            fs_write(where, s.size, 31, name);
            fs_write(where, s.size + 31, 1, &newBlock);
            b = bread(0, newBlock);
            b->data[0] = flags;
            for (i = 1; i < 256; i++) {
                b->data[i] = 0;
            }
            b->flags = BLOCK_MODIFIED;
            bfree(b);
            node.idx = newBlock;
        }
    } else {
        node.idx = 0;
    }

    return node;
}

fs_node_t fs_finddir(fs_node_t where, unsigned int *what) {
    fs_node_t node;
    off_t i;
    stat_t s;

    printf("Finding dirent %s at node %d\n", what, where.idx);

    s = fs_stat(where);
    for (i = 0; i < s.size; i += 32) {
        dirent_t dEnt;
        fs_read(where, i, 32, (unsigned int *)&dEnt);
        if (!strcmp(dEnt.name, what)) {
            node.idx = dEnt.idx;
            printf("Found! %d\n", node.idx);
            return node;
        }
    }
    printf("Not found\n");
    node.idx = 0;
    return node;
}

dirent_t fs_readdir(fs_node_t dir, off_t n) {
    dirent_t dEnt;
    if (fs_read(dir, n, 32, (unsigned int *)&dEnt) == 32) {
        return dEnt;
    } else {
        dEnt.idx = 0;
        return dEnt;
    }
}

unsigned int fs_read(fs_node_t node, off_t offset, size_t size,
                     unsigned int *buf) {
    stat_t s;
    unsigned int offsetInBlock;
    blk_t cBlockIdx;
    blk_t cBlock;
    struct Block *nodeBlock;
    struct Block *currentBlock;
    size_t alreadyRead;

    s = fs_stat(node);
    nodeBlock = bread(0, node.idx);

    if (offset > s.size) {
        return 0;
    }

    if (offset + size > s.size) {
        size = s.size - offset;
    }

    alreadyRead = 0;
    cBlockIdx = offset >> 8;
    cBlock = nodeBlock->data[cBlockIdx + 3];
    offsetInBlock = offset & 0xff;

    while (size > 0) {
        size_t read_now = 256 - offsetInBlock;
        if (size < read_now) {
            read_now = size;
        }
        currentBlock = bread(0, cBlock);
        memcpy(buf + alreadyRead, currentBlock->data + offsetInBlock, read_now);

        offset += read_now;
        alreadyRead += read_now;
        size -= read_now;

        cBlockIdx = offset >> 8;
        cBlock = nodeBlock->data[cBlockIdx + 3];
        offsetInBlock = offset & 0xff;
        bfree(currentBlock);
    }

    bfree(nodeBlock);
    return alreadyRead;
}

unsigned int fs_write(fs_node_t node, off_t offset, size_t size,
                      unsigned int *buf) {
    stat_t s;
    unsigned int offsetInBlock;
    blk_t cBlockIdx;
    blk_t cBlock;
    struct Block *nodeBlock;
    struct Block *currentBlock;
    size_t alreadyWritten;

//    printf("Write [%s] to node %d off %d size %d\n", buf, node.idx, offset, size);


    nodeBlock = bread(0, node.idx);

    s.flags = nodeBlock->data[0];
    s.size = nodeBlock->data[1];


    if (offset > s.size) {
        return 0;
    }

    alreadyWritten = 0;
    cBlockIdx = offset >> 8;
    cBlock = nodeBlock->data[cBlockIdx + 3];
    if (cBlock == 0) {
        cBlock = fs_allocBlock();
        nodeBlock->data[cBlockIdx + 3] = cBlock;
    }
    offsetInBlock = offset & 0xff;

    while (size > 0) {
        size_t write_now = 256 - offsetInBlock;
        if (size < write_now) {
            write_now = size;
        }
        currentBlock = bread(0, cBlock);
        memcpy(currentBlock->data + offsetInBlock, buf + alreadyWritten,
               write_now);

        offset += write_now;
        alreadyWritten += write_now;
        size -= write_now;

        cBlockIdx = offset >> 8;
        cBlock = nodeBlock->data[cBlockIdx + 3];
        if (cBlock == 0) {
            cBlock = fs_allocBlock();
            nodeBlock->data[cBlockIdx + 3] = cBlock;
        }

        offsetInBlock = offset & 0xff;
        currentBlock->flags = BLOCK_MODIFIED;
        bfree(currentBlock);
    }

    if (offset > s.size) {
        nodeBlock->data[1] = offset;
        // nodeBlock->data[2] = offset >> 16;
    }

    nodeBlock->flags = BLOCK_MODIFIED;
    bfree(nodeBlock);
    return alreadyWritten;
}

void fs_reset(fs_node_t node) {
    int i;
    struct Block *b;
    b = bread(0, node.idx);
    b->data[1] = 0;
    b->data[2] = 0;
    for (i = 3; i < 256; i++) {
        if (b->data[i] == 0) {
            break;
        }
        fs_freeBlock(b->data[i]);
        b->data[i] = 0;
    }
    b->flags = BLOCK_MODIFIED;
    bfree(b);
}

FILE *fs_open(fs_node_t node, unsigned int mode) {
    int fd;
    stat_t s;
    fd = fs_get_free_fd();
    s = fs_stat(node);
    openFiles[fd].mode = mode;
    openFiles[fd].size = s.size;
    openFiles[fd].pos = 0;
    if (mode == FS_MODE_APPEND) {
        openFiles[fd].pos = s.size;
    }
    openFiles[fd].node = node;
    if (mode == FS_MODE_WRITE) {
        fs_reset(node);
    }

    return &openFiles[fd];
}

fs_node_t fs_lookup(unsigned int *name, fs_node_t * parent) {
    fs_node_t wd;
    size_t cStart = 0;
    size_t cEnd = 0;
    size_t cLen = 0;
    if (name[0] == '/') {
        wd = fs_root;
    } else {
        wd = cProc->cwd;
    }

    if(parent) 
        *parent = wd;

    if (*name == 0) {
        return wd;
    }

    while (1) {
        unsigned int cBuf[32];
        while (name[cStart] == '/')
            cStart++;

        if (name[cStart] == 0) {
            /* /path/to/directory/  <-- return node of directory */
            break;
        }

        cEnd = cStart + 1;
        while (name[cEnd] != '/' && name[cEnd] != 0)
            cEnd++;

        cLen = cEnd - cStart;
        memcpy(cBuf, name + cStart, cLen);
        cBuf[cLen] = 0;

        if(parent)
            *parent = wd;

        wd = fs_finddir(wd, cBuf);
        if (wd.idx == 0) {
            /* file not found */
            if(name[cEnd] != 0) { // "/path/to/nonexist/file" <-- shit happens already on nonexist dir
                //no such parent directory either
                
                if(parent)
                    parent->idx = 0;
            }
            break;
        }
        if (name[cEnd] == 0) {
            /* /path/to/some/shit <-- return node of shit */
            break;
        }

        cStart = cEnd;
    }

    return wd;
}



void fs_close(FILE *fd) {
    fd->mode = FS_MODE_NONE;
}

size_t k_write(FILE *fd, unsigned int *buf, size_t size) {
    fs_write(fd->node, fd->pos, size, buf);
    fd->pos += size;
    return size;
}

size_t k_read(FILE *fd, unsigned int *buf, size_t size) {
    if (size + fd->pos > fd->size) {
        size = fd->size - fd->pos;
    }
    fs_read(fd->node, fd->pos, size, buf);
    fd->pos += size;
    return size;
}

int k_isEOF(FILE *fd) {
    return (fd->pos >= fd->size);
}

FILE *k_open(void *__name, unsigned int mode) {
    fs_node_t parent;
    fs_node_t file;
    unsigned int * name = (unsigned int *)__name;

    file = fs_lookup(name, &parent);
    printf("File lookup result %d\n", file.idx);
    if (parent.idx) {
        if (file.idx) {
            return fs_open(file, mode);
        } else {
            /* get filename */
            unsigned int * s;
            for(s = name + strlen(name); s>=name; s--) {
                if(*s == '/') {
                    s++;
                    break;
                }
            }
            printf("Cropped filename %s\n", s);

            if (mode == FS_MODE_WRITE || mode == FS_MODE_APPEND) {
                file = fs_create(parent, s, FS_FILE);
                return fs_open(file, mode);
            } else {
                // no file
                return NULL;
            }
        }
    } else {
        // no dir
        return NULL;
    }
}
stat_t k_stat(void *name) {
    fs_node_t nd;

    nd = fs_lookup(name, NULL);
    if (nd.idx) {
        return fs_stat(nd);
    } else {
        stat_t res;
        res.flags = FS_NONE;
        return res;
    }
}

void k_close(FILE *fd) {
    fd->mode = FS_NONE;
}

void k_seek(FILE *fd, off_t pos) {
    if (fd->size < pos) {
        fd->pos = fd->size;
    } else {
        fd->pos = pos;
    }
}

FILE * k_opendir(void *dirname) {
    fs_node_t d_inode;

    d_inode = fs_lookup(dirname, NULL);
    if(d_inode.idx == 0) {
        return NULL;
    } else {
        return fs_open(d_inode, 'r');
    }
}

dirent_t k_readdir(FILE * dir) {
    dirent_t res;
    if(dir->size > dir->pos) {
        res = fs_readdir(dir->node, dir->pos);
        dir->pos += 32;
    } else {
        res.idx = 0;
    }
    return res;
}

int k_mkdir(void *__path) {
    fs_node_t dir;
    fs_node_t parent;
    
    unsigned int * path = (unsigned int *)__path;

    dir = fs_lookup(path, &parent);

    if(dir.idx) {
        //file exists!
        return 0;
    } else {
        if(parent.idx == 0) {
            //parent doesn't exist!
            return 0;
        } else {
            unsigned int * s;
            for(s = path + strlen(path); s>=path; s--) {
                if(*s == '/') {
                    s++;
                    break;
                }
            }
            dir = fs_create(parent, s, FS_DIR);

            fs_write(dir, 0, 31, (unsigned int *)("."));
            fs_write(dir, 31, 1, &(dir.idx));
            fs_write(dir, 32, 31, (unsigned int *)(".."));
            fs_write(dir, 63, 1, &(parent.idx));

            return 0;
        }
    }
}
