typedef union Header {
    struct {
        union Header *nxt;
        unsigned int     sz;
    }s;
} Hdr;

static Hdr *basep;
static Hdr *freep;

void
malloc_init(void *start, unsigned int sz) {
    printf("Call krmalloc_init 0x%04x : 0x%04x\n", (unsigned int)start, sz);

    /* create header */
    basep = (Hdr *)((unsigned int* )start - (int)(((unsigned int)start) % 2));
    basep->s.nxt = basep + 1;
    basep->s.sz = 0;

    freep = basep->s.nxt;
    freep->s.nxt = basep;
    /* use number of units instead of byte */
    freep->s.sz = (sz - sizeof(Hdr)) / sizeof(Hdr);
}

void *
malloc(unsigned int sz) {
    /* get # of alined units */
    unsigned int nunits = (sz + sizeof(Hdr) -1) / sizeof(Hdr) + 1;
    Hdr *prevp = freep;
    Hdr *p;

    printf("Call krmalloc\n");

    for (p = prevp->s.nxt; p->s.sz < nunits; prevp = p, p = p->s.nxt) {
        if (p == freep) return 0;
    }

    if (p->s.sz == nunits) {
        prevp->s.nxt = p->s.nxt;
    }
    else {
        p->s.sz -= nunits;
        p += p->s.sz;
        p->s.sz = nunits;
    }
    freep = prevp;
    return (void *)(p + 1);
}

void
free(void *ptr) {
    Hdr *fp = (Hdr *)ptr -1;
    Hdr *p;

    printf("Call krfree\n");

    for (p = freep; !(p < fp && fp < p->s.nxt); p = p->s.nxt) {
        if (p >= p->s.nxt && (fp > p || fp < p->s.nxt)) break;
    }
    if (fp + fp->s.sz == p->s.nxt) {
        fp->s.sz += p->s.nxt->s.sz;
        fp->s.nxt = p->s.nxt->s.nxt;
    }
    else {
        fp->s.nxt = p->s.nxt;
    }
    if (p + p->s.sz == fp) {
        p->s.sz += fp->s.sz;
        p->s.nxt = fp->s.nxt;
    }
    else {
        p->s.nxt = fp;
    }
    freep = p;
}
