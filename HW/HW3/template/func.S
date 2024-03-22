# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.globl LPS

LPS:
	addi sp,sp,-20##store in stack
	sw s0,0(sp)
	sw s1,4(sp)
	sw s2,8(sp)
	sw s3,12(sp)
	sw s4,16(sp)
	mv s0,a0 ##存储指针位置
	li s1,1	##最大长度
	##li s2,0 	##字符串长度-----------------------mistake
	addi sp,sp,-4
	sw ra,0(sp) #####y因为lps就是跳转进来的所以需要存储ra
	li t2,0 ## offset
	jal calculate ## 存储ra 以便返回main主程序
	lw ra,0(sp)
	addi sp,sp,4

	li s3,0## 开始位置
	li s4,0##    int i=0
	addi sp,sp,-8    #####################################是否有必要？ 有必要存loop返回fps主体
	sw ra,0(sp)
	sw s5,4(sp)   ###store in stack
	jal loop ##开始循环
	lw s5,4(sp)   ##load
    lw ra,0(sp)
	addi sp,sp,8
    mv a0,s1
	lw s0,0(sp)
	lw s1,4(sp)
	lw s2,8(sp)  ##load
	lw s3,12(sp)
	lw s4,16(sp)
	addi sp,sp,20
    ret     ##返回main函数
calculate:
	add t1,t2,s0   ##地址
	lb t1,0(t1)     ##读取
	beqz  t1,calculate_end ##判断是否结束
	addi t2,t2,1   ##偏移+1
	j calculate     ##循环
calculate_end:
	mv s2,t2  #将s2赋值
	ret     #跳回           直接ret
loop:
	bge s4,s2,end_1##判断第一个for条件 若大于等于结束
	addi s5,s4,1 ##      int j=i+1
    addi sp,sp,-12
    sw ra,0(sp)  ##store in stack
	sw s8,4(sp)
	sw s9,8(sp)
	jal loop_2##进入第二层循环
	lw s8,4(sp)
	lw s9,8(sp)
    lw ra,0(sp)		###load 
    addi sp,sp,12
    
	addi s4,s4,1#      i++
	##lb a1,0(a0)##读取string中的字符
	##addi a0,a1,x0
	j loop  ##循环
loop_2:
	bge s5,s2,end_2##判断第二个for条件  若大于等于结束
	mv s8,s4 ## int tmp1=i
	mv s9,s5##   int tmp2=j
    
	addi sp,sp,-16  #存ra
	sw ra,0(sp)
	sw s6,4(sp)  ## store in stack
	sw s7,8(sp)
	sw s10,12(sp)

	jal while_loop ##跳转while
	lw ra,0(sp)	### load
	lw s6,4(sp)
	lw s7,8(sp)
	lw s10,12(sp)

	addi sp,sp,16    #取ra

	addi sp,sp,-8 #存ra
	sw ra,0(sp)
	sw s11,4(sp)
	jal determine##跳转if_determine
	lw ra,0(sp)
	lw s11,4(sp)
	addi sp,sp,8   #取ra
    
	addi s5,s5,1 ####j++
	j loop_2	 ##循环

	
determine:
	sub s11,s5,s4 ##j-i
	addi s11,s11,1##j-i+1
	blt s8,s9,determine_end ##tmp1>=tmp2   若小于跳转
	bge s1,s11,determine_end ##maxlen<j-i+1      若大于等于跳转
	##进入该部分满足大于等于和小于
	mv s1,s11
	mv s3,s4
	ret##jr ra##跳转回loop2因为结束

determine_end:####不满足if判断返回directly
	ret##jr ra ##跳转回loop2

while_loop:
    
    add s10,s8,s0
	lb s6,0(s10) ###str[tmp1]------------------------mistake##no mistake direct get the index 
	add s10,s9,s0
	lb s7,0(s10) ###str[tmp2]------------------------mistake

	bge s8,s9,while_end_1 ##判断tmp1<tmp2             若大于等于跳转
	bne s6,s7,while_end_1 ##判断str[tmp1]==str[tmp2] 若不想等跳转
	##进入该部分满足小于且等于
	addi s8,s8,1	##tmp1++
	addi s9,s9,-1 ##tmp2--
	j while_loop ##while循环


while_end_1:
	ret##jr ra ##结束while循环返回loop2

end_2:
	ret##返回第一个loop
end_1:
	ret##返回fps主函数 ###########################################可能存在跳转错误
# YOUR CODE HERE
##results are in a0 first，a0是最终结果初始存储位置