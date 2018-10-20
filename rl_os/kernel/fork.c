#include <fork.h>
#include <kstdio.h>


int enable_copy_on_write = ENABLE_COPY_ON_WRITE;
struct Process * do_fork(struct Process * p, int clone) {
    struct Process * new_p;
    int fd;
    size_t new_selector;
    size_t pageno = 0;
    new_p = get_free_proc();

//    printf("new_p 0x%04X\n", new_p);



    if(!new_p) return NULL;

    memcpy((unsigned int *)new_p, (unsigned int *)p, sizeof(struct Process));

    new_p->state = PROC_STATE_CONSTRUCT;
    new_p->pid = sched_genPid();
    new_p->parent = p;
	if(!clone) {
		new_p->mmuSelector = mmu_get_free_selector();
		mmu_mark_selector(new_p->mmuSelector, 1);
	} else {
		new_p->mmuSelector = p->mmuSelector;
	}

	if(clone) {
		new_p->isThread = 1;
	} else {
		new_p->isThread = 0;
	}

    new_p->intFrame.SW = (new_p->intFrame.SW & 0xff00) | (new_p->mmuSelector & 0x00ff);
//    printf("Forking pid %d to pid %d\n", p->pid, new_p->pid);
	if(!clone) {
		if((!enable_copy_on_write) || (p->pid == 0)) {
		  //copy code
		  while(pageno < 128) {
			  size_t src_page = mmu_read_table(p->mmuSelector, pageno, 1);
			  size_t target_page;
			  if(src_page == FREE_PAGE_MARK) break;

			  target_page = mmu_get_free_page();
	  //        printf("Select target %d\n", target_page);
			  mmu_mark_page(target_page, 1);
			  mmu_copy_pages(src_page, target_page, 0, 14, 15);

			  mmu_write_table(new_p->mmuSelector, pageno, 1, target_page);

			  pageno++;
		  }

		  //copy data
		  pageno = 0;
		  while(pageno < 16) {
			  size_t src_page = mmu_read_table(p->mmuSelector, pageno, 0);
			  size_t target_page;
			  if(src_page == FREE_PAGE_MARK) break;

			  target_page = mmu_get_free_page();
	  //        printf("Select target %d\n", target_page);
			  mmu_mark_page(target_page, 1);
			  mmu_copy_pages(src_page, target_page, 0, 14, 15);

			  mmu_write_table(new_p->mmuSelector, pageno, 0, target_page);

			  pageno++;

			  if(p->pid == 0) {
				  if(pageno == 14) {
					  break;
				  }
			  }
		  }
		} else {
		  //copy on write
		  //copy code
		  while(pageno < 128) {
			  size_t src_page = mmu_read_table(p->mmuSelector, pageno, 1);
			  if(src_page == FREE_PAGE_MARK) break;
			  mmu_inc_page_refcnt(src_page);
			  mmu_write_table(new_p->mmuSelector, pageno, 1, src_page);
			  mmu_write_table_flags(p->mmuSelector, pageno, 1, PAGE_FLAG_READ_ONLY);
			  mmu_write_table_flags(new_p->mmuSelector, pageno, 1, PAGE_FLAG_READ_ONLY);
			  pageno++;
		  }

		  //copy data
		  pageno = 0;
		  while(pageno < 16) {
			  size_t src_page = mmu_read_table(p->mmuSelector, pageno, 0);
			  if(src_page == FREE_PAGE_MARK) break;
			  mmu_inc_page_refcnt(src_page);
			  mmu_write_table(new_p->mmuSelector, pageno, 0, src_page);
			  mmu_write_table_flags(p->mmuSelector, pageno, 0, PAGE_FLAG_READ_ONLY);
			  mmu_write_table_flags(new_p->mmuSelector, pageno, 0, PAGE_FLAG_READ_ONLY);
			  pageno++;
			  if(p->pid == 0) {
				  if(pageno == 14) {
					  break;
				  }
			  }
		  }
		}
	}
/*
    printf("Old proc: "); printProcess(p);
    printf("New proc: "); printProcess(new_p);
*/

    for (fd = 0; fd < MAX_FILES_PER_PROC; fd++) {
      if (p->openFiles[fd]) {
        p->openFiles[fd]->refcnt++;
      }
    }


    return new_p;
}

