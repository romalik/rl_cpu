#include <execve.h>
#define EXEC_READ_CHUNK 0x1000
#define ARGV_BUFFER_SIZE 256

#define MAX_ENVP_ENTRIES 5
#define MAX_ENVP_ENTRY_LENGTH 30

#define MAX_ARGV_ENTRIES 15
unsigned int argvBuffer[ARGV_BUFFER_SIZE];

size_t parseArgs(const char **argv_in, const char **envp_in, unsigned int *buf, size_t off) {
    /*
   argc
   argv **
   envp **
   envp[0] *
   ...
   envp[n] *
   envp[0] [maxlen]
   ...
   envp[n] [maxlen]
   argv[0] *
   ...
   argv[m] *
   argv[0]
   ...
   argv[m]


*/
    unsigned int * target_argc;
    unsigned int * target_argv;
    unsigned int * target_envp;

    unsigned int * target_envp_ptrs_area;
    unsigned int * target_envp_entries_area;

    unsigned int * target_argv_ptrs_area;
    unsigned int * target_argv_entries_area;

    unsigned int * envPtr_target;
    unsigned int * envPtr_source;
    unsigned int nEnv = 0;

    const char ** p;
    char * t;
    unsigned int i;

    target_argc = buf;
    target_argv = buf + 1;
    target_envp = buf + 2;

    target_envp_ptrs_area = buf + 3;
    target_envp_entries_area = target_envp_ptrs_area + MAX_ENVP_ENTRIES;
    target_argv_ptrs_area = target_envp_entries_area + MAX_ENVP_ENTRIES * MAX_ENVP_ENTRY_LENGTH;
    target_argv_entries_area = target_argv_ptrs_area + MAX_ARGV_ENTRIES;

    memset(buf, 0, target_argv_entries_area - buf);

    *target_argv = target_argv_ptrs_area - buf + off;
    *target_envp = target_envp_ptrs_area - buf + off;

    //fill envp pointers
    for(i = 0; i<MAX_ENVP_ENTRIES; i++) {
        target_envp_ptrs_area[i] = target_envp_entries_area + i*MAX_ENVP_ENTRY_LENGTH - buf + off;
    }
    p = envp_in;
    i = 0;
    // copy envp entries
    if(envp_in) {
        while(*p) {
            strncpy((char *)(target_envp_entries_area + i*MAX_ENVP_ENTRY_LENGTH), *p, MAX_ENVP_ENTRY_LENGTH);
            i++;
            p++;
        }
    }
    target_envp_ptrs_area[i] = 0;


    p = argv_in;
    t = (char *)target_argv_entries_area;
    i = 0;
    if(argv_in) {
        while(*p) {
            strcpy(t, *p);
            target_argv_ptrs_area[i] = (size_t)((size_t)t - (size_t)buf + (size_t)off);
            i++;
            t += strlen(*p)+1;
            p++;
        }
    }
    target_argv_ptrs_area[i] = 0;
    *target_argc = i;

    return (size_t)t - (size_t)buf;
}

