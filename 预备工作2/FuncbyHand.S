	.arch armv7-a
	.arm

	.text @代码段
	.global square
square: @function int square(int a)
	str fp, [sp, #-4]! @pre-index mode, sp = sp -4, push fp
	mov fp, sp
	sub sp, sp, #8 @为本地变量开辟空间
	str r0, [fp, #-8] @r0 = [fp, #-8] = a
	mul r1, r0, r0
	mov r0, r1
	add sp, fp, #0
	ldr fp, [sp], #4
	bx lr

	.text @代码段
	.global main
	.type main, %function
main:
	push {fp, lr}
	sub sp, sp, #4
	ldr r0, =_cin
	mov r1, sp
	bl scanf
	ldr r0, [sp, #0] @取出输入的内容放入r0中
	add sp, sp, #4
	bl square
	mov r1, r0
	ldr r0, =_cout
	bl printf
	mov r0, #0
	pop {fp, lr}
	bx lr

	

.data @数据段
_cin:
	.asciz "%d"

_cout:
	.asciz "%d\n"


.section .note.GNU-stack,"",%progbits @ do you know what's the use of this :-)
