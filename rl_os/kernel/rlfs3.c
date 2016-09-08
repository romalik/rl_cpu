#include "rlfs3.h"
#include <blk.h>
#if STANDALONE
#include <stdio.h>
#else
#include <kstdio.h>
#include <sched.h>
#endif
FILE openFiles[MAX_FILES];
struct fs_node fs_root;
struct devOpTable devList[MAX_DEVS];

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
 *  Flags
 *  major|minor
 */

void fs_mkfs() {
    struct Block *b;
    int i;
    size_t n;
    fs_node_t dir;
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
    b->data[0] = S_IFDIR;
    b->data[1] = 0x0000;
    b->data[2] = 0x0000;
    for (n = 3; n < 256; n++) {
        b->data[n] = 0;
    }
    b->flags = BLOCK_MODIFIED;
    bfree(b);

    dir.idx = 34;

    fs_write(&dir, 0, 31, (unsigned int *)("."));
    fs_write(&dir, 31, 1, &(dir.idx));
    fs_write(&dir, 32, 31, (unsigned int *)(".."));
    fs_write(&dir, 63, 1, &(dir.idx));

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
    unsigned int cBitmap = 0;
    struct Block *b;
    for (cBitmap = 0; cBitmap < 32; cBitmap++) {
        b = bread(0, cBitmap + 1);
        for (i = 0; i < 256; i++) {
            if (b->data[i] != 0xffff) {
                for (j = 0; j < 16; j++) {
                    if ((b->data[i] & (1 << j)) == 0) {
                        bfree(b);
                        /*
                                            printf("Found free sector %d mask
                           %04X i
                           %d j %d\n",
                                                   (i << 4) + j, b->data[i], i,
                           j);
                                                   */
                        return (cBitmap << 12) + (i << 4) + j;
                    }
                }
            }
        }
        bfree(b);
    }
    return 0;
}

