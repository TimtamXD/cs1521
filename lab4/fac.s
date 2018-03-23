# COMP1521 Lab 04 ... Simple MIPS assembler


### Global data

   .data
msg1:
   .asciiz "n: "
msg2:
   .asciiz "n! = "
eol:
   .asciiz "\n"

### main() function
number: .space 4
   .data
   .align 2

main_ret_save:
   .word 4
   .text
   .globl main

main:
   sw   $ra, main_ret_save

#  ... your code for main() goes here
   la   $a0, msg1
   li   $v0, 4        # printf("n: ");
   syscall
   li   $v0, 5        # scanf("%d", &number);
   syscall
   move $a0, $v0   # number = reg[v0]
   jal  fac           # call function, result is in $v0
   add  $t2, $v0, $0  # result of function is in $t2
   la   $a0, msg2
   li   $v0, 4        # printf("n!: ")
   syscall
   li   $a0, 0       # put result into $a0 to be printed
   add  $a0, $t2, $0
   li   $v0, 1
   syscall            # print result
   la   $a0, eol
   li   $v0, 4        # printf("\n");
   syscall
   lw   $ra, main_ret_save
   jr   $ra           # return

### fac() function

   .data
   .align 2
fac_ret_save:
   .space 4

   .text

fac:
   sw   $ra, fac_ret_save

#  ... your code for fac() goes here
   li   $t0, 1
   move $t1, $a0      # move number into t1
loop:
   blez $t1, exit     # quit if t1 is less than or equal to zero
   mul  $t0, $t0, $t1 # multiply by 1
   addi $t1, $t1, -1  # minus t1
   j loop             # keep going until t1 = 0
exit:
   move $v0, $t0      # place return value back in 
   lw   $ra, fac_ret_save
   jr   $ra            # return ($v0)
