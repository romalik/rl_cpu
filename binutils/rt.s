.code
.export halt
.label halt
jump_w halt


.code
.label fastcall1
; stack : ....|arg1|func_addr>
alloc_b 1   
addrl_b 0   
iaddrl_w -5 
store       
iaddrl_w -4
call
ret


.code
.label fastcall2
; stack : ....|arg1|arg2|func_addr>

alloc_b 2    
addrl_b 0    
iaddrl_w -6  
store		 

addrl_b 1    
iaddrl_w -5  
store		 

iaddrl_w -4  
call

ret

.code


.label fastcall2_long
; stack : ....|arg1l|arg1h|arg2l|arg2h|func_addr|RET|BP|AP| <sp> 

alloc_b 4 
   
addrl_b 0    
iaddrl_w -8  
store		 

addrl_b 1    
iaddrl_w -7  
store		 


addrl_b 2    
iaddrl_w -6  
store		 


addrl_b 3    
iaddrl_w -5  
store		 

iaddrl_w -4  
call

ret2



.code
.label fastcall3
; stack : ....|arg1|arg2|arg3|func_addr>

alloc_b 3    
addrl_b 0    
iaddrl_w -7  
store		 

addrl_b 1    
iaddrl_w -6  
store		 

addrl_b 2    
iaddrl_w -5  
store		 

iaddrl_w -4  
call
ret

