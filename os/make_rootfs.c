#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define FILE MY_FILE
#define stat_t my_stat_t
#define dirent_t my_dirent_t
#define dirent my_dirent

#include "make_rootfs.h"

uint16_t *hdd_image;

void ataWriteSectorsLBA(uint16_t n, uint16_t *data) {
    memcpy(hdd_image + n * 256, data, 256);
}

void ataReadSectorsLBA(uint16_t n, uint16_t *data) {
    memcpy(data, hdd_image + n * 256, 256);
}

struct Block blockCache[BLOCK_CACHE_SIZE];
uint16_t blockDataCache[BLOCK_CACHE_SIZE * 64 * 4];

void dump_blocks() {
    int i;
    for (i = 0; i < BLOCK_CACHE_SIZE; i++) {
        printf("Block cache %d flags %d device %d n %d cnt %d\n", i,
               blockCache[i].flags, blockCache[i].device, blockCache[i].n,
               blockCache[i].cnt);
    }
}

void block_sync() {
    int i;
    //    printf("Try sync\n");
    //    dump_blocks();
    for (i = 0; i < BLOCK_CACHE_SIZE; i++) {
        if (blockCache[i].cnt == 0) {
            if (blockCache[i].flags == BLOCK_MODIFIED) {
                ataWriteSectorsLBA(blockCache[i].n, blockCache[i].data);
            }
            blockCache[i].flags = BLOCK_FREE;
        }
    }
}

struct Block *bread(uint16_t device, uint16_t n) {
    uint16_t i;
    //    printf("bread: %d %d\n", device, n);
    while (1) {
        for (i = 0; i < BLOCK_CACHE_SIZE; i++) {
            if (blockCache[i].flags != BLOCK_FREE) {
                if (blockCache[i].device == device && blockCache[i].n == n) {
                    blockCache[i].cnt++;
                    //          printf("bread ret %d\n", i);
                    return &blockCache[i];
                }
            }
        }
        for (i = 0; i < BLOCK_CACHE_SIZE; i++) {
            if (blockCache[i].flags == BLOCK_FREE) {
                blockCache[i].flags = BLOCK_USED;
                blockCache[i].device = device;
                blockCache[i].n = n;
                blockCache[i].cnt = 1;
                blockCache[i].data = &blockDataCache[64 * 4 * i];
                ataReadSectorsLBA(n, blockCache[i].data);
                //        printf("bread fetch and ret %d\n", i);
                return &blockCache[i];
            }
        }
        block_sync();
    }
}
void bfree(struct Block *b) {
    //  printf("bfree\n");
    b->cnt--;
}

void block_init() {
    uint16_t i;
    for (i = 0; i < BLOCK_CACHE_SIZE; i++) {
        blockCache[i].flags = BLOCK_FREE;
    }
}

FILE openFiles[MAX_FILES];

