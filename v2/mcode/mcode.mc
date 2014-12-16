conf
#control width
out 23
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
# 111 IntOE

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
UserSet 19 0
UserReset 20 0
Interrupt 21 0
IntReset 22 0

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

################### FIRST OP ########################
                section DRFS0 0


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

                    #pointer to pointer first op
                    section DRF00 1 DRF01 1
                            #PCLatchOE -> ML
                            R1 1 PCV0 1 PCV1 0 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RA
                            R0 1 R1 1 RAClk 1
                    end
                end

                section DRFS0 1
                    #first op not modified
                    section DRF00 0 DRF01 0
                    end

                    #PC->A
                    section DRF00 1 DRF01 0
                        #PCOE
                        R0 1 RAClk 1
                    end

                    #SP->A
                    section DRF00 0 DRF01 1
                        #SPOE
                        R0 1 R2 1 RAClk 1
                    end

                end

###################### SECOND OP #################################

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

                #pointer to pointer second op
                section DRF10 1 DRF11 1
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
                        R0 1 R1 1 RCClk 1
                        #RCOE
                        R1 1 R2 1 ML 1
                        #MR
                        R0 1 R1 1 RBClk 1
                end

		### DEST OP ###

                section DRFS1 0

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

                    #pointer to pointer dest op
                    section DRF20 1 DRF21 1
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
                            #RCOE
                            R1 1 R2 1 ML 1
                            #MR
                            R0 1 R1 1 RCClk 1
                    end
                end


		### JUMP ###

		section J 1
			#RCOE 
			R1 1 R2 1 PCCondLoad 1
		end

		### Store ALU results ###

		section J 0
                    section DRFS1 0
                        #RCOE
			R1 1 R2 1 ML 1
			#ALUOE
			R2 1 MW 1
                    end
                    section DRFS1 1
                        #ALU->SP
                        R2 1 SPLoad 1
                    end
		end

		MCRST 1

        #end INT 0
	end


########### EXTENDED COMMANDS #################
        section INT 1
            # 0000 -> nop
            section DRF21 0 DRF20 0 DRF11 0 DRF10 0
                MCRST 1
            end

############ 0001 PUSH #############################

            section DRF21 0 DRF20 0 DRF11 0 DRF10 1

################### FIRST OP -> RC ########################
                section DRFS0 0


                    #instant first op
                    section DRF00 0 DRF01 0
                            #PCLatchOE
                            R1 1 PCV0 1 PCV1 0 ML 1
                            #MR
                            R0 1 R1 1 RCClk 1
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
                            R0 1 R1 1 RCClk 1
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
                            R0 1 R1 1 RCClk 1
                    end

                    #pointer to pointer first op
                    section DRF00 1 DRF01 1
                            #PCLatchOE -> ML
                            R1 1 PCV0 1 PCV1 0 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RA
                            R0 1 R1 1 RCClk 1
                    end
                end

                section DRFS0 1
                    #first op not modified
                    section DRF00 0 DRF01 0
                    end

                    #PC->A
                    section DRF00 1 DRF01 0
                        #PCOE
                        R0 1 RCClk 1
                    end

                    #SP->A
                    section DRF00 0 DRF01 1
                        #SPOE
                        R0 1 R2 1 RCClk 1
                    end

                end
############ end first op ###############
                #SPOE -> ML
                R0 1 R2 1 ML 1
                #RC -> MW
                R1 1 R2 1 MW 1
                SPUp 1
                MCRST 1
            end
############ end PUSH #####################

############ 0010 POP #############################
            section DRF21 0 DRF20 0 DRF11 1 DRF10 0
                SPDown 1
                #SP -> ML
                R0 1 R2 1 ML 1
                #MR -> A
                R0 1 R1 1 RAClk 1

                #op2 -> B
                #PCLatchOE
                R1 1 PCV0 0 PCV1 1 ML 1
                #MR
                R0 1 R1 1 RBClk 1


################### FIRST OP -> ML ########################
                section DRFS0 0


                    #instant first op
                    section DRF00 0 DRF01 0
                            #PCLatchOE
                            R1 1 PCV0 1 PCV1 0 ML 1
                    end

                    #memory first op
                    section DRF00 1 DRF01 0
                            #PCLatchOE
                            R1 1 PCV0 1 PCV1 0 ML 1
                            #MR
                            R0 1 R1 1 RCClk 1
                            #RCOE
                            R2 1 R1 1 ML 1
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
                    end

                    #pointer to pointer first op
                    section DRF00 1 DRF01 1
                            #PCLatchOE -> ML
                            R1 1 PCV0 1 PCV1 0 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                    end

                    #ALUOE -> MW
                    R2 1 MW 1
                end

                section DRFS0 1
                    #first op not modified
                    section DRF00 0 DRF01 0

                    end

                    #PC->A
                    section DRF00 1 DRF01 0
                        #ALUOE -> PC
                        R2 1 PCLoad 1
                    end

                    #SP->A
                    section DRF00 0 DRF01 1
                        #ALUOE -> SP
                        R2 1 SPLoad 1
                    end

                end
############ end first op ###############

                MCRST 1
            end
############ end POP #####################


############ 0011 CALL #############################
            section DRF21 0 DRF20 0 DRF11 1 DRF10 1

                #pc->*sp
                #sp++
                #(op2, deref=arg) -> pc

                #SPOE -> ML
                R0 1 R2 1 ML 1
                #PCOE -> MW
                R0 1 MW 1

                SPUp 1

################### FIRST OP -> pc ########################
                section DRFS0 0


                    #instant first op
                    section DRF00 0 DRF01 0
                            #PCLatchOE
                            R1 1 PCV0 1 PCV1 0 ML 1
                            #MR
                            R0 1 R1 1 PCLoad 1
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
                            R0 1 R1 1 PCLoad 1
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
                            R0 1 R1 1 PCLoad 1
                    end

                    #pointer to pointer first op
                    section DRF00 1 DRF01 1
                            #PCLatchOE -> ML
                            R1 1 PCV0 1 PCV1 0 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RC
                            R0 1 R1 1 RCClk 1
                            #RCOE -> ML
                            R1 1 R2 1 ML 1
                            #MR -> RA
                            R0 1 R1 1 PCLoad 1
                    end
                end

                section DRFS0 1
                    #first op not modified
                    section DRF00 0 DRF01 0
                    end

                    #PC->A
                    section DRF00 1 DRF01 0
                        #PCOE
                        R0 1 PCLoad 1
                    end

                    #SP->A
                    section DRF00 0 DRF01 1
                        #SPOE
                        R0 1 R2 1 PCLoad 1
                    end

                end
############ end first op ###############

                MCRST 1

            end
############ end CALL #####################

############ 0100 RET #############################
            section DRF21 0 DRF20 1 DRF11 0 DRF10 0
                #sp--
                #*sp->pc
                SPDown 1
                #SPOE -> ML
                R0 1 R2 1 ML 1
                #MR -> PCLoad
                R0 1 R1 1 PCLoad 1

                MCRST 1

            end
############ end RET #####################

        end


end


section IRQ 1
	
end
