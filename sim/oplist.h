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

    neg,
    neg2,

    add,
    add_b,
    add_w,
    add2,

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
    lsh_b,
    lsh_w,
    lsh2,

    rsh,
    rsh_b,
    rsh_w,
    rsh2,

    sub,
    sub_b,
    sub_w,
    sub2,

    eq_w,
    eq2_w,
    ge_w,
    ge2_w,
    gt_w,
    gt2_w,
    le_w,
    le2_w,
    lt_w,
    lt2_w,
    ne_w,
    ne2_w,

    ueq_w,
    ueq2_w,
    uge_w,
    uge2_w,
    ugt_w,
    ugt2_w,
    ule_w,
    ule2_w,
    ult_w,
    ult2_w,
    une_w,
    une2_w,

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

    store2,
    rstore2,

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
    "indir2",

    "neg",
    "neg2",

    "add",
    "add_b",
    "add_w",
    "add2",

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
    "lsh_b",
    "lsh_w",
    "lsh2",

    "rsh",
    "rsh_b",
    "rsh_w",
    "rsh2",

    "sub",
    "sub_b",
    "sub_w",
    "sub2",

    "eq_w",
    "eq2_w",
    "ge_w",
    "ge2_w",
    "gt_w",
    "gt2_w",
    "le_w",
    "le2_w",
    "lt_w",
    "lt2_w",
    "ne_w",
    "ne2_w",

    "ueq_w",
    "ueq2_w",
    "uge_w",
    "uge2_w",
    "ugt_w",
    "ugt2_w",
    "ule_w",
    "ule2_w",
    "ult_w",
    "ult2_w",
    "une_w",
    "une2_w",

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

    "store2",
    "rstore2",

    "dup",
    ""

};