struct fs_node fs_root;
struct fs_node fs_cwd;

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

    memcpy(b->data, (void *)L"RLFS filesystem", 16);
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

    dir.idx = 34;

    fs_write(&dir, 0, 31, (uint16_t *)(L"."));
    fs_write(&dir, 31, 1, &(dir.idx));
    fs_write(&dir, 32, 31, (uint16_t *)(L".."));
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
                                        printf("Found free sector %d mask %04X i
                       %d j %d\n",
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

int fs_stat(fs_node_t *node, stat_t *res) {
    struct Block *b;

    b = bread(0, node->idx);
    res->flags = b->data[0];
    res->size = b->data[1];
    if (res->flags == FS_CHAR_DEV) {
        res->size = 1;
    }

    res->node = *node;
    // s.size |= (b->data[2]<<16);
    bfree(b);

    // printf("FS stat node %d flags %04x size %d\n", node->idx, res->flags,
    // res->size);

    return FS_OK;
}

int fs_create(fs_node_t *where, uint16_t *name, uint16_t flags,
              fs_node_t *res) {
    stat_t s;
    int rv;
    fs_stat(where, &s);
    // printf("FS create at node %d stat %04x name %s\n", where->idx, s.flags,
    //       name);
    if ((s.flags & 0xff) == FS_DIR) {
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
            fs_write(where, s.size, 31, name);
            fs_write(where, s.size + 31, 1, &newBlock);
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

int fs_finddir(fs_node_t *where, uint16_t *what, fs_node_t *res) {
    off_t i;
    stat_t s;

    printf("Finding dirent %s at node %d\n", what, where->idx);

    fs_stat(where, &s);
    for (i = 0; i < s.size; i += 32) {
        dirent_t dEnt;
        fs_read(where, i, 32, (uint16_t *)&dEnt);
        if (!wcscmp(dEnt.name, what)) {
            res->idx = dEnt.idx;
            printf("Found! %d\n", res->idx);
            return FS_OK;
        }
    }
    printf("Not found\n");
    return FS_NO_FILE;
}

int fs_readdir(fs_node_t *dir, off_t n, dirent_t *res) {
    if (fs_read(dir, n, 32, (uint16_t *)res) == 32) {
        return FS_OK;
    } else {
        return FS_NO_FILE;
    }
}

uint16_t fs_read(fs_node_t *node, off_t offset, size_t size, uint16_t *buf) {
    stat_t s;
    uint16_t offsetInBlock;
    blk_t cBlockIdx;
    blk_t cBlock;
    struct Block *nodeBlock;
    struct Block *currentBlock;
    size_t alreadyRead;

    nodeBlock = bread(0, node->idx);

    s.flags = nodeBlock->data[0];
    s.size = nodeBlock->data[1];

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
        memcpy(buf + alreadyRead, currentBlock->data + offsetInBlock,
               read_now * 2);

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

uint16_t fs_write(fs_node_t *node, off_t offset, size_t size, uint16_t *buf) {
    stat_t s;
    uint16_t offsetInBlock;
    blk_t cBlockIdx;
    blk_t cBlock;
    struct Block *nodeBlock;
    struct Block *currentBlock;
    size_t alreadyWritten;

    // printf("Write [%s] to node %d off %d size %d\n", buf, node->idx,
    // offset, size);

    nodeBlock = bread(0, node->idx);

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
               write_now * 2);

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

FILE *fs_open(fs_node_t *node, uint16_t mode) {
    int fd;
    stat_t s;
    fd = fs_get_free_fd();
    fs_stat(node, &s);
    openFiles[fd].mode = mode;
    openFiles[fd].size = s.size;
    openFiles[fd].flags = s.flags;
    openFiles[fd].pos = 0;
    if (mode == FS_MODE_APPEND) {
        openFiles[fd].pos = s.size;
    }
    openFiles[fd].node = *node;
    if (mode == FS_MODE_WRITE && s.flags != FS_BLOCK_DEV &&
        s.flags != FS_CHAR_DEV) {
        fs_reset(node);
    }

    return &openFiles[fd];
}

void fs_cd(uint16_t *name) {
    stat_t s;
    s = k_stat(name);
    fs_cwd = s.node;
}

int fs_lookup(uint16_t *name, fs_node_t *parent, fs_node_t *res) {
    size_t cStart = 0;
    size_t cEnd = 0;
    size_t cLen = 0;
    int rv;
    if (name[0] == '/') {
        *res = fs_root;
    } else {
        *res = fs_cwd;
    }

    if (parent)
        *parent = *res;

    if (*name == 0) {
        return FS_OK;
    }

    while (1) {
        uint16_t cBuf[32];
        while (name[cStart] == L'/')
            cStart++;

        if (name[cStart] == 0) {
            /* /path/to/directory/  <-- return node of directory */
            return FS_OK;
        }

        cEnd = cStart + 1;
        while (name[cEnd] != L'/' && name[cEnd] != 0)
            cEnd++;

        cLen = cEnd - cStart;
        memcpy(cBuf, name + cStart, cLen * sizeof(uint16_t));
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

size_t k_write(FILE *fd, uint16_t *buf, size_t size) {
    if (fd->flags == FS_BLOCK_DEV) {
        return 0;
    } else if (fd->flags == FS_CHAR_DEV) {
        uint16_t major;
        uint16_t minor;
        struct devOpTable *ops;
        major = (fd->device >> 8);
        minor = (fd->device & 0xff);
        ops = &devList[major];
        while (size) {
            ops->write(minor, *buf);
            buf++;
            size--;
        }
        return size;
    } else {
        fs_write(&(fd->node), fd->pos, size, buf);
        fd->pos += size;
        return size;
    }
}

size_t k_read(FILE *fd, uint16_t *buf, size_t size) {
    if (fd->flags == FS_BLOCK_DEV) {
        return 0;
    } else if (fd->flags == FS_CHAR_DEV) {
        uint16_t major;
        uint16_t minor;
        struct devOpTable *ops;
        major = (fd->device >> 8);
        minor = (fd->device & 0xff);
        ops = &devList[major];
        while (size) {
            *buf = ops->read(minor);
            buf++;
            size--;
        }
        return size;
    } else {
        if (size + fd->pos > fd->size) {
            size = fd->size - fd->pos;
        }
        fs_read(&(fd->node), fd->pos, size, buf);
        fd->pos += size;
        return size;
    }
}

int k_isEOF(FILE *fd) {
    return (fd->pos >= fd->size);
}

FILE *k_open(void *__name, uint16_t mode) {
    fs_node_t parent;
    fs_node_t file;
    int rv;
    uint16_t *name = (uint16_t *)__name;

    rv = fs_lookup(name, &parent, &file);
    // printf("File lookup result %d\n", rv);
    if (rv != FS_NO_DIR) {
        if (rv == FS_OK) {
            return fs_open(&file, mode);
        } else {
            /* get filename */
            uint16_t *s = name + wcslen(name);
            while (s >= name) {
                if (*s == '/') {
                    break;
                }
                s--;
            }
            s++;
            // printf("Cropped filename %s\n", s);

            if (mode == FS_MODE_WRITE || mode == FS_MODE_APPEND) {
                rv = fs_create(&parent, s, FS_FILE, &file);
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
stat_t k_stat(void *name) {
    fs_node_t nd;
    stat_t res;
    int rv;
    rv = fs_lookup(name, NULL, &nd);
    if (rv == FS_OK) {
        fs_stat(&nd, &res);
        return res;
    } else {
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

FILE *k_opendir(void *dirname) {
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

int k_mkdir(void *__path) {
    fs_node_t dir;
    fs_node_t parent;
    int rv;
    uint16_t *path = (uint16_t *)__path;

    rv = fs_lookup(path, &parent, &dir);

    if (rv == FS_OK) {
        // file exists!
        return FS_FILE_EXISTS;
    } else {
        if (rv == FS_NO_DIR) {
            // parent doesn't exist!
            return FS_NO_DIR;
        } else {
            uint16_t *s = path + wcslen(path);
            while (s >= path) {
                if (*s == '/') {
                    break;
                }
                s--;
            }
            s++;
            rv = fs_create(&parent, s, FS_DIR, &dir);
            // printf("fs create rv %d\n", rv);

            fs_write(&dir, 0, 31, (uint16_t *)(L"."));
            fs_write(&dir, 31, 1, &(dir.idx));
            fs_write(&dir, 32, 31, (uint16_t *)(L".."));
            fs_write(&dir, 63, 1, &(parent.idx));

            return 0;
        }
    }
}

int k_mknod(void *__path, int type, uint16_t major, uint16_t minor) {
    fs_node_t devNode;
    fs_node_t parent;
    int rv;
    uint16_t *path = (uint16_t *)__path;

    rv = fs_lookup(path, &parent, &devNode);

    if (rv == FS_OK) {
        // file exists!
        return FS_FILE_EXISTS;
    } else {
        if (rv == FS_NO_DIR) {
            // parent doesn't exist!
            return FS_NO_DIR;
        } else {
            uint16_t *s = path + wcslen(path);
            struct Block *b;
            while (s >= path) {
                if (*s == '/') {
                    break;
                }
                s--;
            }
            s++;
            if (type == 'c') {
                type = FS_CHAR_DEV;
            } else {
                type = FS_BLOCK_DEV;
            }
            rv = fs_create(&parent, s, type, &devNode);
            // printf("fs create rv %d\n", rv);
            b = bread(0, devNode.idx);
            b->data[1] = ((major << 8) | (minor & 0xff));
            b->flags = BLOCK_MODIFIED;
            bfree(b);

            return 0;
        }
    }
}

int k_regDevice(uint16_t major, void *writeFunc, void *readFunc) {
    if (major < MAX_DEVS) {
        devList[major].write = writeFunc;
        devList[major].read = readFunc;
    }
    return 0;
}

#undef FILE
#undef stat_t
#undef dirent_t
#undef dirent

#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void charToUInt16(void *dest, const char *src) {
    char *destC = dest;
    char *s = src;
    while (*s) {
        *destC = *s;
        destC++;
        *destC = 0;
        destC++;
        s++;
    }
    *destC = 0;
    destC++;
    *destC = 0;
}

void display_contents(char *name) {
    struct stat sb;
    struct tm *t;
    char link_read[255];
    ssize_t bytes_read;
    lstat(name, &sb);

    printf("%s ", S_ISDIR(sb.st_mode) ? "directory" : "file     ");

    printf("%5.0lu ", sb.st_size);

    printf("%s\n", name);

    uint16_t nameW[100];

    block_sync();

    charToUInt16(nameW, name);

    if (name[0] == '.') {
        printf("skip\n");
    } else if (S_ISDIR(sb.st_mode)) {
        printf("create dir %s\n", name);
        k_mkdir(nameW);
        printf("fs_cd: %d -> ", fs_cwd.idx);
        fs_cd(nameW);
        printf("%d\n", fs_cwd.idx);
        chdir(name);
        DIR *d = opendir(".");
        get_contents(d);
        chdir("..");
        fs_cd(L"..");
    } else {
        printf("create file %s\n", name);
        MY_FILE *myFile = k_open(nameW, FS_MODE_WRITE);
        FILE *fd = fopen(name, "r");
        int n = 0;

        uint16_t buf[2];

        while (n = fread(buf, sizeof(uint16_t), 1, fd)) {
            uint16_t cBuf = 0;
            cBuf = ((buf[0] & 0xff) << 8);
            cBuf |= ((buf[0] & 0xff00) >> 8);
            k_write(myFile, &cBuf, n);
        }

        k_close(myFile);
    }
}
void get_contents(DIR *d) {
    struct dirent *entry;
    int i = 0;
    while ((entry = readdir(d)) != NULL) {
        display_contents(entry->d_name);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: make_rootfs image_file image_size root_dir\n");
        return 0;
    }
    int sz = atoi(argv[2]);

    hdd_image = (uint16_t *)(malloc(sz * sizeof(uint16_t)));

    memset(hdd_image, 0, sz * sizeof(uint16_t));

    block_init();
    fs_init();

    fs_cwd = fs_root;

    printf("Format\n");
    fs_mkfs();
    printf("Create\n");

    block_sync();

    {
        DIR *d;
        int i = 3;
        struct stat s;
        char buf[255];
        lstat(argv[i], &s);
        if (S_ISDIR(s.st_mode)) {
            getwd(buf);
            chdir(argv[i]);
            printf("%s\n", argv[i]);
            d = opendir(".");
            get_contents(d);
            chdir(buf);
        }
    }

    block_sync();

    FILE *fd = fopen(argv[1], "w");
    fwrite(hdd_image, sizeof(uint16_t), sz, fd);
    fclose(fd);
    return 0;
}
