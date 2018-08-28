.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra
write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra
main:
addi $sp, $sp, -100
li $t0, 0
sw $t0, 0($sp)
lw $t0, 0($sp)
sw $t0, 4($sp)
li $t0, 1
sw $t0, 8($sp)
lw $t0, 8($sp)
sw $t0, 12($sp)
li $t0, 0
sw $t0, 16($sp)
lw $t0, 16($sp)
sw $t0, 20($sp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 24($sp)
lw $t0, 24($sp)
sw $t0, 28($sp)
lw $t0, 28($sp)
sw $t0, 32($sp)
label1:
lw $t0, 20($sp)
sw $t0, 36($sp)
lw $t0, 28($sp)
sw $t0, 40($sp)
lw $t0, 36($sp)
lw $t1, 40($sp)
blt $t0, $t1,label2
j label3
label2:
lw $t0, 4($sp)
sw $t0, 44($sp)
lw $t0, 12($sp)
sw $t0, 48($sp)
lw $t0, 44($sp)
lw $t1, 48($sp)
add $t0, $t0, $t1
sw $t0, 52($sp)
lw $t0, 52($sp)
sw $t0, 56($sp)
lw $t0, 12($sp)
sw $t0, 60($sp)
lw $t0, 60($sp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
lw $t0, 12($sp)
sw $t0, 64($sp)
lw $t0, 64($sp)
sw $t0, 4($sp)
lw $t0, 4($sp)
sw $t0, 68($sp)
lw $t0, 56($sp)
sw $t0, 72($sp)
lw $t0, 72($sp)
sw $t0, 12($sp)
lw $t0, 12($sp)
sw $t0, 76($sp)
lw $t0, 20($sp)
sw $t0, 80($sp)
li $t0, 1
sw $t0, 84($sp)
lw $t0, 80($sp)
lw $t1, 84($sp)
add $t0, $t0, $t1
sw $t0, 88($sp)
lw $t0, 88($sp)
sw $t0, 20($sp)
lw $t0, 20($sp)
sw $t0, 92($sp)
j label1
label3:
li $t0, 0
sw $t0, 96($sp)
lw $t0, 96($sp)
move $v0, $t0
addi $sp, $sp, 100
jr $ra
