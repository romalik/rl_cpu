.code
loadsp_w 0x4000
cnst_w 0x4000
popap
cnst_w 0x4000
popbp

call1_w main
call1_w halt
