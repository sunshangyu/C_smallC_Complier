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
fact:
addi $sp, $sp, -92
sw $a0, 0($sp)
lw $t0, 0($sp)
sw $t0, 4($sp)
li $t0, 1
sw $t0, 8($sp)
lw $t0, 4($sp)
lw $t1, 8($sp)
beq $t0, $t1,label1
j label2
label1:
lw $t0, 0($sp)
sw $t0, 12($sp)
lw $t0, 12($sp)
move $v0, $t0
addi $sp, $sp, 92
jr $ra
j label3
label2:
lw $t0, 0($sp)
sw $t0, 16($sp)
lw $t0, 0($sp)
sw $t0, 20($sp)
li $t0, 1
sw $t0, 24($sp)
lw $t0, 20($sp)
lw $t1, 24($sp)
sub $t0, $t0, $t1
sw $t0, 28($sp)
lw $t0, 28($sp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal fact
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 32($sp)
lw $t0, 16($sp)
lw $t1, 32($sp)
mul $t0, $t0, $t1
sw $t0, 36($sp)
lw $t0, 36($sp)
move $v0, $t0
addi $sp, $sp, 92
jr $ra
label3:
main:
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 40($sp)
lw $t0, 40($sp)
sw $t0, 44($sp)
lw $t0, 44($sp)
sw $t0, 48($sp)
lw $t0, 44($sp)
sw $t0, 52($sp)
li $t0, 1
sw $t0, 56($sp)
lw $t0, 52($sp)
lw $t1, 56($sp)
bgt $t0, $t1,label4
j label5
label4:
lw $t0, 44($sp)
sw $t0, 60($sp)
lw $t0, 60($sp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal fact
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 64($sp)
lw $t0, 64($sp)
sw $t0, 68($sp)
lw $t0, 68($sp)
sw $t0, 72($sp)
j label6
label5:
li $t0, 1
sw $t0, 76($sp)
lw $t0, 76($sp)
sw $t0, 68($sp)
lw $t0, 68($sp)
sw $t0, 80($sp)
label6:
lw $t0, 68($sp)
sw $t0, 84($sp)
lw $t0, 84($sp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t0, 0
sw $t0, 88($sp)
lw $t0, 88($sp)
move $v0, $t0
addi $sp, $sp, 92
jr $ra
