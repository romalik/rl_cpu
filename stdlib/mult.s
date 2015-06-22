.code
.label MULI1
cnst_b 0			;here we keep the result, accessable as local 0
.label mult_begin
iaddrf_b 0			;load A
cnst_b 0			;compare with 0
eq_w mult_end		;A == 0 - jump to end

iaddrf_b 0			;load A again
band_b 1			;check LSB
cnst_b 0			;if equals to zero
eq_w skip			;jump to shift
					;else
iaddrl_b 0			;load current result
iaddrf_b 1			;load current B
add					;sum them
addrl_b 0			;store new result
rstore				;

iaddrf_b 1			;load B again
lsh_b 1				;shift it left
addrf_b 1			;store it back
rstore


.label skip
iaddrf_b 0			;load A
rsh_b 1				;shift it right
addrf_b 0			;store A back
rstore

jump_w mult_begin

.label mult_end
ret
