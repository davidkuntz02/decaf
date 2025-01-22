
#	Package foo

.section .rodata # read only data section
percentD:  .string "%d" # ALWAYS needed for print int
# strings
_L0:	.string  "\n"	# global string 
_L1:	.string  "\n"	# global string 
# end of strings

.data # start of the DATA section for variables
	.comm _SCANFHOLD,8,8	# MANDATORY space for SCANF reads
	.comm A,800,800		# global variable defined
	.align 8
#end of data section

.text	#start of code segment

	.globl main 
	.type main @function 
main:	#Start of Function 
	.cfi_startproc	# 	STANDARD FUNCTION HEADER FOR GAS
	pushq   %%rbp	# 	STANDARD FUNCTION HEADER FOR GAS
	.cfi_def_cfa_offset 16	# 	STANDARD FUNCTION HEADER FOR GAS
	.cfi_offset 6, -16	# 	STANDARD FUNCTION HEADER FOR GAS
	movq    %rsp, %rbp	# 	STANDARD FUNCTION HEADER FOR GAS
	.cfi_def_cfa_register 6	# 	STANDARD FUNCTION HEADER FOR GAS

	subq $872, %rsp		# Carve out stack for method

	mov $13, %rax		# expr load number
	mov %rax, 808(%rsp)		# store RHS value into memory
	mov $0, %rax		# Load Local variable offset into RAX
	add %rsp, %rax		# Add stack pointer to RAX

	mov 808(%rsp), %rbx		# get RHS stored value
	mov %rbx, (%rax)		# assignstmt final store

	mov $12, %rax		# expr load number
	mov %rax, 816(%rsp)		# store RHS value into memory
	mov $1, %rax		# expr load number
	mov %rax, %rbx		# store array index in RBX
	shl $3, %rbx		# multiply wordsize for array reference
	mov $8, %rax		# Load Local variable offset into RAX
	add %rsp, %rax		# Add stack pointer to RAX
	add %rbx, %rax		# move add on %rbx as this is an array reference

	mov 816(%rsp), %rbx		# get RHS stored value
	mov %rbx, (%rax)		# assignstmt final store

	mov $0, %rax		# Load Local variable offset into RAX
	add %rsp, %rax		# Add stack pointer to RAX

	mov (%rax), %rax		# read in memory value from rhs
	mov %rax, 824(%rsp)		# store RHS value into memory
	mov $2, %rax		# expr load number
	mov %rax, %rbx		# store array index in RBX
	shl $3, %rbx		# multiply wordsize for array reference
	mov $8, %rax		# Load Local variable offset into RAX
	add %rsp, %rax		# Add stack pointer to RAX
	add %rbx, %rax		# move add on %rbx as this is an array reference

	mov 824(%rsp), %rbx		# get RHS stored value
	mov %rbx, (%rax)		# assignstmt final store

	mov $999, %rax		# expr load number
	mov %rax, 832(%rsp)		# store RHS value into memory
	mov $0, %rax		# expr load number
	mov %rax, %rbx		# store array index in RBX
	shl $3, %rbx		# multiply wordsize for array reference
	mov $A, %rax		# Load Global variable address into RAX
	add %rbx, %rax		# move add on %rbx as this is an array reference

	mov 832(%rsp), %rbx		# get RHS stored value
	mov %rbx, (%rax)		# assignstmt final store

	mov $2, %rax		# expr load number
	mov %rax, %rbx		# store array index in RBX
	shl $3, %rbx		# multiply wordsize for array reference
	mov $8, %rax		# Load Local variable offset into RAX
	add %rsp, %rax		# Add stack pointer to RAX
	add %rbx, %rax		# move add on %rbx as this is an array reference

	mov (%rax), %rax		# read in memory value from rhs
	mov %rax, %rsi		# RSI needs the value to print
	mov $percentD, %rdi		# RDI needs to be the int format
	mov $0, %rax		# RAX needs to be 0
	call printf		# print a number from expression

	mov $_L0, %rdi		# RDI is the label address
	mov $0, %rax		# RAX needs to be zero
	call printf		# print a string

	mov $0, %rax		# expr load number
	mov %rax, %rbx		# store array index in RBX
	shl $3, %rbx		# multiply wordsize for array reference
	mov $A, %rax		# Load Global variable address into RAX
	add %rbx, %rax		# move add on %rbx as this is an array reference

	mov (%rax), %rax		# read in memory value from rhs
	mov %rax, %rsi		# RSI needs the value to print
	mov $percentD, %rdi		# RDI needs to be the int format
	mov $0, %rax		# RAX needs to be 0
	call printf		# print a number from expression

	mov $_L1, %rdi		# RDI is the label address
	mov $0, %rax		# RAX needs to be zero
	call printf		# print a string

	mov $0, %rax		# Default return value
	leave		# leave the function
	.cfi_def_cfa 7,8		# STANDARD end function for GAS
	ret

	.cfi_endproc		# STANDARD end function for GAS
	.size	main, .-main		# STANDARD end function for GAS

	.ident  "GCC: (SUSE Linux) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
