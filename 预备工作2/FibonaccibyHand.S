@处理器寄存器被指定为R0、R1等。
@MOVE指令的源位于左侧，目标位于右侧。
@伪处理程序中的堆栈从高地址增长到低地址。因此，push会导致堆栈指针的递减。pop会导致堆栈指针的增量。
@寄存器 sp(stack pointer) 用于指向堆栈。
@寄存器 fp(frame pointer) 用作帧指针。帧指针充当被调用函数和调用函数之间的锚。
@当调用一个函数时，该函数首先将 fp 的当前值保存在堆栈上。然后，它将 sp 寄存器的值保存在 fp 寄存器中。然后递减 sp 寄存器来为本地变量分配空间。
@fp 寄存器用于访问本地变量和参数，局部变量位于帧指针的负偏移量处，传递给函数的参数位于帧指针的正偏移量。
@当函数返回时， fp 寄存器被复制到 sp 寄存器中，这将释放用于局部变量的堆栈，函数调用者的 fp 寄存器的值由pop从堆栈中恢复。
@因为在调用者中使用了 bl 调用子函数的时候，会将当前 PC 的值保存在 LR 中，这时将 LR 中的值载入到 PC 中，可以使得程序运行位置返回调用者中
	.arch armv7-a @处理器架构
	.arm
@r0是格式化字符串，r1是对应的printf对应的第二个参数
@代码段
@主函数
	.text @代码段
	.global main
	.type main, %function
main:	
	push {fp, lr} @将fp的当前值保存在堆栈上，然后将sp寄存器的值保存在fp中，lr中存储的是pc的保存在lr中
	sub sp, sp, #4 @在栈中开辟一块大小为4的内存地址，用于存储即将输入的数据
	ldr r0, =_cin
	mov r1, sp @将sp的值传输给r1寄存器，使scanf传入的值存储在栈上，即栈顶的值是n
	bl scanf
	ldr r6, [sp, #0] @取出sp指针指向的地址中的内容，即栈顶中的内容（输入的n的值）
	add sp, sp, #4 @恢复栈顶，释放内存空间

	@测试是否写入
	@ldr r0, =_bridge3
        @mov r1, r2
        @bl printf

	mov r4, #0 @a = 0
	mov r5, #1 @b = 1
	mov r7, #1 @i = 1
	@r4中存a的值，r5中存b的值，r7中存i的值，r6中存n的值
	ldr r0, =_bridge
	mov r1, r4 @将r4中的值即a的值赋予r1
	bl printf @打印a的值
	ldr r0, =_bridge2
	mov r1, r5 @将r5中的值即b的值赋予r1
	bl printf @打印b的值
        ldr r0, =_bridge4
        bl printf
	
	@输出进行调试
	@ldr r0, =_bridge3
        @mov r1, r6
        @bl printf
        @ldr r0, =_bridge3
        @mov r1, r7
        @bl printf

Loop:
	@输出进行调试
	@ldr r0, =_bridge4
	@bl printf
	@ldr r0, =_bridge3
	@mov r1, r6
        @bl printf
	@ldr r0, =_bridge3
        @mov r1, r7
        @bl printf

	cmp r6, r7
	ble RETURN @比较r7和r6（即i和n）的大小用于跳转
	mov r8, r5 @t = b @r8为临时变量的寄存器
	add r5, r5, r4 @b = a + b
	ldr r0, =_bridge3
	mov r1, r5 @将r5中的值即b的值赋予r1
	bl printf  @cout << b << endl;
	mov r4, r8 @a = t
	add r7, r7, #1 @i = i + 1
	b Loop
RETURN:
	pop {fp, lr} @上下文切换
	bx lr @return 0
.data @数据段
_cin:
	.asciz "%d"

_bridge:
	.asciz "a:%d\n"

_bridge2:
	.asciz "b:%d\n"

_bridge3:
        .asciz "%d\n"

_bridge4:
	.asciz "We are going to loop now! \n"

.section .note.GNU-stack,"",%progbits @ do you know what's the use of this :-)


