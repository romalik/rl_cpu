#include <blk.h>
#include <ata.h>


struct Block blockCache[BLOCK_CACHE_SIZE];
unsigned int blockDataCache[BLOCK_CACHE_SIZE * 64*4];

void dump_blocks() {
    int i;
    for(i = 0; i<BLOCK_CACHE_SIZE; i++) {
        printf("Block cache %d flags %d device %d n %d cnt %d\n", i, blockCache[i].flags, blockCache[i].device, blockCache[i].n, blockCache[i].cnt);
    }
}

void block_sync() {
    int i;
    printf("Try sync\n");
    dump_blocks();    
    for(i = 0; i<BLOCK_CACHE_SIZE; i++) {
        if(blockCache[i].cnt == 0) {
            if(blockCache[i].flags == BLOCK_MODIFIED) {
                ataWriteSectorsLBA(blockCache[i].n, blockCache[i].data);
            }
            blockCache[i].flags = BLOCK_FREE;
        }
    }
}

struct Block * bread(unsigned int device, unsigned int n) {
  unsigned int i;
//  printf("bread: %d %d\n", device, n);
  while(1) {
    for(i = 0; i<BLOCK_CACHE_SIZE; i++) {
      if(blockCache[i].flags != BLOCK_FREE) {
        if(blockCache[i].device == device && blockCache[i].n == n) {
          blockCache[i].cnt++;
//          printf("bread ret %d\n", i);
          return &blockCache[i];
        }
      }
    }
    for(i = 0; i<BLOCK_CACHE_SIZE; i++) {
      if(blockCache[i].flags == BLOCK_FREE) {
        blockCache[i].flags = BLOCK_USED;
        blockCache[i].device = device;
        blockCache[i].n = n;
        blockCache[i].cnt = 1;
        blockCache[i].data = &blockDataCache[64*4*i];
        ataReadSectorsLBA(n, blockCache[i].data);
//        printf("bread fetch and ret %d\n", i);
        return &blockCache[i];
      }
    }
    block_sync();
  }
}
void bfree(struct Block * b) {
//  printf("bfree\n");
  b->cnt--;
}

void block_init() {
  unsigned int i;
  for(i = 0; i<BLOCK_CACHE_SIZE; i++) {
    blockCache[i].flags = BLOCK_FREE;
  }
}
