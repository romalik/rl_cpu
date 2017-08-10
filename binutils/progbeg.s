.code
.import main
.import exit
.import malloc_init_auto
.import __io_init_vars
.label __progbeg
.export __progbeg
call0_w malloc_init_auto
call0_w __io_init_vars 
call1_w main
call0_w exit
.data
.label environ
.byte 1 0

