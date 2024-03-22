.data
n: .word 3 # number of test data
data: # input, answer
.word 0x12345678, 0x1E6A2C48
.word 0x71C924BF, 0xFD24938E
.word 0x19260817, 0xE8106498

.text
# === main and helper functions ===
# You don't need to understand these, but reading them may be useful
main:
    la t0, n
    lw s0, 0(t0)
    la s1, data
    main_loop: # run each of the n tests
        beq s0, zero, main_loop_end
        lw a0, 0(s1)
        jal print_hex_and_space # print input
        lw a0, 4(s1)
        jal print_hex_and_space # print answer
        lw a0, 0(s1)
        jal bitrev1
        jal print_hex_and_space # print result of bitrev1
        lw a0, 0(s1)
        jal bitrev2
        jal print_hex_and_space # print result of bitrev2
        jal print_newline
        addi s0, s0, -1
        addi s1, s1, 8
        j main_loop
    main_loop_end:
    li a0, 10
    ecall # exit

print_hex_and_space:
    mv a1, a0
    li a0, 34
    ecall
    li a1, ' '
    li a0, 11
    ecall
    ret

print_newline:
    li a1, '\n'
    li a0, 11
    ecall
    ret

# === The first version ===
# Reverse the bits in a0 with a loop
bitrev1:
    ### TODO: YOUR CODE HERE ###
    addi sp,sp,-4
    sw s0,0(sp)
    mv s0,a0
    addi a0,x0,0
    addi t0,x0,0
    addi t1,x0,31
rev_loop:
    blt t1,t0, end
    addi t2,x0,1
    sll t2,t2,t0
    and t2,s0,t2
    beq t2,x0,reset
    addi t2,x0,1
    sub t3,t1,t0
    sll t2,t2,t3
    or a0,t2,a0
    addi t0,t0,1
    j rev_loop
reset:
    addi t0,t0,1
    j rev_loop
end:
    lw s0,0(sp)
    addi sp,sp,4
    ret
    
# === The second version ===
# Reverse the bits in a0. Only use li, and, or, slli, srli!
bitrev2:
    ### TODO: YOUR CODE HERE ###
    li t0,0x55555555
    and t1,a0,t0
    slli t2,t1,1
    srli a1,a0,1
    and a2,a1,t0
    or a0 t2,a2

    li t0,0x33333333
    and t1,a0,t0
    slli t2,t1,2
    srli a1,a0,2
    and a2,a1,t0
    or a0 t2,a2
    
    li t0,0x0F0F0F0F
    and t1,a0,t0
    slli t2,t1,4
    srli a1,a0,4
    and a2,a1,t0
    or a0 t2,a2
    
    li t0,0x00FF00FF
    and t1,a0,t0
    slli t2,t1,8
    srli a1,a0,8
    and a2,a1,t0
    or a0 t2,a2
    
    li t0,0x0000FFFF
    and t1,a0,t0
    slli t2,t1,16
    srli a1,a0,16
    and a2,a1,t0
    or a0 t2,a2
    ret
