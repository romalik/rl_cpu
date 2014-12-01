conf
#control width
out 19
#sequencer counter width
count 5
#command width
command 11
end


#### output definitions ####

define

# memory
ML 0 0
MR 1 1
MW 2 1

# clock
PCInc 3 0
PCLoad 4 0
PCOE 5 1
PCV0 6 0
PCV1 7 0


# control
IR 8 1
RCClk 9 0
RCOE 10 1
MCRST 11 0
PCLatchClk 12 0
PCLatchOE 13 1
CondJump 14 0

# alu
RAClk 15 0
RBClk 16 0
ALUBufOE 17 1

GenInt 18 0


#command
J 0 -1
DRF00 1 -1
DRF01 2 -1
DRF10 3 -1
DRF11 4 -1
DRF20 5 -1
DRF21 6 -1
DRFS0 7 -1
DRFS1 8 -1
INT 9 -1
IRQ 10 -1


end
#### end of definitions ####

section IRQ 0
	#fetch
	PCOE 0 ML 0 PCLatchClk 1
	MR 0 IR 0 PCInc 1

	section INT 0

		### FIRST OP ###

		#instant first op
		section DRF00 0 DRF01 0
			PCLatchOE 0 PCV0 1 PCV1 0 ML 1
			MR 0 RAClk 1
		end

		#memory first op
		section DRF00 1 DRF01 0
			PCLatchOE 0 PCV0 1 PCV1 0 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RAClk 1
		end

		#pointer first op
		section DRF00 0 DRF01 1
			PCLatchOE 0 PCV0 1 PCV1 0 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RAClk 1
		end


		### SECOND OP ###

		#instant second op
		section DRF10 0 DRF11 0
			PCLatchOE 0 PCV0 0 PCV1 1 ML 1
			MR 0 RBClk 1
		end

		#memory second op
		section DRF10 1 DRF11 0
			PCLatchOE 0 PCV0 0 PCV1 1 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RBClk 1
		end

		#pointer second op
		section DRF10 0 DRF11 1
			PCLatchOE 0 PCV0 0 PCV1 1 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RBClk 1
		end


		### DEST OP ###

		#instant dest op
		section DRF20 0 DRF21 0
			PCLatchOE 0 PCV0 1 PCV1 1 ML 1
			MR 0 RCClk 1
		end

		#memory dest op
		section DRF20 1 DRF21 0
			PCLatchOE 0 PCV0 1 PCV1 1 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RCClk 1
		end

		#pointer dest op
		section DRF20 0 DRF21 1
			PCLatchOE 0 PCV0 1 PCV1 1 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RCClk 1
			RCOE 0 ML 1
			MR 0 RCClk 1
		end

		### JUMP ###

		section J 1
			RCOE 0 CondJump 1
		end

		### Store ALU results ###

		section J 0
			RCOE 0 ML 1
			ALUBufOE 0 MW 0
		end

		MCRST 1

	end

	section INT 1
		# NOP instruction
		section J 0
			GenInt 1		
		end
		MCRST 1
	end

end

section IRQ 1
	
end
