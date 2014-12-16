.text
add:
#prolog
push BP 0 0
add SP 0 BP
push 0 0 0 # allocate for result


add *BP-2 *BP-3 BP-1


#epilog
sub SP 1 SP # free locals
pop BP 0 0
ret 0 0 0


add_caller:
push BP 0 0
add SP 0 BP

push 0 0 0 # result [BP + 1]
push 1 0 0 # param1
push 2 0 0 # param2
call add 0 0
sub SP 2 SP
add *BP+1 0 0xFFFF
sub SP 1 SP
pop BP 0 0
ret 0 0 0