void fs_markSector(blk_t sect, int val) {
    int idx;
    int pos;
    unsigned int cBitmap;
    struct Block *b;

    //    printf("Mark sector %d %d\n", sect, val);

    cBitmap = sect >> 12;
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

int fs_stat(fs_node_t *node, struct stat *res) {
    struct Block *b;

    b = bread(0, node->idx);

    res->st_dev = 0;
    res->st_ino = node->idx;
    res->st_mode = b->data[0];
    res->st_nlink = 0;
    res->st_uid = 0;
    res->st_gid = 0;
    res->st_rdev = 0;
    res->st_size = b->data[1];
    res->st_atime = 0;
    res->st_mtime = 0;
    res->st_ctime = 0;

    if (S_ISCHR(res->st_mode)) {
        res->st_rdev = res->st_size;
        res->st_size = 1;
    }

    // s.size |= (b->data[2]<<16);
    bfree(b);

    // printf("FS stat node %d flags %04x size %d\n", node->idx, res->flags,
    // res->size);

    return FS_OK;
}

int fs_create(fs_node_t *where, const unsigned int *name, unsigned int flags,
              fs_node_t *res) {
    struct stat s;
    int rv;
    fs_stat(where, &s);
    // printf("FS create at node %d stat %04x name %s\n", where->idx, s.flags,
    //       name);
    if (S_ISDIR(s.st_mode)) {
        // printf("Dir ok! check file\n");
        rv = fs_finddir(where, name, res);
        // printf("fs_finddir(%s) = %d\n", name, rv);
        if (rv == FS_OK) {
            return FS_FILE_EXISTS;
        } else {
            int i;
            struct Block *b;
            blk_t newBlock = fs_allocBlock();

            // printf("Dirent: at inode %d off %d write block %d\n", where->idx,
            //       s.size, newBlock);
            fs_write(where, s.st_size, 31, name);
            fs_write(where, s.st_size + 31, 1, &newBlock);
            b = bread(0, newBlock);
            b->data[0] = flags;
            for (i = 1; i < 256; i++) {
                b->data[i] = 0;
            }
            b->flags = BLOCK_MODIFIED;
            bfree(b);
            res->idx = newBlock;
        }
    } else {
        return FS_NOT_A_DIR;
    }

    return FS_OK;
}

int fs_finddir(fs_node_t *where, const unsigned int *what, fs_node_t *res) {
    off_t i;
    struct stat s;

    // printf("Finding dirent %s at node %d\n", what, where->idx);

    fs_stat(where, &s);
    for (i = 0; i < s.st_size; i += 32) {
        dirent_t dEnt;
        fs_read(where, i, 32, (unsigned int *)&dEnt);
        if (!strcmp(dEnt.name, what)) {
            res->idx = dEnt.idx;
            // printf("Found! %d\n", res->idx);
            return FS_OK;
        }
    }
    // printf("Not found\n");
    return FS_NO_FILE;
}

int fs_readdir(fs_node_t *dir, off_t n, dirent_t *res) {
    if (fs_read(dir, n, 32, (unsigned int *)res) == 32) {
        return FS_OK;
    } else {
        return FS_NO_FILE;
    }
}

unsigned int fs_read(fs_node_t *node, off_t offset, size_t size,
                     unsigned int *buf) {
    struct stat s;
    unsigned int offsetInBlock;
    blk_t cBlockIdx;
    blk_t cBlock;
    struct Block *nodeBlock;
    struct Block *currentBlock;
    size_t alreadyRead;

    nodeBlock = bread(0, node->idx);

    s.st_mode = nodeBlock->data[0];
    s.st_size = nodeBlock->data[1];

    if (offset > s.st_size) {
        return 0;
    }

    if (offset + size > s.st_size) {
        size = s.st_size - offset;
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

unsigned int fs_write(fs_node_t *node, off_t offset, size_t size,
                      const unsigned int *buf) {
    struct stat s;
    unsigned int offsetInBlock;
    blk_t cBlockIdx;
    blk_t cBlock;
    struct Block *nodeBlock;
    struct Block *currentBlock;
    size_t alreadyWritten;

    // printf("Write [%s] to node %d off %d size %d\n", buf, node->idx,
    // offset, size);

    nodeBlock = bread(0, node->idx);

    s.st_mode = nodeBlock->data[0];
    s.st_size = nodeBlock->data[1];

    if (offset > s.st_size) {
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

    if (offset > s.st_size) {
        nodeBlock->data[1] = offset;
        // nodeBlock->data[2] = offset >> 16;
    }

    nodeBlock->flags = BLOCK_MODIFIED;
    bfree(nodeBlock);
    return alreadyWritten;
}

void fs_reset(fs_node_t *node) {
    int i;
    struct Block *b;
    b = bread(0, node->idx);
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

FILE *fs_open(fs_node_t *node, unsigned int mode) {
    int fd;
    struct stat s;
    fd = fs_get_free_fd();
    fs_stat(node, &s);
    openFiles[fd].mode = mode;
    openFiles[fd].size = s.st_size;
    openFiles[fd].flags = s.st_mode;
    openFiles[fd].device = s.st_rdev;
    openFiles[fd].pos = 0;
    if (mode & O_APPEND) {
        openFiles[fd].pos = s.st_size;
    }
    openFiles[fd].node = *node;
    if (mode == O_WRONLY && !S_ISCHR(s.st_mode) && !S_ISBLK(s.st_mode)) {
        fs_reset(node);
    }

    return &openFiles[fd];
}

int fs_lookup(const unsigned int *name, fs_node_t *parent, fs_node_t *res) {
    size_t cStart = 0;
    size_t cEnd = 0;
    size_t cLen = 0;
    int rv;
    if (name[0] == '/') {
        *res = fs_root;
    } else {
#if STANDALONE
        *res = fs_root;
#else
        *res = cProc->cwd;
#endif
    }

    if (parent)
        *parent = *res;

    if (*name == 0) {
        return FS_OK;
    }

    while (1) {
        unsigned int cBuf[32];
        while (name[cStart] == '/')
            cStart++;

        if (name[cStart] == 0) {
            /* /path/to/directory/  <-- return node of directory */
            return FS_OK;
        }

        cEnd = cStart + 1;
        while (name[cEnd] != '/' && name[cEnd] != 0)
            cEnd++;

        cLen = cEnd - cStart;
        memcpy(cBuf, name + cStart, cLen);
        cBuf[cLen] = 0;

        if (parent)
            *parent = *res;

        rv = fs_finddir(res, cBuf, res);
        if (rv != FS_OK) {
            /* file not found */
            if (name[cEnd] != 0) { // "/path/to/nonexist/file" <-- shit happens
                                   // already on nonexist dir
                // no such parent directory either
                return FS_NO_DIR;
            }
            return FS_NO_FILE;
        }
        if (name[cEnd] == 0) {
            /* /path/to/some/shit <-- return node of shit */
            return FS_OK;
        }

        cStart = cEnd;
    }

    return FS_OK; // should never get here o_O
}

void fs_close(FILE *fd) {
    fd->mode = FS_MODE_NONE;
}

size_t k_write(FILE *fd, const unsigned int *buf, size_t size) {
  size_t written = 0;
  if (S_ISBLK(fd->flags)) {
        return 0;
    } else if (S_ISCHR(fd->flags)) {
        unsigned int major;
        unsigned int minor;
        struct devOpTable *ops;
        major = (fd->device >> 8);
        minor = (fd->device & 0xff);
        ops = &devList[major];
        written = ops->write(minor, buf, size);
        return written;
    } else {
        written = fs_write(&(fd->node), fd->pos, size, buf);
        fd->pos += written;
        return written;
    }
}

size_t k_read(FILE *fd, unsigned int *buf, size_t size) {
  size_t alreadyRead = 0;
    if (S_ISBLK(fd->flags)) {
        return 0;
    } else if (S_ISCHR(fd->flags)) {
        unsigned int major;
        unsigned int minor;
        struct devOpTable *ops;
        major = (fd->device >> 8);
        minor = (fd->device & 0xff);
        ops = &devList[major];
        alreadyRead = ops->read(minor, buf, size);
        return alreadyRead;
    } else {
        if (size + fd->pos > fd->size) {
            size = fd->size - fd->pos;
        }
        alreadyRead = fs_read(&(fd->node), fd->pos, size, buf);
        fd->pos += alreadyRead;
        return alreadyRead;
    }
}

int k_isEOF(FILE *fd) {
    return (fd->pos >= fd->size);
}

FILE *k_open(const void *__name, unsigned int mode) {
    fs_node_t parent;
    fs_node_t file;
    int rv;
    unsigned int *name = (unsigned int *)__name;

    rv = fs_lookup(name, &parent, &file);
    // printf("File lookup result %d\n", rv);
    if (rv != FS_NO_DIR) {
        if (rv == FS_OK) {
            return fs_open(&file, mode);
        } else {
            /* get filename */
            unsigned int *s = name + strlen(name);
            while (s >= name) {
                if (*s == '/') {
                    break;
                }
                s--;
            }
            s++;
            // printf("Cropped filename %s\n", s);

            if (mode & O_WRONLY || mode & O_APPEND || mode & O_CREAT) {
                rv = fs_create(&parent, s, S_IFREG, &file);
                // printf("fs create rv %d\n", rv);
                return fs_open(&file, mode);
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
int k_stat(const void *name, struct stat * res) {
    fs_node_t nd;
    int rv;
    rv = fs_lookup(name, NULL, &nd);
    if (rv == FS_OK) {
        fs_stat(&nd, res);
        return 0;
    } else {
        res->st_mode = 0;
        return -1;
    }
}

void k_close(FILE *fd) {
    fd->mode = FS_MODE_NONE;
}

void k_seek(FILE *fd, off_t pos) {
    if (fd->size < pos) {
        fd->pos = fd->size;
    } else {
        fd->pos = pos;
    }
}

FILE *k_opendir(const void *dirname) {
    fs_node_t d_inode;
    int rv;
    rv = fs_lookup(dirname, NULL, &d_inode);
    if (rv != FS_OK) {
        return NULL;
    } else {
        return fs_open(&d_inode, 'r');
    }
}

dirent_t k_readdir(FILE *dir) {
    dirent_t res;
    int rv;
    if (dir->size > dir->pos) {
        rv = fs_readdir(&(dir->node), dir->pos, &res);
        dir->pos += 32;
    } else {
        res.idx = 0;
    }
    return res;
}

int k_mkdir(const void *__path) {
    fs_node_t dir;
    fs_node_t parent;
    int rv;
    unsigned int *path = (unsigned int *)__path;

    rv = fs_lookup(path, &parent, &dir);

    if (rv == FS_OK) {
        // file exists!
        return FS_FILE_EXISTS;
    } else {
        if (rv == FS_NO_DIR) {
            // parent doesn't exist!
            return FS_NO_DIR;
        } else {
            unsigned int *s = path + strlen(path);
            while (s >= path) {
                if (*s == '/') {
                    break;
                }
                s--;
            }
            s++;
            rv = fs_create(&parent, s, S_IFDIR, &dir);
            // printf("fs create rv %d\n", rv);

            fs_write(&dir, 0, 31, (unsigned int *)("."));
            fs_write(&dir, 31, 1, &(dir.idx));
            fs_write(&dir, 32, 31, (unsigned int *)(".."));
            fs_write(&dir, 63, 1, &(parent.idx));

            return 0;
        }
    }
}

int k_mknod(const void *__path, int type, unsigned int major, unsigned int minor) {
    fs_node_t devNode;
    fs_node_t parent;
    int rv;
    unsigned int *path = (unsigned int *)__path;

    rv = fs_lookup(path, &parent, &devNode);

    if (rv == FS_OK) {
        // file exists!
        return FS_FILE_EXISTS;
    } else {
        if (rv == FS_NO_DIR) {
            // parent doesn't exist!
            return FS_NO_DIR;
        } else {
            unsigned int *s = path + strlen(path);
            struct Block *b;
            while (s >= path) {
                if (*s == '/') {
                    break;
                }
                s--;
            }
            s++;
            if (type == 'c') {
                type = S_IFCHR;
            } else {
                type = S_IFBLK;
            }
            rv = fs_create(&parent, s, type, &devNode);
            // printf("fs create rv %d\n", rv);
            b = bread(0, devNode.idx);
            b->data[1] = ((major << 8) | (minor & 0xff));

            printf("mknod: create dev maj %d min %d mask %d\n", major, minor, b->data[1]);
            b->flags = BLOCK_MODIFIED;
            bfree(b);

            return 0;
        }
    }
}

int k_regDevice(unsigned int major, void *writeFunc, void *readFunc) {
    if (major < MAX_DEVS) {
        devList[major].write = writeFunc;
        devList[major].read = readFunc;
    }
    return 0;
}
