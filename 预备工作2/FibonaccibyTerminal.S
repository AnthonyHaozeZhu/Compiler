	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"Fibonacci.cpp"
	.text
	.section	.rodata
	.align	2
	.type	_ZStL19piecewise_construct, %object
	.size	_ZStL19piecewise_construct, 1
_ZStL19piecewise_construct:
	.space	1
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,4
	.text
	.align	1
	.global	main
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	main, %function
main:
	.fnstart
.LFB1516:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r7, lr}
	.save {r4, r7, lr}
	.pad #28
	sub	sp, sp, #28
	.setfp r7, sp, #0
	add	r7, sp, #0
	ldr	r4, .L6
.LPIC0:
	add	r4, pc
	ldr	r2, .L6+4
.LPIC1:
	add	r2, pc
	ldr	r3, .L6+8
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #20]
	mov	r3,#0
	movs	r3, #0
	str	r3, [r7, #4]
	movs	r3, #1
	str	r3, [r7, #8]
	movs	r3, #1
	str	r3, [r7, #12]
	mov	r3, r7
	mov	r1, r3
	ldr	r3, .L6+12
	ldr	r3, [r4, r3]
	mov	r0, r3
	bl	_ZNSirsERi(PLT)
	ldr	r1, [r7, #4]
	ldr	r3, .L6+16
	ldr	r3, [r4, r3]
	mov	r0, r3
	bl	_ZNSolsEi(PLT)
	mov	r2, r0
	ldr	r3, .L6+20
	ldr	r3, [r4, r3]
	mov	r1, r3
	mov	r0, r2
	bl	_ZNSolsEPFRSoS_E(PLT)
	ldr	r1, [r7, #8]
	ldr	r3, .L6+16
	ldr	r3, [r4, r3]
	mov	r0, r3
	bl	_ZNSolsEi(PLT)
	mov	r2, r0
	ldr	r3, .L6+20
	ldr	r3, [r4, r3]
	mov	r1, r3
	mov	r0, r2
	bl	_ZNSolsEPFRSoS_E(PLT)
.L3:
	ldr	r3, [r7]
	ldr	r2, [r7, #12]
	cmp	r2, r3
	bge	.L2
	ldr	r3, [r7, #8]
	str	r3, [r7, #16]
	ldr	r2, [r7, #8]
	ldr	r3, [r7, #4]
	add	r3, r3, r2
	str	r3, [r7, #8]
	ldr	r1, [r7, #8]
	ldr	r3, .L6+16
	ldr	r3, [r4, r3]
	mov	r0, r3
	bl	_ZNSolsEi(PLT)
	mov	r2, r0
	ldr	r3, .L6+20
	ldr	r3, [r4, r3]
	mov	r1, r3
	mov	r0, r2
	bl	_ZNSolsEPFRSoS_E(PLT)
	ldr	r3, [r7, #16]
	str	r3, [r7, #4]
	ldr	r3, [r7, #12]
	adds	r3, r3, #1
	str	r3, [r7, #12]
	b	.L3
.L2:
	movs	r3, #0
	ldr	r1, .L6+24
.LPIC2:
	add	r1, pc
	ldr	r2, .L6+8
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #20]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L5
	bl	__stack_chk_fail(PLT)
.L5:
	mov	r0, r3
	adds	r7, r7, #28
	mov	sp, r7
	@ sp needed
	pop	{r4, r7, pc}
.L7:
	.align	2
.L6:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC1+4)
	.word	__stack_chk_guard(GOT)
	.word	_ZSt3cin(GOT)
	.word	_ZSt4cout(GOT)
	.word	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(GOT)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.fnend
	.size	main, .-main
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z41__static_initialization_and_destruction_0ii, %function
_Z41__static_initialization_and_destruction_0ii:
	.fnstart
.LFB2002:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r7, lr}
	sub	sp, sp, #12
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7]
	ldr	r4, .L11
.LPIC5:
	add	r4, pc
	ldr	r3, [r7, #4]
	cmp	r3, #1
	bne	.L10
	ldr	r3, [r7]
	movw	r2, #65535
	cmp	r3, r2
	bne	.L10
	ldr	r3, .L11+4
.LPIC3:
	add	r3, pc
	mov	r0, r3
	bl	_ZNSt8ios_base4InitC1Ev(PLT)
	ldr	r3, .L11+8
.LPIC4:
	add	r3, pc
	mov	r2, r3
	ldr	r3, .L11+12
	ldr	r3, [r4, r3]
	mov	r1, r3
	ldr	r3, .L11+16
.LPIC6:
	add	r3, pc
	mov	r0, r3
	bl	__aeabi_atexit(PLT)
.L10:
	nop
	adds	r7, r7, #12
	mov	sp, r7
	@ sp needed
	pop	{r4, r7, pc}
.L12:
	.align	2
.L11:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC5+4)
	.word	_ZStL8__ioinit-(.LPIC3+4)
	.word	__dso_handle-(.LPIC4+4)
	.word	_ZNSt8ios_base4InitD1Ev(GOT)
	.word	_ZStL8__ioinit-(.LPIC6+4)
	.cantunwind
	.fnend
	.size	_Z41__static_initialization_and_destruction_0ii, .-_Z41__static_initialization_and_destruction_0ii
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_GLOBAL__sub_I_main, %function
_GLOBAL__sub_I_main:
	.fnstart
.LFB2003:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	add	r7, sp, #0
	movw	r1, #65535
	movs	r0, #1
	bl	_Z41__static_initialization_and_destruction_0ii(PLT)
	pop	{r7, pc}
	.cantunwind
	.fnend
	.size	_GLOBAL__sub_I_main, .-_GLOBAL__sub_I_main
	.section	.init_array,"aw",%init_array
	.align	2
	.word	_GLOBAL__sub_I_main(target1)
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",%progbits
