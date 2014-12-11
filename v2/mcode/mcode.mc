conf
#control width
out 19
#sequencer counter width
count 5
#command width
command 11
#split width
split 8
end


#### output definitions ####

define

#read:(3 bit)
# 000 #NONE##
# 001 PCOE
# 010 PCLatchOE
# 011 MR
# 100 ALUOE
# 101 SPOE
# 110 RCOE

R0 0 0
R1 1 0
R2 2 0

# write (AND-ed with overlap clk)
IRClk 3 0
PCLoad 4 0
PCLatchClk 5 0
RAClk 6 0
RBClk 7 0
RCClk 8 0
ML 9 0
MW 10 0
SPLoad 11 0
PCCondLoad 12 0


# Control
PCUp 13 0
PCV0 14 0
PCV1 15 0
SPUp 16 0
SPDown 17 0
MCRST 18 0


#command
J 0 -1
DRF00 1 -1
DRF01 2 -1
DRF10 4 -1
DRF11 5 -1
DRF20 6 -1
DRF21 7 -1
DRFS0 3 -1
DRFS1 8 -1
INT 9 -1
IRQ 10 -1


end
#### end of definitions ####

section IRQ 0
	#fetch

	#PCOE
	R0 1 ML 1 PCLatchClk 1
	#MR
	R0 1 R1 1 IRClk 1 PCUp 1

	section INT 0

		### FIRST OP ###

		#instant first op
		section DRF00 0 DRF01 0
			#PCLatchOE
			R1 1 PCV0 1 PCV1 0 ML 1
			#MR
			R0 1 R1 1 RAClk 1
		end

		#memory first op
		section DRF00 1 DRF01 0
			#PCLatchOE 
			R1 1 PCV0 1 PCV1 0 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R2 1 R1 1 ML 1
			#MR
			R0 1 R1 1 RAClk 1
		end

		#pointer first op
		section DRF00 0 DRF01 1
			#PCLatchOE
			R1 1 PCV0 1 PCV1 0 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RAClk 1
		end


		### SECOND OP ###

		#instant second op
		section DRF10 0 DRF11 0
			#PCLatchOE
			R1 1 PCV0 0 PCV1 1 ML 1
			#MR
			R0 1 R1 1 RBClk 1
		end

		#memory second op
		section DRF10 1 DRF11 0
			#PCLatchOE
			R1 1 PCV0 0 PCV1 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RBClk 1
		end

		#pointer second op
		section DRF10 0 DRF11 1
			#PCLatchOE
			R1 1 PCV0 0 PCV1 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RBClk 1
		end


		### DEST OP ###

		#instant dest op
		section DRF20 0 DRF21 0
			#PCLatchOE
			R1 1 PCV0 1 PCV1 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
		end

		#memory dest op
		section DRF20 1 DRF21 0
			#PCLatchOE
			R1 1 PCV0 1 PCV1 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
		end

		#pointer dest op
		section DRF20 0 DRF21 1
			#PCLatchOE
			R1 1 PCV0 1 PCV1 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
			#RCOE
			R1 1 R2 1 ML 1
			#MR
			R0 1 R1 1 RCClk 1
		end

		### JUMP ###

		section J 1
			#RCOE 
			R1 1 R2 1 PCCondLoad 1
		end

		### Store ALU results ###

		section J 0
			#RCOE
			R1 1 R2 1 ML 1
			#ALUOE
			R2 1 MW 1
		end

		MCRST 1

	end

end

section IRQ 1
	
end
