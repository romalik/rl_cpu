.code
.export test_and_set
.label test_and_set

;; unsigned int test_and_set(unsigned int * addr);

iaddrf_b 0 ; put addr
swp_b 1 ; swap
addrl_b -4
rstore
ret

