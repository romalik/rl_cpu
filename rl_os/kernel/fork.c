#include <fork.h>

int do_fork(struct Process * p) {
    struct Process * new_p;
    size_t new_selector;
    size_t pageno = 0;

    new_p = get_free_proc();
    if(!new_p) return -1;
    new_p->pid = sched_genPid();
    memcpy((unsigned int *)&new_p->ap, (unsigned int *)&p->ap, sizeof(struct Process) - 2);

    new_p->mmuSelector = mmu_get_free_selector();
    mmu_mark_selector(new_p->mmuSelector, 1);


    //copy code
    while(pageno < 128) {
        size_t src_page = mmu_read_table(new_p->mmuSelector, pageno, 1);
        size_t target_page;
        if(src_page == FREE_PAGE_MARK) break;

        target_page = mmu_get_free_page();
        mmu_mark_page(target_page, 1);
        mmu_copy_pages(src_page, target_page, 0, 14, 15);

        mmu_write_table(new_p->mmuSelector, pageno, 1, target_page);

        pageno++;
    }

    //copy data
    pageno = 0;
    while(pageno < 16) {
        size_t src_page = mmu_read_table(new_p->mmuSelector, pageno, 0);
        size_t target_page;
        if(src_page == FREE_PAGE_MARK) break;

        target_page = mmu_get_free_page();
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

    if(p->pid == 0) {
        //do not launch the process if forked from pid 0 - exec follows
        return new_p->pid;
    }

    run_proc(new_p);
    return new_p->pid;
}

