#ifndef OPLIST_H_
#define OPLIST_H_

enum opname {
    nop = 0,

    addrf_b,
    addrf_w,
    iaddrf_b,
    iaddrf_w,

    addrl_b,
    addrl_w,
    iaddrl_b,
    iaddrl_w,

    cnst_b,
    cnst_w,
    icnst_b,
    icnst_w,

    addrs_b,
    addrs_w,
    iaddrs_b,
    iaddrs_w,

    indir,
    indir2,

    add,
    add_b,
    add_w,
    add2,

    sub,
    sub_b,
    sub_w,
    sub2,

    band,
    band_b,
    band_w,
    band2,

    bor,
    bor_b,
    bor_w,
    bor2,

    bxor,
    bxor_b,
    bxor_w,
    bxor2,

    lsh,

    rsh,

    eq_w,
    ge_w,
    gt_w,
    le_w,
    lt_w,
    ne_w,

    uge_w,
    ugt_w,
    ule_w,
    ult_w,

    call0_w,
    call0,
    call1_w,
    call1,
    call2_w,
    call2,

    ret,

    jump_w,
    jump,

    discard_b,
    discard_w,

    alloc_b,
    alloc_w,

    store,
    rstore,

    store2,
    rstore2,

    dup_op,

    ei,
    di
};

char oplist[][16] = {
    "nop",

    "addrf_b",
    "addrf_w",
    "iaddrf_b",
    "iaddrf_w",

    "addrl_b",
    "addrl_w",
    "iaddrl_b",
    "iaddrl_w",

    "cnst_b",
    "cnst_w",
    "icnst_b",
    "icnst_w",

    "addrs_b",
    "addrs_w",
    "iaddrs_b",
    "iaddrs_w",

    "indir",
    "indir2",

    "add",
    "add_b",
    "add_w",
    "add2",

    "sub",
    "sub_b",
    "sub_w",
    "sub2",

    "band",
    "band_b",
    "band_w",
    "band2",

    "bor",
    "bor_b",
    "bor_w",
    "bor2",

    "bxor",
    "bxor_b",
    "bxor_w",
    "bxor2",

    "lsh",

    "rsh",

    "eq_w",
    "ge_w",
    "gt_w",
    "le_w",
    "lt_w",
    "ne_w",

    "uge_w",
    "ugt_w",
    "ule_w",
    "ult_w",

    "call0_w",
    "call0",
    "call1_w",
    "call1",
    "call2_w",
    "call2",

    "ret",

    "jump_w",
    "jump",

    "discard_b",
    "discard_w",

    "alloc_b",
    "alloc_w",

    "store",
    "rstore",

    "store2",
    "rstore2",

    "dup",

    "ei",
    "di",

    ""

};
#endif
