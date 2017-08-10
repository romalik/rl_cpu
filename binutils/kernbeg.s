.code
.import kernel_main
.import __data_end
loadsp_w __data_end
cnst_w __data_end
popap
cnst_w __data_end
popbp

call1_w kernel_main
