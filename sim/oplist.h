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

    neg,

    add,
    add_b,
    add_w,

    band,
    band_b,
    band_w,

    bor,
    bor_b,
    bor_w,

    bxor,
    bxor_b,
    bxor_w,

    lsh,
    lsh_b,
    lsh_w,

    rsh,
    rsh_b,
    rsh_w,

    sub,
    sub_b,
    sub_w,

    eq_w,
    ge_w,
    gt_w,
    le_w,
    lt_w,
    ne_w,

    ueq_w,
    uge_w,
    ugt_w,
    ule_w,
    ult_w,
    une_w,

    call_w,
    call,

    ret,
    ret2,

    jump_w,
    jump,

    discard1,
    discard_b,
    discard_w,

    alloc1,
    alloc_b,
    alloc_w,

    fastcall,
    fastcall_w,

    fastret,
    fastret2,

    store,
    rstore,

    dup_op
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

    "neg",

    "add",
    "add_b",
    "add_w",

    "band",
    "band_b",
    "band_w",

    "bor",
    "bor_b",
    "bor_w",

    "bxor",
    "bxor_b",
    "bxor_w",

    "lsh",
    "lsh_b",
    "lsh_w",

    "rsh",
    "rsh_b",
    "rsh_w",

    "sub",
    "sub_b",
    "sub_w",

    "eq_w",
    "ge_w",
    "gt_w",
    "le_w",
    "lt_w",
    "ne_w",

    "ueq_w",
    "uge_w",
    "ugt_w",
    "ule_w",
    "ult_w",
    "une_w",

    "call_w",
    "call",

    "ret",
    "ret2",

    "jump_w",
    "jump",

    "discard1",
    "discard_b",
    "discard_w",

    "alloc1",
    "alloc_b",
    "alloc_w",

    "fastcall",
    "fastcall_w",

    "fastret",
    "fastret2",

    "store",
    "rstore",

    "dup",
    ""

};
