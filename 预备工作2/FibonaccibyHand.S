	.arch arm7-a
	.arch_extension crc
	.arm
@数据段（全局变量和常量的声明）
@本章代码暂无

@代码段
@主函数
	.global main
	.type main, %function
mian:
	.data 
	.global a
	.align 4
	.size a, 4
a:
	.word 0
	
