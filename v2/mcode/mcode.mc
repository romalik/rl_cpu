conf
#control width
out 16
#sequencer counter width
count 5
#command width
command 11
end


#### output definitions ####

define

# memory
ML 0 0
MR 1 0
MW 2 0

# clock
PCInc 3 0
PCLoad 4 1
PCBuf 5 0
PCV0 6 0
PCV1 7 0

end
#### end of definitions ####

section A[15] = 0

section A[5] = 1, A[6] = 0

end

section A[5] = 1, A[6] = 1

end




end