unsigned int do_execve(struct Process * p, const char * u_filename, const char ** argv, const char ** envp) {
    FILE *fd;
    unsigned int filename[100];
//    unsigned int * filename;
    unsigned int header[9];
    unsigned int cnt = 0;
    unsigned int sizeText;
    unsigned int sizeTextHigh;
    unsigned int sizeData;
//    off_t sizeTextFull;
    size_t stack_placement;
    size_t off;
    size_t pageno = 0;
    struct InterruptFrame launchFrame;

    printf("DO_EXECVE for pid %d\n", p->pid);


//    printf("\n\n\n\n!!!!!!\n!!!!!!\nEXEC LOAD HACK /sh!\n!!!!!!\n!!!!!!\n\n\n\n");
    ugets(p, (size_t)u_filename, 0, 14, 100, 1, filename);
//    filename = (unsigned int *)("/sh");
    printf("DO_EXECVE [1] for pid %d\n", p->pid);

    printf("Try exec %s\n", filename);
    fd = k_open(filename, 'r');
    if(!fd) {
      puts("EXEC: fd == 0\n");
        return 1;
    }
    printf("DO_EXECVE [2] for pid %d\n", p->pid);

    while(cnt != 9) {
        cnt += k_read(fd, header+cnt, (9-cnt));
        printf("EXEC: read %d\n", cnt);
        if(k_isEOF(fd)) {
            k_close(fd);
            puts("EXEC: short header\n");
            return 1;
        }
    }
    printf("DO_EXECVE [3] for pid %d\n", p->pid);

    if(!memcmp(header, &"#!", 2)) {
        //this is a script!!
        char interp[30];
        char * t_argv[3];
        int i;
        puts("EXEC: is a script\n");
        t_argv[0] = (char *)filename;
        t_argv[1] = (char *)filename;
        t_argv[2] = 0;
        k_close(fd);
        fd = k_open(filename, 'r');
        k_read(fd, (unsigned int *)(interp), 30);
        for(i = 0; i<30; i++) {
            if(interp[i] == '\n') interp[i] = 0;
        }
        return do_execve(p, (interp+2), (const char **)t_argv, envp);


    }

    printf("DO_EXECVE [4] for pid %d\n", p->pid);

    if(memcmp(header, &"REXE", 4)) {
        k_close(fd);
        puts("EXEC: bad header\n");
        return 1;
    }

//    mode = header[4];
    sizeTextHigh = header[5];
    sizeText = header[6];
    sizeData = header[8];

    printf("DO_EXECVE [5] for pid %d\n", p->pid);

    printf("Loading bin %s header OK text %d data %d\n", filename, sizeText, sizeData);

    stack_placement = sizeData;
    off = parseArgs(argv, envp, argvBuffer, stack_placement);

    printf("EXEC : freeing old process pages for pid %d\n", p->pid);
    freeProcessPages(p);
    printf("EXEC : freeing old process pages done\n");

    //read text

    while(cnt < sizeText) {
        size_t read_now;
        size_t target_page;
        if(sizeTextHigh) {
            printf("Long bin!\n");
            while(1) {}
        }
        read_now = (sizeText - cnt); //fit to one page
        printf("EXEC: text read now 0x%04x\n", read_now);
        if(read_now > 0x1000) read_now = 0x1000;
        target_page = mmu_get_free_page();
        printf("EXEC: text new target page 0x%04x as pageno %d\n", target_page, pageno);
        mmu_mark_page(target_page, 1);
        mmu_write_table(0, 14, 0, target_page); //assign target page to pageno 14

        mmu_write_table(p->mmuSelector, pageno, 1, target_page);
        pageno++;
        cnt += read_now;
        while(read_now) {
            read_now -= k_read(fd, (unsigned int *)((14 << 12)), read_now);
        }
    }

    //read data
    pageno = 0;
    cnt = 0;
    while(cnt < sizeData) {
        size_t read_now;
        size_t target_page;
        read_now = (sizeData - cnt); //fit to one page
        printf("EXEC: data read now 0x%04x\n", read_now);
        if(read_now > 0x1000) read_now = 0x1000;
        target_page = mmu_get_free_page();
        printf("EXEC: data new target page 0x%04x as pageno %d\n", target_page, pageno);

        mmu_mark_page(target_page, 1);
        mmu_write_table(0, 14, 0, target_page); //assign target page to pageno 14

        mmu_write_table(p->mmuSelector, pageno, 0, target_page);
        pageno++;
        cnt += read_now;
        while(read_now) {
            read_now -= k_read(fd, (unsigned int *)((14 << 12)), read_now);
        }
    }

    while(pageno < 16) { //allocate data seg
      size_t target_page;
      target_page = mmu_get_free_page();
      printf("EXEC: data allocating empty page 0x%04x as pageno %d\n", target_page, pageno);
      mmu_mark_page(target_page, 1);
      mmu_write_table(p->mmuSelector, pageno, 0, target_page);
      pageno++;

    }

    k_close(fd);

    p->sp = stack_placement;

    p->sp += uputs(p, stack_placement, 0, 14, off, 0, argvBuffer);

    launchFrame.AP = stack_placement;
    launchFrame.BP = stack_placement;
    launchFrame.highPC = 0;
    launchFrame.D = 0;
    launchFrame.S = 0;
    launchFrame.SW = SW_INT_ENABLE | SW_MMU_ENABLE | SW_USER_MODE | (p->mmuSelector&0xff);

    printf("EXEC : write int frame\n");
    p->sp += uputs(p, p->sp, 0, 14, sizeof(struct InterruptFrame) - 1, 0, (unsigned int *)(&launchFrame));
    memcpy((unsigned int *)p->cmd, (unsigned int *)filename, 32);

    printf("EXEC: done\n");
    return 0;
}

